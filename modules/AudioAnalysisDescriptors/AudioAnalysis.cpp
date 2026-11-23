//-----------------------------------------------------------------------------
// @file    AudioAnalysis.cpp
// @author  BrainModular team
// @brief   Audio descriptors of the input, computed by the Hanalysis library in a background thread.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include <chrono>
#include <vector>
#include "AudioAnalysis.h"
#include "ModuleDefinition.h"

namespace
{
	// sends the first count values of a result to an array output
	void setArrayOut(const UsineEventClass& arrOut, const std::vector<float>& values, int count)
	{
		arrOut.setSize(count);
		std::copy_n(values.begin(), count, arrOut.getDataAddr());
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new AudioAnalysis();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<AudioAnalysis*>(pModule);
}

AudioAnalysis::~AudioAnalysis()
{
	// waits for the end of an analysis in progress, before the analysis buffers are destroyed. The thread is
	// created by onInitModule, which is not called when the creation is cancelled in the query popup
	if (thread != nullptr)
		sdkThreadDestroy(thread);
}

void AudioAnalysis::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanBeSavedInPreset = FALSE;

	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryListString     = "Audio analysis type";
		pModuleInfo->QueryListValues     = "Basic,Advanced";
		pModuleInfo->QueryListDefaultIdx = 0;
		pModuleInfo->QueryDefaultValue1  = 0;
		pModuleInfo->QueryMaxValue1      = 1;
	}
}

// called after the query popup, before onCreateParameters
void AudioAnalysis::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	if ((queryResult1 == static_cast<int>(AnalysisType::Basic)) || (queryResult1 == static_cast<int>(AnalysisType::Advanced)))
	{
		analysisType             = static_cast<AnalysisType>(queryResult1);
		hanalysis.m_AnalysisType = queryResult1;
	}
}

void AudioAnalysis::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	// these outputs are displayed up to the Nyquist frequency of the current sample rate
	const float nyquist           = static_cast<float>(sdkGetSampleRate() / 2.0);
	const auto  createUpToNyquist = [this, nyquist](const TParamInfo& paramInfo, UsineEventClass& eventClass)
	{
		TParamInfo nyquistParamInfo = paramInfo;
		nyquistParamInfo.MaxValue   = nyquist;
		sdkCreateParam(nyquistParamInfo, eventClass);
	};

	sdkCreateParam(onAnalysisSwInParamInfo, onAnalysisSwIn);
	sdkCreateParam(audioInParamInfo, audioIn);
	createUpToNyquist(estimatedFrequencyFdrOutParamInfo, estimatedFrequencyFdrOut);
	sdkCreateParam(frequencyScoreFdrOutParamInfo, frequencyScoreFdrOut);
	createUpToNyquist(spectralCentroidFdrOutParamInfo, spectralCentroidFdrOut);
	createUpToNyquist(spectralSpreadFdrOutParamInfo, spectralSpreadFdrOut);
	sdkCreateParam(spectralFlatnessFdrOutParamInfo, spectralFlatnessFdrOut);
	sdkCreateParam(rmsFdrOutParamInfo, rmsFdrOut);
	sdkCreateParam(totalLoudnessFdrOutParamInfo, totalLoudnessFdrOut);
	sdkCreateParam(loudnessPerBandsArrOutParamInfo, loudnessPerBandsArrOut);
	sdkCreateParam(fftSizeLboxInParamInfo, fftSizeLboxIn);
	sdkCreateParam(overlapLboxInParamInfo, overlapLboxIn);
	sdkCreateParam(latencyFdrOutParamInfo, latencyFdrOut);

	if (analysisType != AnalysisType::Advanced)
		return;

	sdkCreateParam(onSpectralSwInParamInfo, onSpectralSwIn);
	sdkCreateParam(spectralSkewnessFdrOutParamInfo, spectralSkewnessFdrOut);
	sdkCreateParam(spectralKurtosisFdrOutParamInfo, spectralKurtosisFdrOut);
	sdkCreateParam(spectralSlopeFdrOutParamInfo, spectralSlopeFdrOut);
	createUpToNyquist(spectralRolloffFdrOutParamInfo, spectralRolloffFdrOut);
	sdkCreateParam(rolloffThresholdFdrInParamInfo, rolloffThresholdFdrIn);
	createUpToNyquist(spectralCrestFdrOutParamInfo, spectralCrestFdrOut);
	sdkCreateParam(tonalityCoeffFdrOutParamInfo, tonalityCoeffFdrOut);
	sdkCreateParam(spectralDecreaseFdrOutParamInfo, spectralDecreaseFdrOut);
	sdkCreateParam(spectralEntropyFdrOutParamInfo, spectralEntropyFdrOut);
	sdkCreateParam(peaksAmplitudesArrOutParamInfo, peaksAmplitudesArrOut);
	createUpToNyquist(peaksFrequenciesArrOutParamInfo, peaksFrequenciesArrOut);
	sdkCreateParam(onHarmonicSwInParamInfo, onHarmonicSwIn);
	sdkCreateParam(mfccArrOutParamInfo, mfccArrOut);
	sdkCreateParam(inharmonicityFdrOutParamInfo, inharmonicityFdrOut);
	sdkCreateParam(oddToEvenRatioFdrOutParamInfo, oddToEvenRatioFdrOut);
	sdkCreateParam(tristimulusArrOutParamInfo, tristimulusArrOut);
	sdkCreateParam(onTemporalSwInParamInfo, onTemporalSwIn);
	sdkCreateParam(onPerceptualSwInParamInfo, onPerceptualSwIn);
	sdkCreateParam(sharpnessFdrOutParamInfo, sharpnessFdrOut);
}

