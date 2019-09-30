//-----------------------------------------------------------------------------
//@file  
//	SimplePadExample.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the SimplePadExample class
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "SimplePadExample.h"

using namespace std;
//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new SimplePadExample ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule)
{
    // cast is important to call the good destructor
	delete ((SimplePadExample*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "simple pad example";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "simple pad sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// public constructors/destructors
//-------------------------------------------------------------------------
SimplePadExample::SimplePadExample ()
    : sizePad (20)
    , mouseLeftButtonDown (FALSE)
    , selectedPad (-1)
    , selectedPadOffsetX (0.0f)
    , selectedPadOffsetY (0.0f)
{
}

//-----------------------------------------------------------------------------
// module description
void SimplePadExample::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->DefaultWidth		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->DefaultHeight		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->CanRecord  		= TRUE;
    pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->DontProcess		= TRUE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void SimplePadExample::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	colorPad = sdkGetUsineColor(clVuMeterAudio);

    resetPads();
}

//-----------------------------------------------------------------------------
// Parameters description
void SimplePadExample::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
    // btnRandomize
    if (ParamIndex == 0)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "randomize position";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->EventPtr        = &btnRandomize;
	}
    // arrPosX
    else if (ParamIndex == 1)
    {
        pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "arr pos x";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->IsShared		= TRUE;
		pParamInfo->DontSave		= TRUE;
		pParamInfo->EventPtr        = &arrPosX;
	}
    // arrPosY
    else if (ParamIndex == 2)
    {
        pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "arr pos y";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->IsShared		= TRUE;
		pParamInfo->DontSave		= TRUE;
		pParamInfo->EventPtr        = &arrPosY;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void SimplePadExample::onCallBack (UsineMessage *Message) 
{
	try
	{
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
            int ParamIndex = (int)Message->wParam;
            
            // btnRandomize
            if (ParamIndex == 0)
	        {
                onRandomize();
	        }
            // arrPosX
            else if (ParamIndex == 1)
            {
		        for (int i = 0; i < PAD_OBJ_MAX; i++)
                {
                    posTab[i].x = sdkGetEvtArrayData (arrPosX, i);
                }
                sdkRepaintPanel();
            }
            // arrPosY
            else if (ParamIndex == 2)
            {
		        for (int i = 0; i < PAD_OBJ_MAX; i++)
                {
                    posTab[i].y = 1.0f - sdkGetEvtArrayData (arrPosY, i);
                }
                sdkRepaintPanel();
            }
        }
	}
	catch (...)
	{
		//sdkTraceErrorChar("error");
	}
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// compute the lentgh of the chunk string 
int SimplePadExample::onGetChunkLen (LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets
    // compute the chunk string lentgh to save points coords of pads
	int ChunkLen = sizeof(TPointF) * PAD_OBJ_MAX;
	
	return ChunkLen; 
}

//-----------------------------------------------------------------------------
// store module datas in the chunk string
void SimplePadExample::onGetChunk (void* chunk, LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets 
	// copy points coords array into the chunk
	memcpy(chunk, &(posTab), sizeof(TPointF) * PAD_OBJ_MAX);
}

//-----------------------------------------------------------------------------
// restore module datas from the chunk string
void SimplePadExample::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets

	// copy points coords array from the chunk
	memcpy(&(posTab), chunk, sizeof(TPointF) * PAD_OBJ_MAX);
    
    for (int i = 0; i < PAD_OBJ_MAX; i++)
    {
        sdkSetEvtArrayData (arrPosX, i, posTab[i].x);
        sdkSetEvtArrayData (arrPosY, i, 1.0f - posTab[i].y);

        sdkNeedRemoteUpdate (1);
        sdkNeedRemoteUpdate (2);
    }

    // Ask to repaint the module
    sdkRepaintPanel();
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create user defined layout options in the 'design tab' setting panel
void SimplePadExample::onCreateSettings()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "pad");
	sdkAddSettingLineColor( DESIGN_TAB_NAME, &colorPad, "color" );
	sdkAddSettingLineInteger( DESIGN_TAB_NAME, &sizePad, "size", 1, 30, scLinear, "px", 20);
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redreaw
void SimplePadExample::onSettingsHasChanged()
{
	// Ask to repaint the module
	sdkRepaintPanel();
} 


void SimplePadExample::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit");
    sdkAddCommand("cmd_randomize_position", 0);
}

