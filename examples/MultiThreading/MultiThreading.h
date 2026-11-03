//-----------------------------------------------------------------------------
// @file    MultiThreading.h
// @author  BrainModular team
// @brief   Two Usine threads sharing a critical section.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class MultiThreading : public UserModuleBase
{
public:
	~MultiThreading() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

private:
	UsineEventClass arrOut;

	TThreadPtr          thread1         = nullptr;
	TThreadPtr          thread2         = nullptr;
	TCriticalSectionPtr criticalSection = nullptr;

	// thread functions must be static, the module comes back through pModule
	static void processThread1(void* pModule, TThreadPtr pThread);
	static void processThread2(void* pModule, TThreadPtr pThread);
};
