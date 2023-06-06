// ----------------------------------------------------------------------------
// ----------------- BUTTON : Button Item
// ----------------------------------------------------------------------------
#include "Button.h"
Button::Button()
{

}

Button::Button(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{	
	TFParams = TFParams_val;
	TFPaintP = TFPaintP_val;

	SkinNb = 0;

	Cursor.X = 0;
	Cursor.Y = 0;

	// TODO : Default parameters from display preset
	XYLines_Nb = 0;
	XYLines_Sz = 2.0f;
	XYLines_Rect.left = 0.1f;
	XYLines_Rect.top = 0.0f;
	XYLines_Rect.right = 0.9f;
	XYLines_Rect.bottom = 1.0f;


	//ButtonSh = TFShape();             //need?
	//FX.OCCLUSION_SHAPE = TFShape();

	// TODO : Default parameters from display preset
	// isColorEffect = TRUE;
	//ColorEffect = TFColorEffect(0.33, 0.25 , - 180);
	REDUX = 0.025f;

	Output.OutSrc[0] = 0;

	// TODO : Asses the purpose of ITEM_MAIN_OUT
	TFParams->ITEM_MAIN_OUT = Output.OutSrc[0];

	TFPColors = new TFPColor();   //destruct??

	UPD_COLOR_ARRAY();
	for (int i = 0; i < NB_BUTTON_COLORS; i++) { *BUTTON_COLORS[i] = TFColor(TFParams, TFPColors); }

	TFPColors->COLOR_CORRECT = FALSE;
	TFPColors->BLEND_MODE = BM_Replace;
	TFPColors->BLEND_MIX = 1.0;
	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
	TFPColors->USE_TARGET_COLOR = FALSE;
	TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0; //bypass as swich will hilight itself

	ELMT.BUTTON_SHAPE.RATIO = TFParams->CANVAS_RATIO;
	FX.OCCLUSION_SHAPE.RATIO = TFParams->CANVAS_RATIO;

	PRESET_LOAD(0);

	COLOR.Global.HUE_MODE = 2;//full overide
	TFPColors->HUE_BEAVIOUR = 2;

	MAIN_OUTPUT = 3;

	onSettingsHasChanged(); //import settings
	//UPD_COLOR_ARRAY();
}

void Button::onMouseDown()
{	
	if (TOGGLE_TYPE == TTMomentary) // momentary button
	{
		//TFParams->ITEM_VALUE_SWITCH = !TFParams->INVERT_MODE;

		TFParams->ITEM_VALUE_SWITCH = 1;
	}
	else if (TOGGLE_TYPE == TTToggle)
	{
		TFParams->ITEM_VALUE_SWITCH = !TFParams->ITEM_VALUE_SWITCH;
	}
	TFParams->ITEM_VALUE_Z = TFParams->ITEM_VALUE_SWITCH;

	//UPD_XY();
	UPD_XY(CLAMP01(TFParams->MSX), CLAMP01(1 - TFParams->MSY));

	onStateChange();
}

void Button::onInputChange()
{

	//TFParams->ITEM_VALUE_SWITCH = (TFParams->input_val == 1);		

	//TFParams->ITEM_VALUE_Z = TFParams->ITEM_VALUE_SWITCH;

	UPD_XY(TFParams->ITEM_VALUE_X, TFParams->ITEM_VALUE_Y);
	onStateChange();
}


void Button::onPreset()
{

	TFParams->ITEM_VALUE_Z = TFParams->ITEM_VALUE_SWITCH;

		Cursor.Y = TFParams->ITEM_VALUE_Y;
		Cursor.X = TFParams->ITEM_VALUE_X;

		XYLines_Rect.top = CLAMP(1 - Cursor.Y, 0.25, 0.75);
		XYLines_Rect.left = CLAMP(Cursor.X, 0.25, 0.75);

	onStateChange();
}

void Button::onMouseUp()
{
	if (TOGGLE_TYPE == TTMomentary)
	{
//		TFParams->ITEM_VALUE_SWITCH = TFParams->INVERT_MODE;
//		TFParams->ITEM_VALUE_Z = TFParams->INVERT_MODE;
		TFParams->ITEM_VALUE_SWITCH = 0;
		TFParams->ITEM_VALUE_Z = 0;
		onStateChange();
	}
}

void Button::onMouseMove()
{
	UPD_XY(CLAMP01(TFParams->MSX),CLAMP01(1 - TFParams->MSY));
}

void Button::UPD_XY(float X, float Y)
{

	// If X line or Y line is displayed 
	if (XYLines_Nb > 0)
	{
		// TODO : see if factorization with other classes is needed
		// Get cursor position inside Item and set internal
		Cursor.Y = Y; 
		Cursor.X = X;  
		TFParams->ITEM_VALUE_X = Cursor.X;
		TFParams->ITEM_VALUE_Y = Cursor.Y;

		XYLines_Rect.top = CLAMP(1 - Cursor.Y, 0.25, 0.75);
		XYLines_Rect.left = CLAMP(Cursor.X, 0.25, 0.75);

		UPD_STRINGS();
	}

}

void Button::onStateChange()
{
	ApplyShapeEffect();
	ApplyColorEffect();
	UPD_STRINGS();
}

void Button::BaseShapeSet()
{
	ELMT.BUTTON_SHAPE.RECT = ELMT.BUTTON_SHAPE.RECT_BASE;
	ELMT.BUTTON_SHAPE.Scale_Shape();
	/*
	BUTTON_SHAPE.RECT.left = BUTTON_SHAPE.RECT_BASE.left + REDUX;
	BUTTON_SHAPE.RECT.right = BUTTON_SHAPE.RECT_BASE.right - REDUX;
	BUTTON_SHAPE.RECT.top = BUTTON_SHAPE.RECT_BASE.top + REDUX;
	BUTTON_SHAPE.RECT.bottom = BUTTON_SHAPE.RECT_BASE.bottom - REDUX;
	*/
}

void Button::ApplyShapeEffect()
{

	XYLines_Rect.top = CLAMP(1 - Cursor.Y + REDUX, 0.25, 0.75);
	XYLines_Rect.left = CLAMP(Cursor.X + REDUX, 0.25, 0.75);

	ELMT.BUTTON_SHAPE.Scale_Shape();
	//CONTAINER_SHAPE = BUTTON_SHAPE;

	switch (TRANSFORM)
	{
	case 0:   ELMT.BUTTON_SHAPE.Scale_Shape(); break;

	case 1: if (TFParams->ITEM_VALUE_SWITCH){ ELMT.BUTTON_SHAPE.RECT.bottom -= REDUX; }
			else{ ELMT.BUTTON_SHAPE.RECT.top += REDUX; } break; // V slide

	case 2: if (TFParams->ITEM_VALUE_SWITCH){ ELMT.BUTTON_SHAPE.RECT.left += REDUX; }//
			else{ ELMT.BUTTON_SHAPE.RECT.right -= REDUX; } break; // H slide

	case 3:  if (TFParams->ITEM_VALUE_SWITCH){ ELMT.BUTTON_SHAPE.RECT.left += REDUX / 2.0f; ELMT.BUTTON_SHAPE.RECT.right -= REDUX / 2.0f; ELMT.BUTTON_SHAPE.RECT.top += REDUX / 2.0f; ELMT.BUTTON_SHAPE.RECT.bottom -= REDUX / 2.0f; } break; //scale
	default:break;
	}

	if (FX.OCCLUSION){ UPD_OCCLUSION(); }

}

void Button::ApplyColorEffect()
{

	if (TFParams->ITEM_VALUE_SWITCH)
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
	}
	else
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_OFF_COLOR;
	}

	TFPColors->USE_TARGET_COLOR = TFParams->ITEM_VALUE_SWITCH;  //if swich value is on toggles to tgt color
	//////////////////////////////////////////
	for (int i = 0; i < NB_BUTTON_COLORS; i++)
	{
		if (BUTTON_COLORS[i]->TGT_IS_NEXT)
		{	TFPColors->TARGET_COLOR = BUTTON_COLORS[i+1]->IN;
			BUTTON_COLORS[i]->update_out();
		}
	}
	/////////////////////////////////end loop export
}