void AudioAnalysis::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	thread = sdkThreadCreate("process thread", &processThread, tpMedium, UINT32_MAX);

	// a parameter keeping its default value gets no callback, at creation or when a patch is loaded:
	// start from the current values (a later callback with the same value changes nothing)
	sampleRate = static_cast<float>(sdkGetSampleRate());
	for (const NativeInt callbackId : { ON_ANALYSIS_SW_CBID, FFT_SIZE_LBOX_CBID, OVERLAP_LBOX_CBID })
		applyParameter(callbackId);
	if (analysisType == AnalysisType::Advanced)
	{
		for (const NativeInt callbackId : { ON_SPECTRAL_SW_CBID, ROLLOFF_THRESHOLD_FDR_CBID, ON_HARMONIC_SW_CBID, ON_TEMPORAL_SW_CBID, ON_PERCEPTUAL_SW_CBID })
			applyParameter(callbackId);
	}
}

void AudioAnalysis::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	try
	{
		applyParameter(Message->wParam);
	}
	catch (...)
	{
		sdkReport(ERROR_CALLBACK);
	}
}

// reads a parameter into the requested state: called by its callback and by onInitModule
void AudioAnalysis::applyParameter(NativeInt callbackId)
{
	switch (callbackId)
	{
		case ON_ANALYSIS_SW_CBID:
			analysisOn = (onAnalysisSwIn.getData() != 0.0f);
			break;
		case ON_SPECTRAL_SW_CBID:
			spectralOn = (onSpectralSwIn.getData() != 0.0f);
			break;
		case ROLLOFF_THRESHOLD_FDR_CBID:
			changeRolloffThreshold();
			break;
		case ON_HARMONIC_SW_CBID:
			harmonicOn = (onHarmonicSwIn.getData() != 0.0f);
			break;
		case ON_TEMPORAL_SW_CBID:
			temporalOn = (onTemporalSwIn.getData() != 0.0f);
			break;
		case ON_PERCEPTUAL_SW_CBID:
			perceptualOn = (onPerceptualSwIn.getData() != 0.0f);
			break;
		case FFT_SIZE_LBOX_CBID:
			changeFftSize();
			break;
		case OVERLAP_LBOX_CBID:
			changeOverlap();
			break;
		default:
			break;
	}
}

