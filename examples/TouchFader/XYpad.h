//-----------------------------------------------------------------------------
// XYpad ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __XYPAD_H__
#define __XYPAD_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"
using namespace std;
//TFParams_struct TFParams;

class XYpad
{
public:
	//contructeur
	XYpad();
	XYpad(TFPStruct *TFParams, TFPaint *TFPaintP_val);
	
	// methods
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSettingsHasChanged();
	void onSetQuickColor();

	void UPD_FROM_MOUSE();
	void UPD_COLOR_ARRAY();
	void UPD_PAINT();
	void UPD_COLORS_OUT();
	void UPD_OUTPUTS();
	void PRESET_LOAD(int preset);
	TDrawType GET_PAINT_LAYER(int layer);
	void UPD_PAINT_POINTERS();
	void UPD_STRINGS();

	// variables
	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;
	////////////////////////////////////
	int PAINT_NB_ELMTS_TO_DRAW = 9;
	TFShape SHAPE_ELMT;
	int SHAPE_SEG_FROM = 0; //for partial draws
	int SHAPE_NB_SEGS = 4;
	TFLine LINE_ELMT;
	TFValue VALUE_ELMT;

	LongBool APPLY_COLOR_CORRECT = FALSE;

	string Y_VALUE_STRING = "";
	string X_VALUE_STRING = "";
	string VALUE_STRING = "";
	///////////////////////////////////////

	int MAIN_OUTPUT = 1; //y
	float XLINE = 0.5;
	float YLINE = 0.5;
	float ENCODER_COEFF_Y = 0.5;
	float ENCODER_COEFF_X = 0.5;
	LongBool RELATIVE_MODE = FALSE;	
	LongBool XYPAD_DOT_IS_ROUND = FALSE;

	struct TELMT
	{
		TFShape BG; //0
		TRectF XY_RECT; //1
	};
	TELMT ELMT;

	struct TSHOW
	{
		LongBool CROSS = TRUE;
		LongBool DOT = TRUE;
		LongBool FADERS = TRUE;
		LongBool RECTANGLE = TRUE;
		LongBool BG = TRUE;
		//LongBool BG_BORDER = TRUE;
	};

	TSHOW SHOW;

	static const int NB_XYPAD_COLORS = 7;
	TFColor *XYPAD_COLORS[NB_XYPAD_COLORS];  //pointer array to Tfcolors

	struct TCOLOR
	{
		TFColor Global; //0
		TFColor Cross; //1
		TFColor Dot; //2
		TFColor Faders; //3
		TFColor Rect; //4
		TFColor BG; //5
		TFColor BG_BORDER; //6
	};

	TCOLOR COLOR;

	float XYPAD_FADERS_SIZE = 5;
	float XYPAD_FAD_OPACITY = 0.5;
	float XYPAD_CROSS_SIZE = 2;
	float XYPAD_DOT_SIZE = 20;

};
#endif //__XYPAD_H__
