//-----------------------------------------------------------------------------
// @file    Reverb.cpp
// @author  BrainModular team
// @brief   Multichannel reverb based on the Freeverb model, with cross-channel feeds.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <span>
#include "Reverb.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new Reverb();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<Reverb*>(pModule);
}

Reverb::~Reverb()
{
	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		if (temp[v] != nullptr)
			temp[v].destroyEvent();
		if (processInput[v] != nullptr)
			processInput[v].destroyEvent();
		if (processOutput[v] != nullptr)
			processOutput[v].destroyEvent();
	}
}

void Reverb::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanBeSavedInPreset = FALSE;

	// the audio query functions need Usine, pMasterInfo is null when the module is only listed in the browser
	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
		pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
		pModuleInfo->QueryListDefaultIdx = 1;
	}
}

// called after the query popup, before onCreateParameters
void Reverb::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void Reverb::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		temp[v].createEvent(1);
		processInput[v].createEvent(1);
		processOutput[v].createEvent(1);
	}

	reverbInit();
	mode               = INITIAL_MODE;
	roomsize           = INITIAL_ROOM / 100.0f;
	damp               = INITIAL_DAMP / 100.0f;
	channelsSeparation = INITIAL_CHANNELS_SEPARATION / 100.0f;
	wet                = sdkDbToCoeff(INITIAL_WET);
	dry                = sdkDbToCoeff(INITIAL_DRY);
	reverbParamsUpdate();
}

void Reverb::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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

	sdkCreateParam(modeSwInParamInfo, modeSwIn);
	sdkCreateParam(roomSizeFdrInParamInfo, roomSizeFdrIn);
	sdkCreateParam(dampingFdrInParamInfo, dampingFdrIn);
	sdkCreateParam(channelsSeparationFdrInParamInfo, channelsSeparationFdrIn);
	sdkCreateParam(wetLevelFdrInParamInfo, wetLevelFdrIn);
	sdkCreateParam(dryLevelFdrInParamInfo, dryLevelFdrIn);
}

void Reverb::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case MODE_SW_CBID:
			mode = modeSwIn.getData();
			break;
		case ROOM_SIZE_FDR_CBID:
			roomsize = roomSizeFdrIn.getData() / 100.0f;
			break;
		case DAMPING_FDR_CBID:
			damp = dampingFdrIn.getData() / 100.0f;
			break;
		case CHANNELS_SEPARATION_FDR_CBID:
			channelsSeparation = channelsSeparationFdrIn.getData() / 100.0f;
			break;
		case WET_LEVEL_FDR_CBID:
			wet = sdkDbToCoeff(wetLevelFdrIn.getData());
			break;
		case DRY_LEVEL_FDR_CBID:
			dry = sdkDbToCoeff(dryLevelFdrIn.getData());
			break;
		default:
			return;
	}
	reverbParamsUpdate();
}