void Button::UPD_PAINT()
{
	//ApplyColorEffect();
}

void Button::onSettingsHasChanged()
{
	ELMT.BUTTON_SHAPE.RATIO = TFParams->CANVAS_RATIO;
	FX.OCCLUSION_SHAPE.RATIO = TFParams->CANVAS_RATIO;

	switch (ELMT.BUTTON_SHAPE.ROUND_TYPE)
	{
	case 0: ELMT.BUTTON_SHAPE.IS_ROUND = FALSE; ELMT.BUTTON_SHAPE.ROUND_SMOOTHNESS = 0; break;//not round
	case 1: ELMT.BUTTON_SHAPE.IS_ROUND = TRUE;  ELMT.BUTTON_SHAPE.ROUND_SMOOTHNESS = 0; break;// round
	case 2: ELMT.BUTTON_SHAPE.IS_ROUND = TRUE;  ELMT.BUTTON_SHAPE.ROUND_SMOOTHNESS = 1; break;//hexagon
	default:break;
	}

	/////////////////////////////////////////////////container
	ELMT.CONTAINER_SHAPE = ELMT.BUTTON_SHAPE;
	ELMT.CONTAINER_SHAPE.RECT.right = 1 - ELMT.BUTTON_SHAPE.RECT.left;
	ELMT.CONTAINER_SHAPE.RECT.top = 1 - ELMT.BUTTON_SHAPE.RECT.bottom;
	ELMT.CONTAINER_SHAPE.RECT = SCALE_RECT(ELMT.CONTAINER_SHAPE.RECT, 1.1f, 1.1f, TFParams->CANVAS_RATIO);
	////////////////////////////////////////////////////////////////////////////////
	ApplyShapeEffect();
	///////////////////////////////////////////////////////////////////////////////
	FX.OCCLUSION_SHAPE = ELMT.BUTTON_SHAPE;
	UPD_OCCLUSION();

	COLOR.Global.update_in    (0, TRUE, FALSE, 0.2f);
	COLOR.OFF.update_in       (1, TRUE, FALSE, 0.2f ,TRUE);
	COLOR.ON.update_in        (2, TRUE, FALSE, 0.2f);
	COLOR.OFF_Border.update_in(3, TRUE, TRUE, 0.2f  ,TRUE);
	COLOR.ON_Border.update_in (4, TRUE, TRUE, 0.2f);
	COLOR.XY_OFF.update_in    (5, TRUE, FALSE, 0.2f , TRUE);
	COLOR.XY_ON.update_in     (6, TRUE, FALSE, 0.2f);
	COLOR.CONTAINER.update_in (7, TRUE, FALSE, 0.1f);
	COLOR.CONTAINER_BORDER.update_in(8, TRUE,  FALSE, 0.1f);

	UPD_COLOR_ARRAY();

	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE; // use gloabal from 0 tho individual are avaible
	ApplyColorEffect();

	//////////////////////////////////////FX stuff
	switch (FX.LEVEL)
	{
	case 0: FX.OCCLUSION = FALSE; FX.SHADOWS = FALSE; FX.SPECULAR = FALSE; FX.REFLEXIONS = FALSE; FX.FLARES = FALSE; break;  //2d flat design low cpu
	case 1: FX.OCCLUSION = TRUE; FX.SHADOWS = FALSE; FX.SPECULAR = FALSE; FX.REFLEXIONS = FALSE; FX.FLARES = FALSE; break;  //only occlusion
	case 2: FX.OCCLUSION = TRUE; FX.SHADOWS = TRUE; FX.SPECULAR = FALSE; FX.REFLEXIONS = FALSE; FX.FLARES = FALSE; break;  //+shadows
	case 3: FX.OCCLUSION = TRUE; FX.SHADOWS = TRUE; FX.SPECULAR = TRUE; FX.REFLEXIONS = FALSE; FX.FLARES = FALSE; break;  // shadows & speculars
	case 4: FX.OCCLUSION = TRUE; FX.SHADOWS = TRUE; FX.SPECULAR = TRUE; FX.REFLEXIONS = TRUE; FX.FLARES = FALSE; break;  // shadows & speculars & reflexions
	case 5: FX.OCCLUSION = TRUE; FX.SHADOWS = TRUE; FX.SPECULAR = TRUE; FX.REFLEXIONS = TRUE; FX.FLARES = TRUE; break;// shadows & speculars & reflexions & flares
	default:		break;
	}

	if (FX.LEVEL > 0)
	{
		
		FX.LIGHT_X_norm = cos(FX.LIGHTING_ROLL*M2PI);
		FX.LIGHT_Y_norm = sin(FX.LIGHTING_ROLL*M2PI);

		FX.SHADOW_X = FX.LIGHT_X_norm*0.05f*FX.LIGHTING_DEPTH;
		FX.SHADOW_Y = FX.LIGHT_Y_norm*0.05f*FX.LIGHTING_DEPTH;

		FX.SPEC_DIR_X = -(FX.SHADOW_X / 0.05f);
		FX.SPEC_DIR_Y = (FX.SHADOW_Y / 0.05f);

		UPD_FLARE(); 
	}
	///////////////////////////////////////////////////////////////////////////////////////
	TFPColors->COLOR_CORRECT = TRUE;
	if (APPLY_COLOR_CORRECT)     // if clicing on apply color correct, it will make change to all colors set 
	{
		for (int i = 0; i < NB_BUTTON_COLORS; i++)
		{
			BUTTON_COLORS[i]->update_out();
			BUTTON_COLORS[i]->IN = BUTTON_COLORS[i]->OUT;
			BUTTON_COLORS[i]->IN_HSLA = BUTTON_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	}
	///////////////////////////////////////////////////////////


	TFParams->ITEM_MAIN_OUT = MAIN_OUTPUT;

	UPD_PAINT_POINTERS();

	UPD_STRINGS();
}

void Button::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier manips
	BUTTON_COLORS[0] = &COLOR.Global;
	BUTTON_COLORS[1] = &COLOR.OFF;
	BUTTON_COLORS[2] = &COLOR.ON;
	BUTTON_COLORS[3] = &COLOR.OFF_Border;
	BUTTON_COLORS[4] = &COLOR.ON_Border;
	BUTTON_COLORS[5] = &COLOR.XY_OFF;
	BUTTON_COLORS[6] = &COLOR.XY_ON;
	BUTTON_COLORS[7] = &COLOR.CONTAINER;
	BUTTON_COLORS[8] = &COLOR.CONTAINER_BORDER;
	///////////////////////////////////////
}///end upd_color_array

