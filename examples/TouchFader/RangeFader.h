//-----------------------------------------------------------------------------
// Range Fader ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __RANGEFADER_H__
#define __RANGEFADER_H__
#define MAX_STRIPES 512 //max possible stripes/dashes
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"

using namespace std;
//TFParams_struct TFParams;

class RangeFader
{
public:
	//contructeur
	RangeFader();
	RangeFader(TFPStruct *TFParams, TFPaint *TFPaintP_val);

	// methods
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSettingsHasChanged();
	void onSetQuickColor();

	void UPD_PAINT();
	void UPD_OUT();

	TDrawType GET_PAINT_LAYER(int layer);
	void UPD_PAINT_POINTERS();
	void UPD_ELEMENTS_ARRAY();
	void UPD_STRIPES();

	bool INSIDE(TFShape in_shape);
	void UPD_RECTS(bool full_update);
	void UPD_STRINGS();
	void UPD_COLOR_ARRAY();
	void UPD_COLORS_OUT();
	void PRESET_LOAD(int preset);
	void MAP_COLORS_TO_ELEMENTS();

	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;


	string UP_VALUE_STRING = " ";
	string DWN_VALUE_STRING = " ";
	string MID_VALUE_STRING = " ";
	string WIDTH_VALUE_STRING = " ";

	//---------------------------------------
	int PAINT_NB_ELMTS_TO_DRAW = 10; //nb of elmt (layers) to draw max)
	TFShape SHAPE_ELMT;
	TFValue VALUE_ELMT;
	static const int NB_RANGE_FADER_ELEMENTS = 6;
	TFShape *RANGE_FADER_ELEMENTS[NB_RANGE_FADER_ELEMENTS];  //pointer array to Tfshapes
	struct TELMT
	{
		TFShape GLOBAL; //0
		TFShape BG; //1
		TFShape BAR; //2
		TFShape CAP_UP; //3
		TFShape CAP_MID; //4
		TFShape CAP_DWN; //5
	};
	TELMT ELMT;
	//---------------------------------------------------------------

	LongBool WIDTH_INPUT_MODE = FALSE;

	float fad_val = 0.5f;
	float real_val_Y = 0.5f;
	float real_val_X = 0.5f;
	float RF_ENCODER_COEFF_Y = 0.5f;
	float RF_ENCODER_COEFF_X = 0.1f;

	float DELTA_X = 0;
	float DELTA_Y = 0;

	float RANGE_BOX_H = 0.1f;
	float RANGE_BOX_W = 0.9f;
	float CANVAS_RATIO = 1.7f;
	float RANGE_BAR_REDUX = 0.05f;

	float RANGE_BAR_HEIGHT = 1.0f;

	float R_UP_VAL = 1;
	float R_UP_REAL_VAL = 1;
	float R_DWN_VAL = 0;
	float R_DWN_REAL_VAL = 0;
	float R_MID_REAL_VAL = 0;
	float R_WIDTH_REAL_VAL = 0;
	float R_W = 0.2f;  //2x rbox h
	bool RANGE_UP_ACTIVE = FALSE;
	bool RANGE_DWN_ACTIVE = FALSE;
	bool RANGE_MID_ACTIVE = FALSE;


	static const int NB_RANGE_COLORS = 13;
	TFColor *RANGE_COLORS[NB_RANGE_COLORS];  //pointer array to Tfcolors
	LongBool RF_APPLY_CC = FALSE;
	//LongBool TEST = FALSE;

	struct TCOLOR
	{
		TFColor Global;//0
		TFColor CAP_UP;//1
		TFColor CAP_UP_BORDER;//2
		TFColor CAP_DWN;//3
		TFColor CAP_DWN_BORDER;//4
		TFColor CAP_MID;//5
		TFColor CAP_MID_BORDER;//6
		TFColor BAR;//7
		TFColor BAR_BORDER;//8
		TFColor BG;//9
		TFColor BG_BORDER;//10
		TFColor STRIPES;//11
	};
	TCOLOR COLOR;

	struct TSHOW
	{
		LongBool BG = TRUE;
		LongBool BG_BORDER = TRUE;
		LongBool CAP_BORDER = TRUE;
		LongBool STRIPES = TRUE;

	};
	TSHOW SHOW;

	struct TSTRIPES
	{
		int MAX_PIX = 0;
		int   MODULO = 4; // draw nth pixels
		float WIDTH = 0.4f; // width as defined in setting
		float STEP_WIDTH = 0.1f; //step final width
		TColorAhsl STRIPE_COL;
		TRectF STRIPES_LEDS[MAX_STRIPES];
		TUsineColor STRIPES_COLORS[MAX_STRIPES];
	};
	TSTRIPES STRIPES;

};
#endif //__RANGEFADER_H__
