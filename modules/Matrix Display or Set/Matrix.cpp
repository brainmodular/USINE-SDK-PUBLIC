//----------------------------------------------------------------------------
//@file  
//	Matrix.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the TMatrixModule class.
//
//  A module to manipulate an array from a matrix point of view
//  the array contain the value off each cell (column * row).
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
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "Matrix.h"
#include <string>
#include <sstream>
#include <vector>

using namespace std;

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TMatrixModule();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((TMatrixModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Matrix";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Matrix - Display or Set";
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
//-----------------------------------------------------------------------
TMatrixModule::TMatrixModule () 
{	
    m_numOfCols = MATRIX_COLS_DEFAULT;
	m_numOfRows = MATRIX_ROWS_DEFAULT;

	// equivalent to set each pointer to NULL
	memset(m_fdrOptCellValue, 0, (sizeof(UsineEventPtr) * OPT_PARAMS_MAX));

	// populate tabs with 0
	memset(m_tabCellsGuiData, 0, (sizeof(TCellGuiData) * MATRIX_SIZE_MAX));
	memset(m_tabCellsValue, 0, (sizeof(float) * MATRIX_SIZE_MAX));
     
	// important to set to size max to prevent a resize during use 
    // vector resize isn't multi thread safe, cause conflict with the graphic thread
    m_vectCellsCaptions.resize(MATRIX_SIZE_MAX, " ");
}

	// destructor
TMatrixModule::~TMatrixModule () 
{
}

//-----------------------------------------------------------------------------
// module description
void TMatrixModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor (clInterfaceDesignModuleColor);
	// pModuleInfo->NumberOfParams is supplied by the query system
	pModuleInfo->DefaultWidth		= MODULE_DIM_DEFAULT;
	pModuleInfo->DefaultHeight		= MODULE_DIM_DEFAULT;
	pModuleInfo->DontProcess		= TRUE;
    pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->CanBeReset         = TRUE;

	// for optional faders
	pModuleInfo->QueryString		= "Number of faders \?";
	pModuleInfo->QueryListValues	= "\"no fader\",\"16 faders\",\"32 faders\",\"64 faders\",\"128 faders\"";
	pModuleInfo->QueryDefaultIdx	= 1;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Get total parameters number of the module
int TMatrixModule::onGetNumberOfParams( int queryIndex)
{	
	int NumOfFaders = 0;
	int result = 0;

	switch (queryIndex)
	{
	case 1:
		NumOfFaders = 16;
		break;
	case 2:
		NumOfFaders = 32;
		break;
	case 3:
		NumOfFaders = 64;
		break;
	case 4:
		NumOfFaders = 128;
		break;
	default:
		break;
	}

	m_numOfOptFaders = NumOfFaders;
	result =  NumOfFaders + OPT_FADERS_SHIFT;

	return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void TMatrixModule::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
}

//-----------------------------------------------------------------------------
// initialisation
void TMatrixModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
    
    settingsColorsOrientation = 1;
    resizeSettingsLinesColors();

	// edit layout params
	settingsIsCellsRounded = FALSE;
	settingsCellsRoundedSize = 4.0f; 
	settingsCellsGapSize = 2.0f;

    settingsCursorOrientation = 0;
    settingsCursorColor = sdkGetUsineColor(clSelected);
    posCursor = 0;
    
	settingsCellsCaptionVertAlign = 0;
	settingsCellsCaptionHorizAlign = 0;
	settingsCellsCaptionFontSize = 0.5f;
	settingsCellsCaptionColor = sdkGetUsineColor(clFontLight);
		
	settingsFadeModePrecision = 7;
    settingsFadeModeDistance = 50;
		
	m_mouseMode = mmDraw;		// draw mode
	m_mouseLastButtonDown = -1; // no button down
	m_mouseLastCellIndex = -1;	// nothing 
	m_mouseLastCoords.x = 0;
	m_mouseLastCoords.y = 0;

    // set default matrix size and ask to repaint the module
    updateMatrixSize();
	updateCellsColor();
	updateCellsDatas();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void TMatrixModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{
	// matrix size data field output
	if (ParamIndex == 0)
	{
		pParamInfo->ParamType		    = ptDataField;
		pParamInfo->Caption			    = "matrix size";
		pParamInfo->IsInput			    = FALSE;
		pParamInfo->IsOutput		    = TRUE;
		pParamInfo->DefaultValue	    = MATRIX_SIZE_DEFAULT;
		pParamInfo->Symbol			    = "";
		pParamInfo->Format			    = "%g";
		pParamInfo->ReadOnly		    = TRUE;
	}    
	// num of cols data fader input/output
	if (ParamIndex == 1) 
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "num of cols";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 1.0f;
		pParamInfo->MaxValue		= MATRIX_COLS_MAX;
		pParamInfo->DefaultValue	= MATRIX_COLS_DEFAULT;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
	}	
	// num of rows data fader input/output
	else if (ParamIndex == 2)
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "num of rows";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 1.0f;
		pParamInfo->MaxValue		= MATRIX_ROWS_MAX;
		pParamInfo->DefaultValue	= MATRIX_ROWS_DEFAULT;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
	}
	// mouse mode listbox input
	else if (ParamIndex == 3)
	{
		pParamInfo->ParamType		= ptListBox;
		pParamInfo->Caption			= "mouse mode";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ListBoxStrings	= "\"toggle\",\"fade\",\"draw\",\"lock\",\"edit\"";
		pParamInfo->DefaultValue	= 0;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType	= ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
	}
	// cell draw mode value data fader input
	else if (ParamIndex == 4)
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
	else if (ParamIndex == 5)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "randomize";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
        pParamInfo->IsSeparator     = TRUE;
	}
	// reset button input
	else if (ParamIndex == 6)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "reset";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
	}
    // random coverage coeff
    else if (ParamIndex == 7) 
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
	}	
	// value for reset input
	else if (ParamIndex == 8)
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "reset val";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1.0f;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.5f";
		pParamInfo->ReadOnly		= FALSE;
	}
	// last clicked cell data field output
	else if (ParamIndex == 9)
	{
		pParamInfo->ParamType		    = ptDataField;
		pParamInfo->Caption			    = "last click cell";
		pParamInfo->IsInput			    = FALSE;
		pParamInfo->IsOutput		    = TRUE;
		pParamInfo->DefaultValue	    = -1;
		pParamInfo->Symbol			    = "";
		pParamInfo->Format			    = "%g";
		pParamInfo->ReadOnly		    = TRUE;
        pParamInfo->IsSeparator         = TRUE;
	}
	// last clicked col data field output
	else if (ParamIndex == 10)
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
	else if (ParamIndex == 11)
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
	else if (ParamIndex == 12)
	{
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "cells val";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
	}
	// cells val output of the matrix
	else if (ParamIndex == 13)
	{
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "cells out";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType    = ctImmediate;
	}
    // position of the cel surrouding cursor
	if (ParamIndex == 14)
	{
		pParamInfo->ParamType		    = ptDataFader;
		pParamInfo->Caption			    = "cursor pos";
		pParamInfo->IsInput			    = TRUE;
		pParamInfo->IsOutput		    = FALSE;
		pParamInfo->MinValue		    = 0;
		pParamInfo->MaxValue		    = MATRIX_COLS_MAX;
		pParamInfo->DefaultValue	    = 0;
		pParamInfo->Symbol			    = "";
		pParamInfo->Format			    = "%.0f";
		pParamInfo->ReadOnly		    = FALSE;
        pParamInfo->IsSeparator         = TRUE;
		pParamInfo->CallBackType        = ctImmediate;
	}
	// output at cursor pos
	else if (ParamIndex == 15)
	{
		pParamInfo->ParamType			= ptArray;
		pParamInfo->Caption				= "cursor out";
		pParamInfo->IsInput				= FALSE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->MinValue			= - FLT_MAX;
		pParamInfo->MaxValue			= FLT_MAX;
		pParamInfo->CallBackType        = ctImmediate;
	}
	// cells caption input of the matrix
	else if (ParamIndex == 16)
	{
		pParamInfo->ParamType		= ptTextField;
		pParamInfo->Caption			= "cells caption";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= FALSE;
		//pParamInfo->CallBackType    = ctImmediate;
        pParamInfo->IsSeparator     = TRUE;
	}
 	// optional first values faders
	else if (ParamIndex >= OPT_FADERS_SHIFT && ParamIndex < OPT_FADERS_SHIFT + m_numOfOptFaders)
	{
        // the param name
        stringstream Stream; 
        Stream << (ParamIndex - OPT_FADERS_SHIFT + 1);
        m_vectOptFadersCaptions.push_back(string("cell ") + Stream.str());

		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= (AnsiCharPtr)(m_vectOptFadersCaptions[ParamIndex - OPT_FADERS_SHIFT].c_str());
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->Scale			= scLinear;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.5f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
        if (ParamIndex == OPT_FADERS_SHIFT)
        {
            pParamInfo->IsSeparator    = TRUE;
        }
	}
	// default case
	else
	{
		// do nothing
	}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void TMatrixModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{	
	// matrix size
	if (ParamIndex == 0)
	{
		m_dtfMatrixSize = pEvent;
	}
	// num of cols data fader input/output
	if (ParamIndex == 1) 
	{
		m_fdrNumOfCols = pEvent;
	}	
	// num of rows data fader input/output
	else if (ParamIndex == 2)
	{
		m_fdrNumOfRows = pEvent;
	}
	// mouse mode listbox input
	else if (ParamIndex == 3)
	{
		m_lbxMouseMode = pEvent;
	}
	// cell draw mode value data fader input
	else if (ParamIndex == 4)
	{
		m_fdrCellDrawModeValue = pEvent;
	}
	// randomize button input
	else if (ParamIndex == 5)
	{
		m_btnRandomize = pEvent;
	}
	// reset button input
	else if (ParamIndex == 6)
	{
		m_btnReset = pEvent;
	}
    // random coverage coeff
    else if (ParamIndex == 7) 
	{
		m_dtfRandomCoverage = pEvent;
	}
	// value for reset input
	else if (ParamIndex == 8)
	{
		m_dtfResetValue = pEvent;
	}
	// last clicked cell data field output
	else if (ParamIndex == 9)
	{
		m_dtfLastMouseClickCell = pEvent;
	}
	// last clicked col data field output
	else if (ParamIndex == 10)
	{
		m_dtfLastMouseClickCol = pEvent;
	}
	// last clicked row data field output
	else if (ParamIndex == 11)
	{
		m_dtfLastMouseClickRow = pEvent;
	}
	// input of the matrix
	else if (ParamIndex == 12)
	{
		m_arrMatrixInput = pEvent;
	}
	// output of the matrix
	else if (ParamIndex == 13)
	{
		m_arrMatrixOutput = pEvent;
	}
    // position of the cel surrouding cursor
	else if (ParamIndex == 14)
	{
		fdrCursorPos = pEvent;
	}
	// output at cursor pos
	else if (ParamIndex == 15)
	{
		arrCursorOutput = pEvent;
	}
	// caption array input of the matrix
	else if (ParamIndex == 16)
	{
		m_txtCaptionsInput = pEvent;
	}
	// optional first values faders
	else if (ParamIndex >= OPT_FADERS_SHIFT && ParamIndex < OPT_FADERS_SHIFT + m_numOfOptFaders)
	{
		m_fdrOptCellValue[(ParamIndex - OPT_FADERS_SHIFT)] = pEvent;
	}	
	// default case
	else
	{
		// do nothing
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void TMatrixModule::onCallBack (UsineMessage *Message) 
{
	// cast for the opt fader range
	UINT32 NumOfOptFaders = static_cast<unsigned long>(m_numOfOptFaders);
	UINT32 IndexOptFaders = static_cast<unsigned long>(OPT_FADERS_SHIFT);
    NativeInt paramIndex = Message->wParam;

	// num of cols data fader input/output
	if ((paramIndex  == 1) && (Message->lParam == MSG_CHANGE) ) 
	{
        int fdrNumOfCols = (int)(sdkGetEvtData(m_fdrNumOfCols));
		if ( m_numOfCols != fdrNumOfCols )
        {
            // update the total size
		    updateNumOfCols (fdrNumOfCols);
            sdkNeedRemoteUpdate (0);
            updateCursorInfos();
            // update coursor out
            sdkNeedRemoteUpdate (14);
        }
	}
	// num of rows data fader input/output
	else if ( (paramIndex == 2) && (Message->lParam == MSG_CHANGE) ) 
	{
        int fdrNumOfRows = (int)(sdkGetEvtData(m_fdrNumOfRows));
		if ( m_numOfRows != fdrNumOfRows )
        {
            // update the total size
		    updateNumOfRows (fdrNumOfRows);
            sdkNeedRemoteUpdate (0);
            updateCursorInfos();
            // update cursor out
            sdkNeedRemoteUpdate (14);
        }
	}
	// mouse mode listbox input
	else if ( (paramIndex == 3) && (Message->lParam == MSG_CHANGE) ) 
	{
		int newmousemode = static_cast<int>(sdkGetEvtData( m_lbxMouseMode ));		
		setMouseMode(static_cast<TMouseMode>(newmousemode));
	}
	// randomize button input 
	else if ( (paramIndex == 5) && (Message->lParam == MSG_CHANGE) ) 
	{
		// randomize cells value 
		randomizeCells();
	}
	// reset cells 
	else if ( (paramIndex == 6) && (Message->lParam == MSG_CHANGE) ) 
	{
		// reset cells datas
		resetCells();
	}
	// input of the matrix
	else if ( (paramIndex == 12) && (Message->lParam == MSG_CHANGE) ) 
	{
        //
        arrayInChange();
		updateCursorInfos();
	}
    
	// output of the matrix
	else if ( (paramIndex == 13) && (Message->lParam == MSG_CHANGE) ) 
	{
		//
		arrayOutChange();
    }
    // position of the cel surrouding cursor
	else if ( (paramIndex == 14) && (Message->lParam == MSG_CHANGE) ) 
	{
        if (posCursor != (int) sdkGetEvtData(fdrCursorPos))
        {
            updateCursorInfos();
            // update cursor out
            sdkNeedRemoteUpdate (14);
        }
	}
	// captions array input of the matrix
	else if ( (paramIndex == 16) && (Message->lParam == MSG_CHANGE) ) 
	{
		// security check
        if (sdkGetEvtSize (m_txtCaptionsInput) > 0)
        {
            updateCellsCaptions();
		    updateCellsDatas();
        }

	}
    
	// optional first values faders
	else if ( (paramIndex >= (NativeInt)IndexOptFaders) 
        && (paramIndex < (NativeInt)(IndexOptFaders + NumOfOptFaders) ) 
        && (Message->lParam == MSG_CHANGE) 
        ) 
	{
		// deduce index of the cell from index of the control
		int CellIndex = std::max(0, (int)paramIndex - OPT_FADERS_SHIFT);

		// make sure the matrix is enough big
		if ( CellIndex < (m_numOfCols * m_numOfRows) )
		{
			m_tabCellsValue[CellIndex] = sdkGetEvtData( m_fdrOptCellValue[CellIndex] );	
            // ask to update the datas and repaint the module
            setCellsGuiColor( CellIndex);	
            sdkRepaintPanel();
		}
	}
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// // compute the lentgh of the chunk string
int TMatrixModule::onGetChunkLen (LongBool Preset) 
{
    int ChunkLen = 0;
    
    if (Preset)
    {
        ChunkLen = (  sizeof(char) * 4                                                      // the version header
                    + sizeof(int)															// num of cols
                    + sizeof(int)															// num of rows
                    + (sizeof(float) * (m_numOfCols * m_numOfRows))                         // cells values
                    );
    }
	else
    {
        ChunkLen = (  sizeof(char) * 4                                                      // the version header
                    + sizeof(int)															// num of cols
                    + sizeof(int)															// num of rows
                    + (sizeof(float) * (m_numOfCols * m_numOfRows))                         // cells values
                    + sizeof(int) + (sizeof(float) * sdkGetEvtSize(m_txtCaptionsInput))		// cells caption
                    );
	}
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// store module datas in the chunk string
void TMatrixModule::onGetChunk (void* chunk, LongBool Preset) 
{

    //int sizeValues = pMatrix->m_numOfCols * pMatrix->m_numOfRows;
//	int sizeColors			= sdkGetEvtSize(m_colColorsInput);
	//int sizeBorderColors	= sdkGetEvtSize(m_colBorderColorsInput);
	int sizeCaptions		= sdkGetEvtSize(m_txtCaptionsInput);
	float* addrCaptions		= sdkGetEvtDataAddr(m_txtCaptionsInput);

    int charInt = sizeof(int);
    int charFloat = sizeof(float);

    int charHeader = 4;
    int charValues = charFloat * (m_numOfCols * m_numOfRows);
    int charCaptions = charFloat * sizeCaptions;
	
	int offset = 0;
	// -----------------------------------------------------------------------
	// HEADER section
    // copy the header code into the chunk
	memcpy (chunk
        , (void*)CHUNCK_MX20
        , charHeader 
        );
	offset += charHeader;
	// -----------------------------------------------------------------------
	// Values section
	// copy the num of cols into the chunk
    memcpy ((void*)((char*)chunk + offset)
        , (void*)&m_numOfCols
        , charInt 
        );
	offset += charInt;
	// copy the num of rows into the chunk
    memcpy ((void*)((char*)chunk + offset)
        , (void*)&m_numOfRows
        , charInt 
        );
	offset += charInt;
	// copy cells value array into the chunk
    memcpy ((void*)((char*)chunk + offset)
        , (void*)m_tabCellsValue
        , charValues 
        );
	offset += charValues;
    
    if (! Preset)
    {

	// -----------------------------------------------------------------------
	// Cells captions section
	// copy the captions input size into the chunk
    memcpy ((void*)((char*)chunk + offset)
        , (void*)&sizeCaptions
        , charInt 
        );
	offset += charInt;
	// copy the color array content into the chunk
    memcpy ((void*)((char*)chunk + offset)
        , (void*)addrCaptions
        , charCaptions 
        );
    }

}

//-----------------------------------------------------------------------------
// restore module datas from the chunk string
void TMatrixModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset)
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

    //chunk was saved with version 2.0
    if ( string(CHUNCK_MX20, 4) == string(headerChunk, 4) )
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

		updateCellsValues();
		updateCursorInfos();
		updateOptFaders();

        if (! Preset)
        {
            int sizeCaptions;
            // extract the captions input size from the chunk
            memcpy( (void*)&sizeCaptions
                   , (void*)((char*)chunk + offset)
                   , charInt
                   );
            offset += charInt;
            
            int charCaptions = charFloat * sizeCaptions;
            
            sdkSetEvtSize(m_txtCaptionsInput, sizeCaptions);
            
			// important to get the address after the sdkSetEvtSize()
			float* addrCaptions = sdkGetEvtDataAddr(m_txtCaptionsInput);
			// extract the color array content from the chunk
			memcpy( (void*)addrCaptions
                   , (void*)((char*)chunk + offset)
                   , charCaptions 
                   );
        }
        
        // ask to update the datas and repaint the module
        updateCursorInfos();
	    updateCellsDatas();
        updateCellsColor();   
    }
}

//-----------------------------------------------------------------------------
// Global Randomize
void TMatrixModule::onRandomize ()
{
	// randomize cells value 
	randomizeCells();
}

//-----------------------------------------------------------------------------
// Global Reset
void TMatrixModule::onReset()
{
	// reset cells value 
	resetCells();
}

//----------------------------------------------------------------------------
// Settings
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void TMatrixModule::onCreateSettings()
{
	// we had our user section in the edit layout panel
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cursor");
	sdkAddSettingLineCombobox (DESIGN_TAB_NAME, &(settingsCursorOrientation), "cursor orientation", "\"vertical\",\"horizontal\"");
	sdkAddSettingLineColor    (DESIGN_TAB_NAME, &(settingsCursorColor), "cursor color");

	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cells");
	sdkAddSettingLineBoolean (DESIGN_TAB_NAME, &(settingsIsCellsRounded), "cells rounded");
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsRoundedSize), "cells round size", 0.0f, 200.0f, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 5.0f);
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsGapSize), "cells gap", 0.0f, 800.0f, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 5.0f );
	
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "cells colors");
	sdkAddSettingLineCombobox (DESIGN_TAB_NAME, &(settingsColorsOrientation), "colors orientation", "\"vertical\",\"horizontal\"");
    
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
	sdkAddSettingLineCombobox(DESIGN_TAB_NAME, &(settingsCellsCaptionVertAlign), "vertical align", "\"center\",\"top\",\"bottom\"");
	sdkAddSettingLineCombobox(DESIGN_TAB_NAME, &(settingsCellsCaptionHorizAlign), "horizontal align", "\"center\",\"left\",\"right\"");
	sdkAddSettingLineSingle  (DESIGN_TAB_NAME, &(settingsCellsCaptionFontSize), "cells caption size", 0.0f, 1.0f, scLinear, "%", DEFAULT_FORMAT_FLOAT_2, 0.5f);
	sdkAddSettingLineColor   (DESIGN_TAB_NAME, &(settingsCellsCaptionColor), "cells caption color");
	
	sdkAddSettingLineCaption (DESIGN_TAB_NAME, "mouse precision");
	sdkAddSettingLineInteger (DESIGN_TAB_NAME, &(settingsFadeModeDistance), "fade distance", 10, 200, scLinear, "pix", 50);
	sdkAddSettingLineInteger (DESIGN_TAB_NAME, &(settingsFadeModePrecision), "SHIFT fade factor", 1, 10, scLinear, "coeff", 7);
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redraw
void TMatrixModule::onSettingsHasChanged()
{
	updateSettingsParams();

} 

