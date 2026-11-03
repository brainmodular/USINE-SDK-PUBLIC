//-----------------------------------------------------------------------------
// @file    Granulator.cpp
// @author  BrainModular team
// @brief   Granular synthesis on live audio inputs, with automation recording.
//          Inspired from the granulator of the Fragmental VST plugin by Niall Moody.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "Granulator.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new Granulator();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<Granulator*>(pModule);
}

void Granulator::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->DefaultWidth       = 100;
	pModuleInfo->DefaultHeight      = 100;
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanRecord          = TRUE;
	pModuleInfo->CanBeSavedInPreset = TRUE;

	// the audio query functions need Usine, pMasterInfo is null when the module is only listed in the browser
	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
		pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
		pModuleInfo->QueryListDefaultIdx = 1;
	}
}

// called after the query popup, before onCreateParameters
void Granulator::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void Granulator::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	lineColor = sdkGetUsineColor(clAudioFlow);

	for (size_t type = 0; type < envelopePoints.size(); type++)
		envelopePoints[type].assign(ENVELOPE_GRAPH_POINTS[type], TPointF {});

	grains.clear();
	grains.resize(numOfAudioInsOuts);

	grainIntervalMs = grainIntervalFdrIn.getData();
	grainDurationMs = grainDurationFdrIn.getData();
	// allocates the delay lines and converts the durations in samples
	onSampleRateChange(sdkGetSampleRate());
	changeDuration();
	changePitch();
	changeEnvelopeVar();
}

void Granulator::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		TParamInfo audioIn = audioInParamInfo;
		audioIn.Caption    = sdkGetAudioQueryChannelNames("in ", num + 1, queryResult);
		sdkCreateParam(audioIn, audioInputs[num]);
	}

	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		TParamInfo audioOut = audioOutParamInfo;
		audioOut.Caption    = sdkGetAudioQueryChannelNames("out ", num + 1, queryResult);
		sdkCreateParam(audioOut, audioOutputs[num]);
	}

	sdkCreateParam(grainStartSwOutParamInfo, grainStartSwOut);
	sdkCreateParam(grainIntervalFdrInParamInfo, grainIntervalFdrIn);
	sdkCreateParam(grainEnvelopeLboxInParamInfo, grainEnvelopeLboxIn);
	sdkCreateParam(grainEnvVarFdrInParamInfo, grainEnvVarFdrIn);
	sdkCreateParam(grainDurationFdrInParamInfo, grainDurationFdrIn);
	sdkCreateParam(grainPitchFdrInParamInfo, grainPitchFdrIn);
	sdkCreateParam(grainModeLboxInParamInfo, grainModeLboxIn);
	sdkCreateParam(grainGoBtnInParamInfo, grainGoBtnIn);
}

void Granulator::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case GRAIN_INTERVAL_FDR_CBID:
			changeInterval();
			break;
		case GRAIN_ENVELOPE_LBOX_CBID:
			changeGrainEnvelope();
			break;
		case GRAIN_ENV_VAR_FDR_CBID:
			changeEnvelopeVar();
			break;
		case GRAIN_DURATION_FDR_CBID:
			changeDuration();
			break;
		case GRAIN_PITCH_FDR_CBID:
			changePitch();
			break;
		case GRAIN_MODE_LBOX_CBID:
			changeGrainMode();
			break;
		case GRAIN_GO_BTN_CBID:
			wantNewGrain = true;
			break;
		default:
			break;
	}
}