void Button::UPD_FLARE()
{
	FX.FLARE_RECT = ELMT.BUTTON_SHAPE.RECT;

	float opa = FX.FLARES_OPACITY / 2.0f;
	float radius_mult = 0.75f - ELMT.BUTTON_SHAPE.ROUND_SIZE;
	float len = (FX.FLARE_RECT.right - FX.FLARE_RECT.left);
	float hei = (FX.FLARE_RECT.bottom - FX.FLARE_RECT.top);
	float CTX = (len / 2.0f);
	float CTY = (hei / 2.0f);
	float Xmult = 0.0f;

	FX.FLARE_INIT.x = CTX - (FX.LIGHT_X_norm*len*radius_mult);
	FX.FLARE_INIT.y = CTY - (FX.LIGHT_Y_norm*hei*radius_mult);
	FX.FLARE_SYM_INIT.x = CTX + (FX.LIGHT_X_norm*len*radius_mult);
	FX.FLARE_SYM_INIT.y = CTY + (FX.LIGHT_Y_norm*hei*radius_mult);

	Xmult = (abs(CTX - FX.FLARE_INIT.x) * 2) * (abs(CTY - FX.FLARE_INIT.y) * 2);
	Xmult /= hei;
	Xmult /= len;
	Xmult *= 1 + (ELMT.BUTTON_SHAPE.ROUND_SIZE*4.0f);
	Xmult = pow(Xmult, 3);

	FX.FLARE_INTENSITY = opa*Xmult;

}

