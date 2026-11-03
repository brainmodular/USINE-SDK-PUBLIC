//-----------------------------------------------------------------------------
// @file    UsinePointers.cpp
// @author  BrainModular team
// @brief   Copies the data of a pointer input to a pointer output, shows typed pointers.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "UsinePointers.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsinePointers();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<UsinePointers*>(pModule);
}

void UsinePointers::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType     = mtSimple;
	pModuleInfo->BackColor      = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->DontProcess    = TRUE;
}

void UsinePointers::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	copyPointer();
}

void UsinePointers::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(ptrInParamInfo, ptrIn);
	sdkCreateParam(ptrOutParamInfo, ptrOut);
}

void UsinePointers::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == PTR_IN_CBID)
		copyPointer();
}

void UsinePointers::copyPointer()
{
	// a pointer input accepts any pointer type, check it before reading the data
	if (ptrIn.getPointerDataType() != usinePtrFloat)
	{
		sdkRaiseError(ERROR_POINTER_NOT_FLOAT);
		return;
	}

	// sizes are in bytes
	const UINT32 size = ptrIn.getPointerDataSize();
	ptrOut.setPointerDataSize(size);
	if (size != 0)
		std::memcpy(ptrOut.getPointerData(), ptrIn.getPointerData(), size);

	// tells Usine that the data has changed
	ptrOut.setPointerDataChanged();
}