void Granulator::onProcess()
{
	if (cbufSize == 0)
		return;

	// the trigger led stays on during one bloc
	if (grainStartSwOut.getData() == 1.0f)
		grainStartSwOut.setData(0.0f);

	const int blockSize = sdkGetBlocSize();
	std::array<TPrecision*, USINE_AUDIO_CHANNEL_MODULE_MAX> inBlock {};
	std::array<TPrecision*, USINE_AUDIO_CHANNEL_MODULE_MAX> outBlock {};
	ChannelFlags                                            isConnected {};
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		audioOutputs[num].setSize(blockSize);
		inBlock[num]     = audioInputs[num].getDataAddr();
		outBlock[num]    = audioOutputs[num].getDataAddr();
		isConnected[num] = (audioInputs[num].getSize() == blockSize);
	}

	for (int i = 0; i < blockSize; i++)
	{
		if (currentGrainMode == GrainMode::Interval)
		{
			--audioLineNextOnset;
			if (audioLineNextOnset < 0)
			{
				audioLineNextOnset = grainSize + grainInterval - 1;
				wantNewGrain       = true;
			}
		}

		// in the Play Button mode, a grain can only start on the first sample of a bloc
		if (wantNewGrain && (currentGrainMode == GrainMode::Interval || i == 0))
			startNewGrain(isConnected);
		wantNewGrain = false;

		for (int num = 0; num < numOfAudioInsOuts; num++)
		{
			outBlock[num][i] = 0.0f;
			if (!isConnected[num])
				continue;

			circularBuffer[num][cbufReadPos] = inBlock[num][i];
			for (Grain& grain : grains[num])
				grain.addSampleTo(outBlock[num][i]);
		}

		cbufReadPos = (cbufReadPos + 1) % cbufSize;
	}
}

void Granulator::onCreateSettings()
{
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "line", 0);
	sdkAddSettingLineSingle(DESIGN_TAB_NAME, &lineThickness, "line width", 0, 100, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 2.0f);
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &lineColor, "line color");
}

void Granulator::onSettingsHasChanged()
{
	sdkRepaintPanel();
}

void Granulator::onPaint()
{
	std::vector<TPointF>& points = currentEnvelopePoints();
	if (!points.empty())
		sdkDrawPolyLine(points.data(), static_cast<int>(points.size()), lineColor, lineThickness);

	sdkFillText(dimPitch, (caPitch + " ht").c_str(), sdkGetUsineColor(clFontLight), 0.12f);
	sdkFillText(dimGrainDuration, (caGrainDuration + " ms").c_str(), sdkGetUsineColor(clFontLight), 0.12f);
}

void Granulator::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton == mbLeft)
	{
		isRecording         = true;
		mouseLeftButtonDown = true;
	}

	mouseLastCoords = { .x = X, .y = Y };
}

// dragging in the panel edits the grain and records the moves as an automation
void Granulator::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (isRecording && mouseLeftButtonDown)
	{
		float pitch = grainPitchFdrIn.getData();
		float dur   = grainDurationFdrIn.getData();

		if (Shift & ssShift)
		{
			// Shift + vertical move changes the pitch
			pitch = std::clamp(pitch - (Y - mouseLastCoords.y) * 5, GRAIN_PITCH_MIN, GRAIN_PITCH_MAX);
			grainPitchFdrIn.setData(pitch);
			changePitch();
		}
		else
		{
			// vertical move changes the envelope variable
			grainEnvelopeVar = std::clamp(grainEnvelopeVar - (Y - mouseLastCoords.y), 0.0f, 1.0f);
			grainEnvVarFdrIn.setData(grainEnvelopeVar);
			changeEnvelopeVar();
		}

		// horizontal move changes the grain duration
		dur = std::clamp(dur + (X - mouseLastCoords.x) * 10, GRAIN_DUR_MIN, GRAIN_DUR_MAX);
		grainDurationFdrIn.setData(dur);
		changeDuration();

		try
		{
			sdkProcessRecordAutomation(dur, grainEnvelopeVar, pitch);
		}
		catch (std::exception&)
		{
			sdkRaiseError(ERROR_RECORD_AUTOMATION);
		}
	}

	mouseLastCoords = { .x = X, .y = Y };
}

void Granulator::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (isRecording)
	{
		isRecording = false;
		try
		{
			sdkStopRecordAutomation();
		}
		catch (std::exception&)
		{
			sdkRaiseError(ERROR_STOP_RECORD_AUTOMATION);
		}
	}

	if (MouseButton == mbLeft)
		mouseLeftButtonDown = false;
}

