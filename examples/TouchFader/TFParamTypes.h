// ----------------------------------------------------------------------------
// ----------------- TF PARAM TYPES : Generic Item component types
// ----------------------------------------------------------------------------
// ----------------- Authors : 23fx23
// ----------------------------------------------------------------------------
#ifndef __TFPARAMTYPES_H__
#define __TFPARAMTYPES_H__

// ----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"
#include "TouchFader_Tools.h"
#include <string>
// ----------------------------------------------------------------------------
// ----------------------- TFPStruct : Touch Fader Internal Params Class
// ----------------------------------------------------------------------------


class TFPStruct
{
public:
	TFPStruct();

	float ITEM_VALUE_W;
	float ITEM_VALUE_X;
	float ITEM_VALUE_Y;
	float ITEM_VALUE_Z;
	bool  ITEM_VALUE_SWITCH;
	int   ITEM_MAIN_OUT;

	float MIN_POS;
	float MAX_POS;

	LongBool SHOW_VALUE;
	int VALUE_PRECISION;
	int VALUE_PRECISION_DECA;
	float OUT_MIN;
	float OUT_MAX;
	int QUANTIZE_INT;

	bool MSDWN;
	float MSX;
	float MSY;
	float X;
	float Y;
	float RESET_VALUE;
	float RESET_VALUE_01;
	float input_val;
	float item_val;

//	float ITEM_HUE;
	float CANVAS_W;
	float CANVAS_H;
	float CANVAS_RATIO;
	float CANVAS_S;
	float hilight;

	float DELTA_X;
	float DELTA_Y;

	bool PULSE;
	int PUSE_DURATION;
	int PULSE_count;
	float PULSE_RAMP;

	TRectF VALUE_RECT;
	TUsineColor VALUE_ON_COLOR;
	TUsineColor VALUE_OFF_COLOR;
	TUsineColor NAME_ON_COLOR;
	TUsineColor NAME_OFF_COLOR;
};
// ----------------------------------------------------------------------------

enum TDrawType {DT_None, DT_Shape, DT_Shape_Reflexion, DT_Shape_Partial, DT_TRect_Array, DT_Points_Array, DT_Line, DT_Text_Value, DT_Lines_Array, DT_Ring,
	DT_Enc_Leds, DT_Enc_Cursor, DT_Enc_Center, DT_Enc_Panels, DT_Square_Dot, DT_Round_Dot
}; //Paint layer modes

enum TBlendMode { BM_Replace, BM_Mix, BM_Additive, BM_Multiply, BM_Overlay, BM_Color, BM_Luminosity }; //Color mix a&B modes

class TFPColor
{
public:
	TFPColor();

	TUsineColor TARGET_COLOR;
	bool USE_TARGET_COLOR;
	int HUE_BEAVIOUR=1;
	int HUE_ROLL = 0;
	float SATURATION_OVERIDE = 0.0f;
	float LUMINOSITY_CONTRAST = 1.0f;
	float LUMINOSITY_OFFSET = 0.0f;
	float LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
	bool COLOR_CORRECT = FALSE;
	TBlendMode BLEND_MODE = BM_Replace;
	float BLEND_MIX = 1.0f;
};
// ----------------------------------------------------------------------------
// ----------------------- TFColor : Touch Fader Component HSLA Color
// ----------------------------------------------------------------------------
class TFColor
{
#include <string>
public:
	// constructors
	// TODO : Default parameters from display preset
	TFColor();
	TFColor(TFPStruct *TFParams,TFPColor *TFPColor);
	TFColor(float s, float l, float a);
	TFColor(float h, float s, float l, float a);
	// variable members
	//TUsineColor TCUOut;
	float H;
	float S;
	float L;
	float A;
	int HUE_MODE;  //hue beahviour
	float HILIGHT_MULT;  //hilight
	TUsineColor IN;
	TUsineColor OUT;
	TColorAhsl  IN_HSLA;
	TColorAhsl  OUT_HSLA;
	TColorAhsl  TGT_HSLA;
	bool IS_VISIBLE;
	bool IS_BORDER;
	int ID;
	bool TGT_IS_NEXT;

	// function members
	// TODO : onPaint - TFColor print specificities
	// TODO : export - TFColor serialization specificities ?
	// TODO : import - TFColor unserialization specificities ?


	void update_in(int id, bool is_visible = TRUE, bool isBorder = FALSE, float hi_mult = 0, bool  TGT_IS_NEXT = false);
	void update_out();
	void OnSetQuickColor();


	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TUsineColor HSLA();

