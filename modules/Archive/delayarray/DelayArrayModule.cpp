//-----------------------------------------------------------------------------
//@file  
//	DelayArrayModule.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DelayArrayModule class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
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
#include "DelayArrayModule.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Create module
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DelayArrayModule();
}

//-----------------------------------------------------------------------------
// destroy module
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((DelayArrayModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "DelayArr";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Delay Array";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
DelayArrayModule::DelayArrayModule ()
    : readPos (0)
{
    int i,j;
    for (i = 0; i < MAXBLOC; i++)
    {
	    delayBuffer[i] = 0;
	    for (j = 0; j < MAX_DELAY; j++)
        {
		    delayLine[i][j] = 0;
	    }
    }
}

//-----------------------------------------------------------------------------------------
DelayArrayModule::~DelayArrayModule()
{	
}


//void DelayArrayModule::onCreate(AnsiCharPtr optionalString);
//void DelayArrayModule::onDestroy(); 
void DelayArrayModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->Version			= MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// query system and init
//int  DelayArrayModule::onGetNumberOfParams( int queryIndex) {return 0;}
//void DelayArrayModule::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex) {}
//void DelayArrayModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) {}

//-----------------------------------------------------------------------------
// parameters and process
void DelayArrayModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
		// arrInArray
	case 0: 
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array in";
		pParamInfo->IsInput			= true;
		pParamInfo->IsOutput		= false;
		pParamInfo->ReadOnly		= true;
        pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType	= ctImmediate;
		break;

		// arrInDelay
	case 1:
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "delay in blocs";
		pParamInfo->IsInput			= true;
		pParamInfo->IsOutput		= false;
		pParamInfo->ReadOnly		= true;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= MAX_DELAY;
		break;

		// arrOutArray
	case 2: 
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array out";
		pParamInfo->IsInput			= false;
		pParamInfo->IsOutput		= true;
		pParamInfo->ReadOnly		= true;
        pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		break;

		// default case
	default:
		break;
	}
}
void DelayArrayModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
	switch (ParamIndex) 
    {
	// arrInArray
	case 0: 
		arrInArray = pEvent;
		break;

	// arrInDelay
	case 1:
		arrInDelay = pEvent;
		break;

	// arrOutArray
	case 2: 
		arrOutArray = pEvent;
		break;

	// default case
	default:
		break;
	}
}
void DelayArrayModule::onCallBack (UsineMessage *Message) 
{	
    // arrInArray
    if ((Message->wParam == 1) && (Message->lParam == MSG_CHANGE))
    {
        int i;
		int length = sdkGetEvtSize (arrInDelay);

		for (i=0; i < length; i++)
		{
			delayBuffer[i] = (long)sdkGetEvtArrayData (arrInDelay, i);

		}
		for (i = length; i < MAXBLOC; i++)
		{
			delayBuffer[i] = 0;

		}
	}
}

void DelayArrayModule::onProcess () 
{	
    int length = sdkGetEvtSize( arrInDelay );

	sdkSetEvtSize (arrOutArray, length);
	readPos = (readPos + 1) % MAX_DELAY ;
			
	for ( int i = 0; i < length; i++ )
	{
		long writePos = (readPos + delayBuffer[i]) % MAX_DELAY ;
		delayLine[i][writePos] = sdkGetEvtArrayData (arrInArray, i);
		sdkSetEvtArrayData (arrOutArray, i, delayLine[i][readPos]);
		delayLine[i][readPos] = 0;

	}
}

//-----------------------------------------------------------------------------
// midi out callbacks
//void DelayArrayModule::onMidiSendOut (int DeviceID, UsineMidiCode Code) {}
//void DelayArrayModule::onMidiSysexSendOut (int DeviceID, char* Sysex) {}
//
//-----------------------------------------------------------------------------
// chunk system
//int  DelayArrayModule::onGetChunkLen (LongBool Preset) {return 0;}
//void DelayArrayModule::onGetChunk (void* chunk, LongBool Preset) {}
//void DelayArrayModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}
//
//-----------------------------------------------------------------------------
// layout
//void DelayArrayModule::onCreateSettings() {}
//void DelayArrayModule::onSettingsHasChanged() {}
//void DelayArrayModule::onPaint () {}
//
//-----------------------------------------------------------------------------
// mouse and multi touch interaction
//void DelayArrayModule::onMouseMove(TShiftState Shift, float X, float Y) {}
//void DelayArrayModule::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
//void DelayArrayModule::onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
//void DelayArrayModule::onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
//void DelayArrayModule::onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
//void DelayArrayModule::onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y) {}
//void DelayArrayModule::onOpenEditor() {}
//void DelayArrayModule::onBringToFront() {}
//
//-----------------------------------------------------------------------------
// audio setup update
//void DelayArrayModule::onBlocSizeChange (int BlocSize) {}
//void DelayArrayModule::onSampleRateChange (double SampleRate) {}
//
//-----------------------------------------------------------------------------
// recording 
//void DelayArrayModule::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z) {}

