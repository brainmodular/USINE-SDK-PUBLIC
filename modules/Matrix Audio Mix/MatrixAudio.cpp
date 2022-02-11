//----------------------------------------------------------------------------
//@file  
//	MatrixAudio.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the MatrixAudioModule class.
//
//  A graphical module to mix audio from a matrix point of view.
//  Derived from the more generic Matrix - Display or Set module.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
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

//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "MatrixAudio.h"
#include <string>
#include <sstream>
#include <vector>
#include <cmath>

using namespace std;

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MatrixAudioModule();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((MatrixAudioModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Matrix Audio";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Matrix - Audio Mix";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.43";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// constructors/destructors
//-------------------------------------------------------------------------
//
MatrixAudioModule::MatrixAudioModule ()
{
	// dummy values overridden by the query system
    m_numOfCols = 1;
	m_numOfRows = 1;

	m_numOfOptInOut = 0;

	// equivalent to set each pointer to NULL
	memset(m_tevtOptInputs, 0, (sizeof(UsineEventPtr) * OPT_PARAMS_MAX));
	memset(m_tevtOptOutputs, 0, (sizeof(UsineEventPtr) * OPT_PARAMS_MAX));
	memset(m_tevtOptInputsCaption, 0, (sizeof(UsineEventPtr) * OPT_PARAMS_MAX));
	memset(m_tevtOptOutputsCaption, 0, (sizeof(UsineEventPtr) * OPT_PARAMS_MAX));

	// populate tabs with 0
	memset(m_tabCellsGuiData, 0, (sizeof(TCellGuiData) * MATRIX_SIZE_MAX));
	memset(m_tabInCaptionsGuiData, 0, (sizeof(TCellGuiData) * OPT_PARAMS_MAX));
	memset(m_tabOutCaptionsGuiData, 0, (sizeof(TCellGuiData) * OPT_PARAMS_MAX));
	memset(m_tabCellsValue, 0, (sizeof(float) * MATRIX_SIZE_MAX));

    // audio smooth
	memset(m_smoothOldCoeff, 0, (sizeof(float) * MATRIX_SIZE_MAX));
        
	m_tevtAudioMixBuffer = NULL;
	m_tevtSmoothCurrentCoeff = NULL;

	// important to set to size max to prevent a resize during use 
    // vector resize isn't multi thread safe, cause conflict with the graphic thread
    m_vectCellsCaptions.resize(MATRIX_SIZE_MAX);
		
}; 

//
MatrixAudioModule::~MatrixAudioModule ()
{
	if (m_tevtAudioMixBuffer != NULL )
		sdkDestroyEvt(m_tevtAudioMixBuffer);

	if (m_tevtSmoothCurrentCoeff != NULL )
		sdkDestroyEvt(m_tevtSmoothCurrentCoeff);

};


//-----------------------------------------------------------------------------
// module description
void MatrixAudioModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	// pModuleInfo->NumberOfParams is supplied by the query system
	pModuleInfo->DefaultWidth		= MODULE_DIM_DEFAULT;
	pModuleInfo->DefaultHeight		= MODULE_DIM_DEFAULT;
	pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->CanBeReset         = TRUE;

	// for optional params
	pModuleInfo->QueryString		= "max inputs/outputs \?";
	pModuleInfo->QueryListValues	= "\"4 in x 4 out\",\"8 in x 8 out\",\"16 in x 16 out\",\"2 in x 16 out\",\"16 in x 2 out\",\"2 in x 32 out\",\"32 in x 2 out\",\"8 in x 32 out\",\"32 in x 8 out\"";
	pModuleInfo->QueryDefaultIdx	= 1;
    
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int MatrixAudioModule::onGetNumberOfParams( int queryIndex)
{
    int cols, rows;
	int result = 0;

    // rows = inputs, cols = outputs
	switch (queryIndex)
	{
    default:
    case 0:
        rows = 4;
        cols = 4;
        break;
    case 1:
        rows = 8;
        cols = 8;
        break;
    case 2:
        rows = 16;
        cols = 16;
        break;
    case 3:
        rows = 2;
        cols = 16;
        break;
    case 4:
        rows = 16;
        cols = 2;
        break;
    case 5:
        rows = 2;
        cols = 32;
        break;
    case 6:
        rows = 32;
        cols = 2;
        break;
    case 7:
        rows = 8;
        cols = 32;
        break;
    case 8:
        rows = 32;
        cols = 8;
        break;
	}
	
    m_rowsMax = rows;
    m_numOfRows = rows;
    m_colsMax = cols;
    m_numOfCols = cols;

	result = (rows*OPT_IN_PARAMS) + (cols*OPT_OUT_PARAMS) + OPT_AFTER_INOUT;
	m_numOfOptInOut = result - OPT_AFTER_INOUT - 1;

    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void MatrixAudioModule::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
    // create the audio buffer event
    sdkCreateEvt(m_tevtAudioMixBuffer, pMasterInfo->BlocSize);
    sdkCreateEvt(m_tevtSmoothCurrentCoeff, pMasterInfo->BlocSize);

    int cols, rows;

    // rows = inputs, cols = outputs
    switch (queryIndex)
	{
    default:
    case 0:
        rows = 4;
        cols = 4;
        break;
    case 1:
        rows = 8;
        cols = 8;
        break;
    case 2:
        rows = 16;
        cols = 16;
        break;
    case 3:
        rows = 2;
        cols = 16;
        break;
	case 4:
		rows = 16;
		cols = 2;
		break;
	case 5:
		rows = 2;
		cols = 32;
		break;
	case 6:
		rows = 32;
		cols = 2;
		break;
	case 7:
		rows = 8;
		cols = 32;
		break;
	case 8:
		rows = 32;
		cols = 8;
		break;
	}

    m_rowsMax = rows;
    m_numOfRows = rows;
    m_colsMax = cols;
    m_numOfCols = cols;
}

//-----------------------------------------------------------------------------
// initialisation
void MatrixAudioModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	settingsWidthHeaderRows = 1.0f;
	settingsHeightHeaderCols = 1.0f;
    
	settingsHeadersCaptionColor = sdkGetUsineColor(clFontLight);
	settingsHeadersCaptionSize	= 0.4f;

    settingsColorsOrientation = 1;
    resizeSettingsLinesColors();
    
	settingsIsCellsRounded = FALSE;
	settingsCellsRoundedSize = 4.0f; 
	settingsCellsGapSize = 2.0f;
		
	settingsCellsCaptionVertAlign = 0;
	settingsCellsCaptionFontSize = 0.5f;
	settingsCellsCaptionColor = sdkGetUsineColor(clFontLight);
		
	settingsCellsCaptionShow = TRUE;
	settingsFadeModePrecision = 7;
    settingsFadeModeDistance = 50;


	m_mouseMode = mmDraw; // draw mode
	m_mouseLastButtonDown = -1; // no button down
	m_mouseLastCellIndex = -1; // nothing 
	m_mouseLastCoords.x = 0;
	m_mouseLastCoords.y = 0;

    updateMatrixSize();
	updateHeadersCaptions();
	updateCellsColor();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void MatrixAudioModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{
 	// audio inputs/outputs params
	if ( ParamIndex <= m_numOfOptInOut )
	{        
        // the param name
        stringstream Stream; 

        int number = ParamIndex;

        // Audio inputs
        if (number < m_rowsMax)
        {
            Stream << number + 1;
            m_vectOptParamsCaptions.push_back(string("in ") + Stream.str());

            // Audio input
            pParamInfo->ParamType		= ptAudio;
		    pParamInfo->Caption			= (AnsiCharPtr)(m_vectOptParamsCaptions[ParamIndex].c_str());
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->ReadOnly		= FALSE;
			if (number == 0)
			{
				pParamInfo->IsSeparator			= TRUE;
				pParamInfo->SeparatorCaption    = "audio";
			}
        }
        // Audio outputs
        else if ( number < (m_rowsMax + m_colsMax) )
        {
            number -= m_rowsMax;
            Stream << number + 1;
            m_vectOptParamsCaptions.push_back(string("out ") + Stream.str());

            // Audio output
		    pParamInfo->ParamType		= ptAudio;
		    pParamInfo->Caption			= (AnsiCharPtr)(m_vectOptParamsCaptions[ParamIndex].c_str());
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
		    pParamInfo->ReadOnly		= TRUE;
        }
        // Caption inputs 
        else if ( number < ((m_rowsMax*2) + m_colsMax) )
        {
            number -= (m_rowsMax + m_colsMax);
            Stream << number + 1;
            m_vectRowsHeaderCaptions.push_back(string("caption in ") + Stream.str());

			m_vectRowsHeaderTextDefaults.push_back(string("in ") + Stream.str());
            
            // Caption input
		    pParamInfo->ParamType			= ptTextField;
		    pParamInfo->Caption				= (AnsiCharPtr)(m_vectRowsHeaderCaptions[number].c_str());
			pParamInfo->TextValue			= (AnsiCharPtr)(m_vectRowsHeaderTextDefaults[number].c_str());
		    pParamInfo->IsInput				= TRUE;
		    pParamInfo->IsOutput			= FALSE;
		    pParamInfo->ReadOnly			= FALSE;
		    pParamInfo->CallBackType		= ctImmediate;
			pParamInfo->IsStoredInPreset	= FALSE;
			pParamInfo->IsVisibleByDefault	= FALSE;
			if (number == 0)
			{
				pParamInfo->IsSeparator			= TRUE;
				pParamInfo->SeparatorCaption    = "headers";
			}
        }
        // Caption outputs
        else if ( number < ((m_rowsMax*2) + (m_colsMax*2)) )
        {
            number -= (m_rowsMax*2) + m_colsMax;
            Stream << number + 1;
            m_vectColsHeaderCaptions.push_back(string("caption out ") + Stream.str());

			m_vectColsHeaderTextDefaults.push_back(string("out ") + Stream.str());
            
            // Caption output 
		    pParamInfo->ParamType			= ptTextField;
		    pParamInfo->Caption				= (AnsiCharPtr)(m_vectColsHeaderCaptions[number].c_str());
			pParamInfo->TextValue			= (AnsiCharPtr)(m_vectColsHeaderTextDefaults[number].c_str());
		    pParamInfo->IsInput				= TRUE;
		    pParamInfo->IsOutput			= FALSE;
		    pParamInfo->ReadOnly			= FALSE;
		    pParamInfo->CallBackType		= ctImmediate;
			pParamInfo->IsStoredInPreset	= FALSE;
			pParamInfo->IsVisibleByDefault	= FALSE;
        }
	}
	// num of rows Data input
	else if (ParamIndex == m_numOfOptInOut + 1)
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "nb audio in";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 1.0f;
		pParamInfo->MaxValue		= (float)m_rowsMax;
		pParamInfo->DefaultValue	= (float)m_rowsMax;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
		pParamInfo->IsSeparator     = TRUE;
	}
	// num of cols Data output
	else if (ParamIndex == m_numOfOptInOut + 2) 
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "nb audio out";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 1.0f;
		pParamInfo->MaxValue		= (float)m_colsMax;
		pParamInfo->DefaultValue	= (float)m_colsMax;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
	}	
	// mouse mode listbox input
	else if (ParamIndex == m_numOfOptInOut + 3)
	{
		pParamInfo->ParamType		= ptListBox;
		pParamInfo->Caption			= "mouse mode";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ListBoxStrings	= "\"toggle\",\"fade\",\"draw\",\"lock\",\"edit\"";
		pParamInfo->DefaultValue	= 0;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
	}
	// cell draw mode value data fader input
	else if (ParamIndex == m_numOfOptInOut + 4)
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "cell draw val";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1.0f;
		pParamInfo->DefaultValue	= 1.0f;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.5f";
		pParamInfo->ReadOnly		= FALSE;
	}
	// randomize button input
	else if (ParamIndex == m_numOfOptInOut + 5)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "randomize";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->IsSeparator		= TRUE;
	}
	// reset button input
	else if (ParamIndex == m_numOfOptInOut + 6)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "reset";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
	}
    // random coverage coeff
    else if (ParamIndex == m_numOfOptInOut + 7) 
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "random coverage";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= 0.0f;
		pParamInfo->MaxValue		= 1.0f;
		pParamInfo->DefaultValue	= 0.25f;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.2f";
		pParamInfo->ReadOnly		= FALSE;
        pParamInfo->DisplayOrder    = 8;
	}	
	// last clicked cell data field output
	else if (ParamIndex == m_numOfOptInOut + 8)
	{
		pParamInfo->ParamType		    = ptDataField;
		pParamInfo->Caption			    = "last click cell";
		pParamInfo->IsInput			    = FALSE;
		pParamInfo->IsOutput		    = TRUE;
		pParamInfo->DefaultValue	    = -1;
		pParamInfo->Symbol			    = "";
		pParamInfo->Format			    = "%g";
		pParamInfo->ReadOnly		    = TRUE;
		pParamInfo->IsSeparator		    = TRUE;
	}
	// last clicked col data field output
	else if (ParamIndex == m_numOfOptInOut + 9)
	{
		pParamInfo->ParamType		= ptDataField;
		pParamInfo->Caption			= "last click col";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->DefaultValue	= -1;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%g";
		pParamInfo->ReadOnly		= TRUE;
	}
	// last clicked row data field output
	else if (ParamIndex == m_numOfOptInOut + 10)
	{
		pParamInfo->ParamType		= ptDataField;
		pParamInfo->Caption			= "last click row";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->DefaultValue	= -1;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%g";
		pParamInfo->ReadOnly		= TRUE;
	}
	// cells val input of the matrix
	else if (ParamIndex == m_numOfOptInOut + 11)
	{
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "cells val";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
	}
	// cells val output of the matrix
	else if (ParamIndex == m_numOfOptInOut + 12)
	{
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "cells out";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->CallBackType    = ctImmediate;
	}
	else
	{
		// do nothing
	}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void MatrixAudioModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{	
	// optional midi inputs
	if (ParamIndex < m_rowsMax )
	{
		m_tevtOptInputs[ParamIndex] = pEvent;
	}
	// optional midi outputs
	else if ( ParamIndex < (m_rowsMax + m_colsMax))
	{
		m_tevtOptOutputs[ParamIndex - m_rowsMax] = pEvent;
	}
	// captions inputs
	else if ( ParamIndex < ((m_rowsMax*2) + m_colsMax))
	{
		m_tevtOptInputsCaption[ParamIndex - (m_rowsMax + m_colsMax)] = pEvent;
	}
	// captions outputs
	else if ( ParamIndex < ((m_rowsMax*2) + (m_colsMax*2)))
	{
		m_tevtOptOutputsCaption[ParamIndex - ((m_rowsMax*2) + m_colsMax)] = pEvent;
	}
	// num of rows data fader input/output
	else if (ParamIndex == m_numOfOptInOut + 1)
	{
		m_fdrNumOfRows = pEvent;
	}
	// num of cols data fader input/output
	else if (ParamIndex == m_numOfOptInOut + 2) 
	{
		m_fdrNumOfCols = pEvent;
	}
	// mouse mode listbox input
	else if (ParamIndex == m_numOfOptInOut + 3)
	{
		m_lbxMouseMode = pEvent;
	}
	// cell draw mode value data fader input
	else if (ParamIndex == m_numOfOptInOut + 4)
	{
		m_fdrCellDrawModeValue = pEvent;
	}
	// randomize button input
	else if (ParamIndex == m_numOfOptInOut + 5)
	{
		m_btnRandomize = pEvent;
	}
	// reset button input
	else if (ParamIndex == m_numOfOptInOut + 6)
	{
		m_btnReset = pEvent;
	}
    // random coverage coeff
    else if (ParamIndex == m_numOfOptInOut + 7) 
	{
		m_dtfRandomCoverage = pEvent;
	}
	// last clicked cell data field output
	else if (ParamIndex == m_numOfOptInOut + 8)
	{
		m_dtfLastMouseClickCell = pEvent;
	}
	// last clicked col data field output
	else if (ParamIndex == m_numOfOptInOut + 9)
	{
		m_dtfLastMouseClickCol = pEvent;
	}
	// last clicked row data field output
	else if (ParamIndex == m_numOfOptInOut + 10)
	{
		m_dtfLastMouseClickRow = pEvent;
	}
	// input of the matrix
	else if (ParamIndex == m_numOfOptInOut + 11)
	{
		m_arrMatrixInput = pEvent;
	}
	// output of the matrix
	else if (ParamIndex == m_numOfOptInOut + 12)
	{
		m_arrMatrixOutput = pEvent;
	}
	// default case
	else
	{
		// do nothing
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void MatrixAudioModule::onCallBack (UsineMessage *Message) 
{
	// cast for the opt in/out range
	NativeInt NumOfOptInOut = (NativeInt)m_numOfOptInOut;
	
	// headers captions for inputs/outputs 
	if (   (Message->wParam >= (NativeInt)(m_rowsMax + m_colsMax))
		&& (Message->wParam  < (NativeInt)((m_rowsMax*2) + (m_colsMax*2))) 
		&& (Message->lParam == MSG_CHANGE)
		)
	{
        updateHeadersCaptions();
	}
 //   // Opt colors
 //   else if ( (Message->wParam  < (NativeInt)((m_rowsMax*2) + (m_colsMax*2) + max(m_colsMax, m_rowsMax))) && (Message->lParam == MSG_CHANGE))
	//{
	//    // ask to update the datas and repaint the module
	//    updateCellsColor();
	//}
	//// colors for cols(OFF) or rows(ON)
	//else if ( (Message->wParam == NumOfOptInOut + 1) && (Message->lParam == MSG_CHANGE) ) 
	//{
	//	updateCellsColor();
	//}
	// num of rows data input
	else if ( (Message->wParam == NumOfOptInOut + 1) && (Message->lParam == MSG_CHANGE) ) 
	{
        int fdrNumOfRows = (int)(sdkGetEvtData(m_fdrNumOfRows));
		if ( m_numOfRows != fdrNumOfRows )
        {
            // update the total size
		    updateNumOfRows( fdrNumOfRows );
        }
	}
	// num of rows data input
	else if ( (Message->wParam == NumOfOptInOut + 2) && (Message->lParam == MSG_CHANGE) ) 
	{
        int fdrNumOfCols = (int)(sdkGetEvtData(m_fdrNumOfCols));
		if ( m_numOfCols != fdrNumOfCols )
        {
            // update the total size
		    updateNumOfCols( fdrNumOfCols );
        }
	}
	// mouse mode listbox input
	else if ( (Message->wParam == NumOfOptInOut + 3) && (Message->lParam == MSG_CHANGE) ) 
	{
		int newmousemode = static_cast<int>(sdkGetEvtData( m_lbxMouseMode ));		
		setMouseMode(static_cast<TMouseMode>(newmousemode));
        
	}
	// randomize button input 
	else if ( (Message->wParam == NumOfOptInOut + 5) && (Message->lParam == MSG_CHANGE) ) 
	{
		randomizeCells();

	}
	// reset cells 
	else if ( (Message->wParam == NumOfOptInOut + 6) && (Message->lParam == MSG_CHANGE) ) 
	{
		resetCells();

	}
	// input of the matrix
	else if ( (Message->wParam == NumOfOptInOut + 11) && (Message->lParam == MSG_CHANGE) ) 
	{
        whenArrayInChange();
	}
	// output of the matrix
	else if ( (Message->wParam == NumOfOptInOut + 12) && (Message->lParam == MSG_CHANGE) ) 
	{
        whenArrayOutChange();
    }
}

//-----------------------------------------------------------------------------
// Main process
void MatrixAudioModule::onProcess () 
{ 
    // for each output(cols)
    for (int i = 0; i < m_numOfCols; i++)
    {
        UsineEventPtr audioOut = m_tevtOptOutputs[i];
        sdkClearAudioEvt(audioOut);

        // take each input(rows)
        for (int j = 0; j < m_numOfRows; j++)
        {
            UsineEventPtr audioIn = m_tevtOptInputs[j];

            // cell value index
            int valIndex = (m_numOfCols*j) + i;

            float cellValue = m_tabCellsValue[valIndex];
			if ((cellValue == 0) && (m_smoothOldCoeff[valIndex] ==1)) //  
			{
		       sdkAddEvt2(audioIn, audioOut);
			}
			if ((cellValue > 0) || (m_smoothOldCoeff[valIndex] > 0.0f)) //  
			{
				float tmp = std::pow(cellValue, 4.0f);
				sdkSmoothEvent(m_smoothOldCoeff[valIndex], m_tevtSmoothCurrentCoeff, tmp, SMOOTH_SLOW);
				sdkMultEvt3(audioIn, m_tevtSmoothCurrentCoeff, m_tevtAudioMixBuffer);
				sdkAddEvt2(m_tevtAudioMixBuffer, audioOut);
			}
		}
    }
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// // compute the lentgh of the chunk string
int MatrixAudioModule::onGetChunkLen (LongBool Preset) 
{
	// compute size of the cells value array
	int ChunkLen = ( sizeof(char) * 4											// for the version header
		+ sizeof(int)															// num of cols
		+ sizeof(int)															// num of rows
        + sizeof(int) +  (sizeof(float) * m_numOfCols * m_numOfRows)			// cells values
        );
	
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// store module datas in the chunk string
void MatrixAudioModule::onGetChunk (void* chunk, LongBool Preset) 
{
    int sizeValues = m_numOfCols * m_numOfRows;

    int charInt = sizeof(int);
    int charFloat = sizeof(float);

    int charHeader = 4;
    int charValues = charFloat * sizeValues;
	
	int offset = 0;
	// -----------------------------------------------------------------------
	// HEADER section
    // copy the header code into the chunk
	memcpy( chunk
        , (void*)CHUNCK_MA20
        , charHeader 
        );

	offset += charHeader;
	// -----------------------------------------------------------------------
	// Values section
	// copy the num of cols into the chunk
	memcpy( (void*)((char*)chunk + offset)
        , (void*)&m_numOfCols
        , charInt 
        );
	offset += charInt;
	// copy the num of rows into the chunk
	memcpy( (void*)((char*)chunk + offset)
        , (void*)&m_numOfRows
        , charInt 
        );
	offset += charInt;
	// copy cells value array into the chunk
	memcpy( (void*)((char*)chunk + offset)
        , (void*)m_tabCellsValue
        , charValues 
        );
	offset += charValues;
}

//-----------------------------------------------------------------------------
// restore module datas from the chunk string
void MatrixAudioModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset)
{
    int charInt = sizeof(int);
    int charFloat = sizeof(float);
  
    int charHeader = 4;

    char headerChunk[4];
	
	int offset = 0;
	// extract the header code from the chunk
	memcpy( (void*)headerChunk
        , (void*)((char*)chunk)
        , charHeader 
        );
	offset += charHeader;
    //chunk was saved with version 1.1
    if ( string(CHUNCK_MA20, 4) == string(headerChunk, 4) )
    {
        int numOfCols;
	    // extract the values array size from the chunk
	    memcpy( (void*)&numOfCols
               , (void*)((char*)chunk + offset)
               , charInt
               );
		offset += charInt;
        
        int numOfRows;
	    // extract the values array size from the chunk
	    memcpy( (void*)&numOfRows
               , (void*)((char*)chunk + offset)
               , charInt
               );
		offset += charInt;
        
		sdkSetEvtData(m_fdrNumOfCols, (float)numOfCols);
		updateNumOfCols(numOfCols);
		sdkSetEvtData(m_fdrNumOfRows, (float)numOfRows);
		updateNumOfRows(numOfRows);
        
        int charValues = charFloat * (numOfCols * numOfRows);
		
	    // copy cells value array from the chunk
	    memcpy( (void*)m_tabCellsValue
               , (void*)((char*)chunk + offset)
               , charValues
               );
		offset += charValues;
	
        // ask to update the datas and repaint the module
        updateCellsCaptions();
		
	    updateCellsDatas();
		updateCellsColor();
        
	}
    //chunk was saved with version 1.1
    else if ( string(CHUNCK_MA11, 4) == string(headerChunk, 4) )
    {
        int numOfCols;
	    // extract the values array size from the chunk
	    memcpy( (void*)&numOfCols
            , (void*)((char*)chunk + offset)
            , charInt 
            );
		offset += charInt;

        int numOfRows;
	    // extract the values array size from the chunk
	    memcpy( (void*)&numOfRows
            , (void*)((char*)chunk + offset)
            , charInt 
            );
		offset += charInt;

		sdkSetEvtData(m_fdrNumOfCols, (float)numOfCols);
		updateNumOfCols(numOfCols);
		sdkSetEvtData(m_fdrNumOfRows, (float)numOfRows);
		updateNumOfRows(numOfRows);

        int charValues = charFloat * (numOfCols * numOfRows);
		
	    // copy cells value array from the chunk
	    memcpy( (void*)m_tabCellsValue
            , (void*)((char*)chunk + offset) 
            , charValues 
            );
		offset += charValues;
	
        // ask to update the datas and repaint the module
        updateCellsCaptions();
		
	    updateCellsDatas();
		updateCellsColor();

	}
}

//-----------------------------------------------------------------------------
// Global Randomize
void MatrixAudioModule::onRandomize ()
{
	// randomize cells value 
	randomizeCells();
}

//-----------------------------------------------------------------------------
// Global Reset
void MatrixAudioModule::onReset()
{
	// reset cells value 
	resetCells();
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void MatrixAudioModule::onCreateSettings ()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "headers");
	sdkAddSettingLineSingle (DESIGN_TAB_NAME, &(settingsWidthHeaderRows), "inputs header width", 0, 1.5f, scLinear, "%", DEFAULT_FORMAT_FLOAT_2, 1.0f);
	sdkAddSettingLineSingle (DESIGN_TAB_NAME,  &(settingsHeightHeaderCols), "outputs header height", 0, 1.5f, scLinear, "%", DEFAULT_FORMAT_FLOAT_2, 1.0f);
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsHeadersCaptionSize), "header caption size", 0.0f, 1.0f, scLinear, "%", DEFAULT_FORMAT_FLOAT_2, 0.05f);
	sdkAddSettingLineColor   (DESIGN_TAB_NAME, &(settingsHeadersCaptionColor), "header caption color");
	
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cells");
	sdkAddSettingLineBoolean (DESIGN_TAB_NAME, &(settingsIsCellsRounded), "cells rounded");
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsRoundedSize), "cells round size", 0.0f, 200.0f, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 5.0f);
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsGapSize), "cells gap", 0.0f, 800.0f, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 5.0f);
                             
    sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cells colors");
	sdkAddSettingLineCombobox (DESIGN_TAB_NAME, &(settingsColorsOrientation), "orientation", "\"vertical\",\"horizontal\"");

        TColorUsine* ptrOn = settingsLinesColorsOn.data();
    for (int i = 0; i < numOfLinesColors; i++)
    {
        sdkAddSettingLineColor (DESIGN_TAB_NAME, ptrOn, (AnsiCharPtr)nameLinesColorsOn[i].c_str());
        ++ptrOn;
    }
    TColorUsine* ptrOff = settingsLinesColorsOff.data();
    for (int i = 0; i < numOfLinesColors; i++)
    {
        sdkAddSettingLineColor (DESIGN_TAB_NAME, ptrOff, (AnsiCharPtr)nameLinesColorsOff[i].c_str());
        ++ptrOff;
    }
	
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cells caption");
	sdkAddSettingLineBoolean (DESIGN_TAB_NAME, &(settingsCellsCaptionShow), "show dB value");
	sdkAddSettingLineCombobox (DESIGN_TAB_NAME, &(settingsCellsCaptionVertAlign), "vertical align", "\"center\",\"top\",\"bottom\"");
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsCaptionFontSize), "cells caption size", 0.0f, 1.0f, scLinear, "%", DEFAULT_FORMAT_FLOAT_2, 0.05f);
	sdkAddSettingLineColor   (DESIGN_TAB_NAME, &(settingsCellsCaptionColor), "cells caption color");
	
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "mouse precision");
	sdkAddSettingLineInteger (DESIGN_TAB_NAME, &(settingsFadeModeDistance), "fade distance", 10, 200, scLinear, "pix", 50 );
	sdkAddSettingLineInteger (DESIGN_TAB_NAME, &(settingsFadeModePrecision), "SHIFT fade factor", 1, 10, scLinear, "coeff", 7);
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redraw
void MatrixAudioModule::onSettingsHasChanged()
{
	updateSettingsParams();
} 