void TMatrixModule::onResize (float contentWidth, float contentHeight)
{
	updateSettingsParams();
}

void TMatrixModule::onCreateCommands()
{
    sdkAddCommandSeparator("cmd_edit", TRUE);
    sdkAddCommand("cmd_reset", 6, TRUE);
    sdkAddCommand("cmd_randomize", 5, TRUE);
}

//-----------------------------------------------------------------------------
// paint the module panel
void TMatrixModule::onPaint ()
{
	// compute variables before the loop
	unsigned int CellIndex;
	int c = 0;
	int r = 0;
	int cols = m_numOfCols;
	int rows = m_numOfRows;

    // draw the cursor
	if (posCursor > 0)
    {			
        if (settingsIsCellsRounded != FALSE)
            sdkFillRect(dimCursor, settingsCursorColor, sdkPixelToHeightPercent(settingsCellsRoundedSize), settingsCursorColor, 0);
		else
		sdkFillRect(dimCursor, settingsCursorColor, 0, settingsCursorColor, 0);
    }
	
    // draw the cells
    for (c = 0; c<cols; c++)
	{
		for (r = 0; r<rows; r++)
		{
			CellIndex = ((c * (int)rows) + r );

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
			if (! m_vectCellsCaptions[CellIndex].empty())	
			{
				sdkFillText( m_tabCellsGuiData[CellIndex].dim, (AnsiCharPtr)(m_vectCellsCaptions[CellIndex].c_str()), m_tabCellsGuiData[CellIndex].captionColor, m_tabCellsGuiData[CellIndex].captionSize, FALSE, FALSE, m_tabCellsGuiData[CellIndex].alignHorizontal, m_tabCellsGuiData[CellIndex].alignVertical);
			}
		}
	}
}


