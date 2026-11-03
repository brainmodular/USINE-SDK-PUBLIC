//-----------------------------------------------------------------------------
// @file    BackgroundJob.cpp
// @author  BrainModular team
// @brief   Runs a long computation in a background thread with the job API.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "BackgroundJob.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new BackgroundJob();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<BackgroundJob*>(pModule);
}

void BackgroundJob::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtBackgroundJob; // the SDK adds the 'processing' and 'processing time' outputs
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void BackgroundJob::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(oneFdrInParamInfo, oneFdrIn);
	sdkCreateParam(twoFdrInParamInfo, twoFdrIn);
	sdkCreateParam(startBtnInParamInfo, startBtnIn);
	sdkCreateParam(outFdrOutParamInfo, outFdrOut);
}

void BackgroundJob::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case ONE_FDR_CBID:
			valueOne = oneFdrIn.getData();
			break;
		case TWO_FDR_CBID:
			valueTwo = twoFdrIn.getData();
			break;
		case START_BTN_CBID:
			// onJobBegin and onJobProcess run in a separate thread, onJobEnd is called by Usine once the job is done
			startJob();
			break;
		default:
			break;
	}
}

void BackgroundJob::onJobBegin()
{
	// nothing to set up: the SDK switches the 'processing' led on
}

void BackgroundJob::onJobProcess()
{
	TPrecision       ret  = 0.0f;
	const TPrecision vOne = valueOne;
	const TPrecision vTwo = valueTwo;
	for (int i = 0; i < 100000000; i++)
		ret = std::sin(vOne * std::sin(std::sin(vTwo) + ret));

	retValue = vOne * vTwo * ret;
}

void BackgroundJob::onJobEnd()
{
	outFdrOut.setData(retValue);
}