void MatrixAudioModule::onResize (float contentWidth, float contentHeight)
{
	updateCellsDatas();
}

void MatrixAudioModule::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit", TRUE);
    sdkAddCommand("cmd_reset", m_numOfOptInOut + 6, TRUE);
    sdkAddCommand("cmd_randomize", m_numOfOptInOut + 5, TRUE);
}

//-----------------------------------------------------------------------------
// paint the module panel
void MatrixAudioModule::onPaint () 
{
	updateCellsDatas();

	// compute variables before the loop
	unsigned int CellIndex;
	int c = 0;
	int r = 0;
	int cols = m_numOfCols;
	int rows = m_numOfRows;
	//unsigned int sizeVectorCellsCaption;


	for (c = 0; c<cols; c++)
	{
		for (r = 0; r<rows; r++)
		{
			CellIndex = ((c * (int)rows) + r );

			// draw cols header caption
			if (r == 0 && settingsHeightHeaderCols > 0)
			{ 
				sdkFillText( m_tabOutCaptionsGuiData[c].dim, (AnsiCharPtr)(m_vectColsHeaderTextDefaults[c].c_str()), settingsHeadersCaptionColor, m_tabOutCaptionsGuiData[c].captionSize, TRUE, TRUE);
			}		
			// draw row header caption
			if (c == 0 && settingsWidthHeaderRows > 0)
			{
				sdkFillText( m_tabInCaptionsGuiData[r].dim, (AnsiCharPtr)(m_vectRowsHeaderTextDefaults[r].c_str()), settingsHeadersCaptionColor, m_tabInCaptionsGuiData[r].captionSize,  TRUE);
			}	
				
			// draw the cell
			if (settingsIsCellsRounded != FALSE)
			{
				sdkFillRect(m_tabCellsGuiData[CellIndex].dim, m_tabCellsGuiData[CellIndex].color, m_tabCellsGuiData[CellIndex].cornerRadius, m_tabCellsGuiData[CellIndex].color, 0);
			}
			else
			{
				sdkFillRect(m_tabCellsGuiData[CellIndex].dim, m_tabCellsGuiData[CellIndex].color, 0, m_tabCellsGuiData[CellIndex].color, 0);
			}
				
			// draw cell caption
			if (   (settingsCellsCaptionShow != FALSE)  
				&& (m_mouseMode == mmFade)
                && m_vectCellsCaptions[CellIndex].compare(" ") != 0
				)	
			{
				sdkFillText( m_tabCellsGuiData[CellIndex].dim, (AnsiCharPtr)(m_vectCellsCaptions[CellIndex].c_str()), m_tabCellsGuiData[CellIndex].captionColor, m_tabCellsGuiData[CellIndex].captionSize, FALSE, FALSE, taCenter, m_tabCellsGuiData[CellIndex].alignVertical);
			}
		}
	}
}

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MouseMove callback
void MatrixAudioModule::onMouseMove (TShiftState Shift, float X, float Y)
{
	if ((m_mouseMode != mmLock) && (m_mouseMode != mmEdit) && (m_mouseLastButtonDown == mbLeft) )
	{	
		// so, mouse is dragging
		if ((m_mouseLastCellIndex >= 0) && (m_mouseMode == mmFade))
		{
			mouseFadeCell( m_mouseLastCellIndex, Shift, X,Y);
		}
		else if (m_mouseMode == mmDraw)
		{
			int NewCellIndex = foundNewMouseCellIndex(X, Y);

			if ( (NewCellIndex >= 0)&& (NewCellIndex !=  m_mouseLastCellIndex) )
			{
				mouseToggleCell( NewCellIndex);
			}

			m_mouseLastCellIndex = NewCellIndex; 
		}

		m_mouseLastCoords.x = X;
		m_mouseLastCoords.y = Y;
	}
}

