//-----------------------------------------------------------------------------
// @file    GlobalArray.cpp
// @author  BrainModular team
// @brief   Writes random values in Usine global arrays and reads them back.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <string>
#include "GlobalArray.h"
#include "ModuleDefinition.h"

namespace
{
	float randomUnit()
	{
		return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	}

	std::string randomString(int len)
	{
		static const std::string alphanum = "0123456789"
		                                    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		                                    "abcdefghijklmnopqrstuvwxyz";
		std::string result(len, ' ');
		for (char& c : result)
			c = alphanum[std::rand() % alphanum.size()];
		return result;
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new GlobalArray();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<GlobalArray*>(pModule);
}

void GlobalArray::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void GlobalArray::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(setArrInParamInfo, setArrIn);
	sdkCreateParam(getArrOutParamInfo, getArrOut);
}

void GlobalArray::onProcess()
{
	if (sdkSlowClock())
		sdkRaiseError(INFO_GA_NAMES, sdkGlobalArrayGetAllNames());

	if (!sdkRefreshSpeedClock())
		return;

	// a hash of 0 means the global array does not exist in Usine
	UINT64 hash = sdkGlobalArrayGetHash(GA_FLOAT);
	if (hash > 0)
	{
		for (int i = 0; i < GA_SIZE; i++)
			sdkGlobalArraySetValueFloat(hash, i, randomUnit(), GA_FLOAT);

		sdkRaiseError(INFO_GA_FLOAT, std::to_string(sdkGlobalArrayGetValueFloat(hash, 0, GA_FLOAT)));
	}

	hash = sdkGlobalArrayGetHash(GA_COLOR);
	if (hash > 0)
	{
		for (int i = 0; i < GA_SIZE; i++)
			sdkGlobalArraySetValueColor(hash, i, static_cast<TUsineColor>(randomUnit() * 0xFFFFFFFF), GA_COLOR);

		sdkRaiseError(INFO_GA_COLOR, std::to_string(sdkGlobalArrayGetValueColor(hash, 0, GA_COLOR)));
	}

	hash = sdkGlobalArrayGetHash(GA_STRING);
	if (hash > 0)
	{
		for (int i = 0; i < GA_SIZE; i++)
			sdkGlobalArraySetValueAnsiChar(hash, i, randomString(5).c_str(), GA_STRING);

		sdkRaiseError(INFO_GA_STRING, sdkGlobalArrayGetValueAnsiChar(hash, 0, GA_STRING));
	}

	hash = sdkGlobalArrayGetHash(GA_EVENT);
	if (hash > 0)
	{
		setArrIn.setSize(GA_SIZE);
		for (int i = 0; i < GA_SIZE; i++)
			setArrIn.setArrayData(i, randomUnit());

		sdkGlobalArraySetValueEvt(hash, setArrIn, GA_EVENT);
		sdkGlobalArrayGetValueEvt(hash, GA_EVENT, getArrOut);
	}
}
