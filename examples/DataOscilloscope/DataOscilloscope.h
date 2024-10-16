//-----------------------------------------------------------------------------
//@file  
//	DataOscilloscope.h
//
//@author
//	BrainModular team
//
//@brief 
//	Definitions of the DataOscilloscope class.
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
class DataOscilloscope : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

    // constructor
	DataOscilloscope(){};

    // destructor
	~DataOscilloscope(){};

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (TUsineMessage *Message);
    void onProcess();

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
    void onCreateCommands();
	void onPaint ();

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventClass btnErase;			// Erase trajectory button input
    UsineEventClass listSmooth;       // smooth modes
    UsineEventClass fdrSpeed;         // cube advancement step
    UsineEventClass yControl;         // value to display
    UsineEventClass colorControl;     // color to display the value

	//-------------------------------------------------------------------------
	// Other	
		
	// store the mouse left button state
	LongBool	mouseLeftButtonDown;			
    TPointF lastMousePos;

	// settings
	TUsineColor colorBox;


	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------
private:
    void eraseBox();
    void updateBox(float x, float y);
    void advance();


}; // class DataOscilloscope

#endif // __EXAMPLE_DRAW_BOX_H__
