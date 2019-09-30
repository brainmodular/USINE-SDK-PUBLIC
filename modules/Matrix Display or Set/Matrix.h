//-----------------------------------------------------------------------------
//@file  
//	Matrix.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the TMatrixModule class.
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
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __MATRIXE_MODULE_H__
#define __MATRIXE_MODULE_H__

//-----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>
 
//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
// matrix defines and constants
#define MATRIX_COLS_DEFAULT 4;
#define MATRIX_ROWS_DEFAULT 4;
#define MATRIX_SIZE_DEFAULT 16;

// maximum is 128*128, is it enough ;-)
const int MATRIX_COLS_MAX = 128;
const int MATRIX_ROWS_MAX = 128;
const int MATRIX_SIZE_MAX = 16384;

// optional faders constants
const int OPT_PARAMS_MAX = 128;
const int OPT_FADERS_SHIFT = 17;
//const int OPT_AFTER_FADERS = 3;

// module constant
const int MODULE_DIM_DEFAULT = 200; 

// buffer size for strings
static int const STRING_BUFF_SIZE = 512;

// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
//static char const CHUNCK_MX16[] = { 'M', 'X', '1', '6' };
static char const CHUNCK_MX20[] = { 'M', 'X', '2', '0' };

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
}TMouseMode;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class TMatrixModule : public UserModuleBase
{
	
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

	// constructor
	TMatrixModule(); 

	// destructor
	virtual ~TMatrixModule();

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
	//void onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	// parameters events
	// size of the matrix (cols * rows) output
	UsineEventPtr m_dtfMatrixSize;
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
	// value for reset input
	UsineEventPtr m_dtfResetValue; 
	// random coverage
	UsineEventPtr m_dtfRandomCoverage; 
    
	// last clicked cell index output
	UsineEventPtr m_dtfLastMouseClickCell;  
	// last clicked col index output
	UsineEventPtr m_dtfLastMouseClickCol; 
	// last clicked row index output
	UsineEventPtr m_dtfLastMouseClickRow; 

	// input of the matrix
	UsineEventPtr m_arrMatrixInput;
	// output of the matrix
	UsineEventPtr m_arrMatrixOutput;
    // position of the cel surrouding cursor
	UsineEventPtr fdrCursorPos; 
	// output at cursor pos
	UsineEventPtr arrCursorOutput;
	// input of the matrix
	UsineEventPtr m_colBorderColorsInput;
	// input of the matrix
	UsineEventPtr m_txtCaptionsInput;
	// optional first cells value of the matrix
	UsineEventPtr m_fdrOptCellValue[OPT_PARAMS_MAX];

	//-------------------------------------------------------------------------
	// Gui datas	

	// template graphic attributes of a cell
	//TCellGuiData m_paintCell;

	// tab to store the cells matrix datas for the UI
	TCellGuiData m_tabCellsGuiData[MATRIX_SIZE_MAX];
	
	// tab to store cells caption caption datas for the UI
	//POLYTEXT m_gdiCellsGuiData[MATRIX_SIZE_MAX];

	//-------------------------------------------------------------------------
	// Other

	// number of columns and rows off the matrix
	int m_numOfCols;
	int m_numOfRows;

	// number of optional fader from query system
	int m_numOfOptFaders;
	
	// tab to store the cells matrix data value
	// used to populate m_tabCellsGuiData and to save in workspace (see SetChunk and GetChunk)
	float m_tabCellsValue[MATRIX_SIZE_MAX];
    
	TColorUsine m_tabCellsColorsOn[MATRIX_SIZE_MAX];
	TColorUsine m_tabCellsColorsOff[MATRIX_SIZE_MAX];


    // to store cells captions
    std::vector<std::string> m_vectCellsCaptions;

    // used to generate the names of optionnal cells faders
    std::vector<std::string> m_vectOptFadersCaptions;
	
    // cursor 
    int         settingsCursorOrientation;
    TColorUsine settingsCursorColor;
    int         posCursor;
	TRectF      dimCursor;

    // datas for the edit layout parameters
	LongBool    settingsIsCellsRounded;
	float       settingsCellsRoundedSize;
	float       settingsCellsGapSize; 

    // cells colors
    int                       settingsColorsOrientation;
    std::vector <TColorUsine> settingsLinesColorsOn;
    std::vector<std::string>  nameLinesColorsOn;
    std::vector <TColorUsine> settingsLinesColorsOff;
    std::vector<std::string>  nameLinesColorsOff;
    int numOfLinesColors; 
	
	int         settingsCellsCaptionVertAlign;
	float       settingsCellsCaptionFontSize;
	TColorUsine settingsCellsCaptionColor;

	int         settingsFadeModeDistance;	
	int         settingsFadeModePrecision;	

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
	//
	inline void updateSettingsParams();
	inline void updateMatrixSize();
	inline void updateNumOfCols(int NewNumOfCols);
	inline void updateNumOfRows(int NewNumOfRows);

	// reset the matrix to reset value
	inline void resetCells();
	// randomize the matrix
	inline void randomizeCells();
	//-------------------------------------------------------------------------
    inline void updateCursorInfos();
	// update each cell color ON data
	inline void updateCellsColor();
	inline void updateCellsBorderColor();
	inline void updateCellsCaptions();
	inline void updateCellsDatas();
    inline void updateCellsValues();

	//
    inline void arrayInChange();
    inline void arrayOutChange();

	inline void updateOptFaders();

	// reset optionals faders to 0
	inline void resetOptFaders(float ResetValue, int BeginId, int EndId);

    inline void resizeSettingsLinesColors();
	//
	inline int  foundNewMouseCellIndex( float X, float Y);
	inline void mouseToggleCell( int CellIndex);
	inline void mouseFadeCell( int CellIndex, TShiftState Shift, float X, float Y);

	// deduce the color from a cell value
	inline void setCellsGuiColor(int index);

}; // class TMatrixModule

#endif //__MATRIXE_MODULE_H__