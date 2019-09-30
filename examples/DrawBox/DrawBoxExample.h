//-----------------------------------------------------------------------------
//@file  
//	DrawBoxExample.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the DrawBoxExample class.
//
//  Example user module of a graphical interface with mouse interactions.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.org/support/
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 Sensomusic
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
#ifndef __EXAMPLE_DRAW_BOX_H__
#define __EXAMPLE_DRAW_BOX_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

// Panel Height
const int DEFAULT_MODULEHEIGHT = 134;  


//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class DrawBoxExample : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

    // constructor
	DrawBoxExample(){};

    // destructor
	~DrawBoxExample(){};

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
    void onMouseWheel (TShiftState Shift, int WheelDelta);

    //-----------------------------------------------------------------------------
    // usine preset randomize
    virtual void onRandomize ();
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr btnErase;			// Erase trajectory button input
	UsineEventPtr btnRandomize;     // randomize button input

	//-------------------------------------------------------------------------
	// Other	
		
	// store the mouse left button state
	LongBool	mouseLeftButtonDown;			
    TPointF lastMousePos;

	// store the coords of our box
    TRectF boxCoords;
    float boxScale;

	// settings
	TColorUsine colorBox;


	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
    void eraseBox();

    void updateBox(float x, float y);


}; // class DrawBoxExample

#endif // __EXAMPLE_DRAW_BOX_H__