//-----------------------------------------------------------------------------
// @file    AudioVolume.cpp
// @author  BrainModular team
// @brief   Gain and mute applied to a user-chosen number of audio channels.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "AudioVolume.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new AudioVolume();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<AudioVolume*>(pModule);
}

void AudioVolume::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType          = mtSimple;
	pModuleInfo->BackColor           = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx = 1;
	pModuleInfo->CanBeSavedInPreset  = FALSE;
}

// called after the query popup, before onCreateParameters
void AudioVolume::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void AudioVolume::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	// one audio in and one audio out per channel
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		TParamInfo audioIn = audioInParamInfo;
		audioIn.Caption    = sdkGetAudioQueryChannelNames("in ", num + 1, queryResult);
		if (num == 0)
		{
			audioIn.IsSeparator      = TRUE;
			audioIn.SeparatorCaption = "audio in";
		}
		sdkCreateParam(audioIn, audioInputs[num]);
	}

	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		TParamInfo audioOut = audioOutParamInfo;
		audioOut.Caption    = sdkGetAudioQueryChannelNames("out ", num + 1, queryResult);
		if (num == 0)
		{
			audioOut.IsSeparator      = TRUE;
			audioOut.SeparatorCaption = "audio out";
		}
		sdkCreateParam(audioOut, audioOutputs[num]);
	}

	sdkCreateParam(gainFdrInParamInfo, gainFdrIn);
	sdkCreateParam(muteSwInParamInfo, muteSwIn);
}

void AudioVolume::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	// the fader is in dB: mute must zero the linear coefficient, not the dB value
	if (Message->wParam == GAIN_FDR_CBID || Message->wParam == MUTE_SW_CBID)
		coeffGain = sdkDbToCoeff(gainFdrIn.getData()) * (1.0f - muteSwIn.getData());
}

void AudioVolume::onProcess()
{
	const TPrecision coeff = coeffGain;
	for (int i = 0; i < numOfAudioInsOuts; i++)
	{
		audioOutputs[i].copyfrom(audioInputs[i]);
		audioOutputs[i].mult(coeff);
	}
}
