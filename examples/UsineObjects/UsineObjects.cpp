//-----------------------------------------------------------------------------
// @file    UsineObjects.cpp
// @author  BrainModular team
// @brief   Reads and writes Usine objects (faders, arrays, texts, triggers) by their path.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <string>
#include "UsineObjects.h"
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

	std::string indexedValue(int index, const std::string& value)
	{
		return "[" + std::to_string(index) + "] = " + value;
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsineObjects();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<UsineObjects*>(pModule);
}

void UsineObjects::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void UsineObjects::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(setArrInParamInfo, setArrIn);
	sdkCreateParam(getArrOutParamInfo, getArrOut);
}

void UsineObjects::onProcess()
{
	if (!sdkSlowClock())
		return;

	sdkRaiseError(INFO_VIDEO_OBJECTS, sdkUsineObjectFind("video"));
	sdkRaiseError(INFO_OBJECTS_HASH, sdkUsineObjectListHash());

	// a single value
	sdkUsineObjectSetFloat(PATH_FADER_DATA, randomUnit());
	sdkRaiseError(INFO_FADER_DATA, std::to_string(sdkUsineObjectGetFloat(PATH_FADER_DATA)));

	// a data array: write the input of the arraydata module, read its output
	sdkUsineObjectSetLength(PATH_ARRAY_IN, OBJECT_ARRAY_SIZE);
	for (int i = 0; i < OBJECT_ARRAY_SIZE; i++)
	{
		sdkUsineObjectSetArray(PATH_ARRAY_IN, i, 100.0f * randomUnit());
		sdkRaiseError(INFO_ARRAY_OUT, indexedValue(i, std::to_string(sdkUsineObjectGetArray(PATH_ARRAY_OUT, i))));
	}

	// a color array
	sdkUsineObjectSetLength(PATH_COLOR_ARRAY, OBJECT_ARRAY_SIZE);
	sdkUsineObjectSetInteger(PATH_COLOR_ROWS, OBJECT_ARRAY_SIZE);
	sdkRaiseError(INFO_COLOR_ARRAY_LENGTH, std::to_string(sdkUsineObjectGetLength(PATH_COLOR_ARRAY)));
	for (int i = 0; i < OBJECT_ARRAY_SIZE; i++)
	{
		sdkUsineObjectSetArrayColor(PATH_COLOR_ARRAY, i, static_cast<TUsineColor>(randomUnit() * 0xFFFFFFFF));
		sdkRaiseError(INFO_COLOR_ARRAY, indexedValue(i, std::to_string(sdkUsineObjectGetArrayColor(PATH_COLOR_ARRAY, i))));
	}

	// a text
	if (sdkUsineObjectExists(PATH_COMMA_TEXT))
		sdkRaiseError(INFO_COMMA_TEXT_EXISTS);
	sdkUsineObjectSetAnsiChar(PATH_COMMA_TEXT, randomString(50).c_str());
	sdkRaiseError(INFO_COMMA_TEXT, sdkUsineObjectGetAnsiChar(PATH_COMMA_TEXT));

	// a trigger
	sdkUsineObjectSetTrigger(PATH_TRIGGER);
}