void Reverb::onProcess()
{
	const int sampleFrames = sdkGetBlocSize();
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		if (audioInputs[num].getSize() != sampleFrames)
			audioInputs[num].clearAudio();
		if (audioOutputs[num].getSize() != sampleFrames)
			audioOutputs[num].clearAudio();
	}

	currentwet1 = currentwet1 * smooth + (1.0f - smooth) * wet1;
	currentwet2 = currentwet2 * smooth + (1.0f - smooth) * wet2;
	currentdry  = currentdry * smooth + (1.0f - smooth) * dry;

	const float coeffwet1 = currentwet1 * gain;
	const float coeffwet2 = (numOfAudioInsOuts > 2) ? currentwet2 * gain / static_cast<float>(numOfAudioInsOuts - 1)
	                                                : currentwet2 * gain;

	// reverb input of each channel, and dry part of the outputs
	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		processInput[v].copyfrom(audioInputs[v]);
		processInput[v].mult(coeffwet1);
		processOutput[v].clearAudio();

		if (currentdry != 0)
		{
			audioOutputs[v].copyfrom(audioInputs[v]);
			audioOutputs[v].mult(currentdry);
		}
		else
		{
			audioOutputs[v].clearAudio();
		}
	}

	// each channel also feeds the reverb of the other channels
	if (coeffwet2 != 0)
	{
		for (int v = 0; v < numOfAudioInsOuts; ++v)
		{
			temp[v].copyfrom(audioInputs[v]);
			temp[v].mult(coeffwet2);
		}
		for (int v = 0; v < numOfAudioInsOuts; ++v)
		{
			for (int n = 0; n < numOfAudioInsOuts; ++n)
			{
				if (v != n)
					processInput[v].add(temp[n]);
			}
		}
	}

	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		const TPrecision* in  = processInput[v].getDataAddr();
		TPrecision*       out = processOutput[v].getDataAddr();
		if (in == nullptr || out == nullptr)
			continue;

		const std::span<comb>    combs     = std::span(combvoices).subspan(v * numcombs, numcombs);
		const std::span<allpass> allpasses = std::span(allpassvoices).subspan(v * numallpasses, numallpasses);
		for (int s = 0; s < sampleFrames; ++s)
		{
			// combs in parallel, then allpasses in series
			for (comb& filter : combs)
				out[s] += filter.process(in[s]);
			for (allpass& filter : allpasses)
				out[s] = filter.process(out[s]);
		}
	}

	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		processOutput[v].mult(currentwet1);
		audioOutputs[v].add(processOutput[v]);
	}

	if (currentwet2 != 0)
	{
		for (int v = 0; v < numOfAudioInsOuts; ++v)
		{
			temp[v].copyfrom(audioOutputs[v]);
			temp[v].mult(currentwet2);
		}
		for (int v = 0; v < numOfAudioInsOuts; ++v)
		{
			for (int n = 0; n < numOfAudioInsOuts; n++)
			{
				if (v != n)
					audioOutputs[v].add(temp[n]);
			}
		}
	}
}

void Reverb::reverbInit()
{
	currentwet1 = 0.0f;
	currentwet2 = 0.0f;
	currentdry  = 0.0f;

	const int numCombVoices    = numcombs * numOfAudioInsOuts;
	const int numAllpassVoices = numallpasses * numOfAudioInsOuts;
	combvoices.assign(numCombVoices, comb());
	allpassvoices.assign(numAllpassVoices, allpass());
	bufcombvoices.assign(numCombVoices, {});
	bufallpassvoices.assign(numAllpassVoices, {});

	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		// each channel gets slightly longer buffers to decorrelate the channels
		const int spread = static_cast<int>((static_cast<float>(stereospread) / static_cast<float>(numOfAudioInsOuts)) * v);

		for (int c = 0; c < numcombs; ++c)
		{
			const int index = v * numcombs + c;
			bufcombvoices[index].assign(combtuning[c] + spread, 0.0f);
			combvoices[index].setbuffer(bufcombvoices[index]);
			combvoices[index].mute();
		}

		for (int a = 0; a < numallpasses; ++a)
		{
			const int index = v * numallpasses + a;
			bufallpassvoices[index].assign(allpasstuning[a] + spread, 0.0f);
			allpassvoices[index].setbuffer(bufallpassvoices[index]);
			allpassvoices[index].setfeedback(0.5f);
			allpassvoices[index].mute();
		}
	}
}

void Reverb::reverbParamsUpdate()
{
	wet1 = wet * (channelsSeparation / 2.0f + 0.5f);
	wet2 = wet * ((1.0f - channelsSeparation) / 2.0f);

	if (mode >= freezemode)
	{
		roomsize1 = 1.0f;
		damp1     = 0.0f;
		gain      = muted;
	}
	else
	{
		roomsize1 = roomsize;
		damp1     = damp;
		gain      = fixedgain;
	}

	for (comb& voice : combvoices)
	{
		voice.setfeedback(roomsize1);
		voice.setdamp(damp1);
	}
}