void Button::UPD_OCCLUSION()
{
	//if (FX.OCCLUSION)
	{
		float scale;
		scale = 1 - (ELMT.BUTTON_SHAPE.BORDER_SIZE / TFParams->CANVAS_S) *4.0f;
		FX.OCCLUSION_SHAPE.RECT = SCALE_RECT(ELMT.BUTTON_SHAPE.RECT, scale, scale, 1.0f);
	}
}

TDrawType Button::GET_PAINT_LAYER(int layer)

{

	switch (layer)
	{

	case 0: 	if (TRANSFORM > 0){
		
		ELMT.CONTAINER_SHAPE.FILL_COLOR = COLOR.CONTAINER.OUT;
		ELMT.CONTAINER_SHAPE.BORDER_COLOR = COLOR.CONTAINER_BORDER.OUT;
		ELMT.CONTAINER_SHAPE.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE;
		SHAPE_ELMT = ELMT.CONTAINER_SHAPE;
		return DT_Shape;
		}
		else { return DT_None; }
		 break;

	case 1: 	if (FX.SHADOWS)//shadow first
		{
			SHAPE_ELMT = ELMT.BUTTON_SHAPE;
		SHAPE_ELMT.offset_X = FX.SHADOW_X;
		SHAPE_ELMT.offset_Y = FX.SHADOW_Y;
		SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.OFF.A);
		SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.OFF_Border.A);
		SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE;
		return  DT_Shape;
		}//shdow
		else { return DT_None; }
		 break;


	case 2: 	SHAPE_ELMT = ELMT.BUTTON_SHAPE;                   //normal draw
				SHAPE_ELMT.FILL_COLOR = COLOR.OFF.OUT;
				SHAPE_ELMT.BORDER_COLOR = COLOR.OFF_Border.OUT;
				SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE;
				SHAPE_ELMT.SHOW_BORDER = ELMT.BUTTON_SHAPE.SHOW_BORDER;
				return  DT_Shape; break;

	case 3:	if (FX.OCCLUSION)
			{
			SHAPE_ELMT=FX.OCCLUSION_SHAPE;
			SHAPE_ELMT.FILL_COLOR = HSLA(0.0f, 0.0f, 0.0f, 0.0f);
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.0f, TFParams->ITEM_VALUE_SWITCH, FX.OCLLUSION_OPACITY*0.75f);
			SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE;
			return  DT_Shape;
			}
			else { return DT_None; }
			 break;

	case 4: 	if (FX.REFLEXIONS) // reflexions
			{
				SHAPE_ELMT = ELMT.BUTTON_SHAPE;
				SHAPE_ELMT.FILL_COLOR = HSLA(COLOR.OFF.OUT_HSLA.h, COLOR.OFF.OUT_HSLA.s, TFParams->ITEM_VALUE_SWITCH*0.5f, FX.REFLEXIONS_OPACITY*0.5f);
				FX.REFLEXIONS_TYPE = FX.REFLEXIONS_TYPE_IN;
				//SHAPE_ELMT.SHOW_BORDER = FALSE;
				SHAPE_ELMT.BORDER_COLOR = COLOR.OFF_Border.OUT;
				return DT_Shape_Reflexion;
			}
				else { return DT_None; }
				 break;

	case 5: 	if (FX.SPECULAR && ELMT.BUTTON_SHAPE.SHOW_BORDER && !TFParams->ITEM_VALUE_SWITCH)
			{//specs
				float SpecOffset = (ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
				SHAPE_ELMT = ELMT.BUTTON_SHAPE;
			//SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, 0);
			SHAPE_ELMT.SHOW_FILL = FALSE;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.15f, 0.0f, FX.SPECULAR_OPACITY*COLOR.OFF_Border.A*0.75f);
			SHAPE_ELMT.SHOW_BORDER = ELMT.BUTTON_SHAPE.SHOW_BORDER;
			SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f;
			SHAPE_ELMT.offset_X = -SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = SpecOffset*FX.SPEC_DIR_Y;
			return DT_Shape; //cause non partial
			}
				else { return DT_None; }
			 break;

	case 6: 	if (FX.SPECULAR && ELMT.BUTTON_SHAPE.SHOW_BORDER && !TFParams->ITEM_VALUE_SWITCH)
	{//specs
		float SpecOffset = (ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
		SHAPE_ELMT = ELMT.BUTTON_SHAPE;
		//SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, 0);
		SHAPE_ELMT.SHOW_FILL = FALSE;
		SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.15f, 0, FX.SPECULAR_OPACITY*COLOR.OFF_Border.A*0.50f);
		SHAPE_ELMT.SHOW_BORDER = ELMT.BUTTON_SHAPE.SHOW_BORDER;
		SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f;
		SHAPE_ELMT.offset_X = SpecOffset*FX.SPEC_DIR_X;
		SHAPE_ELMT.offset_Y = -SpecOffset*FX.SPEC_DIR_Y;
		return DT_Shape;
	}
				else { return DT_None; }
				 break;

	case 7:
		if (FX.SPECULAR && ELMT.BUTTON_SHAPE.SHOW_BORDER && !TFParams->ITEM_VALUE_SWITCH)
		{//specs
			float SpecOffset = (ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.BUTTON_SHAPE;
			SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, 0);
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.7f, 1.0f, FX.SPECULAR_OPACITY*COLOR.OFF_Border.A*(1.0f - FX.LIGHT_X_norm)*0.5f);
			SHAPE_ELMT.SHOW_BORDER = ELMT.BUTTON_SHAPE.SHOW_BORDER;
			SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE*0.25f;
			SHAPE_ELMT.offset_X = SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = -SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_SEG_FROM = 1; //for partial draws
			SHAPE_NB_SEGS = 2;
			return DT_Shape_Partial;
		}
		else { return DT_None; }
		 break;

	case 8:
		if (FX.SPECULAR && ELMT.BUTTON_SHAPE.SHOW_BORDER && !TFParams->ITEM_VALUE_SWITCH)
		{//specs
			float SpecOffset = (ELMT.BUTTON_SHAPE.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.BUTTON_SHAPE;
			SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, 0);
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.7f, 1.0f, FX.SPECULAR_OPACITY*COLOR.OFF_Border.A*( FX.LIGHT_X_norm)*0.5f);
			SHAPE_ELMT.SHOW_BORDER = ELMT.BUTTON_SHAPE.SHOW_BORDER;
			SHAPE_ELMT.BORDER_SIZE = ELMT.BUTTON_SHAPE.BORDER_SIZE*0.25f;
			SHAPE_ELMT.offset_X = SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_SEG_FROM = 3; //for partial draws
			SHAPE_NB_SEGS = 2;
			return DT_Shape_Partial;
		}
		else { return DT_None; }
		 break;


	case 9:
		if (FX.FLARES && !TFParams->ITEM_VALUE_SWITCH) //cap  hot spots flares
		{
			float t_spot = 0;
			FX.FLARE_RECT = ELMT.BUTTON_SHAPE.RECT;
			FX.FLARE = OFFSET_POINT(FX.FLARE_INIT, FX.FLARE_RECT.left, FX.FLARE_RECT.top);
			FX.FLARE_SYM = OFFSET_POINT(FX.FLARE_SYM_INIT, FX.FLARE_RECT.left, FX.FLARE_RECT.top);

			for (int i = 0; i < FX.NB_FLARES; i++)
			{
				t_spot = float(i) / float(FX.NB_FLARES - 1);

				FX.FLARES_COLORS[i] = HSLA(0.0f, 0.25f, 1.0f, t_spot*FX.FLARE_INTENSITY);
				FX.FLARES_SYM_COLORS[i] = HSLA(0.0f, 0.25f, 1.0f, t_spot*FX.FLARE_INTENSITY*0.66f);

				FX.FLARES_SIZES[i] = 50 * (1 - t_spot);
				FX.FLARES_SYM_SIZES[i] = 33 * (1 - t_spot);
			}
			return DT_Points_Array;
		}//cap flares
		else { return DT_None; }
		 break;

	case 10:
		if (XYLines_Nb > 0)       ////xy
		{
			if (XYLines_Nb == 1 || XYLines_Nb == 3)
			{
				//LINE_ELMT.SHOW = TRUE;
				LINE_ELMT.LINE_FROM.x = Cursor.X;
				LINE_ELMT.LINE_FROM.y = ELMT.BUTTON_SHAPE.RECT.top + 0.1f;
				LINE_ELMT.LINE_TO.x = LINE_ELMT.LINE_FROM.x;
				LINE_ELMT.LINE_TO.y = ELMT.BUTTON_SHAPE.RECT.bottom - 0.1f;
				LINE_ELMT.LINE_COLOR = COLOR.XY_OFF.OUT;
				LINE_ELMT.LINE_SIZE = XYLines_Sz;
				return DT_Line;
			}
			else { return DT_None; }
			break;			
		}
		else { return DT_None; }
		 break;

	case 11:
		if (XYLines_Nb > 0)       ////xy
		{
			if (XYLines_Nb == 2 || XYLines_Nb == 3)
			{
				//LINE_ELMT.SHOW = TRUE;
				LINE_ELMT.LINE_FROM.x = ELMT.BUTTON_SHAPE.RECT.left + 0.1f;
				LINE_ELMT.LINE_FROM.y = 1 - Cursor.Y;
				LINE_ELMT.LINE_TO.x = ELMT.BUTTON_SHAPE.RECT.right - 0.1f;
				LINE_ELMT.LINE_TO.y = LINE_ELMT.LINE_FROM.y;
				LINE_ELMT.LINE_COLOR = COLOR.XY_OFF.OUT;
				LINE_ELMT.LINE_SIZE = XYLines_Sz;
				return DT_Line;
			}
			else { return DT_None; }
			break;
			
		}
		else { return DT_None; }
		 break;


	case 12:
		if (XYLines_Nb > 0)       ////xy values
		{
			if (XYLines_Nb == 1 || XYLines_Nb == 3)
			{
				VALUE_ELMT.VALUE_COLOR = COLOR.XY_OFF.OUT;
				VALUE_ELMT.H_Align = taLeading;
				VALUE_ELMT.V_Align = taCenter;
				VALUE_ELMT.RECT = XYLines_Rect;
				VALUE_ELMT.VALUE_STRING = X_VALUE_STRING;
			}
			return DT_Text_Value;
		}
		else { return DT_None; }
		 break;

	case 13:
		if (XYLines_Nb > 0 && TFParams->SHOW_VALUE)       ////xy values
		{
			if (XYLines_Nb == 2 || XYLines_Nb == 3)
			{
				VALUE_ELMT.VALUE_COLOR = COLOR.XY_OFF.OUT;
				VALUE_ELMT.H_Align = taTrailing;
				VALUE_ELMT.V_Align = taLeading;
				VALUE_ELMT.RECT = XYLines_Rect;
				VALUE_ELMT.VALUE_STRING = Y_VALUE_STRING;
			}
			return DT_Text_Value;
		}
		else { return DT_None; }
		 break;

	case 14:
		if (TFParams->SHOW_VALUE)
		{
			//VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
			VALUE_ELMT.H_Align = taCenter;
			VALUE_ELMT.V_Align = taLeading;
			VALUE_ELMT.RECT = TFParams->VALUE_RECT;
			VALUE_ELMT.VALUE_STRING = SWITCH_VALUE_STRING;
			return DT_Text_Value;
		}
				
		else { return DT_None; }
		break;


	default:
		return DT_None;
		break;
	}




}

