//-----------------------------------------------------------------------------
// @file    RingMod.cpp
// @author  BrainModular team
// @brief   Multiplies audio inputs by a modulator, shows how to process audio buffers.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "RingMod.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new RingMod();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<RingMod*>(pModule);
}

RingMod::~RingMod()
{
	if (bufferTemp != nullptr)
		bufferTemp.destroyEvent();
}

void RingMod::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType          = mtSimple;
	pModuleInfo->BackColor           = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx = 1;
	pModuleInfo->CanBeSavedInPreset  = FALSE;
}

// called after the query popup, before onCreateParameters
void RingMod::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void RingMod::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	bufferTemp.createEvent(sdkGetBlocSize());
}

void RingMod::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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

	sdkCreateParam(modAudioInParamInfo, modAudioIn);
	sdkCreateParam(mixFdrInParamInfo, mixFdrIn);
}

void RingMod::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == MIX_FDR_CBID)
		coeffMix = mixFdrIn.getData();
}

void RingMod::onProcess()
{
	const TPrecision mix = coeffMix;
	for (int i = 0; i < numOfAudioInsOuts; i++)
	{
		audioOutputs[i].mult(audioInputs[i], modAudioIn);

		// Mixing
		// Dry
		bufferTemp.copyfrom(audioInputs[i]);
		bufferTemp.mult(1.0f - mix);
		// Wet
		audioOutputs[i].mult(mix);
		audioOutputs[i].add(bufferTemp);
	}
}
