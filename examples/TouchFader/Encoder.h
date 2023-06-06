//-----------------------------------------------------------------------------
// ENCODER ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __ENCODER_H__
#define __ENCODER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"

#define MAX_CENTER 17  //max center segments+1
#define MAX_LEDS 128  //max leds segments
#define MAX_QUADS 16  //max quad path segments
#define MAX_QUADS_DUAL 32 // for up & down type quadratic ring
#define MAX_SHAPE_POINTS 18 // max shape segments +2


using namespace std;
//if (TFItem.IEncoder.CENTER_LAYER_POSITION == 0) { goto GTcursor; }

class Encoder
{
public:
	//contructeur
	Encoder();
	Encoder(TFPStruct *TFParams_val, TFPaint *TFPaintP_val);

	// methods
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSettingsHasChanged();
	void onSetQuickColor();

	void UPD_COLORS();
	void UPD_COLOR_ARRAY();
	void UPD_PAINT();
	bool LED_TEST(float test_val);
	void QUADRA_RING(float in_val, int draw_mode);
	void QUADRA_RING_DUAL(float in_val, int draw_mode, TPointF MAINS[], TPointF  CTRLS[]);
	void SHAPE_TURN(float in_val);
	float GET_GRADIENT(float in_val, int draw_mode);
	void CENTER_TURN(float in_val);
	void UPD_FROM_MOUSE();
	void SET_RECT_ORIENTATION(TRectF &RECT, float invalA, float centerB, float symX, float symY, bool is_vertical);
	void SWAP_RECT_ORIENTATION(TRectF &RECT);
	TDrawType GET_PAINT_LAYER(int layer);
	void UPD_PAINT_POINTERS();
	void PRESET_LOAD(int preset);
	void UPD_STRINGS();
	void GET_ENCODER_ANGLE();

	// --- Variables
	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;
	//////////////////////////////////////////////////////
	int INPUT_TYPE = 0; // 0y 1x, 2 angular
	int PAINT_NB_ELMTS_TO_DRAW = 7;
	TFShape SHAPE_ELMT;
	int SHAPE_SEG_FROM = 0; //for partial draws
	int SHAPE_NB_SEGS = 4;
	TFLine LINE_ELMT;
	TFValue VALUE_ELMT;

	string ENCODER_VALUE_STRING="";
	///////////////////////////////////////
	LongBool EN_HORIZONTAL_MODE = FALSE;//////////////////////////////////////////////////// <-------------

	int ENCODER_SUBMODE = 0;  //0 simple 1 button 2 switch
	int ENCODER_LAYOUT_TYPE = 0; //0 ring 1 fader 2 color sw, 3 val only
	int ENCODER_LEDS_MODE = 0; // 0 normal 1 rev 2 spread 3 pan 4cursor
	float RING_RADIUS = 0.25f;  //global radius
	float RING_LENGTH = 0.80f;  //global length    							  
	float start_offset = 0.2f;  //(1-ring)/2.0   is start and end width offset

	float angle = 0.0f;          // for angular input type
	float angle_prev = 0.0f;
	float angle_delta = 0.0f;
	bool ANGLE_RESET_DELTA = FALSE; //to reset delta on mouse dwn

	float CENTER_A_RADIUS = 1.50f;
	float RING_BORDER_SIZE = 2.0f;
	float LEDS_RADIUS = 0.9f;
	float LEDS_SIZE = 2.0f;
	int MAIN_OUTPUT = 1;
	int LEDS_TYPE = 1;
	LongBool RELATIVE_MODE = TRUE;
	LongBool CURSOR_LINKED_TO_RADIUS = TRUE;

	//paint variables
	int CENTER_LAYER_POSITION = 1;
	LongBool FX_SHOW_SPECULAR = TRUE;
	float DEBUG_TEST = 0.0f;
	float SPECULAR_OPACITY = 0.5f;
	float REFLEXIONS_OPACITY = 0.5f;
	LongBool FX_SHOW_SHADOWS = TRUE;
	LongBool FX_SHOW_REFLEXIONS = TRUE;
	TUsineColor SHADOW_COLOR = HSLA(200, 0.0f, 0.1f, 0.5f);
	TUsineColor SHADOW_COLOR_LO = HSLA(200, 0.0f, 0.1f, 0.25f);
	TUsineColor SPECULAR_COLOR_LO;
	TUsineColor SPECULAR_COLOR_VLO;
	TUsineColor SPECULAR_COLOR = HSLA(200, 0.0f, 0.7f, 0.25f);
	TUsineColor SPECULAR_COLOR_HI = HSLA(200, 0.0f, 0.7f, 0.5f);
	float SHADOW_OPACITY = 0.5f;
	float SHADOW_X = -0.025f;
	float SHADOW_Y = 0.050f;
	/*
	TPointF MIDDLE_INDICATOR_FROM;
	TPointF MIDDLE_INDICATOR_TO;
	float MIDDLE_INDICATOR_RADIUS_FROM = 1.25;
	float MIDDLE_INDICATOR_RADIUS_TO = 1.33;
	float MIDDLE_INDICATOR_SIZE = 10.0;
	*/
	float CURSOR_EXTRA_OFFSET = 0.0f;
	LongBool CURSOR_FLIP = FALSE;
	float SHAPE_H_OFFSET = 0.0f;
	float SHAPE_V_OFFSET = 0.0f;
	LongBool BAR_IS_ROUND = TRUE;
	float BAR_ROUND_SIZE = 0.03f;

