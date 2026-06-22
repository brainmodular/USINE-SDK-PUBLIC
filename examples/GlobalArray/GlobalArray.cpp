//-----------------------------------------------------------------------------
//@file  
//	GlobalArray.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the GlobalArray class.
//
//  Example user module to show how to process global array.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
//  support@brainmodular.com 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 BrainModular
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "GlobalArray.h"
#include <string>

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new GlobalArray();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((GlobalArray*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "global array";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "global array sdk module example";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr AnsiCharPtr GA_FLOAT = "GA-FLOAT";
constexpr AnsiCharPtr GA_COLOR = "GA-COLOR";
constexpr AnsiCharPtr GA_STRING = "GA-STRING";
constexpr AnsiCharPtr GA_EVENT = "GA-EVENT";


// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
GlobalArray::GlobalArray() : rng(std::random_device{}()), fdist(0.0f, 1.0f)
{
	//
}

// destructor
GlobalArray::~GlobalArray()
{
	// 
}

void GlobalArray::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;  
	pModuleInfo->CanBeSavedInPreset = FALSE;
	pModuleInfo->NumberOfParams     = 2;
}


//-----------------------------------------------------------------------------
// Called after the query popup
//void GlobalArray::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
//{
//}


//-----------------------------------------------------------------------------
// initialisation
void GlobalArray::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void GlobalArray::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
	if (ParamIndex == 0)
	{
		pParamInfo->ParamType           = ptArray;
		pParamInfo->Caption             = "array set";
		pParamInfo->IsInput             = TRUE;
		pParamInfo->IsOutput            = FALSE;
		pParamInfo->IsSeparator         = TRUE;
		pParamInfo->CallBackType        = ctNone;
		pParamInfo->SeparatorCaption    = "";
		pParamInfo->setEventClass       (arraySet);
	}
	else if (ParamIndex == 1)
	{
		pParamInfo->ParamType           = ptArray;
		pParamInfo->Caption             = "array get";
		pParamInfo->IsInput             = FALSE;
		pParamInfo->IsOutput            = TRUE;
		pParamInfo->CallBackType        = ctNone;
		pParamInfo->setEventClass       (arrayGet);
	}
}


//-----------------------------------------------------------------------------
// Parameters callback
//void GlobalArray::onCallBack(TUsineMessage *Message) 
//{
//    // filter only message specific to this user module
//    if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
//    {
//    }
//}

//-----------------------------------------------------------------------------
// random string generator
static std::string gen_random(const int len, std::mt19937 seed) {
	static const std::string alphanum = {
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
	};
    static std::uniform_int_distribution<size_t> dist(0, alphanum.size() - 1);
	std::string tmp_s;
	tmp_s.resize(len);

	for (char& c : tmp_s) {
		c = alphanum[dist(seed)];
	}
	return tmp_s;
}


//-----------------------------------------------------------------------------
// Process
void GlobalArray::onProcess() 
{
	if (sdkSlowClock())
	{
		sdkTraceChar(sdkGlobalArrayGetAllNames());		
	}
	if (sdkRefreshSpeedClock())
	{
		UINT64 hash;
		float v;
		std::string s;
		TUsineColor c;
		

		// Global Array Float
		hash = sdkGlobalArrayGetHash(GA_FLOAT);
		if (hash > 0)
		{
			// set
			for (int i = 0; i < 16; i++)
			{
				v = fdist(rng);
				sdkGlobalArraySetValueFloat(hash, i, v, GA_FLOAT);
			}
			// get
			v = sdkGlobalArrayGetValueFloat(hash, 0, GA_FLOAT);
			s = "FLOAT = " + std::to_string(v);
			sdkTraceChar(const_cast<AnsiCharPtr>(s.c_str()));
		}

		// Global Array Color
		hash = sdkGlobalArrayGetHash(GA_COLOR);
		if (hash > 0)
		{
			// set
			for (int i = 0; i < 16; i++)
			{
				float r = fdist(rng);
				c = static_cast<TUsineColor>(r * 0xFFFFFFFF);
				sdkGlobalArraySetValueColor(hash, i, c, GA_COLOR);
			}
			// get
			c = sdkGlobalArrayGetValueColor(hash, 0, GA_COLOR);
			s = "COLOR = " + std::to_string(c);
			sdkTraceChar(const_cast<AnsiCharPtr>(s.c_str()));
		}

		// Global Array String
		hash = sdkGlobalArrayGetHash(GA_STRING);
		if (hash > 0)
		{
			// set
			for (int i = 0; i < 16; i++)
			{
				s = gen_random(5, rng);
				sdkGlobalArraySetValueAnsiChar(hash, i, const_cast<AnsiCharPtr>(s.c_str()), GA_STRING);
			}
			// get
			sdkTraceChar(sdkGlobalArrayGetValueAnsiChar(hash, 0, GA_STRING));
		}

		// Global Array EVENT
		hash = sdkGlobalArrayGetHash(GA_EVENT);
		if (hash > 0)
		{
			// set
			arraySet.setSize(16);
			for (int i = 0; i < 16; i++)
			{
				v = fdist(rng);
				arraySet.setArrayData(i, v);
			}
			sdkGlobalArraySetValueEvt(hash, arraySet, GA_EVENT);
			// get
			sdkGlobalArrayGetValueEvt(hash, GA_EVENT, arrayGet);
		}
	}
}
