//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "RangeFader.h"

RangeFader::RangeFader()
{

}

RangeFader::RangeFader(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{
	TFParams = TFParams_val;
	TFPaintP = TFPaintP_val;

	TFPColors = new TFPColor();   //destruct??

	UPD_COLOR_ARRAY();
	for (int i = 0; i < NB_RANGE_COLORS; i++) { *RANGE_COLORS[i] = TFColor(TFParams, TFPColors); }

	UPD_ELEMENTS_ARRAY();
	UPD_PAINT_POINTERS();

	PRESET_LOAD(0);
	UPD_RECTS(TRUE);
	UPD_STRINGS();
}

void RangeFader::onMouseDown()
{
	TFParams->hilight = 1.0;

			if ( INSIDE(ELMT.BAR) || INSIDE(ELMT.CAP_MID) ) // asks if mouse is in one of the elments rects
			{
				RANGE_MID_ACTIVE = TRUE;                    // so that will trig appropriate code
			}
			 else if ( INSIDE(ELMT.CAP_UP))
			{
				RANGE_UP_ACTIVE = TRUE;
			}
			else if (INSIDE(ELMT.CAP_DWN))
			{
				RANGE_DWN_ACTIVE = TRUE;
			}
			 UPD_COLORS_OUT();
}//end msdwn

void RangeFader::onInputChange()
{


	if (WIDTH_INPUT_MODE)  // if on mid and width are master
	{
		R_MID_REAL_VAL = TFParams->ITEM_VALUE_Z;
		R_WIDTH_REAL_VAL = TFParams->ITEM_VALUE_W;

		R_UP_VAL = R_MID_REAL_VAL + R_WIDTH_REAL_VAL / 2;
		R_DWN_VAL = R_MID_REAL_VAL - R_WIDTH_REAL_VAL / 2;
	}
	else
	{
		R_UP_VAL = TFParams->ITEM_VALUE_Y;
		R_DWN_VAL = TFParams->ITEM_VALUE_X;

	}

	//R_UP_VAL  = TFParams->MAX_POS;
	//R_DWN_VAL = TFParams->MIN_POS;

	R_UP_VAL = fmax(R_UP_VAL, R_DWN_VAL);
	R_UP_VAL = CLAMP01(R_UP_VAL);
	R_DWN_VAL = fmin(R_DWN_VAL, R_UP_VAL);
	R_DWN_VAL = CLAMP01(R_DWN_VAL);

	R_UP_VAL = (R_UP_VAL*(1 - R_W*2)) + (R_W*2);

	R_DWN_VAL = R_DWN_VAL*(1 - R_W * 2);


	onMouseMove();
}

void RangeFader::onPreset()
{

	R_UP_VAL = TFParams->ITEM_VALUE_Y;
	R_DWN_VAL = TFParams->ITEM_VALUE_X;

	//R_UP_VAL = fmax(R_UP_VAL, R_DWN_VAL);
	//R_DWN_VAL = fmin(R_DWN_VAL, R_UP_VAL);

	R_UP_VAL = (R_UP_VAL*(1 - R_W * 2)) + (R_W * 2);

	R_DWN_VAL = R_DWN_VAL*(1 - R_W * 2);

	onMouseMove();

}

void RangeFader::onMouseUp()
{
	    TFParams->hilight = 0;
		RANGE_UP_ACTIVE = FALSE;
		RANGE_DWN_ACTIVE = FALSE;
		RANGE_MID_ACTIVE = FALSE;
		UPD_COLORS_OUT();
}

void RangeFader::onMouseMove()
{

	bool OK_MOVE_Y;
	bool USE_WIDTH = TRUE;
	R_W = RANGE_BOX_H*2.0f;

	OK_MOVE_Y = TRUE; 

	if (TFParams->DELTA_Y>0 && !RANGE_UP_ACTIVE){ TFParams->DELTA_Y = fminf(TFParams->DELTA_Y, R_DWN_VAL); }
	else if (TFParams->DELTA_Y<0 && !RANGE_DWN_ACTIVE){ TFParams->DELTA_Y = fmaxf(TFParams->DELTA_Y, -(1 - R_UP_VAL)); } //avoid speed moving unwanted redux

	float DX = TFParams->DELTA_X*RF_ENCODER_COEFF_X;


	if (RANGE_UP_ACTIVE)
	{
	R_UP_VAL = R_UP_VAL - TFParams->DELTA_Y;
	R_UP_VAL = CLAMP01(R_UP_VAL);
	R_UP_VAL = fmaxf(R_UP_VAL, R_DWN_VAL + R_W + R_W);
	}
	if (RANGE_DWN_ACTIVE)
	{
	R_DWN_VAL = R_DWN_VAL - TFParams->DELTA_Y;
	R_DWN_VAL = CLAMP01(R_DWN_VAL);
	R_DWN_VAL = fminf(R_UP_VAL - R_W - R_W, R_DWN_VAL);
	}
	if (RANGE_MID_ACTIVE)
	{
	if (OK_MOVE_Y)
	{
	R_UP_VAL = R_UP_VAL - TFParams->DELTA_Y;
	R_UP_VAL = CLAMP01(R_UP_VAL);
	R_UP_VAL = fmaxf(R_UP_VAL, R_DWN_VAL);
	R_DWN_VAL = R_DWN_VAL - TFParams->DELTA_Y;
	R_DWN_VAL = CLAMP01(R_DWN_VAL);
	R_DWN_VAL = fminf(R_UP_VAL, R_DWN_VAL);
	}

	if (USE_WIDTH)
	{
	if (abs(TFParams->DELTA_X*TFParams->CANVAS_RATIO) > abs(TFParams->DELTA_Y))
	{
	R_UP_VAL = R_UP_VAL + DX;
	R_UP_VAL = fminf(1.0, fmaxf(R_UP_VAL, R_DWN_VAL + R_W + R_W));
	R_DWN_VAL = R_DWN_VAL - DX;
	R_DWN_VAL = fmaxf(0, fminf(R_UP_VAL - R_W - R_W, R_DWN_VAL));
	}
	}
	}


	UPD_RECTS(FALSE);


	R_UP_REAL_VAL = RE_RANGE_NORM_OUT(R_UP_VAL, R_W*2.0f, 1.0f);
	R_DWN_REAL_VAL = RE_RANGE_NORM_OUT(R_DWN_VAL, 0, 1.0f-(R_W*2.0f));
	R_MID_REAL_VAL = (R_UP_REAL_VAL + R_DWN_REAL_VAL) / 2.0f;

	R_WIDTH_REAL_VAL = R_UP_REAL_VAL - R_DWN_REAL_VAL;

	UPD_OUT();
}

void RangeFader::UPD_PAINT()
{

}

void RangeFader::UPD_OUT()
{
		TFParams->ITEM_VALUE_Y = R_UP_REAL_VAL;
		TFParams->ITEM_VALUE_X = R_DWN_REAL_VAL;
		//TFParams->ITEM_VALUE_W = R_MID_REAL_VAL;
		//TFParams->ITEM_VALUE_Z = R_WIDTH_REAL_VAL;

		TFParams->ITEM_VALUE_Z = R_MID_REAL_VAL;
		TFParams->ITEM_VALUE_W = R_WIDTH_REAL_VAL;
		/////////
		//TFParams->MAX_POS = R_UP_REAL_VAL;
		//TFParams->MIN_POS = R_DWN_REAL_VAL;
		TFParams->ITEM_MAIN_OUT = 4; //route width W to main

		UPD_STRIPES();
		UPD_STRINGS();
}

void RangeFader::onSettingsHasChanged()
{
	UPD_ELEMENTS_ARRAY();
	TFPColors->COLOR_CORRECT = FALSE;

	switch (ELMT.GLOBAL.ROUND_TYPE)
	{
	case 0: ELMT.GLOBAL.IS_ROUND = FALSE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 0; break;//not round
	case 1: ELMT.GLOBAL.IS_ROUND = TRUE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 0; break;// round
	case 2: ELMT.GLOBAL.IS_ROUND = TRUE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 1; break;//hexagon
	default:break;
	}

	for (int i = 1; i < NB_RANGE_FADER_ELEMENTS; i++)
	{
		RANGE_FADER_ELEMENTS[i]->ROUND_TYPE = ELMT.GLOBAL.ROUND_TYPE;
		RANGE_FADER_ELEMENTS[i]->IS_ROUND = ELMT.GLOBAL.IS_ROUND;
		RANGE_FADER_ELEMENTS[i]->ROUND_SIZE = ELMT.GLOBAL.ROUND_SIZE;
		RANGE_FADER_ELEMENTS[i]->ROUND_SMOOTHNESS = ELMT.GLOBAL.ROUND_SMOOTHNESS;
		RANGE_FADER_ELEMENTS[i]->BORDER_SIZE = ELMT.GLOBAL.BORDER_SIZE;
		//RANGE_FADER_ELEMENTS[i]->SHOW_BORDER = ELMT.GLOBAL.SHOW_BORDER;
	}

	ELMT.BG.SHOW_BORDER = SHOW.BG_BORDER;
	ELMT.CAP_DWN.SHOW_BORDER = SHOW.CAP_BORDER;
	ELMT.CAP_MID.SHOW_BORDER = SHOW.CAP_BORDER;
	ELMT.CAP_UP.SHOW_BORDER = SHOW.CAP_BORDER;


	////////////////////////////////////////////////////Colors import
	COLOR.Global.update_in(0, TRUE, FALSE, 0);
	COLOR.CAP_UP.update_in(1, TRUE, FALSE, 0);
	COLOR.CAP_UP_BORDER.update_in(2, TRUE, TRUE, 0);
	COLOR.CAP_DWN.update_in(3, TRUE, FALSE, 0);
	COLOR.CAP_DWN_BORDER.update_in(4, TRUE, TRUE, 0);
	COLOR.CAP_MID.update_in(5, TRUE, FALSE, 0);
	COLOR.CAP_MID_BORDER.update_in(6, TRUE, TRUE, 0);
	COLOR.BAR.update_in(7, TRUE, FALSE, 0);
	COLOR.BAR_BORDER.update_in(8, TRUE, TRUE, 0);
	COLOR.BG.update_in(9, TRUE, FALSE, 0);
	COLOR.BG_BORDER.update_in(10, TRUE, TRUE, 0);
	COLOR.STRIPES.update_in(11, TRUE, FALSE, 0);

	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE; // use gloabal from 0 tho individual are avaible
	UPD_COLOR_ARRAY();
	//////////////////////////////////////////////////////////////////////

	UPD_COLORS_OUT();
/*	if (RF_APPLY_CC = TRUE)     // if clicing on apply color correct, it will make change to all colors set //WHY DOES THAT BUG ONLY FOR RF???
	{
		TFPColors->COLOR_CORRECT = TRUE;
		for (int i = 0; i < NB_RANGE_COLORS; i++)
		{
			RANGE_COLORS[i]->update_out();
			RANGE_COLORS[i]->IN = RANGE_COLORS[i]->OUT;
			RANGE_COLORS[i]->IN_HSLA = RANGE_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		RF_APPLY_CC = FALSE;            // and swich back off color correct till a new press
	}
	*/
	
	///////////////////////////////////////////////////////////


	MAP_COLORS_TO_ELEMENTS();

	onMouseMove();

	UPD_RECTS(TRUE);

	if (SHOW.STRIPES)
	{
		for (int i = 0; i < MAX_STRIPES; i++ )
		{
			STRIPES.STRIPES_COLORS[i] = COLOR.STRIPES.IN;
		}

	}

}////////////////////////////////////////////////////////////////////

TDrawType RangeFader::GET_PAINT_LAYER(int layer)

{

	UPD_PAINT_POINTERS();

	switch (layer)
	{

	case 0: 
		if (SHOW.BG)
		{
			SHAPE_ELMT = ELMT.BG;
			return DT_Shape;
		}	
		else { return DT_None; }
		break;

	case 1:
			SHAPE_ELMT = ELMT.BAR;
			return DT_Shape;
		break;

	case 2: if (SHOW.STRIPES) //fader dash /stripes
	{
		return DT_TRect_Array;
	}
			else { return DT_None; }
			break;//stripes

	case 3:
		SHAPE_ELMT = ELMT.CAP_DWN;
		return DT_Shape;
		break;

	case 4:
		SHAPE_ELMT = ELMT.CAP_UP;
		return DT_Shape;
		break;

	case 5:
		SHAPE_ELMT = ELMT.CAP_MID;
		return DT_Shape;
		break;

		
	case 6: if (TFParams->SHOW_VALUE)
	{
		VALUE_ELMT.RECT = ELMT.CAP_UP.RECT;
		VALUE_ELMT.VALUE_STRING = UP_VALUE_STRING;
		VALUE_ELMT.H_Align = taCenter;
		VALUE_ELMT.V_Align = taCenter;
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
		return DT_Text_Value;
		break;
	}
			else { return DT_None; }
			break;

	case 7: if (TFParams->SHOW_VALUE)
	{
		VALUE_ELMT.RECT = ELMT.CAP_DWN.RECT;
		VALUE_ELMT.VALUE_STRING = DWN_VALUE_STRING;
		VALUE_ELMT.H_Align = taCenter;
		VALUE_ELMT.V_Align = taCenter;
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
		return DT_Text_Value;
		break;
	}
			else { return DT_None; }
			break;

	case 8: if (TFParams->SHOW_VALUE)
	{
		VALUE_ELMT.RECT = ELMT.CAP_MID.RECT;
		VALUE_ELMT.VALUE_STRING = MID_VALUE_STRING;
		VALUE_ELMT.H_Align = taCenter;
		VALUE_ELMT.V_Align = taCenter;
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
		return DT_Text_Value;
		break;
	}
			else { return DT_None; }
			break;


	case 9: if (TFParams->SHOW_VALUE)
	{
		VALUE_ELMT.RECT = ELMT.BAR.RECT;
		VALUE_ELMT.VALUE_STRING = WIDTH_VALUE_STRING;
		VALUE_ELMT.H_Align = taTrailing;
		VALUE_ELMT.V_Align = taTrailing;
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
		return DT_Text_Value;
		break;
	}
			else { return DT_None; }
			break;



	default:return DT_None;	break;
	}
}

void RangeFader::UPD_PAINT_POINTERS()
{
	// the TFPaint structure will point to local elemnts

	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	TFPaintP->SHAPE = &SHAPE_ELMT;
	TFPaintP->STRIPES_MAX_PIX = &STRIPES.MAX_PIX;
	TFPaintP->STRIPES_STRIPES_COLORS = STRIPES.STRIPES_COLORS;
	TFPaintP->STRIPES_STRIPES_LEDS = STRIPES.STRIPES_LEDS;
	TFPaintP->COLOR_Grid_OUT = nullptr;// &COLOR.Grid.OUT;
	TFPaintP->FX_FLARE = nullptr;
	TFPaintP->FX_FLARES_COLORS = nullptr;
	TFPaintP->FX_FLARES_SIZES = nullptr;
	TFPaintP->FX_FLARES_SYM_COLORS = nullptr;
	TFPaintP->FX_FLARES_SYM_SIZES = nullptr;
	TFPaintP->FX_FLARE_SYM = nullptr;
	TFPaintP->FX_NB_FLARES = 0;
	TFPaintP->FX_REFLEXIONS_EACH = nullptr;
	TFPaintP->FX_REFLEXIONS_TYPE = nullptr;
	TFPaintP->GRID_GRID_FROM = nullptr;//GRID.GRID_FROM;
	TFPaintP->GRID_GRID_TO = nullptr;//GRID.GRID_TO;
	TFPaintP->GRID_SUBDIVS = nullptr;//&GRID.GRID_SUBDIVS;
	TFPaintP->LINE_ELMT = nullptr;
	TFPaintP->LINE_SIZE = nullptr;
	TFPaintP->SHAPE_NB_SEGS = nullptr;
	TFPaintP->SHAPE_SEG_FROM = nullptr;
	TFPaintP->STRIPES_DUAL_DRAW = nullptr;//&STRIPES.DUAL_DRAW;
	TFPaintP->STRIPES_STRIPES_LEDS_SYM = nullptr;//STRIPES.STRIPES_LEDS_SYM;
	TFPaintP->VALUE_ELMT = &VALUE_ELMT;

}

void RangeFader::UPD_ELEMENTS_ARRAY()
{
	// put each element in an array for easier apply setting to all, ie is round, border size ect
	RANGE_FADER_ELEMENTS[0] = &ELMT.GLOBAL;
	RANGE_FADER_ELEMENTS[1] = &ELMT.BG;
	RANGE_FADER_ELEMENTS[2] = &ELMT.BAR;
	RANGE_FADER_ELEMENTS[3] = &ELMT.CAP_DWN;
	RANGE_FADER_ELEMENTS[4] = &ELMT.CAP_MID;
	RANGE_FADER_ELEMENTS[5] = &ELMT.CAP_UP;

}

void RangeFader::UPD_STRIPES()
{
	// computes  stripes leds rectangles positions
		if (SHOW.STRIPES)
		{

			float t_range = 0;
			float y_range = 0;

			RANGE_BAR_HEIGHT = ELMT.CAP_DWN.RECT.top - ELMT.CAP_UP.RECT.bottom;
			STRIPES.MAX_PIX = (int)trunc(TFParams->CANVAS_H * RANGE_BAR_HEIGHT / STRIPES.MODULO);

			STRIPES.MAX_PIX = min(STRIPES.MAX_PIX, MAX_STRIPES);

			STRIPES.STEP_WIDTH = STRIPES.WIDTH*(0.5f / STRIPES.MAX_PIX)*RANGE_BAR_HEIGHT;

			for (int i = 0; i < STRIPES.MAX_PIX; i++)
			{
				t_range = (float)i / (float)STRIPES.MAX_PIX;
				y_range = t_range*(RANGE_BAR_HEIGHT)+ELMT.CAP_UP.RECT.bottom;

				STRIPES.STRIPES_LEDS[i].bottom = y_range + (STRIPES.STEP_WIDTH);
				STRIPES.STRIPES_LEDS[i].top = y_range - (STRIPES.STEP_WIDTH);
			}
		}//loop

}// end ups stripes

void RangeFader::UPD_RECTS(bool full_update)
{
	//if values changed update visual aspect of layout rectangles
	if (full_update)   // if full update relayout both static and dynamic parts of rects, if not only what need to be udpdated will be
		              // ie rects lefts and right part once set on init or setting changes don't really need to be updated each time a move occur, only top and bottom need
	{
		ELMT.CAP_UP.RECT.right = RANGE_BOX_W;
		ELMT.CAP_UP.RECT.left = 1 - ELMT.CAP_UP.RECT.right;
		ELMT.BAR.RECT.left = ELMT.CAP_UP.RECT.left + RANGE_BAR_REDUX;
		ELMT.BAR.RECT.right = ELMT.CAP_UP.RECT.right - RANGE_BAR_REDUX;
		ELMT.CAP_DWN.RECT.left = ELMT.CAP_UP.RECT.left;
		ELMT.CAP_DWN.RECT.right = ELMT.CAP_UP.RECT.right;
		ELMT.CAP_MID.RECT.left = ELMT.CAP_UP.RECT.left + RANGE_BAR_REDUX*2.0f;
		ELMT.CAP_MID.RECT.right = ELMT.CAP_UP.RECT.right - RANGE_BAR_REDUX*2.0f;
		ELMT.BG.RECT.left = ELMT.CAP_DWN.RECT.left;
		ELMT.BG.RECT.right = ELMT.CAP_DWN.RECT.right;
		ELMT.BG.RECT.top = 0;
		ELMT.BG.RECT.bottom = 1;


		for (int i = 0; i < MAX_STRIPES; i++)
		{
			STRIPES.STRIPES_LEDS[i].left = ELMT.BAR.RECT.left + 0.05f;
			STRIPES.STRIPES_LEDS[i].right = ELMT.BAR.RECT.right - 0.05f;
		}

	}

	ELMT.CAP_UP.RECT.top = (1.0f - R_UP_VAL);
	ELMT.CAP_UP.RECT.bottom = ELMT.CAP_UP.RECT.top + R_W;
	ELMT.CAP_DWN.RECT.bottom = (1.0f - R_DWN_VAL);
	ELMT.CAP_DWN.RECT.top = ELMT.CAP_DWN.RECT.bottom - R_W;
	ELMT.BAR.RECT.top = ELMT.CAP_UP.RECT.bottom;
	ELMT.BAR.RECT.bottom = ELMT.CAP_DWN.RECT.top;
	ELMT.CAP_MID.RECT.top = ((ELMT.BAR.RECT.top + ELMT.BAR.RECT.bottom) / 2.0f) - (R_W / 2.0f);
	ELMT.CAP_MID.RECT.bottom = ELMT.CAP_MID.RECT.top + R_W;

}///end upd rects

bool RangeFader::INSIDE(TFShape in_shape)
{
	// reurns if mouse is in input shape rectangle
	return sdkPointInRect(TFParams->MSX, TFParams->MSY, in_shape.RECT);

}

void RangeFader::UPD_STRINGS()
{
	// update / generate string from values  to pass to paint

	if (TFParams->SHOW_VALUE)
	{
		UP_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(R_UP_REAL_VAL, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		DWN_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(R_DWN_REAL_VAL, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		MID_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(R_MID_REAL_VAL, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		WIDTH_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(R_WIDTH_REAL_VAL, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
	}

}//endupd strings

void RangeFader::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier manips
	RANGE_COLORS[0] = &COLOR.Global;
	RANGE_COLORS[1] = &COLOR.CAP_UP;
	RANGE_COLORS[2] = &COLOR.CAP_UP_BORDER;
	RANGE_COLORS[3] = &COLOR.CAP_DWN;
	RANGE_COLORS[4] = &COLOR.CAP_DWN_BORDER;
	RANGE_COLORS[5] = &COLOR.CAP_MID;
	RANGE_COLORS[6] = &COLOR.CAP_MID_BORDER;
	RANGE_COLORS[7] = &COLOR.BAR;
	RANGE_COLORS[8] = &COLOR.BAR_BORDER;
	RANGE_COLORS[9] = &COLOR.BAR;
	RANGE_COLORS[10] = &COLOR.BG;
	RANGE_COLORS[11] = &COLOR.BG_BORDER;
	RANGE_COLORS[12] = &COLOR.STRIPES;
	///////////////////////////////////////
}///end upd_color_array

void RangeFader::UPD_COLORS_OUT()
{
		COLOR.CAP_UP.HILIGHT_MULT = (RANGE_UP_ACTIVE || RANGE_MID_ACTIVE)* 0.5f;	
		COLOR.CAP_UP_BORDER.HILIGHT_MULT = (RANGE_UP_ACTIVE || RANGE_MID_ACTIVE) * 0.5f;
		COLOR.CAP_DWN.HILIGHT_MULT = (RANGE_DWN_ACTIVE || RANGE_MID_ACTIVE) * 0.5f;
		COLOR.CAP_DWN_BORDER.HILIGHT_MULT = (RANGE_DWN_ACTIVE || RANGE_MID_ACTIVE) * 0.5f;
		COLOR.CAP_MID.HILIGHT_MULT = RANGE_MID_ACTIVE * 0.5f;
		COLOR.CAP_MID_BORDER.HILIGHT_MULT = RANGE_MID_ACTIVE * 0.5f;
		COLOR.BAR.HILIGHT_MULT = RANGE_MID_ACTIVE * 0.4f;
		COLOR.BAR_BORDER.HILIGHT_MULT = RANGE_MID_ACTIVE * 0.4f;

	for (int i = 0; i < NB_RANGE_COLORS; i++)
	{
		RANGE_COLORS[i]->update_out();	
	}
	MAP_COLORS_TO_ELEMENTS();
}

void RangeFader::MAP_COLORS_TO_ELEMENTS()
{
	ELMT.BAR.FILL_COLOR = COLOR.BAR.OUT;
	ELMT.BAR.BORDER_COLOR = COLOR.BAR_BORDER.OUT;
	ELMT.BG.FILL_COLOR = COLOR.BG.OUT;
	ELMT.BG.BORDER_COLOR = COLOR.BG_BORDER.OUT;
	ELMT.CAP_DWN.FILL_COLOR = COLOR.CAP_DWN.OUT;
	ELMT.CAP_DWN.BORDER_COLOR = COLOR.CAP_DWN_BORDER.OUT;
	ELMT.CAP_MID.FILL_COLOR = COLOR.CAP_MID.OUT;
	ELMT.CAP_MID.BORDER_COLOR = COLOR.CAP_MID_BORDER.OUT;
	ELMT.CAP_UP.FILL_COLOR = COLOR.CAP_UP.OUT;
	ELMT.CAP_UP.BORDER_COLOR = COLOR.CAP_UP_BORDER.OUT;
}

void RangeFader::PRESET_LOAD(int preset)

{


	switch (preset)
	{
	case 0:///////////////////////////// 0_ DFT_SKIN
		//---------------
		RF_APPLY_CC = FALSE;
		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 1.0;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f; 
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//----------------------------------
		RANGE_BOX_H = 0.1f;
		RANGE_BOX_W = 0.9f;
		RANGE_BAR_REDUX = 0.05f;
		//-----------------------------------
		SHOW.BG = TRUE;
		SHOW.BG_BORDER = TRUE;
		SHOW.CAP_BORDER = TRUE;
		SHOW.STRIPES = TRUE;
		//-------------------------------
		COLOR.CAP_UP.IN = 0x83276688;//1
		COLOR.CAP_UP_BORDER.IN = 0xC03984AD;//2
		COLOR.CAP_DWN.IN = 0x83276688;//3
		COLOR.CAP_DWN_BORDER.IN = 0xC03984AD;//4
		COLOR.CAP_MID.IN = 0x7A56626C;//5
		COLOR.CAP_MID_BORDER.IN = 0x7A5A82A2;//6
		COLOR.BAR.IN = 0x7A3A4954;//7
		COLOR.BAR_BORDER.IN = 0x7A1E272E;//8
		COLOR.BG.IN = 0x7A2E2E2E;//9
		COLOR.BG_BORDER.IN = 0x7A5A82A2;//10
		COLOR.STRIPES.IN = 0x7A1F2B35;//11
		//-----------------------------------
		STRIPES.MODULO = 4; 
		STRIPES.WIDTH = 0.4f; 
		//----------------------------------------
		ELMT.GLOBAL.ROUND_TYPE = 1;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.BORDER_SIZE = 4.0f;
		//----------------------
		break;
		/////////////////////////////////////////////////////////
	case 1:///////////////////////////// 1_ Minimal_SKIN
		   //---------------
		RF_APPLY_CC = FALSE;
		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 1.0f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//----------------------------------
		RANGE_BOX_H = 0.1f;
		RANGE_BOX_W = 0.9f;
		RANGE_BAR_REDUX = 0.05f;
		//-----------------------------------
		SHOW.BG = TRUE;
		SHOW.BG_BORDER = TRUE;
		SHOW.CAP_BORDER = TRUE;
		SHOW.STRIPES = FALSE;
		//-------------------------------
		COLOR.CAP_UP.IN = 0x83276688;//1
		COLOR.CAP_UP_BORDER.IN = 0xC03984AD;//2
		COLOR.CAP_DWN.IN = 0x83276688;//3
		COLOR.CAP_DWN_BORDER.IN = 0xC03984AD;//4
		COLOR.CAP_MID.IN = 0x7A56626C;//5
		COLOR.CAP_MID_BORDER.IN = 0x7A5A82A2;//6
		COLOR.BAR.IN = 0x7A3A4954;//7
		COLOR.BAR_BORDER.IN = 0x7A1E272E;//8
		COLOR.BG.IN = 0x7A2E2E2E;//9
		COLOR.BG_BORDER.IN = 0x7A5A82A2;//10
		COLOR.STRIPES.IN = 0x7A1F2B35;//11
									  //-----------------------------------
		STRIPES.MODULO = 4;
		STRIPES.WIDTH = 0.4f;
		//----------------------------------------
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.BORDER_SIZE = 2.0f;
		//----------------------
		break;
		/////////////////////////////////////////////////////////

	default:
		break;
	}



}


void RangeFader::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.CAP_UP.IN = 0x83276688;//1
	COLOR.CAP_UP_BORDER.IN = 0xC03984AD;//2
	COLOR.CAP_DWN.IN = 0x83276688;//3
	COLOR.CAP_DWN_BORDER.IN = 0xC03984AD;//4
	COLOR.CAP_MID.IN = 0x7A56626C;//5
	COLOR.CAP_MID_BORDER.IN = 0x7A5A82A2;//6
	COLOR.BAR.IN = 0x7A3A4954;//7
	COLOR.BAR_BORDER.IN = 0x7A1E272E;//8
	COLOR.BG.IN = 0x7A2E2E2E;//9
	COLOR.BG_BORDER.IN = 0x7A5A82A2;//10
	COLOR.STRIPES.IN = 0x7A1F2B35;//11
	///////////////

	TFPColors->COLOR_CORRECT = TRUE;
	//APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
//	TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum


	for (int i = 0; i < NB_RANGE_COLORS; i++)
	{
		RANGE_COLORS[i]->OnSetQuickColor();
		//FADER_COLORS[i]->IN = FADER_COLORS[i]->OUT;
		//FADER_COLORS[i]->IN_HSLA = FADER_COLORS[i]->OUT_HSLA;
	}

	TFPColors->HUE_ROLL = 0;
	TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
	TFPColors->LUMINOSITY_OFFSET = 0;
	TFPColors->SATURATION_OVERIDE = 0;
	//APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	TFPColors->COLOR_CORRECT = FALSE;


	onSettingsHasChanged(); //reupdates
}
///////////////////////////////////////////////////////////////////////