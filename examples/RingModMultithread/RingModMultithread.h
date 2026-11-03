//-----------------------------------------------------------------------------
// @file    RingModMultithread.h
// @author  BrainModular team
// @brief   Ring modulator processing its channels in two Usine threads.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

class RingModMultithread : public UserModuleBase
{
public:
	~RingModMultithread() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

private:
	using AudioEvents = std::array<UsineEventClass, USINE_AUDIO_CHANNEL_MODULE_MAX>;

	AudioEvents     audioInputs;
	AudioEvents     audioOutputs;
	UsineEventClass modAudioIn;
	UsineEventClass mixFdrIn;

	// work buffers created with createEvent, destroyed in the destructor
	AudioEvents     bufferTemp;

	int        queryResult       = 0;
	int        numOfAudioInsOuts = 0;
	TPrecision coeffMix          = 0.5f;

	TThreadPtr          thread1         = nullptr;
	TThreadPtr          thread2         = nullptr;
	TSyncObjectPtr      syncObject      = nullptr;
	TCriticalSectionPtr criticalSection = nullptr;
	// number of threads still running in the current bloc, protected by criticalSection
	int                 cptEvents       = 0;

	// thread functions must be static, the module comes back through pModule
	static void processThreadCommon(void* pModule, TThreadPtr pThread, int start);
	static void processThread1(void* pModule, TThreadPtr pThread);
	static void processThread2(void* pModule, TThreadPtr pThread);
};
