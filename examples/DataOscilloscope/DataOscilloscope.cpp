//-----------------------------------------------------------------------------
//@file  
//	DataOscilloscope.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the DataOscilloscope class
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "DataOscilloscope.h"
#include <vector>

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataOscilloscope ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule)
{
    // cast is important to call the right destructor
	delete ((DataOscilloscope*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "data oscilloscope example";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "data oscilloscope sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";
float EMPTY_COORD = -999.9f;

// variables
TUsineColor drawColor;
float moveOffset    = 0.005f;
float yValue        = 0.0f;

// TODO/WARNING :
// This is a raw implementation given as an example : the points are stored in a vector
// which is not the most efficient data structure for the job
// For better cpu efficiency you may want to use a Circular Buffer instead
std::vector<TPointF> points;

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void DataOscilloscope::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->DefaultWidth		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->DefaultHeight		= DEFAULT_MODULEHEIGHT;
    pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->DontProcess		= FALSE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;

}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void DataOscilloscope::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	colorBox		= sdkGetUsineColor(clCursor);
	mouseLeftButtonDown       = FALSE;
	eraseBox();
}

//-----------------------------------------------------------------------------
// Parameters description
void DataOscilloscope::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
	switch (ParamIndex) 
	{
    case 0 :

        pParamInfo->ParamType       = ptDataField;
        pParamInfo->Caption         = "y";
        pParamInfo->IsInput         = TRUE;
        pParamInfo->IsOutput        = FALSE;
        pParamInfo->IsSeparator     = FALSE;
        pParamInfo->DefaultValue    = 0.5f;
        pParamInfo->MinValue        = -1.0f;
        pParamInfo->MaxValue        = 1.0f;
        pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->setEventClass   (yControl);
        pParamInfo->CallBackId      = 88;

        break;
            
    case 1 :
        pParamInfo->ParamType       = ptDataFader;
        pParamInfo->Caption         = "speed";
        pParamInfo->IsInput         = TRUE;
        pParamInfo->IsOutput        = FALSE;
        pParamInfo->IsSeparator     = FALSE;
        pParamInfo->DefaultValue    = 0.005f;
        pParamInfo->Scale           = scLog;
        pParamInfo->MaxValue        = 0.5f;
        pParamInfo->CallBackType    = ctNormal;
        pParamInfo->setEventClass   (fdrSpeed);
        pParamInfo->CallBackId      = 89;

        break;
            
    case 2 :

        pParamInfo->ParamType       = ptChooseColor;
        pParamInfo->Caption         = "color";
        pParamInfo->IsInput         = TRUE;
        pParamInfo->IsOutput        = FALSE;
        pParamInfo->IsSeparator     = FALSE;
        pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->setEventClass   (colorControl);
        pParamInfo->CallBackId      = 87;

        break;
	default:
		// do nothing
		break;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DataOscilloscope::onCallBack (TUsineMessage *Message) 
{
	try
	{
        // filter only message specific to this user module and type MS_CHANGE
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
	        // WParam contains the Param Number
	        switch (Message->wParam) 
	        {
            case 87:
                colorBox = colorControl.getColor();
                break;
            case 88:
                yValue = yControl.getData();
                break;
            case 89:
                moveOffset = fdrSpeed.getData();
                break;

	        // default case
	        default:
		        // do nothing
		        break;
	        }
        }
	}
	catch (...)
	{
		//sdkTraceErrorChar("error");
	}
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void DataOscilloscope::onCreateSettings()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
	sdkAddSettingLineColor( DESIGN_TAB_NAME, &colorBox, "box color" );
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redreaw
void DataOscilloscope::onSettingsHasChanged()
{
	// Ask to repaint the module
	sdkRepaintPanel();
} 


void DataOscilloscope::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit");
}


//----------------------------------------------------------------------------
// Updating points position
void DataOscilloscope::advance() {
    
    TPointF newPoint;
    newPoint.x   = 0.0f;
    newPoint.y    = 1 - yValue;
    points.push_back(newPoint);
    
    for (int i = 0; i < points.size(); i++){
        // coord update
        points[i].x += moveOffset;
        points[i].y  += 0.0f;
        
        if (points[i].x >= 1.0) {
            points.erase(points.begin() + i);
        }
    }
    
}

//-----------------------------------------------------------------------------
// paint the module panel
void DataOscilloscope::onPaint ()
{
    // Drawing the path
    sdkDrawPathStart();
    TPointF previous;
    TPointF current;
    previous.x = previous.y = current.x = current.y =  EMPTY_COORD;
    
    for(TPointF point : points) {
        current = point;
        // first iteration
        if (previous.x == EMPTY_COORD && previous.y == EMPTY_COORD) {
            previous = point;
            continue;
        }
        
        sdkDrawPathMoveTo(previous);
        sdkDrawPathLineTo(current);
        previous = current;
    }
    sdkDrawPathClose();
    sdkDrawPathDraw(colorBox, 0.5);

    updateBox(boxCoords.left, boxCoords.top);
}

//-------------------------------------------------------------------------
// private methods implementation
//-----------------------------------------------------------------------
//-------------------------------------------------------------------------
void DataOscilloscope::eraseBox()
{
    // we put all box values to -1
    // in the paint methode, we'll check against the top value if it's negative
    boxCoords.left = -1;
    boxCoords.top = -1;
    boxCoords.bottom = -1;
    boxCoords.right = -1;
    
    points.clear();
    
    // Ask to repaint the module
    sdkRepaintPanel();
}

//-------------------------------------------------------------------------
void DataOscilloscope::updateBox(float x, float y)
{
    boxCoords.bottom = y;
    boxCoords.right = x;
    
    // Ask to repaint the module
    sdkRepaintPanel();
}

//--------------------------------------------------------------------------
void DataOscilloscope::onProcess() {
    if (sdkRefreshSpeedClock()) {
        advance();
    }
}
