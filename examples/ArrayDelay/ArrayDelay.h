//-----------------------------------------------------------------------------
//@file  
//	ArrayDelay.h
//
//@author
//	BrainModular team
//
//@brief 
//	Definitions of the ArrayDelay class.
//
//  A template include to start from for new user module.
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

// include once, no more
#ifndef __ARRAY_DELAY_MODULE_H__
#define __ARRAY_DELAY_MODULE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <limits>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define MAXBLOC     512
#define MAX_DELAY   1000

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class ArrayDelay : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	ArrayDelay();

    // destructor
	virtual ~ArrayDelay();

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	//int  onGetNumberOfParams( int queryResult1, int queryResult2);
	//void onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2);
	//void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (TUsineMessage *Message);
	void onProcess ();
/*
	//-----------------------------------------------------------------------------
	// midi out callbacks
	void onMidiSendOut (int DeviceID, UsineMidiCode Code);
	void onMidiSysexSendOut (int DeviceID, char* Sysex);

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
	void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	void onOpenEditor();
	void onBringToFront();

	//-----------------------------------------------------------------------------
	// audio setup update
	void onBlocSizeChange (int BlocSize);
	void onSampleRateChange (double SampleRate);
	
	//-----------------------------------------------------------------------------
	// recording 
	void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);
*/

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
      UsineEventClass arrInArray;
      UsineEventClass arrInDelay;
      UsineEventClass arrOutArray;	
 
    float delayLine[MAXBLOC][MAX_DELAY];
    long delayBuffer[MAXBLOC];  
    long readPos; 

	//-------------------------------------------------------------------------
	// public methods
	//-------------------------------------------------------------------------
public:

    // public methods goes here

	//-------------------------------------------------------------------------
	// protected methods
	//-------------------------------------------------------------------------
protected:



}; // class ArrayDelay

#endif //__ARRAY_DELAY_MODULE_H__