void AudioAnalysis::onProcess()
{
	// the fft size and the overlap are applied to the input between two blocs, a new fft size clears it
	if (const int newFftSize = requestedFftSize; newFftSize != audioBuffer.getFFTSize())
		audioBuffer.setFFTSize(newFftSize);
	if (const int newOverlap = requestedOverlap; newOverlap != appliedOverlap)
	{
		audioBuffer.setClockTime(newOverlap);
		appliedOverlap = newOverlap;
	}
	if (sdkPatchJustActivated())
		audioBuffer.reset();

	audioBuffer.add(audioIn.getDataAddr(), sdkGetBlocSize());

	// a frame ended in this bloc: a new analysis starts only when the previous one is finished
	if (audioBuffer.isReadyToProcess() && analysisOn && !isProcessing && (thread != nullptr))
	{
		audioBuffer.copyFrame(frame.data());
		frameSize    = audioBuffer.getFFTSize();
		isProcessing = true;
		sdkThreadRestart(thread);
	}
}

// may come from the main thread or from an audio driver thread while an analysis runs:
// the analysis thread applies the new rate before its next analysis
void AudioAnalysis::onSampleRateChange(double SampleRate)
{
	sampleRate = static_cast<float>(SampleRate);
}

void AudioAnalysis::processThread(void* pModule, TThreadPtr pThread)
{
	auto* module = static_cast<AudioAnalysis*>(pModule);
	// set by the audio thread once the frame is written
	if (!module->isProcessing)
		return;
	try
	{
		module->onAnalysis();

		const auto currentTime = std::chrono::steady_clock::now();
		const auto duration    = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - module->lastAnalysisTime).count();
		module->latencyFdrOut.setData(module->hasLastAnalysis ? static_cast<float>(duration) : 0.0f);
		module->lastAnalysisTime = currentTime;
		module->hasLastAnalysis  = true;
	}
	catch (...)
	{
		sdkTraceLogChar("error in AudioAnalysis processThread");
	}
	// the audio thread can write the next frame
	module->isProcessing = false;
}

