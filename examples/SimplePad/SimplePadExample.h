//-----------------------------------------------------------------------------
//@file  
//	SimplePadExample.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the SimplePadExample class.
//
//  Example user module of a graphical interface with multi touch interactions.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
//  support@brainmodular.org 
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
#ifndef __EXAMPLE_SIMPLE_PAD_H__
#define __EXAMPLE_SIMPLE_PAD_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
// Panel Height
const int DEFAULT_MODULEHEIGHT = 134;  

#define PAD_OBJ_MAX 4
#define PAD_INS_PER_OBJ 2

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class SimplePadExample : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

    // constructor
	SimplePadExample();

    // destructor
	~SimplePadExample(){};

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (UsineMessage *Message);

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
    void onCreateCommands();
	void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	
	//void onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);

    //-----------------------------------------------------------------------------
    // usine preset randomize
    virtual void onRandomize ();

	//-----------------------------------------------------------------------------
	// recording 
	 void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);

	//-----------------------------------------------------------------------------
	// recording 
	void onSetQuickColor(TColorUsine color);


	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
    UsineEventPtr arrPosX;  
    UsineEventPtr arrPosY;  
	
	UsineEventPtr btnRandomize; 

	//-------------------------------------------------------------------------
	// used to store pad pos in the chunk	
	TPointF posTab[PAD_OBJ_MAX];
    
	// store the mouse left button state
	LongBool	mouseLeftButtonDown;			
    int selectedPad;
    float selectedPadOffsetX;
    float selectedPadOffsetY;

	// settings
	TColorUsine colorPad;
    int sizePad;
    
	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
    void resetPads();

    void updatePad (int padIndex, float x, float y);

    // return -1 if no hit, otherwise, the pad index (0 to PAD_OBJ_MAX - 1)
    int padHitTest (float x, float y);

}; // class SimplePadExample

#endif // __EXAMPLE_SIMPLE_PAD_H__