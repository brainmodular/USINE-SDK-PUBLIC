//-----------------------------------------------------------------------------
//@file  
//	DrawTrajectory.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the TDrawTrajectoryModule class
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
#include "DrawTrajectory.h"
#include "float.h"


//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TDrawTrajectoryModule ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule)
{
    // cast is important to call the good destructor
	delete ((TDrawTrajectoryModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "draw trajectory";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Draw Trajectory Example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.21";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void TDrawTrajectoryModule::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->NumberOfParams     = 8;
	pModuleInfo->DefaultWidth		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->DefaultHeight		= DEFAULT_MODULEHEIGHT;
	pModuleInfo->DontProcess		= FALSE;
    pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void TDrawTrajectoryModule::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	colorTrajectory		= sdkGetUsineColor(clDataFlow);
	colorCurrentPoint	= sdkGetUsineColor(clCursor);
	
	thicknessTrajectory = 2;

	m_mouseLeftButtonDown       = FALSE;
	m_tabTrajectoryCoords.num	= 0;
	m_selPointIndexValue		= -1;

	// edit layout params
	m_boolInverseYAxis          = FALSE;
    m_boolIsYAxisInversed       = FALSE;
	m_indexDiameter		        = 4;

    m_boolClosedLine            = FALSE;
    m_boolSmoothLine            = FALSE;

}

//-----------------------------------------------------------------------------
// Parameters description
void TDrawTrajectoryModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
	switch (ParamIndex) 
	{
	// PointsCount data field output
	case 0: 
		pParamInfo->ParamType			= ptDataField;
		pParamInfo->Caption				= "nbpoints";
		pParamInfo->IsInput				= FALSE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->MinValue			= 0;
		pParamInfo->MaxValue			= MAXPOINTS;
		pParamInfo->DefaultValue		= 0;
		pParamInfo->Symbol				= " pt";
		pParamInfo->Format				= "%.0f";
		pParamInfo->ReadOnly			= TRUE;
		pParamInfo->setEventClass		(m_dtfPointsCount);
		break;

     // Selected point index data field input/output
     case 1:
		pParamInfo->ParamType			= ptDataField;
		pParamInfo->Caption				= "cursor pos";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->DefaultValue		= -1;
		pParamInfo->Symbol				= "";
		pParamInfo->Format				= "%.0f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->CallBackType		= ctImmediate; 
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->setEventClass		(m_dtfSelPointIndex);
		break;

     // Selected point X coord fader output
     case 2:
		pParamInfo->ParamType       = ptDataFader;
		pParamInfo->Caption         = "pos X";
		pParamInfo->IsInput         = FALSE;
		pParamInfo->IsOutput        = TRUE;
		pParamInfo->MinValue        = 0;
		pParamInfo->MaxValue        = 1;
		pParamInfo->DefaultValue    = 0;
		pParamInfo->Symbol          = "";
		pParamInfo->Format          = "%.3f";
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->IsSeparator		= TRUE;
		pParamInfo->setEventClass	(m_fdrSelPointX);
		break;

     // Selected point Y coord fader output
      case 3:
		pParamInfo->ParamType       = ptDataFader;
		pParamInfo->Caption         = "pos Y";
		pParamInfo->IsInput         = FALSE;
		pParamInfo->IsOutput        = TRUE;
		pParamInfo->MinValue        = 0;
		pParamInfo->MaxValue        = 1;
		pParamInfo->DefaultValue    = 0;
		pParamInfo->Symbol          = "";
		pParamInfo->Format          = "%.3f";
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->setEventClass	(m_fdrSelPointY);
		break;

	// Erase trajectory button input
	case 4:
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "reset";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->IsSeparator		= TRUE;
		pParamInfo->setEventClass	(m_btnErase);
		break;

	// m_btnRandomize
	case 5:
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "randomize";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->setEventClass	(m_btnRandomize);
		break;

    // arrPointsCoordsIn
    case 6:
        pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "coords in";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
		pParamInfo->DontSave		= TRUE;
		pParamInfo->setEventClass	(arrPointsCoordsIn);
        break;

    // arrPointsCoordsOut
    case 7:
        pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "coords out";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->DontSave		= TRUE;
		pParamInfo->setEventClass	(arrPointsCoordsOut);
		break;
        

	default:
		// do nothing
		break;
	}
}

	

//-----------------------------------------------------------------------------
// Parameters callback
void TDrawTrajectoryModule::onCallBack (TUsineMessage *Message) 
{
	// WParam contains the Param Number
	switch (Message->wParam) 
	{
	// Selected point index data field input/output
	case 1:
		if (Message->lParam == MSG_CHANGE) 
		{
            int tmp = (int)m_dtfSelPointIndex.getData();
            if (m_selPointIndexValue != tmp)
            {
			    m_selPointIndexValue = tmp;
                m_trajectoryThickness = m_dtfSelPointIndex.getData() - m_selPointIndexValue;

				// Ask to repaint the module
			    sdkRepaintPanel();
            }
		}
		break;

	// Erase trajectory button input
	case 4:
		if (Message->lParam == MSG_CHANGE) 
		{	
			m_tabTrajectoryCoords.num = 0;
			m_selPointIndexValue = -1;
			m_dtfPointsCount.setData(0.0f);
			m_dtfSelPointIndex.setData(- 1.0f);

			// Ask to repaint the module
			sdkRepaintPanel();
		}
		break;

	// randomize button input
	case 5:
		if (Message->lParam == MSG_CHANGE) 
		{
            onRandomize ();
		}
		break;

    // arrPointsCoordsIn
    case 6:
		if (Message->lParam == MSG_CHANGE) 
		{	
            int coordsInSize = arrPointsCoordsIn.getSize();
			int arrayDim = 3;

            int totalInPoints = coordsInSize / arrayDim;
	        TPointF newCoord;

            for (int i = 0; i < totalInPoints; i++)
            {
                // extract arrai in point
                newCoord.x = arrPointsCoordsIn.getArrayData(i * arrayDim);
                newCoord.y = arrPointsCoordsIn.getArrayData((i * arrayDim) + 1);
                
                // apply inverse option if necessary
                if (m_boolIsYAxisInversed != FALSE)
                    newCoord.y = 1.0f - newCoord.y;

                // populate points tab
                m_tabTrajectoryCoords.Tab[i].x = newCoord.x;
                m_tabTrajectoryCoords.Tab[i].y = newCoord.y;
            
                // populate output array
                arrPointsCoordsOut.setArrayData(i * arrayDim, m_tabTrajectoryCoords.Tab[i].x);
                arrPointsCoordsOut.setArrayData((i * arrayDim) + 1, m_tabTrajectoryCoords.Tab[i].y);

                // always 3 lines for the array out
                if (arrayDim >= 3)
                    arrPointsCoordsOut.setArrayData((i * arrayDim) + 2, arrPointsCoordsIn.getArrayData(i * arrayDim) + 2);
                else
                    arrPointsCoordsOut.setArrayData((i * arrayDim) + 2, 0.5f);
            }
            m_tabTrajectoryCoords.num = totalInPoints;
		    // update points counter
		    m_dtfPointsCount.setData((float)m_tabTrajectoryCoords.num);

            arrPointsCoordsOut.setSize(totalInPoints * ARRAY_OUT_LINES);

            computeAllControlPoints ();
            
			// Ask to repaint the module
			sdkRepaintPanel();
		}
        break;
		
    // arrPointsCoordsOut
    case 7:
		if (Message->lParam == MSG_CHANGE) 
		{            
            int totalInPoints = arrPointsCoordsOut.getSize() / ARRAY_OUT_LINES;

            for (int i = 0; i < totalInPoints; i++)
            {

                // populate points tab
                m_tabTrajectoryCoords.Tab[i].x = arrPointsCoordsOut.getArrayData(i * ARRAY_OUT_LINES);
                m_tabTrajectoryCoords.Tab[i].y = arrPointsCoordsOut.getArrayData((i * ARRAY_OUT_LINES) + 1);

            }
            m_tabTrajectoryCoords.num = totalInPoints;
            // update points counter
		    m_dtfPointsCount.setData((float)m_tabTrajectoryCoords.num);
			// Ask to repaint the module
			sdkRepaintPanel();
		}
        break;

	// default case
	default:
		// do nothing
		break;
	}
}

//-----------------------------------------------------------------------------
// Main process
void TDrawTrajectoryModule::onProcess () 
{
	if (m_selPointIndexValue > -1 && m_selPointIndexValue < m_tabTrajectoryCoords.num)
	{
		TPointF point = m_tabTrajectoryCoords.Tab[m_selPointIndexValue];
        		
        if (m_selPointIndexValue < m_tabTrajectoryCoords.num-1)
		{
			point.x = point.x + (m_tabTrajectoryCoords.Tab[m_selPointIndexValue+1].x - point.x ) * m_trajectoryThickness;
			point.y = point.y + (m_tabTrajectoryCoords.Tab[m_selPointIndexValue+1].y - point.y ) * m_trajectoryThickness;
		}

		m_fdrSelPointX.setData(point.x);
		m_fdrSelPointY.setData(point.y);
	}
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// compute the lentgh of the chunk string 
int TDrawTrajectoryModule::onGetChunkLen (LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets

	// compute the chunk string lentgh to save points coords array
	int ChunkLen = sizeof(m_tabTrajectoryCoords.num) + (sizeof(TPointF) * m_tabTrajectoryCoords.num);
	
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// store module datas in the chunk string
void TDrawTrajectoryModule::onGetChunk (void* chunk, LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets
    
	memcpy(chunk, &(m_tabTrajectoryCoords.num), sizeof(m_tabTrajectoryCoords.num));
	// copy points coords array into the chunk
	memcpy((void*)((char*)chunk + sizeof(m_tabTrajectoryCoords.num)), &(m_tabTrajectoryCoords.Tab), (sizeof(TPointF) * m_tabTrajectoryCoords.num));

}

//-----------------------------------------------------------------------------
// restore module datas from the chunk string
void TDrawTrajectoryModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) 
{
	// no differences between the chunk for wkp/pat and for presets
    int tmp = sizeof(m_tabTrajectoryCoords.num);

	// copy points coords array from the chunk
	memcpy(&(m_tabTrajectoryCoords.num), chunk, sizeof(m_tabTrajectoryCoords.num));
	memcpy(&(m_tabTrajectoryCoords.Tab), (void*)((char*)chunk + tmp), sizeof(TPointF) * m_tabTrajectoryCoords.num);

    computeAllControlPoints ();
    
    arrPointsCoordsOut.setSize(m_tabTrajectoryCoords.num * ARRAY_OUT_LINES);
        
    for (int i = 0; i < m_tabTrajectoryCoords.num; i++)
    {
        arrPointsCoordsOut.setArrayData(i * ARRAY_OUT_LINES, m_tabTrajectoryCoords.Tab[i].x);
        arrPointsCoordsOut.setArrayData((i * ARRAY_OUT_LINES) + 1, m_tabTrajectoryCoords.Tab[i].y);
        arrPointsCoordsOut.setArrayData((i * ARRAY_OUT_LINES) + 2, 0.5f);
    }

	// update points counter
	m_dtfPointsCount.setData((float)m_tabTrajectoryCoords.num);

}

//-----------------------------------------------------------------------------
// Global Randomize
void TDrawTrajectoryModule::onRandomize ()
{
	m_tabTrajectoryCoords.num = 0;
	m_selPointIndexValue = -1;
	m_dtfPointsCount.setData(0.0f);
	m_dtfSelPointIndex.setData(-1.0f);

	computeLissajous();

	// Ask to repaint the module
	sdkRepaintPanel();
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void TDrawTrajectoryModule::onCreateSettings()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "trajectory");
	sdkAddSettingLineBoolean( DESIGN_TAB_NAME, &m_boolInverseYAxis, "inverse Y axis");
	//sdkAddSettingLineSeparator();
	sdkAddSettingLineColor( DESIGN_TAB_NAME, &colorTrajectory, "line color" );
	sdkAddSettingLineInteger( DESIGN_TAB_NAME, &thicknessTrajectory, "line width", 1, 100, scLog, "pix", 2 );
	//sdkAddSettingLineSeparator();
	sdkAddSettingLineColor( DESIGN_TAB_NAME, &colorCurrentPoint, "cursor color" );
	sdkAddSettingLineInteger( DESIGN_TAB_NAME, &m_indexDiameter, "cursor width", 0, 100, scLog, "pix", 2 );
	sdkAddSettingLineBoolean( DESIGN_TAB_NAME, &m_boolClosedLine, "closed line");
	sdkAddSettingLineBoolean( DESIGN_TAB_NAME, &m_boolSmoothLine, "smooth line");
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redreaw
void TDrawTrajectoryModule::onSettingsHasChanged()
{
	toggleInverseYAxis();
	
	// Ask to repaint the module
	sdkRepaintPanel();
} 


void TDrawTrajectoryModule::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit", TRUE);
    sdkAddCommand("cmd_reset", 4, TRUE);
    sdkAddCommand("cmd_randomize", 5, TRUE);
}

//-----------------------------------------------------------------------------
// paint the module panel
void TDrawTrajectoryModule::onPaint ()
{
	// draw points if needed
	if (m_tabTrajectoryCoords.num > 1) 
	{
		int totalpoints = m_tabTrajectoryCoords.num;
		//sdkDrawPolyLine (m_tabTrajectoryCoords.Tab, totalpoints, colorTrajectory, (float)thicknessTrajectory);
        
        // begin draw path sequence
        sdkDrawPathStart ();

        sdkDrawPathMoveTo (m_tabTrajectoryCoords.Tab[0]);
        
        if (m_boolSmoothLine != FALSE)
        {
            for (int i = 1; i < totalpoints; i++)
            {
                sdkDrawPathQuadCurveTo (m_tabTrajectoryCoords.TabControls[i], m_tabTrajectoryCoords.Tab[i]); 
                // for debug
		        //sdkDrawPoint (m_tabTrajectoryCoords.TabControls[i], sdkGetUsineColor(clMidiFlow), 5, true );
    
            }
        }
        else
        {
            for (int i = 1; i < totalpoints; i++)
            {
                sdkDrawPathLineTo (m_tabTrajectoryCoords.Tab[i]); 
            }
        }

        if (m_boolClosedLine != FALSE && totalpoints > 2)
            sdkDrawPathClose ();

        // end draw path sequence
        sdkDrawPathDraw (colorTrajectory, (float)thicknessTrajectory);
        
	}

	// draw current index point if needed
	if (m_selPointIndexValue < m_tabTrajectoryCoords.num && m_selPointIndexValue >= 0) 
	{
		//float radius =  std::max(0.001f, m_indexDiameter / 100.0f );
		float radius =  (float)std::max(1, m_indexDiameter);
		sdkDrawPoint (this->m_tabTrajectoryCoords.Tab[this->m_selPointIndexValue], colorCurrentPoint, radius, true );
	}	
}

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MouseMove callback
void TDrawTrajectoryModule::onMouseMove (TShiftState Shift, float X, float Y)
{
	if ( m_mouseLeftButtonDown && (m_tabTrajectoryCoords.num < MAXPOINTS - 1))
	{
		appendTrajectory( std::min(1.0f, std::max(X, 0.0f)), std::min(1.0f, std::max(Y, 0.0f)) );
	}		
}

//-----------------------------------------------------------------------------
// MouseDown callback
void TDrawTrajectoryModule::onMouseDown (TMouseButton Button, TShiftState Shift, float X,float Y)
{
	if (Button == mbLeft)
	{
		// so, left mouse button is down
		m_mouseLeftButtonDown = TRUE;
		appendTrajectory( std::min(1.0f, std::max(X, 0.0f)), std::min(1.0f, std::max(Y, 0.0f)) );
	}
}

//-----------------------------------------------------------------------------
// MouseUp callback
void TDrawTrajectoryModule::onMouseUp (TMouseButton Button, TShiftState Shift, float X, float Y)
{
	m_mouseLeftButtonDown = FALSE;
}

//-----------------------------------------------------------------------------
// Multi touch callbacks not used

//-------------------------------------------------------------------------
// private methods implementation
//-----------------------------------------------------------------------
//-------------------------------------------------------------------------
// deal with the inverse Y axis option
void TDrawTrajectoryModule::toggleInverseYAxis()
{
	if ( (m_tabTrajectoryCoords.num >= 1) && 
        ((m_boolInverseYAxis == FALSE && m_boolIsYAxisInversed != FALSE) || (m_boolInverseYAxis != FALSE && m_boolIsYAxisInversed == FALSE))
        ) 
    {
			
		int totalpoints = this->m_tabTrajectoryCoords.num;
		TPointF newCoordCoeff;

		for (int i = 0; i < totalpoints; i++)
		{
            newCoordCoeff.x = m_tabTrajectoryCoords.Tab[i].x;
            newCoordCoeff.y = 1.0f - m_tabTrajectoryCoords.Tab[i].y;

            this->m_tabTrajectoryCoords.Tab[i] = newCoordCoeff;
            
//            int index = i * ARRAY_OUT_LINES;
//            int tmp = sdkGetEvtSize (arrPointsCoordsOut);
            arrPointsCoordsOut.setArrayData((i * ARRAY_OUT_LINES) + 1, newCoordCoeff.y);

		}
	}
    m_boolIsYAxisInversed = m_boolInverseYAxis;

}

/*
void TDrawTrajectoryModule::applyInverseYAxis()
{
	if ( (m_tabTrajectoryCoords.num >= 1) && 
         (m_boolIsYAxisInversed != FALSE)
        ) 
    {
			
		int totalpoints = this->m_tabTrajectoryCoords.num;
		TPointF newCoordCoeff;

		for (int i = 0; i < totalpoints; i++)
		{
            newCoordCoeff.x = m_tabTrajectoryCoords.Tab[i].x;
            newCoordCoeff.y = 1.0f - m_tabTrajectoryCoords.Tab[i].y;

            this->m_tabTrajectoryCoords.Tab[i] = newCoordCoeff;
		}

	}

}
*/

//-------------------------------------------------------------------------
// append a point coords in the m_tabTrajectoryCoords structure
// TODO : make it protected methode
void TDrawTrajectoryModule::appendTrajectory( float X, float Y ) 
{
	TPointF lastCoordCoeff;
    
    if (m_tabTrajectoryCoords.num > 0)
    {
        lastCoordCoeff = m_tabTrajectoryCoords.Tab[m_tabTrajectoryCoords.num-1];
    }
    else
    {
        lastCoordCoeff.x = -1.0f;
        lastCoordCoeff.y = -1.0f;
    }

	TPointF newCoordCoeff;
	newCoordCoeff.x = X;
	newCoordCoeff.y = Y;

    float percentWidthForOnePixel = sdkPixelToWidthPercent (1);
    float percentHeightForOnePixel = sdkPixelToHeightPercent (1);
    // deal with the inverse Y axis option
    //if (m_boolInverseYAxis != FALSE)  newCoordCoeff.y = 1.0f -newCoordCoeff.y;

	if ((newCoordCoeff.x > (lastCoordCoeff.x + percentWidthForOnePixel)) ||
		(newCoordCoeff.x < (lastCoordCoeff.x - percentWidthForOnePixel)) ||
		(newCoordCoeff.y < (lastCoordCoeff.y - percentHeightForOnePixel)) ||
		(newCoordCoeff.y > (lastCoordCoeff.y + percentHeightForOnePixel)) )
	{
		// update values
		m_tabTrajectoryCoords.Tab[m_tabTrajectoryCoords.num] = newCoordCoeff;
		m_tabTrajectoryCoords.num++;
        
        int newSize = m_tabTrajectoryCoords.num * ARRAY_OUT_LINES;
        arrPointsCoordsOut.setSize(newSize);
        arrPointsCoordsOut.setArrayData(newSize - 3, newCoordCoeff.x);
        arrPointsCoordsOut.setArrayData(newSize - 2, newCoordCoeff.y);
        arrPointsCoordsOut.setArrayData(newSize - 1, 0.5f);

        if (m_tabTrajectoryCoords.num >= 1)
            computeControlPoint (m_tabTrajectoryCoords.num - 1);

		// update points counter
		m_dtfPointsCount.setData((float)m_tabTrajectoryCoords.num);

        
		// Ask to repaint the module
		sdkRepaintPanel();
	}
}

void TDrawTrajectoryModule::computeControlPoint (int pointIndex)
{
    // smoothing line need at least 2 points to compute control point
	if (pointIndex >= 1 && pointIndex < m_tabTrajectoryCoords.num) 
	{
        TPointF lastCoord;
        TPointF currentCoord;
        //TPointF nextCoord;
        TPointF controlCoord;
        //float tmpDelta;

        lastCoord = m_tabTrajectoryCoords.Tab[pointIndex -1 ]; 
        currentCoord = m_tabTrajectoryCoords.Tab[pointIndex];
        //nextCoord = m_tabTrajectoryCoords.Tab[pointIndex + 1];

        //controlCoord.x = lastCoord.x;
        //controlCoord.y = currentCoord.y;
        
        // found x coord of the control point
        float tmpDeltaX = currentCoord.x - lastCoord.x;
        float tmpDeltaY = currentCoord.y - lastCoord.y;

        if (tmpDeltaX >= 0)
        {
            if (tmpDeltaY >= 0)
            {
                controlCoord.x = currentCoord.x;
                controlCoord.y = lastCoord.y;
            }
            else
            {
                controlCoord.x = lastCoord.x;
                controlCoord.y = currentCoord.y;
            }
        }
        else
        {
            if (tmpDeltaY >= 0)
            {
                controlCoord.x = lastCoord.x;
                controlCoord.y = currentCoord.y;
            }
            else
            {
                controlCoord.x = currentCoord.x;
                controlCoord.y = lastCoord.y;
            }
        }

        m_tabTrajectoryCoords.TabControls[pointIndex] = controlCoord;
    }
}

void TDrawTrajectoryModule::computeAllControlPoints ()
{	
    // smoothing line need at least 2 points to compute control point
	if (m_tabTrajectoryCoords.num > 1) 
	{
	    int totalpoints = m_tabTrajectoryCoords.num;

        for (int i = 1; i < totalpoints; i++)
        {
            computeControlPoint (i);
        }
    }
}
	
void TDrawTrajectoryModule::computeLissajous()
{
    int j = 0;
    float x0,y0,t,x,y = 0;
    float a = 1+2*(float)(rand() / (float)(RAND_MAX));
    float b = 1+2*(float)(rand() / (float)(RAND_MAX));
    float c = (float)(rand() / (float)(RAND_MAX));
    float d = (float)(rand() / (float)(RAND_MAX));  
	TPointF newCoord;
    m_tabTrajectoryCoords.num = 0;

    do  
    {        
        t = j*2*PI/30;  
        x = (float)0.5+cos(a*t+c)/2;
        y = (float)0.5+cos(b*t+d)/2;          
        if (j == 0)
        {
          x0 = x;
          y0 = y;
        }
         
        //SetLength(Output,(j+1)*3);
         
	    newCoord.x = x;
	    newCoord.y = y;
        //SetDataArrayValue(Output,3*j,x);
        //SetDataArrayValue(Output,(3*j)+1,y);           
        //SetDataArrayValue(Output,(3*j)+2,1);  
        
		m_tabTrajectoryCoords.Tab[m_tabTrajectoryCoords.num] = newCoord;
		m_tabTrajectoryCoords.num++;

        int newSize = m_tabTrajectoryCoords.num * ARRAY_OUT_LINES;
        arrPointsCoordsOut.setSize(newSize);
		arrPointsCoordsOut.setArrayData(newSize - 3, newCoord.x);
        arrPointsCoordsOut.setArrayData(newSize - 2, newCoord.y);
        arrPointsCoordsOut.setArrayData(newSize - 1, 0.5f);
		// update points counter
		m_dtfPointsCount.setData((float)m_tabTrajectoryCoords.num); 

        d = sqrt(sqr(x-x0)+sqr(y-y0));
        j = j+1;
        //until ((j>10) and (d<0.1)) or (j >LISS_MAXPOINTS);  
    }while (!(((j>10) && (d<0.1)) || (j >LISS_MAXPOINTS)));
  // fermer le path
  //SetLength(Output,(j+1)*3);      
  //SetDataArrayValue(Output,3*j,x0);
  //SetDataArrayValue(Output,(3*j)+1,y0);           
  //SetDataArrayValue(Output,(3*j)+2,1); 

}
