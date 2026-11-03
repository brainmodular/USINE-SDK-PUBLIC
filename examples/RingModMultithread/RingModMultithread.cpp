//-----------------------------------------------------------------------------
// @file    RingModMultithread.cpp
// @author  BrainModular team
// @brief   Ring modulator processing its channels in two Usine threads.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "RingModMultithread.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new RingModMultithread();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<RingModMultithread*>(pModule);
}

RingModMultithread::~RingModMultithread()
{
	if (thread1 != nullptr)
		sdkThreadDestroy(thread1);
	if (thread2 != nullptr)
		sdkThreadDestroy(thread2);
	if (syncObject != nullptr)
		sdkSyncObjectDestroy(syncObject);
	if (criticalSection != nullptr)
		sdkCriticalSectionDestroy(criticalSection);

	for (int v = 0; v < numOfAudioInsOuts; ++v)
	{
		if (bufferTemp[v] != nullptr)
			bufferTemp[v].destroyEvent();
	}
}

void RingModMultithread::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType          = mtSimple;
	pModuleInfo->BackColor           = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx = 1;
	pModuleInfo->CanBeSavedInPreset  = FALSE;
}

// called after the query popup, before onCreateParameters
void RingModMultithread::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void RingModMultithread::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	thread1         = sdkThreadCreate("thread 1", &RingModMultithread::processThread1, tpMedium, UINT32_MAX);
	thread2         = sdkThreadCreate("thread 2", &RingModMultithread::processThread2, tpMedium, UINT32_MAX);
	syncObject      = sdkSyncObjectCreate();
	criticalSection = sdkCriticalSectionCreate("critical section");

	for (int v = 0; v < numOfAudioInsOuts; ++v)
		bufferTemp[v].createEvent(sdkGetBlocSize());
}

void RingModMultithread::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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

void RingModMultithread::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == MIX_FDR_CBID)
		coeffMix = mixFdrIn.getData();
}

// thread 1 processes the even channels, thread 2 the odd ones,
// onProcess waits until both are done before returning to Usine
void RingModMultithread::onProcess()
{
	sdkCriticalSectionLock(criticalSection);
	cptEvents = 2;
	sdkCriticalSectionUnLock(criticalSection);

	sdkSyncObjectReset(syncObject);
	sdkThreadRestart(thread1);
	sdkThreadRestart(thread2);
	sdkSyncObjectWait(syncObject, 500);
}

void RingModMultithread::processThreadCommon(void* pModule, TThreadPtr pThread, int start)
{
	RingModMultithread* module = static_cast<RingModMultithread*>(pModule);
	const TPrecision    mix    = module->coeffMix;
	for (int i = start; i < module->numOfAudioInsOuts; i += 2)
	{
		module->audioOutputs[i].mult(module->audioInputs[i], module->modAudioIn);

		// Mixing
		// Dry
		module->bufferTemp[i].copyfrom(module->audioInputs[i]);
		module->bufferTemp[i].mult(1.0f - mix);
		// Wet
		module->audioOutputs[i].mult(mix);
		module->audioOutputs[i].add(module->bufferTemp[i]);
	}

	// the last thread to finish wakes up onProcess
	module->sdkCriticalSectionLock(module->criticalSection);
	module->cptEvents -= 1;
	const bool shouldSignal = (module->cptEvents <= 0);
	module->sdkCriticalSectionUnLock(module->criticalSection);
	if (shouldSignal)
		sdkSyncObjectSet(module->syncObject);
}

void RingModMultithread::processThread1(void* pModule, TThreadPtr pThread)
{
	processThreadCommon(pModule, pThread, 0);
}

void RingModMultithread::processThread2(void* pModule, TThreadPtr pThread)
{
	processThreadCommon(pModule, pThread, 1);
}
