// ----------------------------------------------------------------------------
// ----------------- TF PARAM TYPES : Generic Item component types
// ----------------------------------------------------------------------------
#include "TFParamTypes.h"


TFPStruct::TFPStruct()
{
	ITEM_VALUE_W = 0;
	ITEM_VALUE_X = 0;
	ITEM_VALUE_Y = 0;
	ITEM_VALUE_Z = 0;
	ITEM_VALUE_SWITCH = false;
	ITEM_MAIN_OUT = 0;

	MIN_POS = 0;
	MAX_POS = 0;

	SHOW_VALUE = true;
	VALUE_PRECISION = 2;
	VALUE_PRECISION_DECA = 1;
	OUT_MIN = 0;
	OUT_MAX = 1;

	QUANTIZE_INT = 1;

	MSDWN = false;
	MSX = 0;
	MSY = 0;
	X = 0;
	Y = 0;
	RESET_VALUE = 0;
	RESET_VALUE_01 = 0;
	input_val = 0;
	item_val = 0;

//	ITEM_HUE = 200;
	CANVAS_RATIO = 1;
	CANVAS_S = 300;
	hilight = 0;


	DELTA_X = 0;
	DELTA_Y = 0;

	PULSE = FALSE;
	PUSE_DURATION = 300;
	PULSE_count = -1;
	PULSE_RAMP = 0;

	VALUE_RECT.left = 0;
	VALUE_RECT.right = 1.0;
	VALUE_RECT.top = 0;
	VALUE_RECT.bottom = 1;

	VALUE_ON_COLOR = 0xFFCBCBCB;
	VALUE_OFF_COLOR = 0xFFCBCBCB;
	NAME_ON_COLOR = 0xFFCBCBCB;
	NAME_OFF_COLOR = 0xFFCBCBCB;
}

TFPColor::TFPColor()
{
//	TColorUsine SWICH_COLOR;
	//bool USE_SWITCH_COLOR = FALSE;
	//int HUE_BEAVIOUR=0;
	//int HUE_ROLL = 0;
	//float SATURATION_OVERIDE = 0;
	//float LUMINOSITY_CONTRAST = 0;
	//float LUMINOSITY_OFFSET = 0;
	//float LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
	//bool COLOR_CORRECT = FALSE;
	//TBlendMode BLEND_MODE = BM_Replace;
	//float BLEND_MIX= 1.0;
}

TFColor::TFColor()
{
	H = 200;
	L = 0.25f;
	S = 0.15f;
	A = 0.15f;
}

TFColor::TFColor(TFPStruct *TFParams_val, TFPColor *TFPColor_val)
{
	H = 200;
	L = 0.25;
	S = 0.25;
	A = 0.25;


	TFParams = TFParams_val;
	TFPColors = TFPColor_val;

}

TFColor::TFColor(float s, float l, float a)
{
	H = 200;
	L = l;
	S = s;
	A = a;
}

TFColor::TFColor(float h, float s, float l, float a)
{
	H = h;
	L = l;
	S = s;
	A = a;
}

void TFColor::update_in(int id, bool is_visible, bool is_border, float hilight_mult, bool  tgt_is_next)
{

	ID = id;
	IS_VISIBLE = is_visible;
	IS_BORDER = is_border;
	TGT_IS_NEXT = tgt_is_next;
	HILIGHT_MULT = hilight_mult;

	IN_HSLA = sdkAlphaColorToColorAhsl(IN);

	H = IN_HSLA.h;
	L = IN_HSLA.l;
	S = IN_HSLA.s;
	A = IN_HSLA.a;

	OUT_HSLA = IN_HSLA;
	OUT = IN;
}

void TFColor::OnSetQuickColor()
{
	    IN_HSLA = sdkAlphaColorToColorAhsl(IN);
		OUT_HSLA = IN_HSLA; //by default out is in of palette
		OUT_HSLA.h = (float)TFPColors->HUE_ROLL; // use quick color  overides hue of palette
		OUT_HSLA.s = TFPColors->SATURATION_OVERIDE; // use quick color  overides sat of palette
		///////////////////////////////// //! could be placed only in import
		//if (TFPColors->COLOR_CORRECT)  // if a global color correction is used 
		//{
          OUT_HSLA.l += TFPColors->LUMINOSITY_OFFSET*0.5f;  // Apply eventual lum offset
		//
		//}
		/////////////////////////////////
		if (TFPColors->USE_TARGET_COLOR)  //if a target color (replace) is requested
		{
			TGT_HSLA = sdkAlphaColorToColorAhsl(TFPColors->TARGET_COLOR);
			OUT_HSLA = MIX_COLORS(OUT_HSLA, TGT_HSLA, TFPColors->BLEND_MODE, TFPColors->BLEND_MIX);
		}
		///////////////////////////////  Secured mod/clamps
		{
			OUT_HSLA.h = fmodf(OUT_HSLA.h, 360);
			OUT_HSLA.s = CLAMP01(OUT_HSLA.s);
			OUT_HSLA.l = CLAMP01(OUT_HSLA.l);
			OUT_HSLA.a = CLAMP01(OUT_HSLA.a);
		}
		//////////////////////////
		OUT = sdkColorAhslToAlphaColor(OUT_HSLA);
		IN = OUT;
	 //////////////////////////////////////
}//end update Quick color

