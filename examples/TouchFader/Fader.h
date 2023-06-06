//-----------------------------------------------------------------------------
// FADER ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __FADER_H__
#define __FADER_H__
#define MAX_STRIPES 512 //max possible stripes/dashes
#define MAX_GRID 128// max grid subdivs
//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"
#include <vector>

using namespace std;
//TFParams_struct TFParams;

class Fader
{
public:
	//contructeur
	Fader();
	Fader(TFPStruct *TFParams_val, TFPaint *TFPaintP_val);

	// methods
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSettingsHasChanged();
	void onCreateSettings();
	void onSetQuickColor();

	void UPD_FROM_EXTERNAL(float in_val);

	void UPD_FAD_RECTS(float fad_val); // updates rescts that rely on fader val
	void UPD_FAD_RECTS_FIXED(); // updates recs params non relative to fad val (less often)
	float RESCALE(float in_val);
	void COMPUTE_PIX(float fad_val);
	void UPD_XFADER();
	void UPD_PAINT();
	void UPD_COLOR_ARRAY();
	void UPD_ELEMENTS_ARRAY();
	void UPD_FLARE();
	float QUANTIZE(float in_val);
	void PRESET_LOAD(int preset);
	void APPLY_GLOBAL_SETTINGS();

	TDrawType GET_PAINT_LAYER(int layer);
	void UPD_STRIPES(bool FULL_UPDATE);
	void UPD_GRID();
	void UPD_PAINT_POINTERS();
	void UPD_STRINGS();


	// --- Variables
	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;
	//////////////////////////////////////////////////////////////
	int PAINT_NB_ELMTS_TO_DRAW = 32; //nb of elmt (layers) to draw max)
	TFShape SHAPE_ELMT;
	int SHAPE_SEG_FROM = 0; //for partial draws
	int SHAPE_NB_SEGS = 4;
	TFLine LINE_ELMT;
	TFValue VALUE_ELMT;
	///////////////////////////////////////////////

	

	TScale SCALE_TYPE = scLinear;

	string FADER_VALUE_STRING = "";
	string X_FADER_VALUE_STRING = "";

	// variables

	LongBool HORIZONTAL_MODE = FALSE;
	LongBool INVERT_MODE = FALSE;
	LongBool AUTO_RESET_MODE = FALSE;

	//////////////////////////
	LongBool APPLY_COLOR_CORRECT = FALSE;
	LongBool PRESET_RESET = FALSE;
	int MAIN_OUTPUT = 1;
	LongBool RELATIVE_MODE = FALSE;
	int   FADER_SWITCH_SUBMODE = 1;  //0 n 1 button 2switch
	int SWITCH_TARGET = 0; //target layout for switch visualisation
	int   X_FADER_SUBMODE = 0;
	////////////////////////////
	TPointF CAP_CENTER;
	TPointF LINE_FROM;
	TPointF LINE_TO;
	float LINE_SIZE = 1.0;
	LongBool VALUE_IN_CAP = FALSE;
	LongBool NAME_IN_CAP = FALSE;

	float RELATIVE_SPEED = 0.5f;
	bool  WILL_SW_OFF = FALSE;
	float ON_CLIC_VAL = 0;
	float fad_val = 0;
	float XY_input = 0;
	float FAD_BAR_REDUX = 0.05f;

	struct TX_FADER
	{
		float used_val = 0;
		float delta_val = 0;
		float WIDTH = 0;
		float HEIGHT = 0;
		float VAL = 0;
		float RELATIVE_SPEED = 0.5f;
		float SIZE = 0.5f;
	};
	TX_FADER X_FADER;

	struct TSTRIPES
	{
		int MAX_PIX = 0;
		int PIX = 0;
		int PIX_DWN = 0;
		int   MODE = 0; //0 center 1 side ect
		int   MODULO = 4; // draw nth pixels
		float WIDTH = 0.4f; // width
		float DIV_SIZE = 0;
		float H_REDUX = 0.12f;
		float GRADIENT = 0;  // ad lum gradient toward range
		float REDUX = 0;
		TRectF DashLed;  
		TColorAhsl STRIPE_COL;
		bool DUAL_DRAW = FALSE;
		bool GRADIENT_USE = FALSE;
		TPointF P_from;
		TPointF P_to;