void Button::UPD_PAINT_POINTERS()
{

	TFPaintP->COLOR_Grid_OUT = nullptr;// &COLOR.Grid.OUT;
	TFPaintP->FX_FLARE = &FX.FLARE;
	TFPaintP->FX_FLARES_COLORS = FX.FLARES_COLORS;
	TFPaintP->FX_FLARES_SIZES = FX.FLARES_SIZES;
	TFPaintP->FX_FLARES_SYM_COLORS = FX.FLARES_SYM_COLORS;
	TFPaintP->FX_FLARES_SYM_SIZES = FX.FLARES_SYM_SIZES;
	TFPaintP->FX_FLARE_SYM = &FX.FLARE_SYM;
	TFPaintP->FX_NB_FLARES = FX.NB_FLARES;
	TFPaintP->FX_REFLEXIONS_EACH = &FX.REFL_DRAW_EACH;
	TFPaintP->FX_REFLEXIONS_TYPE = &FX.REFLEXIONS_TYPE;
	TFPaintP->GRID_GRID_FROM = nullptr;//GRID.GRID_FROM;
	TFPaintP->GRID_GRID_TO = nullptr;//GRID.GRID_TO;
	TFPaintP->GRID_SUBDIVS = nullptr;//&GRID.GRID_SUBDIVS;
	TFPaintP->LINE_ELMT = &LINE_ELMT;
	TFPaintP->LINE_SIZE = &LINE_SIZE;
	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	TFPaintP->SHAPE = &SHAPE_ELMT;
	TFPaintP->SHAPE_NB_SEGS = &SHAPE_NB_SEGS;
	TFPaintP->SHAPE_SEG_FROM = &SHAPE_SEG_FROM;
	TFPaintP->STRIPES_DUAL_DRAW = nullptr;//&STRIPES.DUAL_DRAW;
	TFPaintP->STRIPES_MAX_PIX = nullptr;//&STRIPES.MAX_PIX;
	TFPaintP->STRIPES_STRIPES_COLORS = nullptr;//STRIPES.STRIPES_COLORS;
	TFPaintP->STRIPES_STRIPES_LEDS = nullptr;//STRIPES.STRIPES_LEDS;
	TFPaintP->STRIPES_STRIPES_LEDS_SYM = nullptr;//STRIPES.STRIPES_LEDS_SYM;
	TFPaintP->VALUE_ELMT = &VALUE_ELMT;

}