//-----------------------------------------------------------------------------
// paint the module panel
void SimplePadExample::onPaint ()
{
    for (int i = 0; i < PAD_OBJ_MAX; i++)
    {
        sdkDrawPoint (posTab[i], colorPad, (float)sizePad, TRUE);
    }
}

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MouseMove callback
void SimplePadExample::onMouseMove (TShiftState Shift, float X, float Y)
{
    // we only want to draw when the left mouse button is down
	if (mouseLeftButtonDown && selectedPad >= 0)
	{
        float posX = X + selectedPadOffsetX;
        float posY = Y + selectedPadOffsetY;
		updatePad (selectedPad, std::min(1.0f, std::max(posX, 0.0f)), std::min(1.0f, std::max(posY, 0.0f)));
        
		try
		{
			sdkProcessRecord((TPrecision)selectedPad, posX, posY);
		}
		catch (std::exception& /*e*/) 
		{
		}

        sdkRepaintPanel();
	}		
}

//-----------------------------------------------------------------------------
// MouseDown callback
void SimplePadExample::onMouseDown (TMouseButton Button, TShiftState Shift, float X,float Y)
{
	if (Button == mbLeft)
	{
        selectedPad = padHitTest (std::min(1.0f, std::max(X, 0.0f)), std::min(1.0f, std::max(Y, 0.0f)));
		// we store the state of the left button
		mouseLeftButtonDown = TRUE;
	}
}

//-----------------------------------------------------------------------------
// MouseUp callback
void SimplePadExample::onMouseUp (TMouseButton Button, TShiftState Shift, float X, float Y)
{
	mouseLeftButtonDown = FALSE;
    selectedPad = -1;

    try
	{
		sdkStopRecord ();
	}
	catch (std::exception& /*e*/) 
	{
	}
}

/*
void SimplePadExample::onMouseMoveMulti(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y)
{
	sdkTraceChar("onMouseMoveMulti");
}

void SimplePadExample::onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y)
{
	sdkTraceChar("onMouseDownMulti");
}

void SimplePadExample::onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y)
{
	sdkTraceChar("onMouseUpMulti");
}
*/

//-----------------------------------------------------------------------------
// usine preset randomize
void SimplePadExample::onRandomize ()
{
    float posX;
    float posY;
    
    for (int i = 0; i < PAD_OBJ_MAX; i++)
    {
        posX = (float)(rand() / (float)(RAND_MAX));
        posY = (float)(rand() / (float)(RAND_MAX));

        updatePad (i, posX, posY);
    }
	// Ask to repaint the module
	sdkRepaintPanel();
}

//-----------------------------------------------------------------------------
// recording 
void SimplePadExample::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z)
{
    updatePad((int)X, Y, Z);
}

//-----------------------------------------------------------------------------
// recording 
void SimplePadExample::onSetQuickColor(TColorUsine color)
{
 colorPad = color;
}


//-------------------------------------------------------------------------
// private methodes implementation
//-----------------------------------------------------------------------

//-------------------------------------------------------------------------
void SimplePadExample::resetPads()
{
    float offset = 1.0f / (float)(PAD_OBJ_MAX + 1);
    for (int i = 0; i < PAD_OBJ_MAX; i++)
    {
        float val = offset + (offset * i);
        updatePad (i, val, val);
    }

    // Ask to repaint the module
    sdkRepaintPanel();
}

//-------------------------------------------------------------------------
void SimplePadExample::updatePad(int padIndex, float x, float y)
{
    posTab[padIndex].x = x;
    posTab[padIndex].y = y;
    
    sdkSetEvtArrayData (arrPosX, padIndex, x);
    sdkSetEvtArrayData (arrPosY, padIndex, 1.0f - y);

    sdkNeedRemoteUpdate (1);
    sdkNeedRemoteUpdate (2);

    // Ask to repaint the module
    sdkRepaintPanel();

}

int SimplePadExample::padHitTest (float x, float y)
{
    int result = -1;
     
    // Determine radii
    float radiiX = sdkPixelToWidthPercent((float)sizePad) / 2.0f;
    float radiiY = sdkPixelToHeightPercent((float)sizePad) / 2.0f;

    float distX;
    float distY;

    for (int i = 0; i < PAD_OBJ_MAX; i++)
    {
        distX = posTab[i].x  - x;
        distY = posTab[i].y  - y;

        if ((distX * distX) / (radiiX * radiiX) + (distY * distY) / (radiiY * radiiY) <= 1)
        {
            result = i;
            selectedPadOffsetX = distX;
            selectedPadOffsetY = distY;

            return result;
        }
    }

    return result;
}
