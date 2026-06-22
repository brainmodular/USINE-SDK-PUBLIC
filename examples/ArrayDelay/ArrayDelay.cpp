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
#include "float.h"


//----------------------------------------------------------------------------
// setup a callback_id constant for all params that specify a callback type
//----------------------------------------------------------------------------
constexpr NativeInt ARR_IN_DELAY_CBID = 0x001000FF;


//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Create module
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new ArrayDelay();
}

//-----------------------------------------------------------------------------
// destroy module
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((ArrayDelay*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "array delay";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "array delay SDK example";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
ArrayDelay::ArrayDelay()
    : arrInArray(nullptr), arrInDelay(nullptr), arrOutArray(nullptr), readPos(0)
{
	std::memset(delayLine, 0, sizeof(delayLine));
	std::memset(delayBuffer, 0, sizeof(delayBuffer));
}

//-----------------------------------------------------------------------------------------
ArrayDelay::~ArrayDelay()
{	
}


//void ArrayDelay::onCreate(AnsiCharPtr optionalString);
//void ArrayDelay::onDestroy(); 
void ArrayDelay::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
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
void ArrayDelay::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
		// arrInArray
	case 0: 
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array in";
		pParamInfo->IsInput			= true;
		pParamInfo->IsOutput		= false;
		//pParamInfo->ReadOnly		= true;
        pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType	= ctNone;
		pParamInfo->setEventClass     (arrInArray);
		break;

		// arrInDelay
	case 1:
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "delay in blocs";
		pParamInfo->IsInput			= true;
		pParamInfo->IsOutput		= false;
		//pParamInfo->ReadOnly		= true;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= MAX_DELAY;
		pParamInfo->CallBackType	= ctImmediate;
		pParamInfo->CallBackId		= ARR_IN_DELAY_CBID;
		pParamInfo->setEventClass     (arrInDelay);
		break;

		// arrOutArray
	case 2: 
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array out";
		pParamInfo->IsInput			= false;
		pParamInfo->IsOutput		= true;
		//pParamInfo->ReadOnly		= true;
        pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType    = ctNone;
		pParamInfo->setEventClass     (arrOutArray);
		break;

		// default case
	default:
		break;
	}
}

void ArrayDelay::onCallBack(TUsineMessage *Message) 
{	
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;
    // arrInArray
    if (Message->wParam == ARR_IN_DELAY_CBID)
    {
        UINT32 i;
		UINT32 length = std::min((UINT32)arrInDelay.getSize(), MAXBLOC);

		for (i=0; i < length; i++)
		{
			delayBuffer[i] = std::lround(arrInDelay.getArrayData(i));
			if (delayBuffer[i] < 0) delayBuffer[i] = 0;
		}
	//	for (i = length; i < MAXBLOC; i++)
	//	{
	//		delayBuffer[i] = 0;
	//	}
	} 
}

void ArrayDelay::onProcess()
{	
    UINT32 length = std::min((UINT32)arrInArray.getSize(), MAXBLOC);
	UINT32 lengthDelay = std::min((UINT32)arrInDelay.getSize(), MAXBLOC);

	arrOutArray.setSize(length);
	readPos = (readPos + 1) % MAX_DELAY;

	for (UINT32 i = 0; i < length; i++)
	{
		UINT32 writePos = (readPos + delayBuffer[i % lengthDelay]) % MAX_DELAY;
		delayLine[writePos][i] = arrInArray.getArrayData(i);
		arrOutArray.setArrayData(i, delayLine[readPos][i]);
	//	delayLine[readPos][i] = 0;
	}
}

//-----------------------------------------------------------------------------
// midi out callbacks
 //void ArrayDelay::onMidiSysexSendOut (int DeviceID, char* Sysex) {}
//
//-----------------------------------------------------------------------------
// chunk system
//int  ArrayDelay::onGetStateChunkLen (LongBool Preset) {return 0;}
//void ArrayDelay::onGetStateChunk (void* chunk, LongBool Preset) {}
//void ArrayDelay::onSetStateChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}
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