// called by the analysis thread, which owns hanalysis and the frame
void AudioAnalysis::onAnalysis()
{
	// changes requested since the last analysis
	const float newSampleRate = sampleRate;
	if (frameSize != analysisFftSize)
	{
		hanalysis.setFFTSize(frameSize, newSampleRate);
		analysisFftSize    = frameSize;
		analysisSampleRate = newSampleRate;
	}
	else if (newSampleRate != analysisSampleRate)
	{
		hanalysis.setSampleRate(newSampleRate);
		analysisSampleRate = newSampleRate;
	}
	hanalysis.spectralBool              = spectralOn;
	hanalysis.harmonicBool              = harmonicOn;
	hanalysis.temporalBool              = temporalOn;
	hanalysis.perceptualBool            = perceptualOn;
	hanalysis.features.rolloffThreshold = rolloffThreshold;

	hanalysis.process(frame.data(), frameSize, analysisSampleRate);

	const Results& results = hanalysis.results;
	if (analysisType == AnalysisType::Basic)
	{
		// frequency estimation
		sendEstimatedFrequency();

		// spectral metrics
		spectralCentroidFdrOut.setData(results.spectralCentroid);
		spectralSpreadFdrOut.setData(results.spectralSpread);
		spectralFlatnessFdrOut.setData(results.spectralFlatness);

		// level metrics
		rmsFdrOut.setData(results.temporalRMS);
		totalLoudnessFdrOut.setData(results.perceptLoudness.totalLoudness);
		setArrayOut(loudnessPerBandsArrOut, results.perceptLoudness.specificLoudness, NUM_BARK_BANDS);
	}
	else
	{
		// the outputs of a section that is OFF keep their last values
		if (hanalysis.spectralBool)
		{
			spectralCentroidFdrOut.setData(results.spectralCentroid);
			spectralSpreadFdrOut.setData(results.spectralSpread);
			spectralSkewnessFdrOut.setData(results.spectralSkewness);
			spectralKurtosisFdrOut.setData(results.spectralKurtosis);
			spectralSlopeFdrOut.setData(results.spectralSlope);
			spectralRolloffFdrOut.setData(results.spectralRolloff);
			spectralFlatnessFdrOut.setData(results.spectralFlatness);
			spectralCrestFdrOut.setData(results.spectralCrest);
			tonalityCoeffFdrOut.setData(results.spectralTonality);
			spectralDecreaseFdrOut.setData(results.spectralDecrease);
			spectralEntropyFdrOut.setData(results.spectralEntropy);
			const int numPeaks = static_cast<int>(results.spectralPeaks.size());
			peaksAmplitudesArrOut.setSize(numPeaks);
			peaksFrequenciesArrOut.setSize(numPeaks);
			for (int i = 0; i < numPeaks; i++)
			{
				peaksFrequenciesArrOut.setArrayData(i, results.spectralPeaks[i].frequency);
				peaksAmplitudesArrOut.setArrayData(i, results.spectralPeaks[i].magnitude);
			}
		}

		if (hanalysis.harmonicBool)
		{
			setArrayOut(mfccArrOut, results.harmonicMfcc, NUM_COEFF_MFCC);
			sendEstimatedFrequency();
			inharmonicityFdrOut.setData(results.harmonicInharmonicity);
			oddToEvenRatioFdrOut.setData(results.harmonicOTERatio);
			setArrayOut(tristimulusArrOut, results.harmonicTristimulus, NUM_TRISTIMULUS);
		}

		if (hanalysis.temporalBool)
			rmsFdrOut.setData(results.temporalRMS);

		if (hanalysis.perceptualBool)
		{
			totalLoudnessFdrOut.setData(results.perceptLoudness.totalLoudness);
			setArrayOut(loudnessPerBandsArrOut, results.perceptLoudness.specificLoudness, NUM_BARK_BANDS);
			sharpnessFdrOut.setData(results.perceptSharpness);
		}
	}
}

// out of its range, the estimate is considered wrong and the output keeps its last value
void AudioAnalysis::sendEstimatedFrequency()
{
	const PitchProbability& pitch = hanalysis.results.harmonicF0_YIN;
	if ((pitch.frequency > ESTIMATED_FREQUENCY_MIN) && (pitch.frequency < ESTIMATED_FREQUENCY_MAX))
		estimatedFrequencyFdrOut.setData(pitch.frequency);
	frequencyScoreFdrOut.setData(pitch.probability);
}

void AudioAnalysis::changeRolloffThreshold()
{
	const float threshold = rolloffThresholdFdrIn.getData();
	// the normalized spectrum sums to 1: a threshold of 1 or more might never be reached
	if ((threshold >= ROLLOFF_THRESHOLD_MIN) && (threshold < 1.0f))
		rolloffThreshold = threshold;
}

void AudioAnalysis::changeFftSize()
{
	int newSize = 1024;
	switch (static_cast<FftSize>(std::clamp(static_cast<int>(fftSizeLboxIn.getData()), 0, static_cast<int>(FftSize::Size8192))))
	{
		case FftSize::Size2048:
			newSize = 2048;
			break;
		case FftSize::Size4096:
			newSize = 4096;
			break;
		case FftSize::Size8192:
			newSize = 8192;
			break;
		default:
			break;
	}
	requestedFftSize = newSize;
}

void AudioAnalysis::changeOverlap()
{
	int overlapFactor = 1;
	switch (static_cast<Overlap>(std::clamp(static_cast<int>(overlapLboxIn.getData()), 0, static_cast<int>(Overlap::Four))))
	{
		case Overlap::Two:
			overlapFactor = 2;
			break;
		case Overlap::Four:
			overlapFactor = 4;
			break;
		default:
			break;
	}
	requestedOverlap = overlapFactor;
}
