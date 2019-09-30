//-----------------------------------------------------------------------------
//@file  
//	MatrixAudio.h
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

// include once, no more
#ifndef _MATRIXEAUDIOMODULE_H
#define _MATRIXEAUDIOMODULE_H

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
 
//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
// matrix defines and constants
const int MATRIX_SIZE_MAX = 256; // 16 cols * 16 rows or 8 * 32 etc...

// optional audio in/out constants
const int OPT_PARAMS_MAX = 32;
const int OPT_AFTER_INOUT = 13;

const int OPT_IN_PARAMS = 2;
const int OPT_OUT_PARAMS = 2;

// module constant
const int MODULE_DIM_DEFAULT = 200; 

const float SMOOTH_SLOW = 0.999f;

// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_MA11[] = { 'M', 'A', '1', '1' };
static char const CHUNCK_MA20[] = { 'M', 'A', '2', '0' };

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------
// UI data we need to store for each cell of the matrix
struct TCellGuiData 
{
	TRectF dim;
	TColorUsine color;
	TColorUsine captionColor;
	float		captionSize;
	float		cornerRadius;
	TTextAlign	alignVertical;
	TTextAlign	alignHorizontal;
};

// to handle different mouse behavior
typedef enum TMouseMode 
{
	mmToggle,
	mmFade,
	mmDraw,
	mmLock,
    mmEdit
} TMouseMode;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class MatrixAudioModule : public UserModuleBase
{
	
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

	// constructor
	MatrixAudioModule();

	// destructor
	virtual ~MatrixAudioModule();


	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	//-----------------------------------------------------------------------------
	// query system and init
	int  onGetNumberOfParams( int queryIndex);
	void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();
	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);
    //-----------------------------------------------------------------------------
	// Global Randomize
    void onRandomize ();
	//-----------------------------------------------------------------------------
	// Global Reset
	void onReset();
	//-----------------------------------------------------------------------------
	// UI Canvas
	void onCreateSettings();
	void onSettingsHasChanged();
    void onResize (float contentWidth, float contentHeight);
    void onCreateCommands();
	void onPaint ();
	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	// num of cols data fader input/output
	UsineEventPtr m_fdrNumOfCols; 
	// num of rows data fader input/output
	UsineEventPtr m_fdrNumOfRows;
	// mouse mode listbox input
	UsineEventPtr m_lbxMouseMode; 
	// cell draw mode value data fader input
	UsineEventPtr m_fdrCellDrawModeValue;

	// randomize button input
	UsineEventPtr m_btnRandomize;  
	// reset button input
	UsineEventPtr m_btnReset; 
	// random coverage
	UsineEventPtr m_dtfRandomCoverage; 

	// last clicked col index output
	UsineEventPtr m_dtfLastMouseClickCol; 
	// last clicked row index output
	UsineEventPtr m_dtfLastMouseClickRow; 
	// last clicked cell index output
	UsineEventPtr m_dtfLastMouseClickCell; 
	// input of the matrix
	UsineEventPtr m_arrMatrixInput;
	// output of the matrix
	UsineEventPtr m_arrMatrixOutput;

	// for optional params
	UsineEventPtr m_tevtOptInputs[OPT_PARAMS_MAX];
	UsineEventPtr m_tevtOptOutputs[OPT_PARAMS_MAX];
	UsineEventPtr m_tevtOptInputsCaption[OPT_PARAMS_MAX];
	UsineEventPtr m_tevtOptOutputsCaption[OPT_PARAMS_MAX];

	// internal buffer to mix audio in onProcess()
	UsineEventPtr m_tevtAudioMixBuffer;
	UsineEventPtr m_tevtSmoothCurrentCoeff;

	//-------------------------------------------------------------------------
	// Gui datas

	// template graphic attributes of inputs header
	TColorUsine settingsHeadersCaptionColor;
	float		settingsHeadersCaptionSize;

    // datas for the edit layout parameters
	float settingsHeightHeaderCols;
	float settingsWidthHeaderRows;

	LongBool settingsIsCellsRounded;
	float    settingsCellsRoundedSize; 
	float    settingsCellsGapSize; 
     
    // cells colors
    int                       settingsColorsOrientation;
    std::vector <TColorUsine> settingsLinesColorsOn;
    std::vector<std::string>  nameLinesColorsOn;
    std::vector <TColorUsine> settingsLinesColorsOff;
    std::vector<std::string>  nameLinesColorsOff;
    int numOfLinesColors;

	LongBool settingsCellsCaptionShow;
	int    settingsCellsCaptionVertAlign;
	float settingsCellsCaptionFontSize;
	TColorUsine settingsCellsCaptionColor;
    
	int settingsFadeModeDistance;	
	int settingsFadeModePrecision;

	// tab to store the cells matrix datas for the UI
	TCellGuiData m_tabCellsGuiData[MATRIX_SIZE_MAX];
    
	// tab to store the inputs caption datas for the UI
	TCellGuiData m_tabInCaptionsGuiData[OPT_PARAMS_MAX];
    
	// tab to store the outputs caption datas for the UI
	TCellGuiData m_tabOutCaptionsGuiData[OPT_PARAMS_MAX];
	//-------------------------------------------------------------------------
	// Other

	// number of columns and rows off the matrix
	int m_numOfCols;
	int m_numOfRows;

	int m_colsMax;
	int m_rowsMax;

	int m_numOfOptInOut;
		
	// tab to store the cells matrix data value
	// used to populate m_tabCellsGuiData and to save in workspace (see SetChunk and GetChunk)
	TPrecision m_tabCellsValue[MATRIX_SIZE_MAX];
	
	TColorUsine m_tabCellsColorsOn[MATRIX_SIZE_MAX];
	TColorUsine m_tabCellsColorsOff[MATRIX_SIZE_MAX];

    // to store cells captions
    std::vector<std::string> m_vectCellsCaptions;

    // used to generate the names of optionnal cells faders
    std::vector<std::string> m_vectOptParamsCaptions;

    // used to generate the names of optionnal cells faders
    std::vector<std::string> m_vectColsHeaderCaptions;
    std::vector<std::string> m_vectColsHeaderTextDefaults;
    std::vector<std::string> m_vectRowsHeaderCaptions;
    std::vector<std::string> m_vectRowsHeaderTextDefaults;

    std::vector<std::string> m_vectOptColorsInlets;
	
	    
	// for audio smooth
    TPrecision m_smoothOldCoeff[MATRIX_SIZE_MAX];
    
	// to deal with the mouse
	TMouseMode	m_mouseMode;				// see TMouseMode at top of file
	int			m_mouseLastButtonDown;	    // -1 = no button, other see TMouseButton in UsineDefinition.h
	int			m_mouseLastCellIndex;		// -1 = no cell, 0 to 16384 = cell index
    TPointF		m_mouseLastCoords;

	
	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:

	//-------------------------------------------------------------------------
	// mouse mode setter
	inline void setMouseMode(TMouseMode Mode);

	//-------------------------------------------------------------------------
	// 
	inline void updateHeadersCaptions();
	inline void updateSettingsParams();
	inline void updateMatrixSize();
	inline void updateNumOfCols(int NewNumOfCols);
	inline void updateNumOfRows(int NewNumOfRows);

	//-------------------------------------------------------------------------
	// reset the matrix to reset value
	inline void resetCells();

	//-------------------------------------------------------------------------
	// randomize the matrix
	inline void randomizeCells();

	//-------------------------------------------------------------------------
	// update each cell color ON data
	inline void updateCellsColor();
	inline void updateCellsCaptions();
	inline void updateCellsDatas();
    inline void updateCellsValues();

	//
    inline void whenArrayInChange();
    inline void whenArrayOutChange();
    
    inline void resizeSettingsLinesColors();

	//
	inline int foundNewMouseCellIndex( float X, float Y);

	//
	inline void mouseToggleCell( int CellIndex);
	inline void mouseFadeCell( int CellIndex, TShiftState Shift, float X, float Y);

	// deduce the color from a cell value
	inline void setCellsGuiColor(int index);
	inline void constructCellCaption(int CellIndex);

}; // class MatrixAudioModule


#endif //_MATRIXEAUDIOMODULE_H