//-----------------------------------------------------------------------------
// MouseDown callback
void MatrixAudioModule::onMouseDown (TMouseButton Button, TShiftState Shift, float X, float Y)
{
	// befor all,
    if (m_mouseMode == mmEdit)
	{	
		//int NewCellIndex = foundNewMouseCellIndex(X, Y);
    }
    else if ((m_mouseMode != mmLock) && (Button == mbLeft) )
	{	
		int NewCellIndex = foundNewMouseCellIndex(X, Y);

        if (NewCellIndex >= 0 && ((m_mouseMode == mmToggle) || (m_mouseMode == mmDraw)))
		{	
			mouseToggleCell( NewCellIndex );
		}
		m_mouseLastCellIndex = NewCellIndex; 
		m_mouseLastButtonDown = (int)mbLeft;	
		m_mouseLastCoords.x = X;
		m_mouseLastCoords.y = Y;
	}
}

//-----------------------------------------------------------------------------
// MouseUp callback
void MatrixAudioModule::onMouseUp (TMouseButton Button, TShiftState Shift, float X,float Y)
{
	if (m_mouseLastButtonDown == mbLeft)
	{
		// mouse is no more down
		m_mouseLastButtonDown = -1;
		m_mouseLastCellIndex = -1;
	}
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// mouse mode setter
void MatrixAudioModule::setMouseMode(TMouseMode Mode)
{
	m_mouseMode = Mode;

    if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
        updateCellsCaptions();

    // finally ask a repaint
	sdkRepaintPanel();
};

//-------------------------------------------------------------------------
// 
void MatrixAudioModule::updateHeadersCaptions()
{
	int i;

	// inputs rows header 
    for (i = 0; i < m_numOfRows; i++)
	{   
		m_vectRowsHeaderTextDefaults[i] = sdkGetEvtPChar( m_tevtOptInputsCaption[i] );
    }

	// outputs cols header 
    for (i = 0; i < m_numOfCols; i++)
	{   
		m_vectColsHeaderTextDefaults[i] = sdkGetEvtPChar( m_tevtOptOutputsCaption[i] );
    }
    // finally ask a repaint
	sdkRepaintPanel();
};

//-------------------------------------------------------------------------
//
void MatrixAudioModule::updateSettingsParams()
{	
	updateCellsColor();

	if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
		updateCellsCaptions();

		updateCellsDatas();
}

//-------------------------------------------------------------------------
//
void MatrixAudioModule::updateMatrixSize()
{
	int newsize = m_numOfCols * m_numOfRows;
	sdkSetEvtSize( m_arrMatrixOutput, newsize );
};
	
//-------------------------------------------------------------------------
//
void MatrixAudioModule::updateNumOfCols(int NewNumOfCols)
{
	// have to reorganize m_tabCellsValue to preserve cells placement
    int addcol = NewNumOfCols - m_numOfCols;
	int newsize = NewNumOfCols * m_numOfRows;

	sdkSetEvtSize( m_arrMatrixOutput, newsize );

    // 
    if (addcol < 0)
    {
        for (int r = 1; r < m_numOfRows; r++)
        {
            memcpy((void*)(m_tabCellsValue + (NewNumOfCols * r)), (void*)(m_tabCellsValue + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
			memcpy((void*)(m_smoothOldCoeff + (NewNumOfCols * r)), (void*)(m_smoothOldCoeff + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
		}
			
		// for each output(cols)
		for (int i = NewNumOfCols; i < m_numOfCols; i++)
		{
			UsineEventPtr audioOut = m_tevtOptOutputs[i];
			sdkClearAudioEvt(audioOut);
		}	
	}
    else
    {
		// found the reset value
		float ResetValue = 0.0f; 

        for (int r = m_numOfRows - 1; r > 0; r--)
        {
            memcpy((void*)(m_tabCellsValue + (NewNumOfCols * r)), (void*)(m_tabCellsValue + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
            memcpy((void*)(m_smoothOldCoeff + (NewNumOfCols * r)), (void*)(m_smoothOldCoeff + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
            for (int c = 0; c < addcol; c++)
			{
				m_tabCellsValue[(m_numOfCols * r)  + c] = ResetValue;
				m_smoothOldCoeff[(m_numOfCols * r)  + c] = 0.0f;
			}
		}
    }
    m_numOfCols = NewNumOfCols;

	if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
		updateCellsCaptions();

  	// ask to update the datas and repaint the module
	updateCellsDatas();
	updateCellsValues();
	updateCellsColor();
};	

//-------------------------------------------------------------------------
//
void MatrixAudioModule::updateNumOfRows(int NewNumOfRows)
{
	int newsize = m_numOfCols * NewNumOfRows;
    m_numOfRows = NewNumOfRows;

	// just size modification
	sdkSetEvtSize( m_arrMatrixOutput, newsize );

	if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
		updateCellsCaptions();

  	// ask to update the datas and repaint the module
	updateCellsDatas();
	updateCellsValues();
	updateCellsColor();
};

//-------------------------------------------------------------------------
// reset the matrix to reset value
void MatrixAudioModule::resetCells()
{		
	// found the reset value
	//float ResetValue = 0.0f; //sdkGetEvtData(m_dtfResetValue);
	//int size = m_numOfCols * m_numOfRows;

	// reset cells datas
    memset(m_tabCellsValue, 0, (sizeof(float) * MATRIX_SIZE_MAX));
        
    if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
	    updateCellsCaptions();
		  		
	updateCellsValues();
    sdkNeedRemoteUpdate (m_numOfOptInOut + 12);
	updateCellsColor();
    
};

//-------------------------------------------------------------------------
// randomize the matrix
void MatrixAudioModule::randomizeCells()
{
	float CellMaxValue = (float)sdkGetEvtData(m_fdrCellDrawModeValue);
	float RandomCoverage = (float)sdkGetEvtData(m_dtfRandomCoverage);

	// randomize with 0 to cell draw value
	if (m_mouseMode == mmFade)
	{
		int size = m_numOfCols * m_numOfRows;
			
		for (int i = 0; i < size; i++)
		{
            if ( RandomCoverage >= (rand() / (float)(RAND_MAX)) )
			    m_tabCellsValue[i] =  CellMaxValue * (rand() / (float)(RAND_MAX));
            else
                m_tabCellsValue[i] = 0;
		}
        if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
	        updateCellsCaptions();
	}
	else
	{
		int size = m_numOfCols * m_numOfRows;
			
		for (int i = 0; i < size; i++)
		{	
            if ( RandomCoverage >= (rand() / (float)(RAND_MAX)) )
			    m_tabCellsValue[i] =  (rand() / (float)(RAND_MAX)) > 0.5f ? CellMaxValue : 0;
            else
                m_tabCellsValue[i] = 0;
		}
	}
    
	updateCellsValues();
    sdkNeedRemoteUpdate (m_numOfOptInOut + 12);
	updateCellsColor();
};

//-------------------------------------------------------------------------
// update each cell color ON data
void MatrixAudioModule::updateCellsColor()
{
    int CellIndex;

    if (settingsColorsOrientation == 0)
    {
		for (int r = 0; r<m_numOfRows; r++)
		{
			for (int c = 0; c<m_numOfCols; c++)
			{
				CellIndex = ((r * (int)m_numOfCols) + c );
                
			    m_tabCellsColorsOn[CellIndex] = settingsLinesColorsOn[c % numOfLinesColors];
			    m_tabCellsColorsOff[CellIndex] = settingsLinesColorsOff[c % numOfLinesColors];
				setCellsGuiColor(CellIndex);
            }
        }
    }
    else
    {
		for (int r = 0; r<m_numOfRows; r++)
		{
			for (int c = 0; c<m_numOfCols; c++)
			{
				CellIndex = ((r * (int)m_numOfCols) + c );
                
			    m_tabCellsColorsOn[CellIndex] = settingsLinesColorsOn[r % numOfLinesColors];
			    m_tabCellsColorsOff[CellIndex] = settingsLinesColorsOff[r % numOfLinesColors];
				setCellsGuiColor(CellIndex);
            }
        }
    }
	sdkRepaintPanel();
}	

//-------------------------------------------------------------------------
// update datas and UI infos for each cell
void MatrixAudioModule::updateCellsCaptions()
{
    int sizeMatrix = m_numOfCols * m_numOfRows;
 
    for (int i = 0; i < sizeMatrix; i++)
	{    
		constructCellCaption(i);
    }
};

//-------------------------------------------------------------------------
// update datas and UI infos for each cell
void MatrixAudioModule::updateCellsDatas()
{
	int cellIndex;
	int c = 0;
	int r = 0;
	float cols = (float)m_numOfCols;
	float rows = (float)m_numOfRows;

    float offsetWidth = 0;
    float offsetHeight = 0;

	TRectF cellDim;
			
	float gapW = sdkPixelToWidthPercent((float)settingsCellsGapSize);
	float gapH = sdkPixelToHeightPercent((float)settingsCellsGapSize);
    float headerRowsWidth  =  settingsWidthHeaderRows * (1.0f / (float)(m_numOfCols + 1));
    float headerColsHeight =  settingsHeightHeaderCols * (1.0f / (float)(m_numOfRows + 1));
	float headerCaptionSize = 0;

	float width  = (1.0f - headerRowsWidth  - (gapW * (cols-1))) / cols;
	float height = (1.0f - headerColsHeight - (gapH * (rows-1))) / rows;

	for (r = 0; r<rows; r++)
	{
		for (c = 0; c<cols; c++)
		{
			cellIndex = ((r * (int)cols) + c );
			offsetWidth = c*(width + gapW);
            offsetHeight = r*(height + gapH);


			cellDim.left	= headerRowsWidth  + offsetWidth;
            cellDim.top		= headerColsHeight + offsetHeight;
            cellDim.right	= headerRowsWidth  + offsetWidth + width;
            cellDim.bottom	= headerColsHeight + offsetHeight + height;
					
			m_tabCellsGuiData[cellIndex].dim = cellDim;
			m_tabCellsGuiData[cellIndex].cornerRadius = sdkPixelToHeightPercent(settingsCellsRoundedSize);
			m_tabCellsGuiData[cellIndex].captionColor = settingsCellsCaptionColor;
			m_tabCellsGuiData[cellIndex].alignVertical = (TTextAlign)settingsCellsCaptionVertAlign;

            if (sdkHeightPercentToPixel(cellDim.bottom - cellDim.top) >= sdkWidthPercentToPixel(cellDim.right - cellDim.left))
			     m_tabCellsGuiData[cellIndex].captionSize = sdkPixelToHeightPercent(settingsCellsCaptionFontSize * sdkWidthPercentToPixel(cellDim.right - cellDim.left));
            else
		         m_tabCellsGuiData[cellIndex].captionSize = sdkPixelToHeightPercent(settingsCellsCaptionFontSize * sdkHeightPercentToPixel(cellDim.bottom - cellDim.top));

            // header caption size
            if (r == 0 && c == 0)
            {                 
                if (sdkHeightPercentToPixel(cellDim.bottom - cellDim.top) >= sdkWidthPercentToPixel(cellDim.right - cellDim.left))
			        headerCaptionSize = sdkPixelToHeightPercent(settingsHeadersCaptionSize * sdkWidthPercentToPixel(cellDim.right - cellDim.left));
                else
		            headerCaptionSize = sdkPixelToHeightPercent(settingsHeadersCaptionSize * sdkHeightPercentToPixel(cellDim.bottom - cellDim.top));              
            }
            
            // cols(outputs) header captions
            if (r == 0/* && (HeightHeaderCols > 0.05)*/)
            {
                cellDim.left	= headerRowsWidth + offsetWidth - (gapW/2.0f);
                cellDim.top		= 0;
                cellDim.right	= headerRowsWidth + offsetWidth + width + (gapW/2.0f);
                cellDim.bottom	= headerColsHeight;
						
                m_tabOutCaptionsGuiData[c].dim = cellDim;
                m_tabOutCaptionsGuiData[c].captionSize = headerCaptionSize;
            }

            // rows(inputs) header captions
            if (c == 0/* && (WidthHeaderRows > 0.05)*/)
            {
                cellDim.left	=  0;
                cellDim.top		=  headerColsHeight + offsetHeight - (gapH/2.0f);
                cellDim.right	=  headerRowsWidth;
                cellDim.bottom	=  headerColsHeight + offsetHeight + height + (gapH/2.0f);
						
                m_tabInCaptionsGuiData[r].dim = cellDim;
                m_tabInCaptionsGuiData[r].captionSize  = headerCaptionSize;
            }
		}
	}
}
		
//
void MatrixAudioModule::updateCellsValues ()
{
	int size = m_numOfCols * m_numOfRows;
            
    // copy cells value to the output array and adjust his size
    // problem using sdkGetEvtDataAddr causing exceptions
    //memcpy( sdkGetEvtDataAddr(m_arrMatrixOutput), (void*)m_tabCellsValue, (sizeof(float) * size) );           
    //sdkSetEvtSize( m_arrMatrixOutput, size );
    
    for (int i = 0; i < size; i++)
        sdkSetEvtArrayData (m_arrMatrixOutput, i, m_tabCellsValue[i]);
}

//
void MatrixAudioModule::whenArrayInChange ()
{	
	int sizeMatrix = std::min(m_numOfCols * m_numOfRows, sdkGetEvtSize(m_arrMatrixInput));
		
	updateCellsValues();

	// copy new threshold datas
    for (int i = 0; i < sizeMatrix; i++)
	{   
		TPrecision val = sdkGetEvtArrayData(m_arrMatrixInput, i);
		if (val != m_tabCellsValue[i])
		{
			m_tabCellsValue[i] = val;
			if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
				constructCellCaption(i);

			setCellsGuiColor(i);
		}
    }
	sdkRepaintPanel();
}
	
//
void MatrixAudioModule::whenArrayOutChange ()
{
	// copy new threshold datas
	memcpy(
        m_tabCellsValue, 
        sdkGetEvtDataAddr(m_arrMatrixOutput), 
        (sizeof(float) * sdkGetEvtSize(m_arrMatrixOutput) )
        );

	if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
		updateCellsCaptions();

  	// ask to update the datas and repaint the module
	updateCellsColor();
	updateCellsDatas();

};

//
void MatrixAudioModule::resizeSettingsLinesColors ()
{
	numOfLinesColors = max(m_colsMax, m_rowsMax);
    settingsLinesColorsOn.resize(numOfLinesColors, sdkGetUsineColor(clAudioFlow));
    settingsLinesColorsOff.resize(numOfLinesColors, sdkAlphaColor(0, 0, 0));

    //stringstream Stream; 
    for (int i = 0; i < numOfLinesColors; i++)
    {
        stringstream onStream;
        onStream << "color on " << i + 1;
        nameLinesColorsOn.push_back(onStream.str()); 
        
        stringstream offStream;
        offStream << "color off " << i + 1;
        nameLinesColorsOff.push_back(offStream.str()); 
    }
}

//
int MatrixAudioModule::foundNewMouseCellIndex ( float X, float Y)
{
	if (X < 0 || Y < 0 || X > 1 || Y > 1)
    { 
		sdkSetEvtData( m_dtfLastMouseClickCol, -1.0f);
		sdkSetEvtData( m_dtfLastMouseClickRow, -1.0f);
        return -1;
    }

    float headerRowsWidth  =  settingsWidthHeaderRows * (1.0f / (float)(m_numOfCols + 1));
    float headerColsHeight =  settingsHeightHeaderCols * (1.0f / (float)(m_numOfRows + 1));
		
	float cellwidth  = (1.0f - headerRowsWidth) / (float)(m_numOfCols);
	float cellheight = (1.0f - headerColsHeight) / (float)(m_numOfRows);

	int NewCellCol = (int)((X - headerRowsWidth) / cellwidth);
	int NewCellRow = (int)((Y - headerColsHeight) / cellheight);
		
	int NewCellIndex = (NewCellRow * m_numOfCols) + NewCellCol;	

    if (NewCellIndex <= ((m_numOfCols * m_numOfRows) - 1))
	{
		sdkSetEvtData( m_dtfLastMouseClickCol, (float)(NewCellCol));
		sdkSetEvtData( m_dtfLastMouseClickRow, (float)(NewCellRow));
		sdkSetEvtData( m_dtfLastMouseClickCell, (float)((NewCellRow * m_numOfCols) + NewCellCol));
	}
	else
	{
		NewCellIndex = -1;
		sdkSetEvtData( m_dtfLastMouseClickCol, -1.0f);
		sdkSetEvtData( m_dtfLastMouseClickRow, -1.0f);
		sdkSetEvtData( m_dtfLastMouseClickCell, -1.0f);
	}
		
	return NewCellIndex;
};

//
void MatrixAudioModule::mouseToggleCell( int CellIndex)
{
	float Threshold = (float)sdkGetEvtData( m_fdrCellDrawModeValue);
	float NewThreshold = 0;

	if (m_tabCellsValue[CellIndex] == Threshold)
	{
		NewThreshold = 0;
	}
	else
	{					
		NewThreshold = Threshold;
	}	

	// update cells and output array
	m_tabCellsValue[CellIndex] = NewThreshold;
	sdkSetEvtArrayData( m_arrMatrixOutput, CellIndex, NewThreshold );
    sdkNeedRemoteUpdate (m_numOfOptInOut + 12);

	setCellsGuiColor( CellIndex);		


	// finally ask a repaint
	sdkRepaintPanel();
};

//
void MatrixAudioModule::mouseFadeCell( int CellIndex, TShiftState Shift, float X, float Y)
{
	float DeltaY = m_mouseLastCoords.y - Y;
	float Coeff = sdkPixelToHeightPercent ((float)settingsFadeModeDistance);

	if ( Shift & ssShift)
	{
		Coeff = Coeff * settingsFadeModePrecision;
	}
		
	float NewThreshold = std::max( 0.0f, std::min( 1.0f, m_tabCellsValue[CellIndex] + (DeltaY / Coeff)));

	// update cell and output array
	m_tabCellsValue[CellIndex] = NewThreshold;
	sdkSetEvtArrayData( m_arrMatrixOutput, CellIndex, NewThreshold );
    sdkNeedRemoteUpdate (m_numOfOptInOut + 12);
					
	constructCellCaption(CellIndex);
	setCellsGuiColor( CellIndex);	
    
	// finally ask a repaint
	sdkRepaintPanel();
};

// deduce the color from a cell value
void MatrixAudioModule::setCellsGuiColor(int index)
{
	TColorArgb ColorSlotOnARGB = sdkAlphaColorToColorArgb(m_tabCellsColorsOn[index]);
	TColorArgb ColorSlotOffARGB = sdkAlphaColorToColorArgb(m_tabCellsColorsOff[index]);

	float Thresh = m_tabCellsValue[index];
        
    // to introduce a color level between = 0 and > 0
	float offLevel = Thresh > 0.0f ? (0.4f + (Thresh*0.6f)) : 0.0f;

	TColorUsine tmp = sdkAlphaColor(
		(ColorSlotOnARGB.r * Thresh) + (ColorSlotOffARGB.r * (1 - offLevel)),
		(ColorSlotOnARGB.g * Thresh) + (ColorSlotOffARGB.g * (1 - offLevel)),
		(ColorSlotOnARGB.b * Thresh) + (ColorSlotOffARGB.b * (1 - offLevel)),
		(ColorSlotOnARGB.a * Thresh) + (ColorSlotOffARGB.a * (1 - offLevel))
		);
	
	m_tabCellsGuiData[index].color = tmp; 

};

// construct a string from a cell value
void MatrixAudioModule::constructCellCaption(int CellIndex)
{
	m_vectCellsCaptions[CellIndex].clear();
	float val = sdkCoeffToDb(powf(m_tabCellsValue[CellIndex], 4));

	if (val > -200.0f && val < -1.0f)
	{
		m_vectCellsCaptions[CellIndex] = sdkIntToString((int)val);
	}
	else
	{
		m_vectCellsCaptions[CellIndex] = "";
	}
}