void TFColor::update_out()
{
	if (IS_VISIBLE) // will update only if is visible
	{
		bool need_check = FALSE; // used to secure clamp output if operations are made

		OUT_HSLA = IN_HSLA; //by default out is in

		///////////////////////////
		if (HILIGHT_MULT != 0 && TFPColors->LUMINOSITY_HIGLIGHT_FORCE !=0) // if color got a highlight mult apply it
		{
			OUT_HSLA.l = L + (TFParams->hilight * HILIGHT_MULT * TFPColors->LUMINOSITY_HIGLIGHT_FORCE);
			need_check = TRUE;
		}
		///////////////////////////////// //! could be placed only in import
		if (TFPColors->COLOR_CORRECT)  // if a global color correction is used 
		{
			//OUT_HSLA.l = (OUT_HSLA.l - 0.5)*(TFPColors->LUMINOSITY_CONTRAST*4.0);
			//OUT_HSLA.l += 0.5;

			if (TFPColors->LUMINOSITY_CONTRAST > 0)  // ADJUST LUM CONTRAST
			{
				OUT_HSLA.l = RE_RANGE_NORM_IN(OUT_HSLA.l, 0 - TFPColors->LUMINOSITY_CONTRAST, 1 + TFPColors->LUMINOSITY_CONTRAST);
			}
			else
			{
				OUT_HSLA.l = RE_RANGE_NORM_IN(OUT_HSLA.l, 0 - TFPColors->LUMINOSITY_CONTRAST*0.5f, 1 + TFPColors->LUMINOSITY_CONTRAST*0.5f);
			}

			OUT_HSLA.l += TFPColors->LUMINOSITY_OFFSET*0.5f;  // Apply eventual lum offset

			if (TFPColors->SATURATION_OVERIDE > 0){ OUT_HSLA.s = TFPColors->SATURATION_OVERIDE; }// Saturation overide if >0
			need_check = TRUE;
		}
		/////////////////////////////////
		if (TFPColors->USE_TARGET_COLOR)  //if a target color (replace) is requested
		{
			TGT_HSLA = sdkAlphaColorToColorAhsl(TFPColors->TARGET_COLOR);
			OUT_HSLA = MIX_COLORS(OUT_HSLA, TGT_HSLA, TFPColors->BLEND_MODE, TFPColors->BLEND_MIX);
			need_check = TRUE;
		}
		///////////////////////////////
		if (TFPColors->HUE_BEAVIOUR != 2)  //if hue mode is not override it will be an operation based on module input hue
		{
			switch (TFPColors->HUE_BEAVIOUR)
			{
			case 0: OUT_HSLA.h = (float)(TFPColors->HUE_ROLL); break;  //input hue kept
			case 1: OUT_HSLA.h = OUT_HSLA.h + TFPColors->HUE_ROLL; break;  //offset input hue by colorset hue
				//case 2: HOUT = H; break;  //replace all
			default:	break;
			}
			need_check = TRUE;
		}
		else if (TFPColors->HUE_BEAVIOUR == 2) // if overide mode pick hue from color set
		{
			OUT_HSLA.h = OUT_HSLA.h + TFPColors->HUE_ROLL;
			need_check = TRUE;
		}
		///////////////////////////////////////////
		if (need_check)   //proceed clamp or modulo
		{
			OUT_HSLA.h = fmodf(OUT_HSLA.h, 360);
			OUT_HSLA.s = CLAMP01(OUT_HSLA.s);
			OUT_HSLA.l = CLAMP01(OUT_HSLA.l);
			OUT_HSLA.a = CLAMP01(OUT_HSLA.a);
		}
		//////////////////////////
		OUT = sdkColorAhslToAlphaColor(OUT_HSLA);
	}//if visible
		
//////////////////////////////////////
}//end update color