void Granulator::onSampleRateChange(double SampleRate)
{
	usineSampleRate = static_cast<float>(SampleRate);
	// room for the longest grain (2 s, with a margin) read at the highest pitch (x16)
	cbufSize    = static_cast<int>(2.1 * 16 * usineSampleRate);
	cbufReadPos = 0;

	for (size_t num = 0; num < grains.size(); num++)
	{
		circularBuffer[num].assign(cbufSize, 0.0f);
		for (Grain& grain : grains[num])
			grain.setInputBuffer(circularBuffer[num]);
	}

	grainInterval = msToSamples(grainIntervalMs);
	grainSize     = msToSamples(grainDurationMs);
}

// replays the values recorded in onMouseMove: X = grain duration, Y = envelope variable, Z = pitch
void Granulator::onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z)
{
	grainDurationFdrIn.setData(std::clamp(X, GRAIN_DUR_MIN, GRAIN_DUR_MAX));
	changeDuration();

	grainEnvelopeVar = std::clamp(Y, 0.0f, 1.0f);
	grainEnvVarFdrIn.setData(grainEnvelopeVar);
	changeEnvelopeVar();

	grainPitchFdrIn.setData(std::clamp(Z, GRAIN_PITCH_MIN, GRAIN_PITCH_MAX));
	changePitch();
}

void Granulator::changeGrainEnvelope()
{
	const int item      = static_cast<int>(grainEnvelopeLboxIn.getData());
	currentEnvelopeType = static_cast<EnvelopeType>(std::clamp(item, 0, ENVELOPE_TYPE_COUNT - 1));
	changeEnvelopeVar();
}

void Granulator::changeEnvelopeVar()
{
	grainEnvelopeVar = grainEnvVarFdrIn.getData();

	// the curves are allocated in onInitModule
	std::vector<TPointF>& points = currentEnvelopePoints();
	if (!points.empty())
		Grain::getGraphPoints(currentEnvelopeType, grainEnvelopeVar, points);
	sdkRepaintPanel();
}

void Granulator::changeInterval()
{
	grainIntervalMs = grainIntervalFdrIn.getData();
	grainInterval   = msToSamples(grainIntervalMs);
}

void Granulator::changeDuration()
{
	grainDurationMs = grainDurationFdrIn.getData();
	grainSize       = msToSamples(grainDurationMs);
	caGrainDuration = sdkFloatToString(grainDurationMs, 2, 8);
	sdkRepaintPanel();
}

void Granulator::changePitch()
{
	const float pitch = grainPitchFdrIn.getData();
	pitchIncrement    = std::pow(2.0f, pitch / 12.0f);
	caPitch           = sdkFloatToString(pitch, 0, 8);
	sdkRepaintPanel();
}

void Granulator::changeGrainMode()
{
	currentGrainMode = (grainModeLboxIn.getData() >= 1.0f) ? GrainMode::PlayButton : GrainMode::Interval;
}

void Granulator::startNewGrain(const ChannelFlags& isConnected)
{
	bool started = false;
	bool overlap = false;
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		if (!isConnected[num])
			continue;

		Grain& grain = grains[num][nextNewGrainIndex];
		overlap = overlap || grain.getIsActive();
		grain.setEnvelope(currentEnvelopeType);
		grain.activate(grainEnvelopeVar, cbufReadPos, grainSize, pitchIncrement);
		started = true;
	}

	if (!started)
		return;

	// warns once when grains start to overlap, not for every new grain
	if (overlap && !grainOverlapWarned)
		sdkRaiseError(WARNING_GRAIN_OVERLAP);
	grainOverlapWarned = overlap;

	nextNewGrainIndex = (nextNewGrainIndex + 1) % NUM_OF_GRAIN;
	grainStartSwOut.setData(1.0f);
}

int Granulator::msToSamples(float ms) const
{
	return static_cast<int>(std::floor(ms * usineSampleRate * 0.001f + 0.5f));
}

std::vector<TPointF>& Granulator::currentEnvelopePoints()
{
	return envelopePoints[static_cast<size_t>(currentEnvelopeType)];
}
