// ----------------------------------------------------------------------------
// ----------------- BUTTON : Button Item
// ----------------------------------------------------------------------------
// ----------------- Authors : 23fx23
// ----------------------------------------------------------------------------
#ifndef __BUTTON_H__
#define __BUTTON_H__
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include "Touch_Fader_Tools.h"
#include "TFParamTypes.h"

using namespace std;

// ----------------------------------------------------------------------------
// ----------------------- Button : Button Item Class
// ----------------------------------------------------------------------------
class Button
{
public:
	// --- Constructor
	Button();
	Button(TFPStruct *TFParams_val, TFPaint *TFPaintP_val);

	// --- Methods
	// Usine SDK UserModuleBase Item complements 
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSettingsHasChanged();
	void onSetQuickColor();
	// Button specific methods
	void onStateChange();
	void BaseShapeSet();
	void UPD_PAINT();
	void UPD_COLOR_ARRAY();
	void UPD_FLARE();
	void UPD_OCCLUSION();
	void UPD_XY(float X, float Y);

	void PRESET_LOAD(int preset);
	TDrawType GET_PAINT_LAYER(int layer);
	void UPD_PAINT_POINTERS();
	void UPD_STRINGS();

	// --- Variables
	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;
	//////////////////////////////// draw/paint elmts
	int PAINT_NB_ELMTS_TO_DRAW = 15; //nb of elmt (layers) to draw)
	TFShape SHAPE_ELMT;
	int SHAPE_SEG_FROM=0; //for partial draws
	int SHAPE_NB_SEGS = 4;
	TFLine LINE_ELMT;
	TFValue VALUE_ELMT;

	string SWITCH_VALUE_STRING = "";
	string X_VALUE_STRING = "";
	string Y_VALUE_STRING = "";
	///////////////////////////////////////
	//fox
	TToggleType TOGGLE_TYPE = TTMomentary; //0: momentary 1 swicth
	//int SUBMODE = 0; 
	//int ROUND_TYPE = 1;
	int TRANSFORM = 0; //0none 1 slid h ,v sclae

	static const int NB_BUTTON_COLORS = 9;
	TFColor *BUTTON_COLORS[NB_BUTTON_COLORS];  //pointer array to Tfcolors
	LongBool APPLY_COLOR_CORRECT = FALSE;
	struct TCOLOR
	{
		TFColor Global;//0
		TFColor OFF;//1
		TFColor OFF_Border;//2
		TFColor ON;//3
		TFColor ON_Border;//4
		TFColor XY_OFF; //5
		TFColor XY_ON; //6
		TFColor CONTAINER;//7
		TFColor CONTAINER_BORDER;//8
		TFColor Grid;//for comp only
	};
	TCOLOR COLOR;


	// Cursor position display
	TFCursor Cursor;
	int XYLines_Nb;
	float XYLines_Sz;
	TRectF XYLines_Rect; //Y_VAL_RECT ?
	// Button shape
	// Colors and effects
	// TODO : manage TFColorEffect array of effects collected from presets 
	TFColorEffect ColorEffect;
	float REDUX;
	// Output
	TFOutput Output;
	int SkinNb;
	float LINE_SIZE = 0;
	int MAIN_OUTPUT = 3;

	struct TELMT
	{
		//TFShape GLOBAL; //0
		TFShape BUTTON_SHAPE;//1
		TFShape CONTAINER_SHAPE;//2
	};
	TELMT ELMT;

	struct TFX
	{
		int LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		float LIGHTING_ROLL = 0.25f;
		float LIGHTING_DEPTH = 1.0f;
		int REFLEXIONS_TYPE = 0;
		int REFLEXIONS_TYPE_IN = 0;
		LongBool REFLEXIONS = TRUE;
		float REFLEXIONS_OPACITY = 0.50f;
		LongBool SHADOWS = TRUE;
		float SHADOW_X = -0.033f;
		float SHADOW_Y = 0.033f;
		float SHADOW_OPACITY = 0.20f;
		LongBool SPECULAR = TRUE;
		float SPECULAR_OPACITY = 0.20f;
		float LIGHT_X_norm = 0.0f;
		float LIGHT_Y_norm = 0.0f;
		float SPEC_DIR_X = 0.0f;
		float SPEC_DIR_Y = 0.0f;
		float REFL_DRAW_EACH = 1;//reflexion precision
		static const int NB_FLARES = 8;//flares subdiv
		LongBool FLARES = TRUE;
		TPointF FLARE_INIT;
		TPointF FLARE_SYM_INIT;
		TPointF FLARE;
		TPointF FLARE_SYM;
		TColorUsine FLARES_COLORS[NB_FLARES];
		TColorUsine FLARES_SYM_COLORS[NB_FLARES];
		float FLARES_SIZES[NB_FLARES];
		float FLARES_SYM_SIZES[NB_FLARES];
		float FLARE_INTENSITY;
		TRectF FLARE_RECT;
		float FLARES_OPACITY = 0.25f;
		float OCLLUSION_OPACITY = 0.25f;
		LongBool OCCLUSION = TRUE;
		TFShape OCCLUSION_SHAPE;
	};
	TFX FX;
	//////////////////unused for comp only

private:
	// TODO : manage differents effects by giving the effect in the function
	void ApplyShapeEffect();
	void ApplyColorEffect();
};
// ----------------------------------------------------------------------------


#endif //__BUTTON_H__
