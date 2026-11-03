//-----------------------------------------------------------------------------
// @file    ErrorHandling.cpp
// @author  BrainModular team
// @brief   Registers errors, warnings and infos, raises them with sdkRaiseError
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
	sdkCreateParam(verbosityTxtInParamInfo, verbosityTxtIn);
}

void ErrorHandling::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case ERROR_BTN_CBID:
			sdkRaiseError(ERROR_WRITE, " And this is an additional msg");
			break;
		case WARNING_BTN_CBID:
			// force_log: also writes the message in the Usine log file
			sdkRaiseError(WARNING_WRITE, true);
			break;
		case INFO_BTN_CBID:
			sdkRaiseError(INFO_WRITE);
			break;
		case VERBOSITY_TXT_CBID:
			changeVerbosityLevel();
			break;
		default:
			break;
	}
}

void ErrorHandling::changeVerbosityLevel()
{
	const std::string verbosityLevel = verbosityTxtIn.getPChar();

	if (verbosityLevel.empty())
		return;
	if (verbosityLevel == "None")
		setVerbosityLevel(Verbosity::None);
	else if (verbosityLevel == "Error")
		setVerbosityLevel(Verbosity::Error);
	else if (verbosityLevel == "Warn")
		setVerbosityLevel(Verbosity::Warn);
	else if (verbosityLevel == "Info" || verbosityLevel == "All")
		setVerbosityLevel(Verbosity::Info);
	else
		sdkRaiseError(WARNING_UNKNOWN_VERBOSITY, verbosityLevel);
}
