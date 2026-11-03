//-----------------------------------------------------------------------------
// @file    MultiThreading.cpp
// @author  BrainModular team
// @brief   Two Usine threads sharing a critical section.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "MultiThreading.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MultiThreading();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<MultiThreading*>(pModule);
}

MultiThreading::~MultiThreading()
{
	if (thread1 != nullptr)
		sdkThreadDestroy(thread1);
	if (thread2 != nullptr)
		sdkThreadDestroy(thread2);
	if (criticalSection != nullptr)
		sdkCriticalSectionDestroy(criticalSection);
}

void MultiThreading::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void MultiThreading::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	criticalSection = sdkCriticalSectionCreate("critical section", true);
	thread1         = sdkThreadCreate("thread 1", &MultiThreading::processThread1, tpMedium, 100);
	thread2         = sdkThreadCreate("thread 2", &MultiThreading::processThread2, tpMedium, 100);
}

void MultiThreading::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(arrOutParamInfo, arrOut);
}

void MultiThreading::processThread1(void* pModule, TThreadPtr pThread)
{
	MultiThreading* module = static_cast<MultiThreading*>(pModule);
	module->sdkCriticalSectionLock(module->criticalSection);
	sdkRaiseError(INFO_THREAD_1);
	module->sdkCriticalSectionUnLock(module->criticalSection);
}

void MultiThreading::processThread2(void* pModule, TThreadPtr pThread)
{
	MultiThreading* module = static_cast<MultiThreading*>(pModule);
	module->sdkCriticalSectionLock(module->criticalSection);
	sdkRaiseError(INFO_THREAD_2);
	module->sdkCriticalSectionUnLock(module->criticalSection);
}