		TRectF STRIPES_LEDS[MAX_STRIPES];
		TRectF STRIPES_LEDS_SYM[MAX_STRIPES];
		TUsineColor STRIPES_COLORS[MAX_STRIPES];

	};
	TSTRIPES STRIPES;

	struct TGRID
	{
		LongBool QUANTIZE_ON_GRID = FALSE;
		int GRID_SUBDIVS = 12;
		TPointF GRID_FROM[MAX_GRID];
		TPointF GRID_TO[MAX_GRID];
	};
	TGRID GRID;

	float FAD_BOX_H = 0.1f;
	float FAD_BOX_W = 0.9f;
	float FAD_BAR_W = 0.1f;
	float fad_width = 0.9f;

	TPointF fad_cap_comp;   //used to calculate compensation to avoid jumps when cliquing on cap

	LongBool FAD_COMPENSATE = TRUE;

	static const int NB_FADER_ELEMENTS = 6;
	TFShape *FADER_ELEMENTS[NB_FADER_ELEMENTS];  //pointer array to Tfshapes
	struct TELMT
	{
		TFShape GLOBAL; //0
		TFShape BG; //1
		TFShape BAR; //2
		TFShape CAP; //3
		TFShape X_FADER_BG; //4
		TFShape X_FADER_BAR; //5
	};
	TELMT ELMT;

	struct TFX
	{
		float SpecOffset = 0;
		int LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		float LIGHTING_ROLL = 0.38f;
		float LIGHTING_DEPTH = 1.0;
		int REFLEXIONS_TYPE = 0;
		int REFLEXIONS_TYPE_IN = 0;
		LongBool REFLEXIONS = TRUE;
		float REFLEXIONS_OPACITY = 0.33f;
		LongBool SHADOWS = TRUE;
		float SHADOW_X = -0.033f;
		float SHADOW_Y = 0.033f;
		float SHADOW_OPACITY = 0.33f;
		LongBool SPECULAR = TRUE;
		float SPECULAR_OPACITY = 0.33f;
		float LIGHT_X_norm = 0;
		float LIGHT_Y_norm = 0;
		float SPEC_DIR_X = 0;
		float SPEC_DIR_Y = 0;
		float REFL_DRAW_EACH = 1;//reflexion precision
		static const int NB_FLARES = 8;//flares subdiv
		LongBool FLARES = TRUE;
		TPointF FLARE_INIT;
		TPointF FLARE_SYM_INIT;
		TPointF FLARE;
		TPointF FLARE_SYM;
		TUsineColor FLARES_COLORS[NB_FLARES];
		TUsineColor FLARES_SYM_COLORS[NB_FLARES];
		float FLARES_SIZES[NB_FLARES];
		float FLARES_SYM_SIZES[NB_FLARES];
		float FLARE_INTENSITY;
		TRectF FLARE_RECT;
		float FLARES_OPACITY = 0.33f;
		float OCLLUSION_OPACITY = 0.20f;
		LongBool OCCLUSION = TRUE;
		TUsineColor OCCLUSION_COLOR;
		TFShape OCCLUSION_SHAPE;
	};
	TFX FX;

	struct TSHOW
	{
		LongBool LINE = FALSE;
		LongBool CAP = TRUE;
		LongBool STRIPES = TRUE;
		LongBool BAR = TRUE;
		LongBool BG = TRUE;
		LongBool GRID = FALSE;
	};
	TSHOW SHOW;

	static const int NB_FADER_COLORS = 14;
	TFColor *FADER_COLORS[NB_FADER_COLORS];  //pointer array to Tfcolors

	struct TCOLOR
	{
		TFColor Global;//0  --all
		TFColor Switch;//1 --all borders
		TFColor BG;//2
		TFColor BG_Border;//3
		TFColor Bar;//4
		TFColor Bar_Border;//5
		TFColor Cap;//6
		TFColor Cap_Border;//7
		TFColor Stripes_ON;//8
		TFColor Stripes_OFF;//9
		TFColor Line; //10
		TFColor Xfader;//11
		TFColor Xfader_Border;//12
		TFColor Grid;//13

		TUsineColor Stripes;
		TUsineColor Shadow;
		TUsineColor Shadow_Lo;
	};
	TCOLOR COLOR;

	
};
#endif //__FADER_H__