void Button::PRESET_LOAD(int preset)
{



	switch (preset)
	{
		///////////////////////////////////////
	case 0:                             //DFT

		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->HUE_ROLL = 0;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 0.75;
		TFPColors->HUE_BEAVIOUR = 2;//overide
		//--------------------
		ELMT.BUTTON_SHAPE.Scale_X = 0.88f;
		ELMT.BUTTON_SHAPE.Scale_Y = 0.70f;
		//----------
		ELMT.BUTTON_SHAPE.ROUND_TYPE = 1;
		ELMT.BUTTON_SHAPE.ROUND_SIZE = 0.05f;
		ELMT.BUTTON_SHAPE.SHOW_BORDER = TRUE;
		ELMT.BUTTON_SHAPE.BORDER_SIZE = 4.0;
		//-------------------------------
		COLOR.OFF.IN = 0xD42A3047;
		COLOR.OFF_Border.IN = 0xFF535F84;
		COLOR.ON.IN = 0xF4FF9E21;
		COLOR.ON_Border.IN = 0xF8F0C379;
		COLOR.XY_OFF.IN = 0x4E5D616B;
		COLOR.XY_ON.IN = 0xFF7C705B;
		COLOR.CONTAINER.IN = 0x71232323;
		COLOR.CONTAINER_BORDER.IN = 0xFF222020;
		//------------------
		XYLines_Nb = 0;
		XYLines_Sz = 2.0;
		TRANSFORM = 3;
		REDUX = 0.03f;
		FX.LEVEL = 4;
		FX.LIGHTING_ROLL = 0.25f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.25f;
		FX.SHADOW_OPACITY = 0.20f;
		FX.SPECULAR_OPACITY = 0.20f;
		FX.REFLEXIONS_OPACITY = 0.5f;
		FX.REFLEXIONS_TYPE_IN = 0; //7
		FX.FLARES_OPACITY = 0.25;
		//---------------
		break;
		///////////////////////////////////////////////
		///////////////////////////////////////
	case 1:                             //1_Minimal skin

		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->HUE_ROLL = 0;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->HUE_BEAVIOUR = 2;//overide
									//--------------------
		ELMT.BUTTON_SHAPE.Scale_X = 0.88f;
		ELMT.BUTTON_SHAPE.Scale_Y = 0.70f;
		//----------
		ELMT.BUTTON_SHAPE.ROUND_TYPE = 0;
		ELMT.BUTTON_SHAPE.ROUND_SIZE = 0.05f;
		ELMT.BUTTON_SHAPE.SHOW_BORDER = TRUE;
		ELMT.BUTTON_SHAPE.BORDER_SIZE = 2.0f;
		//-------------------------------
		COLOR.OFF.IN = 0xD42A3047;
		COLOR.OFF_Border.IN = 0xFF535F84;
		COLOR.ON.IN = 0xF4FF9E21;
		COLOR.ON_Border.IN = 0xF8F0C379;
		COLOR.XY_OFF.IN = 0x4E5D616B;
		COLOR.XY_ON.IN = 0xFF7C705B;
		COLOR.CONTAINER.IN = 0x71232323;
		COLOR.CONTAINER_BORDER.IN = 0xFF222020;
		//------------------
		XYLines_Nb = 0;
		XYLines_Sz = 2.0;
		TRANSFORM = 0;
		REDUX = 0.03f;
		FX.LEVEL = 0;
		FX.LIGHTING_ROLL = 0.25f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.25f;
		FX.SHADOW_OPACITY = 0.20f;
		FX.SPECULAR_OPACITY = 0.20f;
		FX.REFLEXIONS_OPACITY = 0.5f;
		FX.REFLEXIONS_TYPE_IN = 0; //7
		FX.FLARES_OPACITY = 0.25f;
		//---------------
		break;
		///////////////////////////////////////////////
		///////////////////////////////////////
	case 2:                             // LedXY

		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->HUE_ROLL = 0;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->HUE_BEAVIOUR = 2;//overide
									//--------------------
		ELMT.BUTTON_SHAPE.Scale_X = 0.88f;
		ELMT.BUTTON_SHAPE.Scale_Y = 0.70f;
		//----------
		ELMT.BUTTON_SHAPE.ROUND_TYPE = 2;
		ELMT.BUTTON_SHAPE.ROUND_SIZE = 0.05f;
		ELMT.BUTTON_SHAPE.SHOW_BORDER = TRUE;
		ELMT.BUTTON_SHAPE.BORDER_SIZE = 0.5f;
		//-------------------------------
		COLOR.OFF.IN = 0xD42A3047;
		COLOR.OFF_Border.IN = 0xFF535F84;
		COLOR.ON.IN = 0xF4FF9E21;
		COLOR.ON_Border.IN = 0xF8F0C379;
		COLOR.XY_OFF.IN = 0x4E5D616B;
		COLOR.XY_ON.IN = 0xFF7C705B;
		COLOR.CONTAINER.IN = 0x71232323;
		COLOR.CONTAINER_BORDER.IN = 0xFF222020;
		//------------------
		XYLines_Nb = 3;
		XYLines_Sz = 2.0;
		TRANSFORM = 3;
		REDUX = 0.03f;
		FX.LEVEL = 4;
		FX.LIGHTING_ROLL = 0.25f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.25f;
		FX.SHADOW_OPACITY = 0.20f;
		FX.SPECULAR_OPACITY = 0.20f;
		FX.REFLEXIONS_OPACITY = 0.5f;
		FX.REFLEXIONS_TYPE_IN = 7; //7
		FX.FLARES_OPACITY = 0.25f;
		//---------------
		break;
		///////////////////////////////////////////////
		///////////////////////////////////////
	case 3:                             //3 Sliding_led

		TFPColors->COLOR_CORRECT = FALSE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->HUE_ROLL = 0;
		TFPColors->BLEND_MODE = BM_Replace;
		TFPColors->BLEND_MIX = 0.75;
		TFPColors->HUE_BEAVIOUR = 2;//overide
									//--------------------
		ELMT.BUTTON_SHAPE.Scale_X = 0.80f;
		ELMT.BUTTON_SHAPE.Scale_Y = 0.23f;
		//----------
		ELMT.BUTTON_SHAPE.ROUND_TYPE = 1;
		ELMT.BUTTON_SHAPE.ROUND_SIZE = 0.02f;
		ELMT.BUTTON_SHAPE.SHOW_BORDER = TRUE;
		ELMT.BUTTON_SHAPE.BORDER_SIZE = 2.82f;
		//-------------------------------
		COLOR.OFF.IN = 0xD42A3047;
		COLOR.OFF_Border.IN = 0xFF535F84;
		COLOR.ON.IN = 0xF4FF9E21;
		COLOR.ON_Border.IN = 0xF8F0C379;
		COLOR.XY_OFF.IN = 0x4E5D616B;
		COLOR.XY_ON.IN = 0xFF7C705B;
		COLOR.CONTAINER.IN = 0x71232323;
		COLOR.CONTAINER_BORDER.IN = 0xFF222020;
		//------------------
		XYLines_Nb = 0;
		XYLines_Sz = 2.0f;
		TRANSFORM = 2;
		REDUX = 0.40f;
		FX.LEVEL = 3;
		FX.LIGHTING_ROLL = 0.32f;
		FX.LIGHTING_DEPTH = 0.79f;
		FX.OCLLUSION_OPACITY = 0.42f;
		FX.SHADOW_OPACITY = 0.35f;
		FX.SPECULAR_OPACITY = 0.50f;
		FX.REFLEXIONS_OPACITY = 0.5f;
		FX.REFLEXIONS_TYPE_IN = 7; //7
		FX.FLARES_OPACITY = 0.25f;
		//---------------
		break;
		///////////////////////////////////////////////





	default:
		break;
	}

}//end preset

