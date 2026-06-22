//-----------------------------------------------------------------------------
//@file  
//	RingMod.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the RingMod class.
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
#include "RingMod.h"

//----------------------------------------------------------------------------
// setup a callback_id constant for all params that specify a callback type
//----------------------------------------------------------------------------
constexpr NativeInt FDR_MIX_CBID = 0x002200F0;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new RingMod();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((RingMod*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "ring mod example";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "ring mod module example";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

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
RingMod::RingMod()
    : queryResult(0), numOfAudiotInsOuts(0), coeffMix (0.5)
{
}

// destructor
RingMod::~RingMod()
{
	// 
}

void RingMod::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->QueryListString    = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues	= sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx= 1;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int RingMod::onGetNumberOfParams(int queryResult1, int queryResult2)
{
	int result = 0;
    this->queryResult = queryResult1;
    numOfAudiotInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), (int)USINE_AUDIO_CHANNEL_MODULE_MAX);

    // we want 1 in 1 out per channels
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;

    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void RingMod::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}


//-----------------------------------------------------------------------------
// initialisation
void RingMod::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {
	bufferTemp.createEvent(sdkGetBlocSize());
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void RingMod::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
    // audioInputs
    if (ParamIndex < numOfAudiotInsOuts)
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "in ", ParamIndex + 1, queryResult);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->CallBackType    = ctNone;
		pParamInfo->setEventClass	(audioInputs[ParamIndex]);

        if (ParamIndex == 0)
        {
            pParamInfo->IsSeparator			= TRUE;
            pParamInfo->SeparatorCaption	= "audio in";
        }
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames ( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryResult);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->CallBackType    = ctNone;
		pParamInfo->setEventClass	(audioOutputs[ParamIndex - numOfAudiotInsOuts]);

        if (ParamIndex == numOfAudiotInsOuts)
        {
            pParamInfo->IsSeparator			= TRUE;
            pParamInfo->SeparatorCaption	= "audio out";
        }
    }
	// mod input
	else if (ParamIndex == (numOfAudiotInsOuts * 2))
	{
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= "mod";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->CallBackType	= ctNone;
		pParamInfo->setEventClass	(modInput);

	}
	// fdrMix
	else if (ParamIndex == (numOfAudiotInsOuts * 2) + 1)
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "mix";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		= FALSE;
		pParamInfo->MinValue		= 0.0f;
		pParamInfo->MaxValue		= 1.0f;
		pParamInfo->DefaultValue	= 0.5f;
		pParamInfo->CallBackType	= ctNormal;
		pParamInfo->setEventClass	(fdrMix);
		pParamInfo->CallBackId		= FDR_MIX_CBID;
	}
}


//-----------------------------------------------------------------------------
// Parameters callback
void RingMod::onCallBack(TUsineMessage *Message) 
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;
 
	if (Message->wParam == FDR_MIX_CBID)
	{
		coeffMix = fdrMix.getData();
	}
} 

void RingMod::onProcess() 
{
	const TPrecision mix = coeffMix;
	for (int i = 0; i < numOfAudiotInsOuts; i++)
    {
		// Multiplying
   		sdkMultEvt3(audioInputs[i], modInput, audioOutputs[i]);
		
		// Mixing 
		// Dry
		bufferTemp.copyfrom(audioInputs[i]);
		bufferTemp.mult(1.0f - mix);
		// Wet
		audioOutputs[i].mult(mix);
		// Adding 
   		audioOutputs[i].add(bufferTemp);
    }   
}