//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// MouseMove callback
void TMatrixModule::onMouseMove (TShiftState Shift, float X, float Y)
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
void TMatrixModule::onMouseDown (TMouseButton Button, TShiftState Shift, float X, float Y)
{
	// befor all,
    if (m_mouseMode == mmEdit)
	{	
		/*int NewCellIndex = */foundNewMouseCellIndex(X, Y);
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
void TMatrixModule::onMouseUp (TMouseButton Button, TShiftState Shift, float X, float Y)
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
// mouse mode setter
void TMatrixModule::setMouseMode (TMouseMode Mode)
{
	m_mouseMode = Mode;
}


//-------------------------------------------------------------------------
//
void TMatrixModule::updateSettingsParams()
{		
    updateCursorInfos();
	updateCellsColor();

	//if (settingsCellsCaptionShow != FALSE && (m_mouseMode == mmFade))
	//	updateCellsCaptions();
    
	updateCellsDatas();
}

//-------------------------------------------------------------------------
//
void TMatrixModule::updateMatrixSize()
{
	int newsize = m_numOfCols * m_numOfRows;

	sdkSetEvtSize( m_arrMatrixOutput, newsize );
      
    //resizeSettingsLinesColors();

    // reset out of range opt faders
	if (m_numOfOptFaders > newsize )
	{
		// reset out of range opt faders
		resetOptFaders( 0, newsize, m_numOfOptFaders - 1);
	}
};
	
//-------------------------------------------------------------------------
//
void TMatrixModule::updateNumOfCols (int NewNumOfCols)
{

	// have to reorganize m_tabCellsValue to preserve cells placement
    int addcol = NewNumOfCols - m_numOfCols;
		
	//int oldsize = sdkGetEvtSize( m_arrMatrixOutput );
	int newsize = NewNumOfCols * m_numOfRows;
    

	sdkSetEvtSize( m_arrMatrixOutput, newsize );
    

    // update the size outlet
    sdkSetEvtData(m_dtfMatrixSize, (float)newsize);
    
    if (addcol < 0)
    {
        for (int r = 1; r < m_numOfRows; r++)
        {
            memcpy((void*)(m_tabCellsValue + (NewNumOfCols * r)), (void*)(m_tabCellsValue + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
		}
    }
    else
    {
		// found the reset value
		float ResetValue = sdkGetEvtData(m_dtfResetValue);

        for (int r = m_numOfRows - 1; r > 0; r--)
        {
            memcpy((void*)(m_tabCellsValue + (NewNumOfCols * r)), (void*)(m_tabCellsValue + (m_numOfCols * r)), (sizeof(float) * m_numOfCols));
            for (int c = 0; c < addcol; c++)
			{
				m_tabCellsValue[(m_numOfCols * r)  + c] = ResetValue;
			}
		}
    }
    m_numOfCols = NewNumOfCols;
        
    // reset out of range opt faders
	if (m_numOfOptFaders > newsize )
	{
		// reset out of range opt faders
		resetOptFaders( 0, newsize, m_numOfOptFaders - 1);
	}
    //resizeSettingsLinesColors();
  	// ask to update the datas and repaint the module
	updateCellsDatas();
	updateCellsValues();
	updateCellsColor();
	updateCursorInfos();
};	

//-------------------------------------------------------------------------
//
void TMatrixModule::updateNumOfRows (int NewNumOfRows)
{
	int newsize = m_numOfCols * NewNumOfRows;
    m_numOfRows = NewNumOfRows;

	// just size modification
	sdkSetEvtSize( m_arrMatrixOutput, newsize );
  
    // update the size outlet
    sdkSetEvtData(m_dtfMatrixSize, (float)newsize);
    
    //resizeSettingsLinesColors();

    // reset out of range opt faders
	if (m_numOfOptFaders > newsize )
	{
		// reset out of range opt faders
		resetOptFaders( 0, newsize, m_numOfOptFaders - 1);
	}
    //resizeSettingsLinesColors();
  	// ask to update the datas and repaint the module
	updateCellsDatas();
	updateCellsValues();
	updateCursorInfos();
	updateCellsColor();
};

//-------------------------------------------------------------------------
// reset the matrix to reset value
void TMatrixModule::resetCells()
{		
	// found the reset value
	float ResetValue = sdkGetEvtData(m_dtfResetValue);

	// reset opt faders
	resetOptFaders( ResetValue, 0, m_numOfOptFaders - 1);
			
    // reset cells datas
    for (int i = 0; i < MATRIX_SIZE_MAX; i++)
        m_tabCellsValue[i] = ResetValue;

	updateCellsValues();
    // update matrix content
    sdkNeedRemoteUpdate (13);
	updateCellsColor();
	updateCursorInfos();
};

//-------------------------------------------------------------------------
// randomize the matrix
void TMatrixModule::randomizeCells()
{
	float CellMaxValue = (float)sdkGetEvtData(m_fdrCellDrawModeValue);
	float RandomCoverage = (float)sdkGetEvtData(m_dtfRandomCoverage);

	// randomize with 0 to cell draw value
	if (m_mouseMode == mmFade)
	{
		int size = m_numOfCols * m_numOfRows;

		for (int i = 0; i < size; i++)
		{
			if (RandomCoverage >= (rand() / (float)(RAND_MAX)))
				m_tabCellsValue[i] = CellMaxValue * (rand() / (float)(RAND_MAX));
			else
				m_tabCellsValue[i] = 0;
		}
	}
	// randomize with 0 or cell draw value
	else
	{
		int size = m_numOfCols * m_numOfRows;

		for (int i = 0; i < size; i++)
		{
			if (RandomCoverage >= (rand() / (float)(RAND_MAX)))
				m_tabCellsValue[i] = (rand() / (float)(RAND_MAX)) > 0.5f ? CellMaxValue : 0;
			else
				m_tabCellsValue[i] = 0;
		}
	}
	updateCellsValues();
	// update matrix content
	sdkNeedRemoteUpdate(13);
	updateCellsColor();
	updateCursorInfos();
};

void TMatrixModule::updateCursorInfos()
{

    posCursor = (int) sdkGetEvtData(fdrCursorPos);
    int start = posCursor - 1;
    if (settingsCursorOrientation == 0)
    {
	    float cols = (float)m_numOfCols;
	    float gapW = sdkPixelToWidthPercent(settingsCellsGapSize);
	    float width = (1.0f - (gapW * (cols + 1))) / cols;
		dimCursor.left	    = (start * (width + gapW));
        dimCursor.top       = 0;
        dimCursor.right	    = (start * (width + gapW)) + width + (2 * gapW);
        dimCursor.bottom	= 1;
        
        sdkSetEvtSize (arrCursorOutput, m_numOfRows);
        for (int i = 0; i < m_numOfRows; ++i)
        {
            float tmp = m_tabCellsValue[(i * m_numOfCols) + start];

            sdkSetEvtArrayData (arrCursorOutput, i,tmp);
        }
    }
    else
    {
	    float rows = (float)m_numOfRows;
	    float gapH = sdkPixelToHeightPercent(settingsCellsGapSize);
	    float height = (1.0f - (gapH * (rows + 1))) / rows;
		dimCursor.left	    = 0;
        dimCursor.top       = (start * (height + gapH));
        dimCursor.right	    = 1;
        dimCursor.bottom	= (start * (height + gapH)) + height + (2 * gapH);
        
        sdkSetEvtSize (arrCursorOutput, m_numOfCols);
        for (int i = 0; i < m_numOfCols; ++i)
        {
            float tmp = m_tabCellsValue[(start * m_numOfCols) + i];
            sdkSetEvtArrayData (arrCursorOutput, i, tmp); //[(i * m_numOfRows) + start]);
        }
    }
	sdkRepaintPanel();

}

//-------------------------------------------------------------------------
// update each cell color ON data
void TMatrixModule::updateCellsColor()
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
void TMatrixModule::updateCellsCaptions()
{
    int sizeMatrix = m_numOfCols * m_numOfRows;

	std::stringstream Stream; 
	std::string tmp;
    Stream << sdkGetEvtPChar(m_txtCaptionsInput);

    for (int i = 0; i < sizeMatrix; i++)
	{
		// 
		if ( ! Stream.eof() && ! Stream.fail() )
		{
			std::getline (Stream, tmp, ',' );
            // to delete surounding " char from the commatext (when space or special char)
            tmp.assign ( tmp.begin(), std::remove(tmp.begin(), tmp.end(), '\"') );
			
            m_vectCellsCaptions[i] = tmp;
		}
        else
        {
            m_vectCellsCaptions[i] = "";
        }
	}
}

//-------------------------------------------------------------------------
// update datas and UI infos for each cell
void TMatrixModule::updateCellsDatas()
{
	int cellIndex;
	int c = 0;
	int r = 0;
	float cols = (float)m_numOfCols;
	float rows = (float)m_numOfRows;
	TRectF cellDim;
			
	float gapW = sdkPixelToWidthPercent(settingsCellsGapSize);
	float gapH = sdkPixelToHeightPercent(settingsCellsGapSize);
        
	float width  = (1.0f - (gapW * (cols + 1.0f))) / cols;
	float height = (1.0f - (gapH * (rows + 1.0f))) / rows;
		

	for (r = 0; r<rows; r++)
	{
		for (c = 0; c<cols; c++)
		{
			cellIndex = ((r * (int)cols) + c );

			cellDim.left	= gapW + (c*(width  + gapW));
            cellDim.top		= gapH + (r*(height + gapH));
            cellDim.right	= gapW + (c*(width  + gapW)) + width;
            cellDim.bottom	= gapH + (r*(height + gapH)) + height;
					
			m_tabCellsGuiData[cellIndex].dim = cellDim;
			m_tabCellsGuiData[cellIndex].cornerRadius = sdkPixelToHeightPercent(settingsCellsRoundedSize);
			m_tabCellsGuiData[cellIndex].captionColor = settingsCellsCaptionColor;
			m_tabCellsGuiData[cellIndex].alignVertical = (TTextAlign)settingsCellsCaptionVertAlign;
			m_tabCellsGuiData[cellIndex].alignHorizontal = (TTextAlign)settingsCellsCaptionHorizAlign;

            if (sdkHeightPercentToPixel(cellDim.bottom - cellDim.top) >= sdkWidthPercentToPixel(cellDim.right - cellDim.left))
			     m_tabCellsGuiData[cellIndex].captionSize = sdkPixelToHeightPercent(settingsCellsCaptionFontSize * sdkWidthPercentToPixel(cellDim.right - cellDim.left));
            else
		         m_tabCellsGuiData[cellIndex].captionSize = sdkPixelToHeightPercent(settingsCellsCaptionFontSize * sdkHeightPercentToPixel(cellDim.bottom - cellDim.top));
		}
	}
    // finally ask a repaint
	sdkRepaintPanel();
};
		
//
void TMatrixModule::updateCellsValues()
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
void TMatrixModule::arrayInChange()
{	
	int size = min(MATRIX_SIZE_MAX, sdkGetEvtSize(m_arrMatrixInput));
	// copy new threshold datas
	memcpy(
        m_tabCellsValue, 
        sdkGetEvtDataAddr(m_arrMatrixInput), 
        (sizeof(float) * size )
        );

	updateCellsValues();
		
	// ask to update the datas and repaint the module
	updateCellsColor();
	updateCellsDatas();
	updateCursorInfos();
    // update matrix content
    sdkNeedRemoteUpdate (13);
}

	
//
void TMatrixModule::arrayOutChange()
{	
	// copy new threshold datas
	memcpy(
        m_tabCellsValue, 
        sdkGetEvtDataAddr(m_arrMatrixOutput), 
        (sizeof(float) * sdkGetEvtSize(m_arrMatrixOutput) )
        );


	// ask to update the datas and repaint the module
	updateCellsColor();
	updateCellsDatas();
	updateCursorInfos();
}


// reset optionals faders to 0
void TMatrixModule::updateOptFaders()
{
	for (int i = 0; i < m_numOfOptFaders; i++)
	{
		sdkSetEvtData(m_fdrOptCellValue[i], m_tabCellsValue[i]);
	}
}

// reset optionals faders to 0
void TMatrixModule::resetOptFaders(float ResetValue, int BeginId, int EndId)
{
	for (int i = BeginId; i <= EndId; i++)
	{
		sdkSetEvtData( m_fdrOptCellValue[i], ResetValue );
	}
}

void TMatrixModule::resizeSettingsLinesColors()
{
    numOfLinesColors = 12;
    settingsLinesColorsOn.resize(numOfLinesColors, sdkGetUsineColor(clDataFlow));
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
int TMatrixModule::foundNewMouseCellIndex( float X, float Y)
{	
	if (X < 0 || Y < 0 || X > 1 || Y > 1)
    {     
		sdkSetEvtData( m_dtfLastMouseClickCol, -1.0f);
		sdkSetEvtData( m_dtfLastMouseClickRow, -1.0f);
        return -1;
    }
	
	float cellwidth = 1.0f / (float)(m_numOfCols);
	float cellheight = 1.0f / (float)(m_numOfRows);

	int NewCellCol = (int)(X / cellwidth);
	int NewCellRow = (int)(Y / cellheight);
		
	int NewCellIndex = (NewCellRow * m_numOfCols) + NewCellCol;	
			
	if (sdkPointInRect( X, Y, m_tabCellsGuiData[NewCellIndex].dim ))
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
}

//
void TMatrixModule::mouseToggleCell( int CellIndex)
{
	float Threshold = sdkGetEvtData( m_fdrCellDrawModeValue);
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
    // update matrix content
    sdkNeedRemoteUpdate (13);
	updateCursorInfos();

		
	// update opt faders
	if ( CellIndex < m_numOfOptFaders)
	{
		sdkSetEvtData( m_fdrOptCellValue[CellIndex], NewThreshold );
	}

	setCellsGuiColor( CellIndex);		

	// finally ask a repaint
	//sdkRepaintPanel();

}

//
void TMatrixModule::mouseFadeCell( int CellIndex, TShiftState Shift, float X, float Y)
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
    // update matrix content
    sdkNeedRemoteUpdate (13);
	updateCursorInfos();
		
	// update opt fader
	if ( CellIndex < m_numOfOptFaders)
	{
		sdkSetEvtData( m_fdrOptCellValue[CellIndex], NewThreshold );
	}
		
	setCellsGuiColor( CellIndex);	

	// finally ask a repaint
	//sdkRepaintPanel();
}

// deduce the color from a cell value
void TMatrixModule::setCellsGuiColor(int index)
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

}