	TColorAhsl MIX_COLORS(TColorAhsl COLOR_A, TColorAhsl COLOR_B, TBlendMode BM_MODE, float MIX_VALUE);
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ----------------------- TFColorEffect : Effect on color when selected
// ----------------------------------------------------------------------------
class TFColorEffect
{
public:
	// TODO : Default parameters from display preset
	TFColorEffect();
	TFColorEffect(float L_Hi_val, float S_Hi_val, int H_OnOffset_val);

	float	L_Hi;
	float	S_Hi;
	int		H_OnOffset;
};
// ----------------------------------------------------------------------------




class TFValue
{
public:
	TFValue();

	//variables
	TRectF RECT;
	std::string VALUE_STRING;
	TTextAlign H_Align;
	TTextAlign V_Align;
	TUsineColor VALUE_COLOR;

};


class TFLine
{
public :
	TFLine();

	//variables
	TPointF LINE_FROM;
	TPointF LINE_TO;
	TUsineColor LINE_COLOR;
	float LINE_SIZE;
	//LongBool SHOW = FALSE;
};

// ----------------------------------------------------------------------------
// ----------------------- TFShape : Generic item component shape
// ----------------------------------------------------------------------------
class TFShape
{
public :
	// TODO : Default parameters from display preset
	TFShape();
	void RestoreBaseRect();
	void RestoreBaseColors();
	void Scale_Shape();

	// Rounded edges
	LongBool SHOW = TRUE;
	int ROUND_TYPE = 1; // 0 not round 1 round 2 hegagon
	LongBool IS_ROUND = TRUE; // = shapeIsRounded
	float ROUND_SIZE = 0.05f; // = roundSize
	float ROUND_SMOOTHNESS = 0.0f;//round smoothness
	// Global scale
	float Scale_X =1.0f;
	float Scale_Y =1.0f;
	// Border
	LongBool SHOW_BORDER = TRUE; // = borderIsDisplayed
	float	BORDER_SIZE = 4.0f; // = borderSize
	LongBool SHOW_FILL = TRUE; // = will fill shape
	TUsineColor FILL_COLOR;
	TUsineColor BORDER_COLOR;
	float offset_X =0.0f;
	float offset_Y=0.0f;
	
	// Main Shape rectangle and color
	TRectF	RECT_BASE; // = Base Rectangle Shape
	TRectF	RECT; // = Current Rectangle Shape
	float RATIO = 1.0f; //xyratio & canvasratio


};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ----------------------- TFCursor : Mouse Cursor specific data
// ----------------------------------------------------------------------------
class TFCursor
{
public : 
	float Y;
	float X;
};
// ----------------------------------------------------------------------------


// ----------------------------------------------------------------------------
// ----------------------- TFOutput : Content and destination of TF data
// ----------------------------------------------------------------------------
class TFOutput
{
public:
	TFOutput();

	// x = 0, y = 1, z = 2, s = 3, no = -1 // s = switch (on or off)
	int OutSrc[4]; // Four possible simultaneous outputs for each channel
};
// ----------------------------------------------------------------------------

class TFPaint
{
public:
	TFPaint();

	int *PAINT_NB_ELMTS_TO_DRAW;
	TFShape *SHAPE;
	TFLine *LINE_ELMT;
	float *FX_REFLEXIONS_EACH;
	int *FX_REFLEXIONS_TYPE;
	int *SHAPE_SEG_FROM;
	int *SHAPE_NB_SEGS;
	int *STRIPES_MAX_PIX;
	//int *STRIPES_MODULO;
	TRectF *STRIPES_STRIPES_LEDS;
	TRectF *STRIPES_STRIPES_LEDS_SYM;
	TUsineColor *STRIPES_STRIPES_COLORS;
	bool *STRIPES_DUAL_DRAW;
	TFValue *VALUE_ELMT;
	int *GRID_SUBDIVS;
	float *LINE_SIZE;
	TPointF *GRID_GRID_FROM;
	TPointF *GRID_GRID_TO;
	TUsineColor *COLOR_Grid_OUT;
	int FX_NB_FLARES;
	TPointF *FX_FLARE;
	TPointF *FX_FLARE_SYM;
	TUsineColor *FX_FLARES_COLORS;
	TUsineColor *FX_FLARES_SYM_COLORS;
	float *FX_FLARES_SIZES;
	float *FX_FLARES_SYM_SIZES;
	TPointF *QUAD_MAINS;
	TPointF *QUAD_CTRL;
	TUsineColor *QUICK_COLOR;
};


//////////////////genir functions
TUsineColor HI_COLOR(TUsineColor COLOR_IN, float Hilight_val); //hilight color


#endif //__TFPARAMTYPES_H__
