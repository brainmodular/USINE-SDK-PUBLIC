//-----------------------------------------------------------------------------
//@file  
//	UsineObjects.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the UsineObjects class.
//
//  Example user module to show how to process audio buffers.
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
#include "UsineObjects.h"
#include <string> 

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsineObjects();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((UsineObjects*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "usine objects";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "usine objects sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

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
UsineObjects::UsineObjects()
 
{
	//
}

// destructor
UsineObjects::~UsineObjects()
{
	// 
}

void UsineObjects::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;    
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int UsineObjects::onGetNumberOfParams (int queryResult1, int queryResult2)
{
	int result = 2;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void UsineObjects::onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}


//-----------------------------------------------------------------------------
// initialisation
void UsineObjects::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void UsineObjects::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	if (ParamIndex == 0)
	{
	pParamInfo->ParamType			= ptArray;
	pParamInfo->Caption				= "array set";
	pParamInfo->IsInput				= TRUE;
	pParamInfo->IsOutput			= FALSE;
	pParamInfo->IsSeparator			= TRUE;
	pParamInfo->CallBackType		= ctNormal;
	pParamInfo->SeparatorCaption	= "";
	pParamInfo->setEventClass		(arraySet);

	}
	 else if (ParamIndex == 1)
	{
	pParamInfo->ParamType			= ptArray;
	pParamInfo->Caption				= "array get";
	pParamInfo->IsInput				= FALSE;
	pParamInfo->IsOutput			= TRUE;
	pParamInfo->CallBackType		= ctNormal;
	pParamInfo->setEventClass		(arrayGet);
	}

}


//-----------------------------------------------------------------------------
// Parameters callback
void UsineObjects::onCallBack (TUsineMessage *Message) 
{
    // filter only message specific to this user module
    if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
    {
    }
}

//-----------------------------------------------------------------------------
// random string generator
std::string gen_random(const int len) {
	static const char alphanum[] =
		"0123456789"
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz";
	std::string tmp_s;
	tmp_s.reserve(len);

	for (int i = 0; i < len; ++i) {
		tmp_s += alphanum[rand() % (sizeof(alphanum) - 1)];
	}

	return tmp_s;
}



const AnsiCharPtr GA_FLOAT = "GA-FLOAT";
const AnsiCharPtr GA_COLOR = "GA-COLOR";
const AnsiCharPtr GA_STRING = "GA-STRING";
const AnsiCharPtr GA_EVENT = "GA-EVENT";


//-----------------------------------------------------------------------------
// Parameters callback
void UsineObjects::onProcess () 
{
	
	if (sdkSlowClock())
	{
		std::string s;
		int size = 16;
		float v;
		sdkTraceChar("FIND ALL video OBJECTS:");;
		sdkTraceChar(sdkUsineObjectFind("video"));
		sdkTraceChar("OBJECTS HASH:");
		sdkTraceChar(sdkUsineObjectListHash());
		
		// float value
		v = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
		sdkUsineObjectSetFloat("usine.workspace.rack01.patch01.faderdata", v);
		v = sdkUsineObjectGetFloat("usine.workspace.rack01.patch01.faderdata");
		s = "FLOAT = " + std::to_string(v);		
		sdkTraceChar(s.c_str());
		
		// data array
		sdkUsineObjectSetLength("usine.workspace.rack01.patch01.arraydata.array-in", size);
		for (int i = 0; i < size; i++)
		{
			v = 100.0f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			sdkUsineObjectSetArray("usine.workspace.rack01.patch01.arraydata.array-in", i, v);
			v = sdkUsineObjectGetArray("usine.workspace.rack01.patch01.arraydata.array-out",i);
			s = "ARRAYDATA["+std::to_string(i)+"] = " + std::to_string(v);
			sdkTraceChar(s.c_str());
		}

		// color array
		sdkUsineObjectSetLength("usine.workspace.rack01.patch01.arraycolor.color-array", size);
		sdkUsineObjectSetInteger("usine.workspace.rack01.patch01.arraycolor.num-of-rows", size);
		for (int i = 0; i < size; i++)
		{
			v = 100.0f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			sdkUsineObjectSetArrayColor("usine.workspace.rack01.patch01.arraycolor.color-array", i, (TUsineColor)(v * 0xFFFFFFFF));
			UINT32 c = sdkUsineObjectGetArrayColor("usine.workspace.rack01.patch01.arraycolor.color-array", i);
			s = "COLORARRAY[" + std::to_string(i) + "] = " + std::to_string(c);
			sdkTraceChar(s.c_str());
		}

		// color array
		sdkUsineObjectSetLength("usine.workspace.rack01.patch01.arraycolor.color-array", size);
		sdkUsineObjectSetInteger("usine.workspace.rack01.patch01.arraycolor.num-of-rows", size);
		
		// get the length
		int l = sdkUsineObjectGetLength("usine.workspace.rack01.patch01.arraycolor.color-array");
		s = "LENGTH = "+ std::to_string(l);
		sdkTraceChar(s.c_str());

		for (int i = 0; i < size; i++)
		{
			v = 100.0f * static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
			sdkUsineObjectSetArrayColor("usine.workspace.rack01.patch01.arraycolor.color-array", i, (TUsineColor)(v * 0xFFFFFFFF));
			UINT32 c = sdkUsineObjectGetArrayColor("usine.workspace.rack01.patch01.arraycolor.color-array", i);
			s = "COLORARRAY[" + std::to_string(i) + "] = " + std::to_string(c);
			sdkTraceChar(s.c_str());
		}
		// string
		if (sdkUsineObjectExists("usine.workspace.rack01.patch01.string.comma-text"))
		{
			sdkTraceChar("string.comma-text EXISTS !!");
		}
		s = gen_random(50);
		sdkUsineObjectSetAnsiChar("usine.workspace.rack01.patch01.string.comma-text", s.c_str());
		sdkTraceChar(sdkUsineObjectGetAnsiChar("usine.workspace.rack01.patch01.string.comma-text"));

		// trigger
		sdkUsineObjectSetTrigger("usine.workspace.rack01.patch01.trigger");
	}
	if (sdkRefreshSpeedClock())
	{
		
	}


}
