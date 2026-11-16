//-----------------------------------------------------------------------------
// @file    AudioAnalysis.h
// @author  BrainModular team
// @brief   Audio descriptors of the input, computed by the Hanalysis library in a background thread.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <atomic>
#include <chrono>
#include <vector>
#include "Hanalysis/Hanalysis.h"
#include "Hanalysis/HaudioBuffer.h"
#include "../../sdk/UserDefinitions.h"

// length of the ring buffer receiving the audio input, in samples: a frame of the largest fft size and a bloc
constexpr int PROCESS_BUFFER_SIZE  = 20480;
// size of the analysis buffers until the fft size of the parameter is applied
constexpr int PROCESS_FFT_SIZE     = 4096;
// largest fft size: size of the frame given to the analysis thread
constexpr int PROCESS_MAX_FFT_SIZE = 8192;

class AudioAnalysis : public UserModuleBase
{
public:
	~AudioAnalysis() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;
	void onSampleRateChange(double SampleRate) override;

private:
	// items of the query popup: the advanced analysis adds parameters to the basic one
	enum class AnalysisType
	{
		Basic,
		Advanced
	};

	// items of the fft size list box
	enum class FftSize
	{
		Size1024,
		Size2048,
		Size4096,
		Size8192
	};

	// items of the overlap list box: number of analyses per window
	enum class Overlap
	{
		One,
		Two,
		Four
	};

	// basic and advanced analysis
	UsineEventClass onAnalysisSwIn;
	UsineEventClass audioIn;
	UsineEventClass estimatedFrequencyFdrOut;
	UsineEventClass frequencyScoreFdrOut;
	UsineEventClass spectralCentroidFdrOut;
	UsineEventClass spectralSpreadFdrOut;
	UsineEventClass spectralFlatnessFdrOut;
	UsineEventClass rmsFdrOut;
	UsineEventClass totalLoudnessFdrOut;
	UsineEventClass loudnessPerBandsArrOut;
	UsineEventClass fftSizeLboxIn;
	UsineEventClass overlapLboxIn;
	UsineEventClass latencyFdrOut;
	// advanced analysis only
	UsineEventClass onSpectralSwIn;
	UsineEventClass spectralSkewnessFdrOut;
	UsineEventClass spectralKurtosisFdrOut;
	UsineEventClass spectralSlopeFdrOut;
	UsineEventClass spectralRolloffFdrOut;
	UsineEventClass rolloffThresholdFdrIn;
	UsineEventClass spectralCrestFdrOut;
	UsineEventClass tonalityCoeffFdrOut;
	UsineEventClass spectralDecreaseFdrOut;
	UsineEventClass spectralEntropyFdrOut;
	UsineEventClass peaksAmplitudesArrOut;
	UsineEventClass peaksFrequenciesArrOut;
	UsineEventClass onHarmonicSwIn;
	UsineEventClass mfccArrOut;
	UsineEventClass inharmonicityFdrOut;
	UsineEventClass oddToEvenRatioFdrOut;
	UsineEventClass tristimulusArrOut;
	UsineEventClass onTemporalSwIn;
	UsineEventClass onPerceptualSwIn;
	UsineEventClass sharpnessFdrOut;

	AnalysisType analysisType = AnalysisType::Basic;

	// state requested by the parameters (callbacks and onInitModule) and by onSampleRateChange, whatever their
	// thread: the audio thread applies the fft size and the overlap, the analysis thread the rest
	std::atomic<int>   requestedFftSize { PROCESS_FFT_SIZE };
	std::atomic<int>   requestedOverlap { 1 };
	std::atomic<bool>  analysisOn { true };
	std::atomic<bool>  spectralOn { true };
	std::atomic<bool>  harmonicOn { true };
	std::atomic<bool>  temporalOn { true };
	std::atomic<bool>  perceptualOn { true };
	std::atomic<float> rolloffThreshold { 0.95f };
	std::atomic<float> sampleRate { static_cast<float>(sdkGetSampleRate()) };

	// audio thread only: the input, and the overlap applied to it
	AudioBuffer<float> audioBuffer { PROCESS_BUFFER_SIZE };
	int                appliedOverlap = 1;

	// the frame to analyze, given by the audio thread to the analysis thread: isProcessing is true from the
	// restart of the thread to the end of the analysis, the audio thread writes the frame only when it is false
	std::vector<float> frame = std::vector<float>(PROCESS_MAX_FFT_SIZE, 0.0f);
	int                frameSize = PROCESS_FFT_SIZE;
	std::atomic<bool>  isProcessing { false };
	TThreadPtr         thread = nullptr;

	// analysis thread only: the analysis buffers, the size and rate they are built for, the time of the last analysis
	Hanalysis                             hanalysis { PROCESS_FFT_SIZE, PROCESS_FFT_SIZE / 2, static_cast<float>(sdkGetSampleRate()), this };
	int                                   analysisFftSize    = PROCESS_FFT_SIZE;
	float                                 analysisSampleRate = static_cast<float>(sdkGetSampleRate());
	std::chrono::steady_clock::time_point lastAnalysisTime {};
	bool                                  hasLastAnalysis = false;

	void applyParameter(NativeInt callbackId);
	void onAnalysis();
	void sendEstimatedFrequency();
	void changeRolloffThreshold();
	void changeFftSize();
	void changeOverlap();

	static void processThread(void* pModule, TThreadPtr pThread);
};