	TFShape BAR;
	TFShape  BAR_OFF;

	float panel_lum = 0.0f;
	float CURSOR_lum = 0.0f;
	float fader_lum = 0.0f;
	float fader_sat = 0.0f;
	TUsineColor PANEL_COLOR;
	TUsineColor FADER_COLOR;
	TPointF CURSOR_FROM;
	TPointF CURSOR_TO;
	float BAR_V_SIZE = 0.0f;
	float BAR_H_SIZE = 0.0f;
	TRectF LedRect;
	bool ring_on_part = FALSE;
	TPointF CENTER_POINTS[MAX_CENTER];
	int CENTER_NBP = 7;
	int CENTER_DRAW_TYPE = 0;
	TPointF DOTS[MAX_LEDS];
	TRectF LEDS_RECTS[MAX_LEDS];
	TUsineColor DOTS_COLOR[MAX_LEDS];

	TPointF QUAD_MAIN_DOTS[MAX_QUADS]; //single way ring
	TPointF QUAD_CTRL_DOTS[MAX_QUADS];


	TPointF QUAD_OFF_MAINS[MAX_QUADS_DUAL + 1];
	TPointF QUAD_OFF_CTRLS[MAX_QUADS_DUAL + 1];

	TPointF QUAD_ON_MAINS[MAX_QUADS_DUAL + 1];
	TPointF QUAD_ON_CTRLS[MAX_QUADS_DUAL + 1];
	
	int QUAD_NBP_B = MAX_QUADS*2;

	float QUAD_INNER_RADIUS = 0.8f;
	TPointF QUAD_DOT;
	TPointF QUAD_CTRL;
	int QUAD_NBP = MAX_QUADS;
	TRectF Center_Led;
	float center_v_off = 0.0f;
	float center_h_off = 0.0f;
	float cap_width = 0.0f;
	float cap_len = 0.0f;
	TPointF CENTER;
	float CENTER_A_SIZE = 0.0f;
	//float CENTER_B_SIZE = 0;
	float line_add_h_off = 0.0f;
	float line_add_v_off = 0.0f;
	TPointF LINE_FROM;
	TPointF LINE_TO;
	float QUAD_SIZE_USED;
	float CENTER_BORDER_SIZE = 4.0f;

	TPointF SHAPE[MAX_SHAPE_POINTS];
	int SHAPE_NB_POINTS = 3;
	float CURSOR_LENGTH_B = 0.5f;
	int CURSOR_TYPE = 2;  //0 dot 1 line 2 shape
	float encoder_val = 0.0f;
	float encoder_memo = 0.0f; //use to restore in button-switch mode
	int ENCODER_SUBMODE_BEHAVIOUR = 0; //for switch/button behaviour, tab from teset_val to last val, or restart from reset val

	bool ENCODER_WILL_SW_OFF = TRUE;

	float RING_ROTATION_OFFSET = 0.0f;

	float RING_H_OFFSET = 0.0f;
	float RING_V_OFFSET = 0.0f;

	TRectF Encoder_Rect;
	TRectF Cusor_Fader;
	float PANEL_MIN_LUM = 0.10f;
	float PANEL_MAX_LUM = 0.75f;
	float PANEL_BORDER_SIZE = 4.0f;
	float PANEL_ROUND_SIZE = 0.05f;
	LongBool PANEL_IS_ROUND = TRUE;

	LongBool ADD_PANEL_CURSOR_ = TRUE;
	float PANEL_CURSOR_WIDTH = 4.0f;

	float ENCODER_SPEED = 0.5f;
	int RING_LEDS = 32;
	float DOT_X = 0.0f;
	float DOT_Y = 0.0f;
	float DOT_T = 0.0f;
	float DOT_SIZE_USED = 5.0f;// DOT_SIZE;
	float MSD_ENC_VAL = 0.0f;
	float CURSOR_SIZE = 4.0f;
	float CURSOR_SIZE_USED = CURSOR_SIZE;
	float CURSOR_LENGTH = 0.75f;
	float LEDS_GRADIENT = 0.50f;

	LongBool APPLY_COLOR_CORRECT = FALSE;

	TUsineColor ENCODER_RING_COLOR;

	struct TSHOW
	{
		LongBool RING = TRUE;
		LongBool FILLED_RING = TRUE;
		LongBool RING_BORDER = TRUE;
		LongBool QUAD_RING = TRUE;
		LongBool LEDS_RING = TRUE;
		LongBool CURSOR_ = FALSE;
		LongBool CENTER = TRUE;
		LongBool MIDDLE_INDICATOR = FALSE;
	};

	TSHOW SHOW;

	static const int NB_ENCODER_COLORS = 13;
	TFColor *ENCODER_COLORS[NB_ENCODER_COLORS];  //pointer array to Tfcolors

	struct TCOLOR
	{
		TFColor Global;//0
		TFColor Switch;//1
		TFColor Ring_Off;//2
		TFColor Ring_Off_Border;//3
		TFColor Ring_On;//4
		TFColor Ring_On_Border;//5
		TFColor Leds_Off;//6
		TFColor Leds_On;//7
		TFColor Cursor;//8
		TFColor Center_Cap;//9
		TFColor Center_Cap_Border;//10
		TFColor Middle_indicator;//11
		TFColor Panel;//12
	};

	TCOLOR COLOR;


};
#endif //__ENCODER_H__
