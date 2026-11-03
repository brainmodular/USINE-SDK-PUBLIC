//-----------------------------------------------------------------------------
// @file    ArrayDelay.cpp
// @author  BrainModular team
// @brief   Delays each element of an array by its own number of blocs.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "ArrayDelay.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new ArrayDelay();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<ArrayDelay*>(pModule);
}

void ArrayDelay::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType     = mtSimple;
	pModuleInfo->BackColor      = sdkGetUsineColor(clArrayModuleColor);
}

void ArrayDelay::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	for (auto& line : delayLine)
		line.fill(0.0f);
	delayBuffer.fill(0);
}

void ArrayDelay::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(arrInParamInfo, arrIn);
	sdkCreateParam(delayArrInParamInfo, delayArrIn);
	sdkCreateParam(arrOutParamInfo, arrOut);
}

void ArrayDelay::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == DELAY_ARR_CBID)
	{
		const int length = std::min(delayArrIn.getSize(), MAXBLOC);
		for (int i = 0; i < length; i++)
		{
			const int delay = static_cast<int>(std::lround(delayArrIn.getArrayData(i)));
			delayBuffer[i]  = std::clamp(delay, 0, MAX_DELAY - 1);
		}
	}
}

void ArrayDelay::onProcess()
{
	const int length      = std::min(arrIn.getSize(), MAXBLOC);
	const int lengthDelay = std::min(delayArrIn.getSize(), MAXBLOC);

	arrOut.setSize(length);
	readPos = (readPos + 1) % MAX_DELAY;

	for (int i = 0; i < length; i++)
	{
		const int delay    = (lengthDelay > 0) ? delayBuffer[i % lengthDelay] : 0;
		const int writePos = (readPos + delay) % MAX_DELAY;
		delayLine[writePos][i] = arrIn.getArrayData(i);
		arrOut.setArrayData(i, delayLine[readPos][i]);
	}
}
