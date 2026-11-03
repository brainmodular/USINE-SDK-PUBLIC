//-----------------------------------------------------------------------------
// @file    DataMultiply.cpp
// @author  BrainModular team
// @brief   Multiplies two data inputs, shows how to update outputs when inputs change.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DataMultiply.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataMultiply();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DataMultiply*>(pModule);
}

void DataMultiply::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void DataMultiply::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(aFdrInParamInfo, aFdrIn);
	sdkCreateParam(bFdrInParamInfo, bFdrIn);
	sdkCreateParam(resultFdrOutParamInfo, resultFdrOut);
}

void DataMultiply::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == INPUT_FDR_CBID)
		resultFdrOut.mult(aFdrIn, bFdrIn);
}
