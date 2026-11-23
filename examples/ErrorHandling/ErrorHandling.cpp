//-----------------------------------------------------------------------------
// @file    ErrorHandling.cpp
// @author  BrainModular team
// @brief   Registers errors, warnings and infos, raises them with sdkReport
//          and filters them with a verbosity level.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "ErrorHandling.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new ErrorHandling();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<ErrorHandling*>(pModule);
}

void ErrorHandling::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void ErrorHandling::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(errorBtnInParamInfo, errorBtnIn);
	sdkCreateParam(warningBtnInParamInfo, warningBtnIn);
	sdkCreateParam(infoBtnInParamInfo, infoBtnIn);
}

void ErrorHandling::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case ERROR_BTN_CBID:
			sdkReport(ERROR_WRITE, " And this is an additional msg");
			break;
		case WARNING_BTN_CBID:
			// forceLog: also writes the message in the Usine log file
			sdkReport(WARNING_WRITE, true);
			break;
		case INFO_BTN_CBID:
			sdkReport(INFO_WRITE);
			break;
		default:
			break;
	}
}