void Button::UPD_STRINGS()
{

	if (TFParams->SHOW_VALUE)
	{

		SWITCH_VALUE_STRING = VAL_TO_STRING(RE_RANGE_NORM_IN(TFParams->ITEM_VALUE_SWITCH, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION);

		if (XYLines_Nb == 1 || XYLines_Nb == 3)
		{
			X_VALUE_STRING = VAL_TO_STRING(RE_RANGE_NORM_IN(Cursor.X, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION);
		}
		 if (XYLines_Nb > 1)
		{
			Y_VALUE_STRING = VAL_TO_STRING(RE_RANGE_NORM_IN(Cursor.Y, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION);
		}

	}

}// upd strings

void Button::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.OFF.IN = 0xD42A3047;
	COLOR.OFF_Border.IN = 0xFF535F84;
	COLOR.ON.IN = 0xF4FF9E21;
	COLOR.ON_Border.IN = 0xF8F0C379;
	COLOR.XY_OFF.IN = 0x4E5D616B;
	COLOR.XY_ON.IN = 0xFF7C705B;
	COLOR.CONTAINER.IN = 0x71232323;
	COLOR.CONTAINER_BORDER.IN = 0xFF222020;
	///////////////

	TFPColors->COLOR_CORRECT = TRUE;
	APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
	//TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum


	for (int i = 0; i < NB_BUTTON_COLORS; i++)
	{
		BUTTON_COLORS[i]->OnSetQuickColor();
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

