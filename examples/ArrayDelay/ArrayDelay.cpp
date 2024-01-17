//-----------------------------------------------------------------------------
//@file  
//	ArrayDelay.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the ArrayDelay class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
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
#include "ArrayDelay.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Create module
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new ArrayDelay();
}

//-----------------------------------------------------------------------------
// destroy module
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((ArrayDelay*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "array delay";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "array delay SDK example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
ArrayDelay::ArrayDelay ()
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
ArrayDelay::~ArrayDelay()
{	
}


//void ArrayDelay::onCreate(AnsiCharPtr optionalString);
//void ArrayDelay::onDestroy(); 
void ArrayDelay::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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
//int  ArrayDelay::onGetNumberOfParams( int queryResult1, int queryResult2) {return 0;}
//void ArrayDelay::onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) {}
//void ArrayDelay::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {}

//-----------------------------------------------------------------------------
// parameters and process
void ArrayDelay::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
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
		pParamInfo->setEventClass     (arrInArray);
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
		pParamInfo->setEventClass     (arrInDelay);
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
		pParamInfo->setEventClass     (arrOutArray);
		break;

		// default case
	default:
		break;
	}
}

void ArrayDelay::onCallBack (TUsineMessage *Message) 
{	
    // arrInArray
    if ((Message->wParam == 1) && (Message->lParam == MSG_CHANGE))
    {
        int i;
		int length = arrInDelay.getSize();

		for (i=0; i < length; i++)
		{
			delayBuffer[i] = (long)arrInDelay.getArrayData(i);

		}
		for (i = length; i < MAXBLOC; i++)
		{
			delayBuffer[i] = 0;

		}
	}
}

void ArrayDelay::onProcess () 
{	
    int length = arrInArray.getSize();

	arrOutArray.setSize(length);
	readPos = (readPos + 1) % MAX_DELAY ;
			
	for ( int i = 0; i < length; i++ )
	{
		long writePos = (readPos + delayBuffer[i]) % MAX_DELAY ;
		delayLine[i][writePos] = arrInArray.getArrayData(i);
		arrOutArray.setArrayData(i, delayLine[i][readPos]);
		delayLine[i][readPos] = 0;

	}
}

//-----------------------------------------------------------------------------
// midi out callbacks
//void ArrayDelay::onMidiSendOut (int DeviceID, UsineMidiCode Code) {}
//void ArrayDelay::onMidiSysexSendOut (int DeviceID, char* Sysex) {}
//
//-----------------------------------------------------------------------------
// chunk system
//int  ArrayDelay::onGetChunkLen (LongBool Preset) {return 0;}
//void ArrayDelay::onGetChunk (void* chunk, LongBool Preset) {}
//void ArrayDelay::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}
//
//-----------------------------------------------------------------------------
// layout
//void ArrayDelay::onCreateSettings() {}
//void ArrayDelay::onSettingsHasChanged() {}
//void ArrayDelay::onPaint () {}
//
//-----------------------------------------------------------------------------
// mouse and multi touch interaction
//void ArrayDelay::onMouseMove(TShiftState Shift, float X, float Y) {}
//void ArrayDelay::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
//void ArrayDelay::onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
//void ArrayDelay::onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
//void ArrayDelay::onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
//void ArrayDelay::onMouseUpMulti (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y) {}
//void ArrayDelay::onOpenEditor() {}
//void ArrayDelay::onBringToFront() {}
//
//-----------------------------------------------------------------------------
// audio setup update
//void ArrayDelay::onBlocSizeChange (int BlocSize) {}
//void ArrayDelay::onSampleRateChange (double SampleRate) {}
//
//-----------------------------------------------------------------------------
// recording 
//void ArrayDelay::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z) {}

