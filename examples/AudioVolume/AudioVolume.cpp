//-----------------------------------------------------------------------------
//@file  
//	AudioVolume.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the AudioVolume class.
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
#include "AudioVolume.h"

//----------------------------------------------------------------------------
// setup a callback_id constant for all params that specify a callback type
//----------------------------------------------------------------------------
constexpr NativeInt AUDIO_VOLUME_CBID = 0x001100F0;
constexpr NativeInt AUDIO_MUTE_CBID   = 0x001100FF;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new AudioVolume();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((AudioVolume*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "audio volume";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "audio volume sdk module example";
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
AudioVolume::AudioVolume()
	: queryResult(0), numOfAudiotInsOuts(0), coeffGain(1)
{
}

// destructor
AudioVolume::~AudioVolume()
{
	// 
}

void AudioVolume::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;
    pModuleInfo->QueryListString	= sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues	= sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx= 1;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int AudioVolume::onGetNumberOfParams(int queryResult1, int queryResult2)
{
	int result = 0;
    this->queryResult = queryResult1;
	numOfAudiotInsOuts = std::min(
		sdkGetAudioQueryToNbChannels(queryResult1),
		(int)USINE_AUDIO_CHANNEL_MODULE_MAX
	);
    // we want 1 in 1 out per channels
	result = (numOfAudiotInsOuts * 2) + numOfParamAfterAudiotInOut;

    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void AudioVolume::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}


//-----------------------------------------------------------------------------
// initialisation
void AudioVolume::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void AudioVolume::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
    // audioInputs
    if (ParamIndex < numOfAudiotInsOuts)
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames( "in ", ParamIndex + 1, queryResult);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctNone;
		pParamInfo->setEventClass(audioInputs[ParamIndex]);

        if (ParamIndex == 0)
        {
            pParamInfo->IsSeparator     = TRUE;
            pParamInfo->SeparatorCaption = "audio in";
        }
    }
    // audioOutputs
    else if (ParamIndex >= numOfAudiotInsOuts && ParamIndex < (numOfAudiotInsOuts*2))
    {
		pParamInfo->ParamType		= ptAudio;
		pParamInfo->Caption			= sdkGetAudioQueryChannelNames( "out ", ParamIndex - numOfAudiotInsOuts + 1, queryResult);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->CallBackType    = ctNone;
		pParamInfo->setEventClass(audioOutputs[ParamIndex - numOfAudiotInsOuts]);

        if (ParamIndex == numOfAudiotInsOuts)
        {
            pParamInfo->IsSeparator     = TRUE;
            pParamInfo->SeparatorCaption = "audio out";
        }
    }
    // fdrGain
    else if (ParamIndex == (numOfAudiotInsOuts*2))
	{
		pParamInfo->ParamType		= ptGainFader;
		pParamInfo->Caption			= "gain";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
        pParamInfo->IsSeparator		= TRUE;
		pParamInfo->CallBackType	= ctImmediate;
		pParamInfo->CallBackId      = AUDIO_VOLUME_CBID;
        pParamInfo->SeparatorCaption = "Volume";
		pParamInfo->setEventClass(fdrGain);
    }
    // switchMute
    else if (ParamIndex == (numOfAudiotInsOuts*2) + 1)
	{
		pParamInfo->ParamType		= ptLeftLed;
		pParamInfo->Caption			= "mute";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
        pParamInfo->CallBackType    = ctNormal;
		pParamInfo->CallBackId      = AUDIO_MUTE_CBID;
		pParamInfo->setEventClass(switchMute);
    }
}


//-----------------------------------------------------------------------------
// Parameters callback
void AudioVolume::onCallBack(TUsineMessage *Message) 
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;
	if (Message->wParam == AUDIO_VOLUME_CBID || Message->wParam == AUDIO_MUTE_CBID)
	{
		coeffGain = fdrGain.getData() * (1.0f - switchMute.getData());
	}
} 

//-----------------------------------------------------------------------------
// Process
void AudioVolume::onProcess() 
{
	TPrecision gain = coeffGain;
	for (int i = 0; i < numOfAudiotInsOuts; i++)
    {
		audioOutputs[i].copyfrom(audioInputs[i]);
        audioOutputs[i].mult(gain);
    }
}