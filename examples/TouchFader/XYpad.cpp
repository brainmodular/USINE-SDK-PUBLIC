//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "XYpad.h"

XYpad::XYpad()
{

}

XYpad::XYpad(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{
	TFParams = TFParams_val;
	TFPaintP = TFPaintP_val;;

	ELMT.BG.RECT.left = 0;
	ELMT.BG.RECT.right = 1;
	ELMT.BG.RECT.top = 0;
	ELMT.BG.RECT.bottom = 1;

	ELMT.XY_RECT.left = 0;
	ELMT.XY_RECT.right = 0.5;
	ELMT.XY_RECT.top = 0.5;
	ELMT.XY_RECT.bottom = 1;

	TFPColors = new TFPColor();   //destruct??

	UPD_COLOR_ARRAY();
	for (int i = 0; i < NB_XYPAD_COLORS; i++) { *XYPAD_COLORS[i] = TFColor(TFParams, TFPColors); }

	PRESET_LOAD(0);

	onSettingsHasChanged(); //import settings
}

void XYpad::onMouseDown()
{
	TFParams->hilight = 1;
	UPD_FROM_MOUSE();
	TFParams->ITEM_VALUE_Z = TFParams->MSDWN;
	TFParams->ITEM_VALUE_SWITCH = TFParams->MSDWN;
	UPD_OUTPUTS();
	UPD_COLORS_OUT();
	UPD_STRINGS();
}

void XYpad::onInputChange()
{

	//YLINE = 1 - TFParams->input_val;
	YLINE = 1 - TFParams->ITEM_VALUE_Y;
	XLINE = TFParams->ITEM_VALUE_X;

	UPD_OUTPUTS();
	UPD_STRINGS();
}

void XYpad::onPreset()
{
	YLINE = 1 - TFParams->ITEM_VALUE_Y;
	XLINE = TFParams->ITEM_VALUE_X;
	UPD_OUTPUTS();
	UPD_STRINGS();
}

void XYpad::onMouseUp()
{
//	if (TFParams->AUTO_RESET)
//	{
//		XLINE = TFParams->RESET_VALUE;
//		YLINE = 1 - TFParams->RESET_VALUE;
//	}
	TFParams->hilight = 0;
	TFParams->ITEM_VALUE_Z = TFParams->MSDWN;
	TFParams->ITEM_VALUE_SWITCH = TFParams->MSDWN;
	UPD_OUTPUTS();
	UPD_COLORS_OUT();
}

void XYpad::onMouseMove()
{
	UPD_FROM_MOUSE();
	UPD_OUTPUTS();
	UPD_STRINGS();
}

void XYpad::onSettingsHasChanged()
{
	switch (ELMT.BG.ROUND_TYPE)
	{
	case 0: ELMT.BG.IS_ROUND = FALSE; ELMT.BG.ROUND_SMOOTHNESS = 0; break;//not round
	case 1: ELMT.BG.IS_ROUND = TRUE;  ELMT.BG.ROUND_SMOOTHNESS = 0; break;// round
	case 2: ELMT.BG.IS_ROUND = TRUE;  ELMT.BG.ROUND_SMOOTHNESS = 1; break;//hexagon
	default:break;
	}


	////////////////////////////////////////////////////importing colors as tfcolors
	COLOR.Global.update_in(0,TRUE, FALSE, 0.4f);
	COLOR.Cross.update_in(1,SHOW.CROSS, FALSE, 0.4f);
	COLOR.Dot.update_in(2,SHOW.DOT, FALSE, 0.4f);
	COLOR.Faders.update_in(3,SHOW.FADERS, FALSE, 0.4f);
	COLOR.Rect.update_in(4,SHOW.RECTANGLE, FALSE, 0.4f);
	COLOR.BG.update_in(5, SHOW.BG, FALSE, 0.1f);
	COLOR.BG_BORDER.update_in(6,ELMT.BG.SHOW_BORDER, TRUE, 0.1f);
	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE; // use gloabal from 0 tho individual are avaible

	///////////////////////////////////////////////////
	TFPColors->COLOR_CORRECT = TRUE;
	if (APPLY_COLOR_CORRECT)     // if clicing on apply color correct, it will make change to all colors set 
	{
		for (int i = 0; i < NB_XYPAD_COLORS; i++)
		{
			XYPAD_COLORS[i]->update_out();
			XYPAD_COLORS[i]->IN = XYPAD_COLORS[i]->OUT;
			XYPAD_COLORS[i]->IN_HSLA = XYPAD_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	}
	///////////////////////////////////////////////////////////


	UPD_COLOR_ARRAY();
	///////////////////////////////////////////////////////////////////////////////////////


	TFParams->ITEM_MAIN_OUT = MAIN_OUTPUT;
	UPD_COLORS_OUT();
	UPD_PAINT_POINTERS();
	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	UPD_STRINGS();
}

void XYpad::UPD_PAINT()
{


}

void XYpad::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier manips
	XYPAD_COLORS[0] = &COLOR.Global;
	XYPAD_COLORS[1] = &COLOR.Cross;
	XYPAD_COLORS[2] = &COLOR.Dot;
	XYPAD_COLORS[3] = &COLOR.Faders;
	XYPAD_COLORS[4] = &COLOR.Rect;
	XYPAD_COLORS[5] = &COLOR.BG;
	XYPAD_COLORS[6] = &COLOR.BG_BORDER;
	///////////////////////////////////////
}///end upd_color_array

void XYpad::UPD_COLORS_OUT()
{

	for (int i = 0; i < NB_XYPAD_COLORS; i++)
	{
		//if (XYPAD_COLORS[i]->IS_VISIBLE) //now in color proc
		{XYPAD_COLORS[i]->update_out();	}
	}
}

void XYpad::UPD_OUTPUTS()
{
	if (SHOW.RECTANGLE)
	{
		ELMT.XY_RECT.right = XLINE;
		ELMT.XY_RECT.top = YLINE;
	}

	TFParams->ITEM_VALUE_X = XLINE;
	TFParams->ITEM_VALUE_Y = 1-YLINE;
	//TFParams->ITEM_VALUE_Z = TFParams->MSDWN;
	//TFParams->ITEM_VALUE_SWITCH = TFParams->MSDWN;

}

void XYpad::UPD_FROM_MOUSE()
{
	if (RELATIVE_MODE)
	{
		XLINE = XLINE + (TFParams->DELTA_X*ENCODER_COEFF_X);
		YLINE = YLINE + (TFParams->DELTA_Y*ENCODER_COEFF_Y);
	}
	else
	{
		XLINE = TFParams->MSX;
		YLINE = TFParams->MSY;
	}
	XLINE = CLAMP01(XLINE);
	YLINE = CLAMP01(YLINE);

}

void XYpad::PRESET_LOAD(int preset)

{
	

	switch (preset)
	{
	case 0:///////////////////////////////0_DFT skin
		//--------------------------
		COLOR.Global.HUE_MODE = 0;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 1.0;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 1.0; 
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.25;
		//---------------------------
		SHOW.BG = TRUE;
		ELMT.BG.SHOW_BORDER = TRUE;
		SHOW.CROSS = TRUE;
		SHOW.DOT = TRUE;
		SHOW.FADERS = TRUE;
		SHOW.RECTANGLE = TRUE;
		//------------------------
		COLOR.BG.IN = 0x48202020;
		COLOR.BG_BORDER.IN = 0x8B445577;
		COLOR.Cross.IN = 0x8B445577;
		COLOR.Dot.IN = 0x8B445577;
		COLOR.Faders.IN = 0x8B445577;
		COLOR.Rect.IN = 0x48445577;
		//-----------------------
		XYPAD_CROSS_SIZE = 2;
		XYPAD_DOT_SIZE = 20;
		XYPAD_DOT_IS_ROUND = FALSE;
		XYPAD_FADERS_SIZE = 5;
		XYPAD_FAD_OPACITY = 0.5f;
		ELMT.BG.BORDER_SIZE = 2.0f;
		ELMT.BG.ROUND_TYPE = 1;
		ELMT.BG.ROUND_SIZE = 0.025f;
		break;
		//----------------------------
		///////////////////////////////////////////////
	case 1:///////////////////////////////1_ Minimal skin
		   //--------------------------
		COLOR.Global.HUE_MODE = 0;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 1.0f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 1.0f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0.0f;
		TFPColors->LUMINOSITY_OFFSET = 0.0f;
		TFPColors->SATURATION_OVERIDE = 0.0f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.25f;
		//---------------------------
		SHOW.BG = FALSE;
		ELMT.BG.SHOW_BORDER = FALSE;
		SHOW.CROSS = FALSE;
		SHOW.DOT = TRUE;
		SHOW.FADERS = FALSE;
		SHOW.RECTANGLE = FALSE;
		//------------------------
		COLOR.BG.IN = 0x48202020;
		COLOR.BG_BORDER.IN = 0x8B445577;
		COLOR.Cross.IN = 0x8B445577;
		COLOR.Dot.IN = 0x8B445577;
		COLOR.Faders.IN = 0x8B445577;
		COLOR.Rect.IN = 0x48445577;
		//-----------------------
		XYPAD_CROSS_SIZE = 2;
		XYPAD_DOT_SIZE = 20;
		XYPAD_DOT_IS_ROUND = TRUE;
		XYPAD_FADERS_SIZE = 5;
		XYPAD_FAD_OPACITY = 0.5;
		ELMT.BG.BORDER_SIZE = 2.0;
		ELMT.BG.ROUND_TYPE = 1;
		ELMT.BG.ROUND_SIZE = 0.025f;
		break;
		//----------------------------
		///////////////////////////////////////////////


	default:
		break;
	}



}

TDrawType XYpad::GET_PAINT_LAYER(int layer)
{
	switch (layer)
	{
	case 0:
		if (SHOW.BG)
		{
			ELMT.BG.FILL_COLOR = COLOR.BG.OUT;
			ELMT.BG.BORDER_COLOR = COLOR.BG_BORDER.OUT;
			TFPaintP->SHAPE = &ELMT.BG;
			return DT_Shape;
		}
		else { return DT_None; }
		break;



	case 1:
		if (SHOW.RECTANGLE)
		{
			SHAPE_ELMT.IS_ROUND = FALSE;
			SHAPE_ELMT.RECT = ELMT.XY_RECT;
			SHAPE_ELMT.FILL_COLOR = COLOR.Rect.OUT;
			SHAPE_ELMT.BORDER_SIZE = 1.0;
			SHAPE_ELMT.SHOW_BORDER = FALSE;
			SHAPE_ELMT.BORDER_COLOR = COLOR.Rect.OUT;
			TFPaintP->SHAPE = &SHAPE_ELMT;
			return DT_Shape;
		}
		else { return DT_None; }
		break;

	case 2:
		if (SHOW.CROSS)
		{
			LINE_ELMT.LINE_FROM.x = 0;
			LINE_ELMT.LINE_FROM.y = YLINE;
			LINE_ELMT.LINE_TO.x = 1;
			LINE_ELMT.LINE_TO.y = YLINE;
			LINE_ELMT.LINE_COLOR = COLOR.Cross.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_CROSS_SIZE;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			return DT_Line;
		}
		else { return DT_None; }
		break;

	case 3:
		if (SHOW.CROSS)
		{
			LINE_ELMT.LINE_FROM.y = 0;
			LINE_ELMT.LINE_FROM.x = XLINE;
			LINE_ELMT.LINE_TO.y = 1;
			LINE_ELMT.LINE_TO.x = XLINE;
			LINE_ELMT.LINE_COLOR = COLOR.Cross.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_CROSS_SIZE;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			return DT_Line;
		}
		else { return DT_None; }
		break;

	case 4:
		if (SHOW.FADERS)
		{
			LINE_ELMT.LINE_FROM.x = 0;
			LINE_ELMT.LINE_FROM.y = YLINE;
			LINE_ELMT.LINE_TO.y = 1;
			LINE_ELMT.LINE_TO.x = 0;
			LINE_ELMT.LINE_COLOR = COLOR.Faders.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_FADERS_SIZE;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			return DT_Line;
		}
		else { return DT_None; }
		break;

	case 5:
		if (SHOW.FADERS)
		{
			LINE_ELMT.LINE_FROM.x = 0;
			LINE_ELMT.LINE_FROM.y = 1;
			LINE_ELMT.LINE_TO.x = XLINE;
			LINE_ELMT.LINE_TO.y = 1;
			LINE_ELMT.LINE_COLOR = COLOR.Faders.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_FADERS_SIZE;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			return DT_Line;
		}
		else { return DT_None; }
		break;

	case 6:
		if (SHOW.DOT)
		{
			LINE_ELMT.LINE_FROM.x = XLINE;
			LINE_ELMT.LINE_FROM.y = YLINE;
			LINE_ELMT.LINE_COLOR = COLOR.Dot.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_DOT_SIZE;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			if (XYPAD_DOT_IS_ROUND){ return DT_Round_Dot; }
			else { return DT_Square_Dot; }
		}
		else { return DT_None; }
		break;

	case 7:
		if (SHOW.DOT && TFParams->MSDWN)
		{
			LINE_ELMT.LINE_FROM.x = XLINE;
			LINE_ELMT.LINE_FROM.y = YLINE;
			LINE_ELMT.LINE_COLOR = COLOR.Dot.OUT;
			LINE_ELMT.LINE_SIZE = XYPAD_DOT_SIZE*2.0f;
			TFPaintP->LINE_ELMT = &LINE_ELMT;
			if (XYPAD_DOT_IS_ROUND){ return DT_Round_Dot; }
			else { return DT_Square_Dot; }
		}
		else { return DT_None; }
		break;

	case 8:
		if (TFParams->SHOW_VALUE)
		{
			VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
			VALUE_ELMT.H_Align = taCenter;
			VALUE_ELMT.V_Align = taLeading;
			VALUE_ELMT.RECT = TFParams->VALUE_RECT;
			VALUE_ELMT.VALUE_STRING = VALUE_STRING;
			return DT_Text_Value;
		}

		else { return DT_None; }
		break;




	//case 10: return DT_XY_PAD; break;

	


	default: return DT_None;  break;
	}



}
//////////////////////////////////////////
void XYpad::UPD_PAINT_POINTERS()
{
	TFPaintP->COLOR_Grid_OUT = nullptr;// &COLOR.Grid.OUT;
	TFPaintP->FX_FLARE = nullptr;//&FX.FLARE;
	TFPaintP->FX_FLARES_COLORS = nullptr;//FX.FLARES_COLORS;
	TFPaintP->FX_FLARES_SIZES = nullptr;//FX.FLARES_SIZES;
	TFPaintP->FX_FLARES_SYM_COLORS = nullptr;//FX.FLARES_SYM_COLORS;
	TFPaintP->FX_FLARES_SYM_SIZES = nullptr;//FX.FLARES_SYM_SIZES;
	TFPaintP->FX_FLARE_SYM = nullptr;//&FX.FLARE_SYM;
	TFPaintP->FX_NB_FLARES = 0;// nullptr;//FX.NB_FLARES;
	TFPaintP->FX_REFLEXIONS_EACH = nullptr;//&FX.REFL_DRAW_EACH;
	TFPaintP->FX_REFLEXIONS_TYPE = nullptr;//&FX.REFLEXIONS_TYPE;
	TFPaintP->GRID_GRID_FROM = nullptr;// GRID.GRID_FROM;
	TFPaintP->GRID_GRID_TO = nullptr;//GRID.GRID_TO;
	TFPaintP->GRID_SUBDIVS = nullptr;// &GRID.GRID_SUBDIVS;
	TFPaintP->LINE_ELMT = &LINE_ELMT;
	TFPaintP->LINE_SIZE = &LINE_ELMT.LINE_SIZE;
	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	TFPaintP->SHAPE = &SHAPE_ELMT;
	TFPaintP->SHAPE_NB_SEGS = &SHAPE_NB_SEGS;
	TFPaintP->SHAPE_SEG_FROM = &SHAPE_SEG_FROM;
	TFPaintP->STRIPES_DUAL_DRAW = nullptr;// &STRIPES.DUAL_DRAW;
	TFPaintP->STRIPES_MAX_PIX = nullptr;// &STRIPES.MAX_PIX;
	TFPaintP->STRIPES_STRIPES_COLORS = nullptr;// STRIPES.STRIPES_COLORS;
	TFPaintP->STRIPES_STRIPES_LEDS = nullptr;//STRIPES.STRIPES_LEDS;
	TFPaintP->STRIPES_STRIPES_LEDS_SYM = nullptr;// STRIPES.STRIPES_LEDS_SYM;
	TFPaintP->VALUE_ELMT = &VALUE_ELMT;

}
///////////////////////////////////////////////////////////////////////
void XYpad::UPD_STRINGS()
{
	if (TFParams->SHOW_VALUE)
	{
		Y_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(TFParams->ITEM_VALUE_Y, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		X_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(TFParams->ITEM_VALUE_X, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		VALUE_STRING = "";
		VALUE_STRING.append(Y_VALUE_STRING);
		VALUE_STRING.append(" - ");
		VALUE_STRING.append(X_VALUE_STRING);
	}

}// upd string

void XYpad::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.BG.IN = 0x48202020;
	COLOR.BG_BORDER.IN = 0x8B445577;
	COLOR.Cross.IN = 0x8B445577;
	COLOR.Dot.IN = 0x8B445577;
	COLOR.Faders.IN = 0x8B445577;
	COLOR.Rect.IN = 0x48445577;
	///////////////

	TFPColors->COLOR_CORRECT = TRUE;
	APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
//	TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum


	for (int i = 0; i < NB_XYPAD_COLORS; i++)
	{
		XYPAD_COLORS[i]->OnSetQuickColor();
		//FADER_COLORS[i]->IN = FADER_COLORS[i]->OUT;
		//FADER_COLORS[i]->IN_HSLA = FADER_COLORS[i]->OUT_HSLA;
	}

	TFPColors->HUE_ROLL = 0;
	TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
	TFPColors->LUMINOSITY_OFFSET = 0;
	TFPColors->SATURATION_OVERIDE = 0;
	APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	TFPColors->COLOR_CORRECT = FALSE;


	onSettingsHasChanged(); //reupdates
}