TColorAhsl TFColor::MIX_COLORS(TColorAhsl COLOR_A, TColorAhsl COLOR_B, TBlendMode BM_MODE, float MIX)
{
	float MIX_INV = 1 - MIX;
	TColorAhsl OUT_COLOR{0,0,0,0};

	switch (BM_MODE)
	{
	case BM_Replace: OUT_COLOR = COLOR_B;  // full replace 
		break;
	case BM_Mix:   // mix colors as transparent layer
		OUT_COLOR.a = COLOR_A.a * MIX_INV + COLOR_B.a * MIX;
		OUT_COLOR.h = COLOR_A.h * MIX_INV + COLOR_B.h * MIX;
		OUT_COLOR.s = COLOR_A.s * MIX_INV + COLOR_B.s * MIX;
		OUT_COLOR.l = COLOR_A.l * MIX_INV + COLOR_B.l * MIX;
		break;
	case BM_Additive: // color A increased by color B
		OUT_COLOR.a = COLOR_A.a  ;
		OUT_COLOR.h = COLOR_A.h  + COLOR_B.h * MIX;
		OUT_COLOR.s = COLOR_A.s  + COLOR_B.s * MIX;
		OUT_COLOR.l = COLOR_A.l  + COLOR_B.l * MIX;
		break;
	case BM_Multiply: // color A multiplied by color B
		OUT_COLOR.a = COLOR_A.a ;
		OUT_COLOR.h = (COLOR_A.h * MIX_INV) + (COLOR_B.h * MIX);
		OUT_COLOR.s = (COLOR_A.s * MIX_INV) + (COLOR_B.s * MIX);
		OUT_COLOR.l = (COLOR_A.l * MIX_INV) + (COLOR_B.l * COLOR_A.l * MIX);
		break;
	case BM_Overlay:  // darker parts are multiplied, higher added
		OUT_COLOR.a = COLOR_A.a;
		OUT_COLOR.h = (COLOR_A.h * MIX_INV) + (COLOR_B.h * MIX);
		OUT_COLOR.s = (COLOR_A.s * MIX_INV) + (COLOR_B.s * MIX);
		if (COLOR_A.l < 0.5){ OUT_COLOR.l = (COLOR_A.l *MIX_INV) + (COLOR_B.l * COLOR_A.l*MIX); }else{ OUT_COLOR.l = COLOR_A.l + (COLOR_B.l * MIX); };
		break;
	case BM_Color:   // affects only hue & sat componments
		OUT_COLOR.a = COLOR_A.a ;
		OUT_COLOR.h = COLOR_A.h * MIX_INV + COLOR_B.h * MIX;
		OUT_COLOR.s = COLOR_A.s * MIX_INV + COLOR_B.s * MIX;
		OUT_COLOR.l = COLOR_A.l;
		break;
	case BM_Luminosity:   // affects only lum componments
		OUT_COLOR.a = COLOR_A.a;
		OUT_COLOR.h = COLOR_A.h;
		OUT_COLOR.s = COLOR_A.s;
		OUT_COLOR.l = COLOR_A.l * MIX_INV + COLOR_B.l * MIX;
		break;
		

	default:
		break;
	}

	return OUT_COLOR;
}

TColorUsine TFColor::HSLA()
{
	//H = fmin(360, fmax(0, H));
	return sdkColorAhslToAlphaColor(sdkColorAhsl(fmodf(H, 360), fminf(1, fmaxf(S, 0)), fminf(1, fmaxf(L, 0)), fminf(1, fmaxf(A, 0))));
}

TFColorEffect::TFColorEffect()
{
	L_Hi = 0.33f;
	S_Hi = 0.25f;
	H_OnOffset = -180;
}

TFColorEffect::TFColorEffect(float L_Hi_val, float S_Hi_val, int H_OnOffset_val)
{
	L_Hi = L_Hi_val;
	S_Hi = S_Hi_val;
	H_OnOffset = H_OnOffset_val;
}

TFValue::TFValue()
{

}

TFLine::TFLine()
{

}

TFShape::TFShape()
{


//	RECT_BASE = { 0.5 - (Scale_X*0.5), 0.5 -(Scale_Y*0.5), 0.5+(Scale_X*0.5), 0.5+(Scale_Y*0.5) };
//	RestoreBaseRect();

}

void TFShape::Scale_Shape()
{
	RECT_BASE.left = 0.5f - (Scale_X*0.5f);
	RECT_BASE.top = 0.5f - (Scale_Y*0.5f);
	RECT_BASE.right = 0.5f + (Scale_X*0.5f);
	RECT_BASE.bottom = 0.5f + (Scale_Y*0.5f);
	RestoreBaseRect();
}

void TFShape::RestoreBaseRect()
{
	// TODO : find something to map a structure to another ?
	RECT.left = RECT_BASE.left;
	RECT.right = RECT_BASE.right;
	RECT.bottom = RECT_BASE.bottom;
	RECT.top = RECT_BASE.top;
}

void TFShape::RestoreBaseColors()
{
}

TFOutput::TFOutput()
{
	OutSrc[0] = -1;
	OutSrc[1] = -1;
	OutSrc[2] = -1;
	OutSrc[3] = -1;
}

TFPaint::TFPaint()
{
}


TColorUsine HI_COLOR(TColorUsine COLOR_IN, float Hilight_val)
{
	
	TColorAhsl TEMP_COLOR = sdkAlphaColorToColorAhsl(COLOR_IN);

	TEMP_COLOR.l += Hilight_val;
	TEMP_COLOR.l = CLAMP01(TEMP_COLOR.l);


	 return (sdkColorAhslToAlphaColor(TEMP_COLOR));
}
