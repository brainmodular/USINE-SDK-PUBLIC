//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "Fader.h"
//#include <vector>

Fader::Fader()
{

}

Fader::Fader(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{
	TFParams = TFParams_val;
	TFPaintP =  TFPaintP_val;
	TFPColors = new TFPColor();

	UPD_COLOR_ARRAY();
	for (int i = 0; i < NB_FADER_COLORS; i++){ *FADER_COLORS[i] = TFColor(TFParams, TFPColors);}
	
	PRESET_LOAD(0);	


	onSettingsHasChanged();

}
//////////////////////////////////////
void Fader::onInputChange()
{
	//UPD_FROM_EXTERNAL(TFParams->input_val);  //input 0-1 ranged

	UPD_FROM_EXTERNAL(TFParams->ITEM_VALUE_Y);  //input 0-1 ranged

	//TFParams->ITEM_VALUE_Y = TFParams->input_val; //passes to out

}//end input chg

void Fader::onPreset()  //presets or autom incoming
{
	UPD_FROM_EXTERNAL(TFParams->ITEM_VALUE_Y);
}

void Fader::UPD_FROM_EXTERNAL(float in_val)
{

	in_val = QUANTIZE(in_val); 

	if (INVERT_MODE) { fad_val = 1 - in_val; }else { fad_val = in_val; }

	fad_val = REVERT_SCALE_TYPE(fad_val, SCALE_TYPE);
	X_FADER.VAL = TFParams->ITEM_VALUE_X;

	TFParams->ITEM_VALUE_Y = in_val;
	UPD_FAD_RECTS(fad_val);
	UPD_PAINT();
	
}
////////////////////////////////////////////////////
void Fader::onMouseDown()
{
	//////////////////////////////////////////////
	if (FADER_SWITCH_SUBMODE != 0)  // if using switch feature
	{	if (FADER_SWITCH_SUBMODE == 1){ TFParams->ITEM_VALUE_SWITCH = TRUE; }//button
		else  { ON_CLIC_VAL = fad_val; WILL_SW_OFF = TRUE; }//switch if (FADER_SWITCH_SUBMODE == 2)
	}
	////////////////////////////////////
	if (!RELATIVE_MODE) // if absolute mode
	{
		if (SHOW.CAP && FAD_COMPENSATE) // if using offset compensation so that clicking on cap doesn't jump 
		{
			fad_cap_comp.x = 0; fad_cap_comp.y = 0; 

				if (sdkPointInRect(TFParams->X, TFParams->Y, ELMT.CAP.RECT)) //if in cap box update offsets values to apply
				{
					CAP_CENTER.x = ((ELMT.CAP.RECT.right - ELMT.CAP.RECT.left) / 2.0f) + ELMT.CAP.RECT.left;
					CAP_CENTER.y = ELMT.CAP.RECT.top + FAD_BOX_H;
					fad_cap_comp.x =  (TFParams->X - CAP_CENTER.x); 
					fad_cap_comp.y = -(TFParams->Y -  CAP_CENTER.y);
				} 
		}//fad compensate

		if (HORIZONTAL_MODE){ XY_input = TFParams->X; if (SHOW.CAP && FAD_COMPENSATE){ XY_input-=fad_cap_comp.x; } }
		else{ XY_input = (1 - TFParams->Y); if (SHOW.CAP && FAD_COMPENSATE){ XY_input-=fad_cap_comp.y; } }

		fad_val = XY_input;

		if (SHOW.CAP){ fad_val = RE_RANGE_NORM_OUT(fad_val, FAD_BOX_H, 1 - FAD_BOX_H); }  

		fad_val = QUANTIZE(fad_val);

		fad_val = CLAMP01(fad_val);

		TFParams->ITEM_VALUE_Y = APPLY_SCALE_TYPE(fad_val, SCALE_TYPE);

		if (INVERT_MODE){ TFParams->ITEM_VALUE_Y = 1 - TFParams->ITEM_VALUE_Y; }
	}//end absolute mode
	////////////////////////////////////////
	if (X_FADER_SUBMODE > 0) { UPD_XFADER(); }///xfader
	//////////////////////////////////////////////////
	TFParams->hilight = 1.0;

	TFParams->ITEM_VALUE_Z = 1.0;

	UPD_FAD_RECTS(fad_val);
	UPD_PAINT();
}//end msdwn

void Fader::onMouseUp()
{
	if (FADER_SWITCH_SUBMODE == 1){ TFParams->ITEM_VALUE_SWITCH = FALSE; } //button

	if (AUTO_RESET_MODE)
			{
				if (!INVERT_MODE){ fad_val = TFParams->RESET_VALUE_01; TFParams->ITEM_VALUE_Y = TFParams->RESET_VALUE_01; }
				else{ fad_val = 1 - TFParams->RESET_VALUE_01; TFParams->ITEM_VALUE_Y = 1 - TFParams->RESET_VALUE_01; }

				fad_val = QUANTIZE(fad_val);

				TFParams->ITEM_VALUE_Y = fad_val;// RESCALE(fad_val);
				if (INVERT_MODE){ TFParams->ITEM_VALUE_Y = 1 - TFParams->ITEM_VALUE_Y; }
				UPD_FAD_RECTS(fad_val);
			}//end msup

	if (FADER_SWITCH_SUBMODE == 2 && WILL_SW_OFF){ TFParams->ITEM_VALUE_SWITCH = !TFParams->ITEM_VALUE_SWITCH; }//switch

	TFParams->hilight = 0;
	TFParams->ITEM_VALUE_Z = 0;
	UPD_PAINT();
}//end msup

void Fader::onMouseMove()
{
	
	if (!RELATIVE_MODE)//absolute
	{
		if (HORIZONTAL_MODE){ XY_input = TFParams->X; if (SHOW.CAP && FAD_COMPENSATE){ XY_input-=fad_cap_comp.x; } }
		else{ XY_input = (1 - TFParams->Y); if (SHOW.CAP && FAD_COMPENSATE){ XY_input-=fad_cap_comp.y; } }
		fad_val = XY_input;
		if (SHOW.CAP){fad_val = RE_RANGE_NORM_OUT(fad_val, FAD_BOX_H, 1 - FAD_BOX_H);}

	    fad_val = QUANTIZE(fad_val); 

	}
	else //relative mode
	{
		if (HORIZONTAL_MODE){ fad_val += TFParams->DELTA_X*RELATIVE_SPEED; }
		else{ fad_val  -= TFParams->DELTA_Y*RELATIVE_SPEED; }		
	}	
	fad_val = CLAMP01(fad_val);
	
	////////////////////////////////////////
	if (X_FADER_SUBMODE > 0)  {UPD_XFADER();}///xfader
	//////////////////////////////////////////////////

	UPD_FAD_RECTS(fad_val);

	TFParams->ITEM_VALUE_Y = APPLY_SCALE_TYPE(fad_val, SCALE_TYPE);    // RESCALE(fad_val);
	TFParams->ITEM_VALUE_X = X_FADER.VAL;

	if (INVERT_MODE){ TFParams->ITEM_VALUE_Y = 1 - TFParams->ITEM_VALUE_Y; }

	if (FADER_SWITCH_SUBMODE==2 && fad_val != ON_CLIC_VAL){WILL_SW_OFF = FALSE;} //if swich mode and value was changed won't sw off

	UPD_PAINT();
	
}//end msmove
//////////////////////
void Fader::UPD_FAD_RECTS(float fad_val)
{

	if (SHOW.CAP){fad_val = RE_RANGE_NORM_IN(fad_val, FAD_BOX_H, 1 - FAD_BOX_H);}

	float OFFSET_CAP = abs(FAD_BOX_H*(float)(SHOW.CAP));
	float OFFSET_BAR = 0;//-OFFSET_CAP;  used to offset bar to upper/lower part of cap if cap displayed
	float border_add = 0; // offset bar to fit in bg+border rec

	///////////////////////////////////////////////////
	if (!HORIZONTAL_MODE) //vertical mode
	{
		border_add = ELMT.GLOBAL.BORDER_SIZE / TFParams->CANVAS_H;
		ELMT.CAP.RECT.top    = (1 - fad_val) - OFFSET_CAP;
		ELMT.CAP.RECT.bottom = ELMT.CAP.RECT.top + OFFSET_CAP * 2;
		if (!INVERT_MODE)
		{
			ELMT.BAR.RECT.top = (1-fad_val) + OFFSET_BAR + border_add;
			ELMT.BAR.RECT.bottom = 1 - border_add;
			ELMT.BAR.RECT.top = min(1 - border_add, ELMT.BAR.RECT.top);
		}
		else
		{
			ELMT.BAR.RECT.top = 0 + border_add;
			ELMT.BAR.RECT.bottom = (1 - fad_val) - OFFSET_BAR - border_add;
			ELMT.BAR.RECT.bottom = max(border_add, ELMT.BAR.RECT.bottom);

		}
	}/////////////////////////////////////
	else if (HORIZONTAL_MODE)
	{
		border_add = ELMT.GLOBAL.BORDER_SIZE / TFParams->CANVAS_W;
		ELMT.CAP.RECT.left = fad_val - OFFSET_CAP;
		ELMT.CAP.RECT.right = ELMT.CAP.RECT.left + OFFSET_CAP * 2;
		if (INVERT_MODE)
		{
			ELMT.BAR.RECT.left = fad_val + OFFSET_BAR;
			ELMT.BAR.RECT.right = 1 - border_add;
			ELMT.BAR.RECT.left = min(ELMT.BAR.RECT.left, 1 - border_add);
		}
		else
		{
			ELMT.BAR.RECT.left = 0 + border_add;
			ELMT.BAR.RECT.right = fad_val - OFFSET_BAR;
			ELMT.BAR.RECT.right = min(ELMT.BAR.RECT.right, 1 - border_add);
		}
	}//horizontal mode
	/////////////////////////////////////////////////////
	if (SHOW.STRIPES){COMPUTE_PIX(fad_val);}
	///////////////////////////////////////////
	if (X_FADER_SUBMODE > 0)  // adding xfader
	{
		if (!HORIZONTAL_MODE)
		{
			if (!INVERT_MODE)
			{
				ELMT.X_FADER_BG.RECT.top = ELMT.BAR.RECT.top;
				ELMT.X_FADER_BG.RECT.bottom = ELMT.X_FADER_BG.RECT.top + X_FADER.HEIGHT;
			}
			else //invert mode
			{
				ELMT.X_FADER_BG.RECT.bottom = ELMT.BAR.RECT.bottom;
				ELMT.X_FADER_BG.RECT.top = ELMT.BAR.RECT.bottom - X_FADER.HEIGHT;
			}
			ELMT.X_FADER_BAR.RECT.top = ELMT.X_FADER_BG.RECT.top + 0.01f;
			ELMT.X_FADER_BAR.RECT.bottom = ELMT.X_FADER_BG.RECT.bottom - 0.01f;
			ELMT.X_FADER_BAR.RECT.right = 0.5f + (X_FADER.VAL*X_FADER.WIDTH*0.5f);
			ELMT.X_FADER_BAR.RECT.left = 1 - ELMT.X_FADER_BAR.RECT.right;

		}
		else // horizontal mode
		{
			if (!INVERT_MODE)
			{
				ELMT.X_FADER_BG.RECT.right = ELMT.BAR.RECT.right;
				ELMT.X_FADER_BG.RECT.left = ELMT.BAR.RECT.right - X_FADER.WIDTH;
			}
			else //invert
			{
				ELMT.X_FADER_BG.RECT.left = ELMT.BAR.RECT.left;
				ELMT.X_FADER_BG.RECT.right = ELMT.BAR.RECT.left + X_FADER.WIDTH;
			}
			ELMT.X_FADER_BAR.RECT.left = ELMT.X_FADER_BG.RECT.left + 0.01f;
			ELMT.X_FADER_BAR.RECT.right = ELMT.X_FADER_BG.RECT.right - 0.01f;

			//ELMT.X_FADER_BAR.RECT.top = 0.5 - (X_FADER.VAL*X_FADER.HEIGHT*0.5);
			ELMT.X_FADER_BAR.RECT.bottom = 1 - ELMT.BAR.RECT.top;
			ELMT.X_FADER_BAR.RECT.top = ELMT.X_FADER_BAR.RECT.bottom - (X_FADER.VAL*X_FADER.HEIGHT*1.25f);


		}
	}//Xfader////////////////////////////////////////////////
	//////////////////line
	if (SHOW.LINE)
	{
		if (HORIZONTAL_MODE){ LINE_FROM.x = fad_val; LINE_TO.x = LINE_FROM.x; LINE_FROM.y = ELMT.BAR.RECT.top; LINE_TO.y = ELMT.BAR.RECT.bottom; }
	else{ LINE_FROM.x = ELMT.BAR.RECT.left; LINE_TO.x = ELMT.BAR.RECT.right; LINE_FROM.y = 1 - fad_val; LINE_TO.y = LINE_FROM.y; }
	}//end line

	UPD_STRINGS();

}//UPD rects
/////////////////////////////////////////////////////////////
void Fader::UPD_FAD_RECTS_FIXED()
{
	//////////////RECTS modify
	//float OFFSET_CAP = abs(FAD_BOX_H*(float)(SHOW.CAP));
	//float OFFSET_BAR = 0;//-OFFSET_CAP;  used to offset bar to upper/lower part of cap if cap displayed
	float border_add = 0; // offset bar to fit in bg+border rec
	///////////////////////////////////////////////////
	if (!HORIZONTAL_MODE) //vertical mode
	{
		border_add = ELMT.GLOBAL.BORDER_SIZE / TFParams->CANVAS_H;
		ELMT.BG.RECT.top = 0;
		ELMT.BG.RECT.bottom = 1;
		ELMT.CAP.RECT.right = FAD_BOX_W;
		ELMT.CAP.RECT.left = 1 - ELMT.CAP.RECT.right;
		ELMT.BG.RECT.left = FAD_BAR_W;
		ELMT.BAR.RECT.left = ELMT.BG.RECT.left + FAD_BAR_REDUX;
		ELMT.BG.RECT.right = 1 - ELMT.BG.RECT.left;
		ELMT.BAR.RECT.right = 1 - ELMT.BAR.RECT.left;
	}/////////////////////////////////////
	else if (HORIZONTAL_MODE)// horizontal mode
	{
		border_add = ELMT.GLOBAL.BORDER_SIZE / TFParams->CANVAS_W;
		ELMT.BG.RECT.left = 0;
		ELMT.BG.RECT.right = 1;
		ELMT.CAP.RECT.bottom = FAD_BOX_W;
		ELMT.CAP.RECT.top = 1 - ELMT.CAP.RECT.bottom;
		ELMT.BG.RECT.top = FAD_BAR_W;
		ELMT.BG.RECT.bottom = 1 - ELMT.BG.RECT.top;
		ELMT.BAR.RECT.top = ELMT.BG.RECT.top + FAD_BAR_REDUX;
		ELMT.BAR.RECT.bottom = 1 - ELMT.BAR.RECT.top;
	}//horizontal mode
	//////////////////////////////////////////////////
	if (X_FADER_SUBMODE > 0)  // adding xfader
	{
		if (!HORIZONTAL_MODE)
		{
			ELMT.X_FADER_BG.RECT.left = ELMT.BAR.RECT.left;
			ELMT.X_FADER_BG.RECT.right = ELMT.BAR.RECT.right;
			X_FADER.WIDTH = (ELMT.X_FADER_BG.RECT.right - ELMT.X_FADER_BG.RECT.left)*0.8f;
			X_FADER.HEIGHT = FAD_BOX_H * X_FADER.SIZE;

		}
		else // horizontal mode
		{
			ELMT.X_FADER_BG.RECT.top = ELMT.BAR.RECT.top;
			ELMT.X_FADER_BG.RECT.bottom = ELMT.BAR.RECT.bottom;
			X_FADER.HEIGHT = (ELMT.X_FADER_BG.RECT.bottom - ELMT.X_FADER_BG.RECT.top)*0.8f;
			X_FADER.WIDTH = FAD_BOX_H * X_FADER.SIZE;
		}
	}//Xfader

}
///////////////////////////////////////////////////////
float Fader::RESCALE(float in_val)
{
	if (SHOW.CAP){	return (in_val - FAD_BOX_H) / (1 - FAD_BOX_H*2.0f);}
	else{return in_val;}
}
//////////////////////////////////////////////////
void Fader::UPD_XFADER()
{
	if (X_FADER_SUBMODE > 0)  // adding xfader
	{
		if (!HORIZONTAL_MODE){ if (X_FADER_SUBMODE == 1) { X_FADER.used_val = TFParams->MSX; } else if (X_FADER_SUBMODE == 2){ X_FADER.delta_val = TFParams->DELTA_X; } }
		else { if (X_FADER_SUBMODE == 1){ X_FADER.used_val = 1 - TFParams->MSY; } else if (X_FADER_SUBMODE == 2){ X_FADER.delta_val = -TFParams->DELTA_Y; } }

		if (X_FADER_SUBMODE == 1){ X_FADER.VAL = X_FADER.used_val; }
		else{ X_FADER.VAL += X_FADER.delta_val*X_FADER.RELATIVE_SPEED; }X_FADER.VAL = CLAMP01(X_FADER.VAL);

	}
}//end/xfader
//////////////////////////////////////////////////
void Fader::COMPUTE_PIX(float fad_val)
{
	if (SHOW.STRIPES)
	{
		float used_val;

		if (INVERT_MODE){ used_val = (1 - fad_val); }
		else{ used_val = fad_val; } //+offset

		if (HORIZONTAL_MODE){ STRIPES.MAX_PIX = (int)TFParams->CANVAS_W; }
		else{ STRIPES.MAX_PIX = (int)TFParams->CANVAS_H; }

		STRIPES.MAX_PIX = STRIPES.MAX_PIX / STRIPES.MODULO; ///////////////////////////////

		STRIPES.REDUX = STRIPES.H_REDUX + FAD_BAR_W;
		STRIPES.PIX = (int)(STRIPES.MAX_PIX * used_val);
		STRIPES.PIX_DWN = STRIPES.MAX_PIX - STRIPES.PIX;
		STRIPES.DIV_SIZE = (1.0f / STRIPES.MAX_PIX)  * STRIPES.WIDTH; //* STRIPES.MODULO

		STRIPES.MAX_PIX = min(MAX_STRIPES, STRIPES.MAX_PIX);

		//TFPaintP->STRIPES_MAX_PIX = &STRIPES.MAX_PIX;

	}
}
//////////////////////////////////////////////////////////////
void Fader::UPD_PAINT()
{

	COLOR.Shadow = HSLA(0, 0, 0, FX.SHADOW_OPACITY);
	COLOR.Shadow_Lo = HSLA(0, 0, 0, FX.SHADOW_OPACITY/2.0f);

	bool switch_condition = false;
	//////////////////////////////////////////
	for (int i = 0; i < NB_FADER_COLORS; i++)
	{
		if (FADER_SWITCH_SUBMODE !=0)  ///if using the swich mode
		{
			switch_condition = (TFParams->ITEM_VALUE_SWITCH == true)  // item swich value is on
				&& (											// and one of the cases:				
					(SWITCH_TARGET == 0)					//all was the selected option
				 || (SWITCH_TARGET == 1 && FADER_COLORS[i]->IS_BORDER) //borders only
				 || (i == SWITCH_TARGET)  // on specific target
				   );

				TFPColors->USE_TARGET_COLOR = switch_condition;
		}

		FADER_COLORS[i]->update_out();
	}/////////////////////////////////end loop export

	if (TFParams->ITEM_VALUE_SWITCH)
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
	}
	else
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_OFF_COLOR;
	}


	if (SHOW.STRIPES){ UPD_STRIPES(FALSE); }
}
//////////////////////////////////////////////////////////////////////////
void Fader::onSettingsHasChanged()
{

	if (PRESET_RESET)
	{
		PRESET_LOAD(0);
		PRESET_RESET = FALSE;
	}

	UPD_ELEMENTS_ARRAY();
	APPLY_GLOBAL_SETTINGS();
	
	X_FADER.RELATIVE_SPEED = RELATIVE_SPEED;//linked for less settings


	//////////////////////////////////////////////////////////////////////
	// import TFcolors from Setting pannel to custom class TFColor /////////////////////////////////////
	//special cases      store ids & various optional infos:   (0) int ID, 1) bool IS_BORDER,2) float hilight_mult )
	//
	COLOR.Global.update_in    (0, TRUE, FALSE,0);  //  will apply Hue system to all even if unique ability is preseved
	COLOR.Switch.update_in    (1, FADER_SWITCH_SUBMODE>0, FALSE,0);  //swich color
	COLOR.BG.update_in		  (2, SHOW.BG, FALSE, 0.1f);
	COLOR.BG_Border.update_in(3, SHOW.BG, TRUE, 0.4f);
	COLOR.Bar.update_in       (4, SHOW.BAR, FALSE,0.5f);
	COLOR.Bar_Border.update_in(5, SHOW.BAR, TRUE,0.5f);
	COLOR.Cap.update_in       (6, SHOW.CAP, FALSE,0.5f);
	COLOR.Cap_Border.update_in(7, SHOW.CAP, TRUE,0.6f);
	COLOR.Stripes_ON.update_in(8, SHOW.STRIPES, FALSE,0.5f);
	COLOR.Stripes_OFF.update_in(9, SHOW.STRIPES, FALSE,0.05f);
	COLOR.Line.update_in       (10, SHOW.LINE, FALSE, 0.2f);
	COLOR.Xfader.update_in     (11, X_FADER_SUBMODE>0, FALSE, 0.2f);
	COLOR.Xfader_Border.update_in(12, X_FADER_SUBMODE>0, TRUE, 0.2f);
	COLOR.Grid.update_in        (13, SHOW.GRID, FALSE, 0.1f);

	TFPColors->TARGET_COLOR = COLOR.Switch.IN;        //transmited via pointer to subfunctions
	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE; // use global hue mode set to id0 tho individual settings are possible
	/////////////////////////////////////////
	UPD_COLOR_ARRAY();//// copy to pointer array for easiers loops manips , like hilight all colors

	STRIPES.REDUX = STRIPES.H_REDUX + FAD_BAR_W;
	UPD_FAD_RECTS_FIXED();
	UPD_FAD_RECTS(fad_val);
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

	TFParams->ITEM_MAIN_OUT = MAIN_OUTPUT;


	if (APPLY_COLOR_CORRECT)     // if clicing on apply color correct, it will make change to all colors set 
	{
		for (int i = 0; i < NB_FADER_COLORS; i++)
		{
			FADER_COLORS[i]->update_out();
			FADER_COLORS[i]->IN = FADER_COLORS[i]->OUT;
			FADER_COLORS[i]->IN_HSLA = FADER_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	}

	 //////////////////////////////
	if (SHOW.STRIPES)
	{
		COMPUTE_PIX(fad_val);
		memset(STRIPES.STRIPES_LEDS, -1, sizeof STRIPES.STRIPES_LEDS);          //reset the leds recs array before rearranging in upd_strips
		memset(STRIPES.STRIPES_LEDS_SYM, -1, sizeof STRIPES.STRIPES_LEDS_SYM);
		UPD_STRIPES(TRUE); }

	if (SHOW.GRID){ UPD_GRID(); }

	//TFParams->ITEM_MAIN_OUT = MAIN_OUTPUT;
	UPD_PAINT_POINTERS();
	UPD_PAINT();	
	UPD_STRINGS();
}//en setiings chgd
/////////////////////////////////////////////////
void Fader::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier serial manips
	FADER_COLORS[0] = &COLOR.Global;
	FADER_COLORS[1] = &COLOR.Switch;
	FADER_COLORS[2] = &COLOR.BG;
	FADER_COLORS[3] = &COLOR.BG_Border;
	FADER_COLORS[4] = &COLOR.Bar;
	FADER_COLORS[5] = &COLOR.Bar_Border;
	FADER_COLORS[6] = &COLOR.Cap;
	FADER_COLORS[7] = &COLOR.Cap_Border;
	FADER_COLORS[8] = &COLOR.Stripes_ON;
	FADER_COLORS[9] = &COLOR.Stripes_OFF;
	FADER_COLORS[10] = &COLOR.Line;
	FADER_COLORS[11] = &COLOR.Xfader;
	FADER_COLORS[12] = &COLOR.Xfader_Border;
	FADER_COLORS[13] = &COLOR.Grid;
	///////////////////////////////////////
}///end upd_color_array
void Fader::UPD_ELEMENTS_ARRAY()
{  //create elements pointers array for easier serial manips
	FADER_ELEMENTS[0] = &ELMT.GLOBAL;
	FADER_ELEMENTS[1] = &ELMT.BG;
	FADER_ELEMENTS[2] = &ELMT.BAR;
	FADER_ELEMENTS[3] = &ELMT.CAP;
	FADER_ELEMENTS[4] = &ELMT.X_FADER_BG;
	FADER_ELEMENTS[5] = &ELMT.X_FADER_BAR;

}
///////////////////////////////////////////////////////////////////////
void Fader::UPD_FLARE()
{
	FX.FLARE_RECT = ELMT.CAP.RECT;

	float opa = FX.FLARES_OPACITY/2.0f;
	float radius_mult = 0.625f - ELMT.GLOBAL.ROUND_SIZE;
	float len = (FX.FLARE_RECT.right - FX.FLARE_RECT.left);
	float hei = (FX.FLARE_RECT.bottom - FX.FLARE_RECT.top);
	float CTX = (len / 2.0f);
	float CTY = (hei / 2.0f);
	float Xmult = 0;

	FX.FLARE_INIT.x = CTX - (FX.LIGHT_X_norm*len*radius_mult);
	FX.FLARE_INIT.y = CTY - (FX.LIGHT_Y_norm*hei*radius_mult);
	FX.FLARE_SYM_INIT.x = CTX + (FX.LIGHT_X_norm*len*radius_mult);
	FX.FLARE_SYM_INIT.y = CTY + (FX.LIGHT_Y_norm*hei*radius_mult);

	Xmult = (abs(CTX - FX.FLARE_INIT.x) * 2) * (abs(CTY - FX.FLARE_INIT.y) * 2);
	Xmult /= hei;
	Xmult /= len;
	Xmult *= 1 + (ELMT.GLOBAL.ROUND_SIZE*4.0f);
	Xmult = pow(Xmult, 3);

	FX.FLARE_INTENSITY = opa*Xmult;

}
////////////////////////////////////////////////////////////////////
float Fader::QUANTIZE(float in_val)
{
	float temp_val = in_val;

	if (GRID.QUANTIZE_ON_GRID)
	{

		temp_val *= (float)(GRID.GRID_SUBDIVS - 1);
		temp_val = round(temp_val);
		temp_val /= (float)(GRID.GRID_SUBDIVS - 1);

		return temp_val;
	}
	else
	{
		if (TFParams->VALUE_PRECISION < 2) //if integer will quantize
		{
			temp_val = in_val;

			temp_val *= (float)(TFParams->QUANTIZE_INT);
			temp_val = round(temp_val);
			temp_val /= (float)(TFParams->QUANTIZE_INT);

			return temp_val;
		}
		else
		{
			return in_val;
		}
	}
	
}//end quantize
////////////////////////////////////////////////////////////////////
void Fader::onCreateSettings()
{

	// sdkAddSettingLineCaption("test", "ok");

}
///////////////////////////////////////////
void Fader::APPLY_GLOBAL_SETTINGS()
{
	switch (ELMT.GLOBAL.ROUND_TYPE)
	{
	case 0: ELMT.GLOBAL.IS_ROUND = FALSE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 0; break;//not round
	case 1: ELMT.GLOBAL.IS_ROUND = TRUE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 0; break;// round
	case 2: ELMT.GLOBAL.IS_ROUND = TRUE; ELMT.GLOBAL.ROUND_SMOOTHNESS = 1; break;//hexagon
	default:break;
	}

	for (int i = 1; i < NB_FADER_ELEMENTS; i++)
	{
		FADER_ELEMENTS[i]->ROUND_TYPE = ELMT.GLOBAL.ROUND_TYPE;
		FADER_ELEMENTS[i]->IS_ROUND = ELMT.GLOBAL.IS_ROUND;
		FADER_ELEMENTS[i]->ROUND_SIZE = ELMT.GLOBAL.ROUND_SIZE;
		FADER_ELEMENTS[i]->ROUND_SMOOTHNESS = ELMT.GLOBAL.ROUND_SMOOTHNESS;
		FADER_ELEMENTS[i]->BORDER_SIZE = ELMT.GLOBAL.BORDER_SIZE;
		FADER_ELEMENTS[i]->SHOW_BORDER = ELMT.GLOBAL.SHOW_BORDER;
	}

}
////////////////////////////////////////
void Fader::UPD_STRIPES(bool FULL_UPDATE)
{
 STRIPES.DUAL_DRAW = FALSE;

 STRIPES.GRADIENT_USE = STRIPES.GRADIENT != 0;  //if a gradient value is set compute, otewhise not

float t_bar = 0; float sz_mult = 1.0;

for (int i = 0; i < STRIPES.MAX_PIX ; i++)/////////1   -1
{
	//if (i % STRIPES.MODULO == 0)
	{
		t_bar = float(i) / float(STRIPES.MAX_PIX);   if (!INVERT_MODE){ t_bar = 1 - t_bar; }

		if (i < STRIPES.PIX){
			COLOR.Stripes = COLOR.Stripes_ON.OUT;
			if (STRIPES.GRADIENT_USE)
			{
				STRIPES.STRIPE_COL = sdkAlphaColorToColorAhsl(COLOR.Stripes);
				STRIPES.STRIPE_COL.l -= (t_bar - 0.5f)*STRIPES.GRADIENT;
				STRIPES.STRIPE_COL.l = CLAMP01(STRIPES.STRIPE_COL.l);
				COLOR.Stripes = sdkColorAhslToAlphaColor(STRIPES.STRIPE_COL);
			}
		}
		else{ COLOR.Stripes = COLOR.Stripes_OFF.OUT; }

		STRIPES.STRIPES_COLORS[i] = COLOR.Stripes;

		if (FULL_UPDATE) //modify stripes pos only if needed
		{
			switch (STRIPES.MODE)//different modes
			{
			case 0:  STRIPES.P_from.x = STRIPES.REDUX;  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = 1 - STRIPES.P_from.x;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = FALSE; break; // 0)ok center
			case 1:  STRIPES.P_from.x = STRIPES.REDUX*0.5f;  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = STRIPES.P_from.x + 0.1f;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = TRUE; break; //1)ok side
			case 2:  STRIPES.P_from.x = 0.5f + abs(0.5f - (i % STRIPES.MODULO) / float(STRIPES.MODULO ))*FAD_BOX_W*0.5f;  STRIPES.P_to.x = 1 - STRIPES.P_from.x;  STRIPES.P_from.y = t_bar;   STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = TRUE; break; //2) ruller subdiv
			case 3:  STRIPES.P_from.x = (1 - FAD_BOX_W) + float(i % (STRIPES.MODULO / 2))*0.05f;  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = 0.45f;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = TRUE; sz_mult = float(i % (STRIPES.MODULO / 2)*0.5f); break;//3 zip
			case 4:  STRIPES.P_from.x = (1 - FAD_BOX_W);  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = ((1 - t_bar)*(1 - FAD_BOX_W*0.5f)) + STRIPES.P_from.x;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = FALSE; break;// diagonal raise
			case 5:  STRIPES.P_from.x = ((1 - t_bar)*(1 - FAD_BOX_W*0.75f)) + 0.5f;  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = 1 - STRIPES.P_from.x;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = FALSE; break;// center raise
			case 6:  STRIPES.P_from.x = (i % 2)*0.025f + pow(t_bar*0.42f, 0.50f);  STRIPES.P_from.y = t_bar;  STRIPES.P_to.x = 1.0f - STRIPES.P_from.x;  STRIPES.P_to.y = STRIPES.P_from.y; STRIPES.DUAL_DRAW = FALSE; break;//ok twist
			default:break;
			}

			if (!HORIZONTAL_MODE)
			{
				STRIPES.DashLed.left = STRIPES.P_from.x; STRIPES.DashLed.right = STRIPES.P_to.x;
				STRIPES.DashLed.bottom = STRIPES.P_from.y + STRIPES.DIV_SIZE*0.5f; STRIPES.DashLed.top = STRIPES.DashLed.bottom - STRIPES.DIV_SIZE;
			}//vertical
			else//horizontal
			{
				STRIPES.DashLed.left = 1 - STRIPES.P_from.y; STRIPES.DashLed.right = 1 - STRIPES.P_to.y;
				STRIPES.DashLed.top = STRIPES.P_from.x; STRIPES.DashLed.bottom = STRIPES.P_to.x;
			}

			STRIPES.STRIPES_LEDS[i] = STRIPES.DashLed;


			if (STRIPES.DUAL_DRAW)
			{
				if (!HORIZONTAL_MODE)
				{
					STRIPES.DashLed.left = 1 - STRIPES.DashLed.left;
					STRIPES.DashLed.right = 1 - STRIPES.DashLed.right;
				}//vertical
				else//horizontal
				{
					STRIPES.DashLed.top = 1 - STRIPES.DashLed.top;
					STRIPES.DashLed.bottom = 1 - STRIPES.DashLed.bottom;
				}
				STRIPES.STRIPES_LEDS_SYM[i] = STRIPES.DashLed;
			}//dual drw
		}//full upd
	}//modulo
}//up part

}//end stripes/dash
/////////////////////////////////////
void Fader::UPD_GRID()
{
	if (SHOW.GRID)
	{		
		if (GRID.QUANTIZE_ON_GRID) { FAD_BOX_H = (1.0f / (float)(GRID.GRID_SUBDIVS))*0.5f; }  // match cap height to grid
			float t = 0;
			for (int i = 0; i < GRID.GRID_SUBDIVS - !SHOW.CAP; i++)
			{
				t = (float)(i) / (float)(GRID.GRID_SUBDIVS - !SHOW.CAP); //+show cap?
				GRID.GRID_FROM[i].x = ELMT.BAR.RECT.left;
				GRID.GRID_FROM[i].y = t;
				GRID.GRID_TO[i].x = ELMT.BAR.RECT.right;
				GRID.GRID_TO[i].y = GRID.GRID_FROM[i].y;			
			}
	}//end grid
}
///////////////////////////////////////
TDrawType Fader::GET_PAINT_LAYER(int layer)

{

	switch (layer)
	{
	

	case 0: //return DT_None;
		if (SHOW.BG && FX.SHADOWS)//bg shadow first
		{
		SHAPE_ELMT = ELMT.BG;
		SHAPE_ELMT.offset_X = FX.SHADOW_X;
		SHAPE_ELMT.offset_Y = FX.SHADOW_Y;
		SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.BG.A);
		SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.BG_Border.A);
		return DT_Shape;
		}//shdow		
		else { return DT_None; }
		break;


	case 1:	//return DT_None;
		if (SHOW.BG && FX.OCCLUSION)
			{
			FX.OCCLUSION_SHAPE = ELMT.BG;
			FX.OCCLUSION_SHAPE.RECT = SCALE_RECT(FX.OCCLUSION_SHAPE.RECT, 0.93f, 0.97f);
			SHAPE_ELMT = FX.OCCLUSION_SHAPE;
			SHAPE_ELMT.SHOW_FILL = FALSE;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.OCLLUSION_OPACITY);
			SHAPE_ELMT.BORDER_SIZE = ELMT.BG.BORDER_SIZE*2.0f;
			return DT_Shape;
			}//occlusion
			else { return DT_None; }
			break;

	case 2: //return DT_None;
		if (SHOW.BG)
			{
			SHAPE_ELMT = ELMT.BG;
			SHAPE_ELMT.FILL_COLOR = COLOR.BG.OUT;
			SHAPE_ELMT.BORDER_COLOR = COLOR.BG_Border.OUT;
			return DT_Shape;//normal bg
			}
		else { return DT_None; }
			break;

	case 3: //return DT_None;
			if (SHOW.BG && FX.REFLEXIONS)
			{
				SHAPE_ELMT = ELMT.BG;
				SHAPE_ELMT.FILL_COLOR = HSLA(0.0f, 0.25f, 0, FX.REFLEXIONS_OPACITY*0.25f);
				FX.REFLEXIONS_TYPE = FX.REFLEXIONS_TYPE_IN + 2;
				return DT_Shape_Reflexion; //refl bg
			}
			else { return DT_None; }
			break;
			
	case 4: if (SHOW.BG && FX.SPECULAR && ELMT.BG.SHOW_BORDER)
			{//specs
			FX.SpecOffset = (ELMT.BG.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.BG;
			SHAPE_ELMT.offset_X = -FX.SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = FX.SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.15f, 0, FX.SPECULAR_OPACITY*COLOR.BG_Border.A);
			SHAPE_ELMT.BORDER_SIZE = ELMT.BG.BORDER_SIZE*0.5f;
			return DT_Shape_Partial; //spec hi
			}
			else { return DT_None; }
			break;

	case 5: if (SHOW.BG && FX.SPECULAR && ELMT.BG.SHOW_BORDER)
			{//specs
			FX.SpecOffset = (ELMT.BG.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.BG;
			SHAPE_ELMT.offset_X = FX.SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = -FX.SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.7f, 1.0f, FX.SPECULAR_OPACITY*COLOR.BG_Border.A);
			SHAPE_ELMT.BORDER_SIZE = ELMT.BG.BORDER_SIZE*0.40f;
			return DT_Shape_Partial; //spec lo
			}
			else { return DT_None; }
			break;

	case 6: if (SHOW.BAR && FX.SHADOWS)
			{//bar shadows
				SHAPE_ELMT = ELMT.BAR;
				SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.Bar.A);
				SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0,FX.SHADOW_OPACITY*COLOR.Bar_Border.A);
				return DT_Shape;
			}//bar sdw
			else { return DT_None; }
			break;

	case 7: if (SHOW.BAR)
			{
				SHAPE_ELMT = ELMT.BAR;
				SHAPE_ELMT.FILL_COLOR = COLOR.Bar.OUT;
				SHAPE_ELMT.BORDER_COLOR = COLOR.Bar_Border.OUT;
				return DT_Shape;
			}
			else { return DT_None; }
			break;

	case 8: if (SHOW.BAR && FX.OCCLUSION)
				{
				FX.OCCLUSION_SHAPE = ELMT.BAR;
				FX.OCCLUSION_SHAPE.RECT = SCALE_RECT(FX.OCCLUSION_SHAPE.RECT, 0.95f, 0.95f);
				SHAPE_ELMT = FX.OCCLUSION_SHAPE;
				SHAPE_ELMT.SHOW_FILL = FALSE;
				SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.OCLLUSION_OPACITY);
				return DT_Shape;
				}
			else { return DT_None; }
			break;

	case 9:	if (SHOW.BAR && FX.SPECULAR)
				{
				SHAPE_ELMT = ELMT.BAR;
				FX.SpecOffset = (ELMT.BAR.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
				SHAPE_ELMT.offset_X = -FX.SpecOffset*FX.SPEC_DIR_X;
				SHAPE_ELMT.offset_Y = FX.SpecOffset*FX.SPEC_DIR_Y;
				SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.15f, 0, FX.SPECULAR_OPACITY*COLOR.Bar_Border.A);
				SHAPE_ELMT.BORDER_SIZE = ELMT.BAR.BORDER_SIZE*0.5f;
				return DT_Shape_Partial;
				}
			else { return DT_None; }
				break;

	case 10:	if (SHOW.BAR && FX.SPECULAR)
				{
				SHAPE_ELMT = ELMT.BG;
				FX.SpecOffset = (ELMT.BAR.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
				SHAPE_ELMT.offset_X = FX.SpecOffset*FX.SPEC_DIR_X;
				SHAPE_ELMT.offset_Y = -FX.SpecOffset*FX.SPEC_DIR_Y;
				SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.7f, 0.9f, FX.SPECULAR_OPACITY*COLOR.Bar_Border.A*0.5f);
				SHAPE_ELMT.BORDER_SIZE = ELMT.BAR.BORDER_SIZE*0.25f;
				return DT_Shape_Partial;//bg specs over bar
				}
				else { return DT_None; }
				break;

	case 11:	if (FX.REFLEXIONS && SHOW.BG && SHOW.BAR) //both = dual bars middle
				{
				FX.SpecOffset = -(ELMT.BG.BORDER_SIZE*4.0f) / TFParams->CANVAS_S;
				SHAPE_ELMT = ELMT.BG;
				SHAPE_ELMT.offset_X = -FX.SpecOffset + FX.SPEC_DIR_X*0.0125f;
				SHAPE_ELMT.offset_Y = -FX.SPEC_DIR_Y*0.0125f;
				SHAPE_NB_SEGS = 1;
				SHAPE_SEG_FROM = 0;
				SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.5f, 1 - FX.SPEC_DIR_X*0.5f, FX.REFLEXIONS_OPACITY*COLOR.BG.A*0.4f);
				SHAPE_ELMT.BORDER_SIZE = ELMT.BG.BORDER_SIZE*2.0f;
				return DT_Shape_Partial;
				}
				else { return DT_None; }
				break;
				

	case 12:	if (FX.REFLEXIONS && SHOW.BG && SHOW.BAR) //both = dual bars middle
				{
				FX.SpecOffset = (ELMT.BG.BORDER_SIZE*4.0f) / TFParams->CANVAS_S;
				SHAPE_ELMT = ELMT.BG;
				SHAPE_ELMT.offset_X = -FX.SpecOffset + FX.SPEC_DIR_X*0.0125f;
				SHAPE_ELMT.offset_Y = -FX.SPEC_DIR_Y*0.0125f;
				SHAPE_NB_SEGS = 1;
				SHAPE_SEG_FROM = 2;
				SHAPE_ELMT.BORDER_COLOR = HSLA(0.0f, 0.5f, 0.5f + FX.SPEC_DIR_X*0.5f, FX.REFLEXIONS_OPACITY*COLOR.BG.A*0.4f);
				SHAPE_ELMT.BORDER_SIZE = ELMT.BG.BORDER_SIZE*1.5f;
				return DT_Shape_Partial;
				}
				else { return DT_None; }
				break;
				

	case 13:	if (SHOW.BAR && FX.REFLEXIONS)
				{
					SHAPE_ELMT = ELMT.BAR;
					SHAPE_ELMT.FILL_COLOR = HSLA(0.0f, 0.25f, 0, FX.REFLEXIONS_OPACITY*0.5f);
					FX.REFLEXIONS_TYPE = FX.REFLEXIONS_TYPE_IN + 2;
					return DT_Shape_Reflexion;
				}
				else { return DT_None; }
				break;

	case 14: if (SHOW.STRIPES) //fader dash /stripes
				{return DT_TRect_Array;}
				else { return DT_None; }
				break;//stripes

	case 15:	if (SHOW.LINE)
				{
				LINE_ELMT.LINE_FROM = LINE_FROM;
				LINE_ELMT.LINE_TO = LINE_TO;
				LINE_ELMT.LINE_COLOR = COLOR.Line.OUT;
				LINE_ELMT.LINE_SIZE = LINE_SIZE;
				return DT_Line;//line
				}
				else { return DT_None; }
				break;

	case 16: if (SHOW.GRID)
				{return DT_Lines_Array;}
			 else { return DT_None; }
				break;//grid

	case 17:	if (SHOW.CAP && FX.SHADOWS)
				{
					SHAPE_ELMT = ELMT.CAP;
					SHAPE_ELMT.offset_X = FX.SHADOW_X;
					SHAPE_ELMT.offset_Y = FX.SHADOW_Y;
					SHAPE_ELMT.FILL_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.Cap_Border.A*0.75f);
					SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*COLOR.Cap_Border.A*0.75f);
					return DT_Shape;
				}
				else { return DT_None; }
				break;

	case 18: if (SHOW.CAP)
				{
				SHAPE_ELMT = ELMT.CAP;
				SHAPE_ELMT.FILL_COLOR = COLOR.Cap.OUT;
				SHAPE_ELMT.BORDER_COLOR = COLOR.Cap_Border.OUT;
				return DT_Shape;
				}
			 else { return DT_None; }
				 break;

	case 19: if (SHOW.CAP && FX.REFLEXIONS)
	{
		SHAPE_ELMT = ELMT.CAP;
		SHAPE_ELMT.FILL_COLOR = HSLA(0.0f, 0.25f, 0, FX.REFLEXIONS_OPACITY);
		FX.REFLEXIONS_TYPE = FX.REFLEXIONS_TYPE_IN;
		return DT_Shape_Reflexion;
	}
			 else { return DT_None; }
			 break;

	case 20: if (SHOW.CAP && FX.OCCLUSION)
	{
		FX.OCCLUSION_SHAPE = ELMT.CAP;
		FX.OCCLUSION_SHAPE.RECT = SCALE_RECT(FX.OCCLUSION_SHAPE.RECT, 0.95f, 0.95f);
		SHAPE_ELMT = FX.OCCLUSION_SHAPE;
		SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.OCLLUSION_OPACITY);
		SHAPE_ELMT.SHOW_FILL = FALSE;
		return DT_Shape;
	}
			 else { return DT_None; }
			 break;

	case 21: if (SHOW.CAP && FX.SPECULAR && ELMT.CAP.SHOW_BORDER)
			{
			FX.SpecOffset = -(ELMT.CAP.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.CAP;
			SHAPE_ELMT.offset_X = FX.SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = -FX.SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SPECULAR_OPACITY*COLOR.Cap_Border.A);
			SHAPE_ELMT.BORDER_SIZE = ELMT.CAP.BORDER_SIZE / 2.0f;
			SHAPE_ELMT.SHOW_FILL = FALSE;
			return DT_Shape;
			}
			 else { return DT_None; }
			break;

	case 22: if (SHOW.CAP && FX.SPECULAR && ELMT.CAP.SHOW_BORDER)
			{
			FX.SpecOffset = -(ELMT.CAP.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.CAP;
			SHAPE_ELMT.offset_X = -FX.SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = FX.SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 1.0, FX.SPECULAR_OPACITY*COLOR.Cap_Border.A);
			SHAPE_ELMT.BORDER_SIZE = ELMT.CAP.BORDER_SIZE / 2.0f;
			SHAPE_ELMT.SHOW_FILL = FALSE;
			return DT_Shape;
			}
			 else { return DT_None; }
			 break;

	case 23: if (SHOW.CAP && FX.SPECULAR && ELMT.CAP.SHOW_BORDER)
			{
			FX.SpecOffset = -(ELMT.CAP.BORDER_SIZE*0.5f) / TFParams->CANVAS_S;
			SHAPE_ELMT = ELMT.CAP;
			SHAPE_ELMT.offset_X = -FX.SpecOffset*FX.SPEC_DIR_X;
			SHAPE_ELMT.offset_Y = FX.SpecOffset*FX.SPEC_DIR_Y;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 1.0f, FX.SPECULAR_OPACITY*COLOR.Cap_Border.A*0.5f);
			SHAPE_ELMT.BORDER_SIZE = ELMT.CAP.BORDER_SIZE / 4.0f;
			SHAPE_NB_SEGS = 2;
			SHAPE_SEG_FROM = 1;
			SHAPE_ELMT.SHOW_FILL = FALSE;
			return DT_Shape_Partial;
			}
			 else { return DT_None; }
			 break;

	case 24: if (SHOW.CAP && FX.SPECULAR && ELMT.CAP.SHOW_BORDER)
	{
		FX.SpecOffset = (ELMT.CAP.BORDER_SIZE*2.0f) / TFParams->CANVAS_S;
		SHAPE_ELMT = ELMT.CAP;
		SHAPE_ELMT.offset_X = FX.SpecOffset*FX.SPEC_DIR_X;
		SHAPE_ELMT.offset_Y = -FX.SpecOffset*FX.SPEC_DIR_Y*0.5f;
		SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SPECULAR_OPACITY*COLOR.Cap_Border.A*0.5f);
		SHAPE_ELMT.BORDER_SIZE = ELMT.CAP.BORDER_SIZE *2.0f;
		SHAPE_NB_SEGS = 2;
		SHAPE_SEG_FROM = 3;
		SHAPE_ELMT.SHOW_FILL = FALSE;
		return DT_Shape_Partial;
	}
			 else { return DT_None; }
			 break;

	case 25: if (SHOW.CAP && FX.SPECULAR && ELMT.CAP.SHOW_BORDER)
	{
		FX.SpecOffset = (ELMT.CAP.BORDER_SIZE*2.0f) / TFParams->CANVAS_S;
		SHAPE_ELMT = ELMT.CAP;
		SHAPE_ELMT.offset_X = -FX.SpecOffset*0.5f*FX.SPEC_DIR_X;
		SHAPE_ELMT.offset_Y = FX.SpecOffset*FX.SPEC_DIR_Y*-0.5f;
		SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 1.0f, FX.SPECULAR_OPACITY*COLOR.Cap_Border.A*0.25f);
		SHAPE_ELMT.BORDER_SIZE = ELMT.CAP.BORDER_SIZE *1.0f;
		SHAPE_NB_SEGS = 2;
		SHAPE_SEG_FROM = 1;
		SHAPE_ELMT.SHOW_FILL = FALSE;
		return DT_Shape_Partial;
	}
			 else { return DT_None; }
			 break;
	case 26:
		if (FX.FLARES) //cap  hot spots flares
		{
			float t_spot = 0;
			FX.FLARE_RECT = ELMT.CAP.RECT;
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
		break;//flares

	case 27: if (X_FADER_SUBMODE > 0 && FX.SHADOWS)  //adding Xfader
			{
			SHAPE_ELMT = ELMT.X_FADER_BG;
			SHAPE_ELMT.offset_X = FX.SHADOW_X*0.5f;
			SHAPE_ELMT.offset_Y = FX.SHADOW_Y*0.5f;
			SHAPE_ELMT.BORDER_COLOR = HSLA(0, 0, 0, FX.SHADOW_OPACITY*0.5f);
			SHAPE_ELMT.FILL_COLOR = SHAPE_ELMT.BORDER_COLOR;
			SHAPE_ELMT.BORDER_SIZE = ELMT.X_FADER_BG.BORDER_SIZE*0.75f;
			return DT_Shape;
			}
			 else { return DT_None; }
			break;

	case 28:if (X_FADER_SUBMODE > 0)
			{
				SHAPE_ELMT = ELMT.X_FADER_BG;
				SHAPE_ELMT.FILL_COLOR = COLOR.Cap.OUT;
				SHAPE_ELMT.BORDER_COLOR = COLOR.Cap_Border.OUT;
				SHAPE_ELMT.SHOW_BORDER = ELMT.GLOBAL.SHOW_BORDER;
				SHAPE_ELMT.BORDER_SIZE = ELMT.X_FADER_BAR.BORDER_SIZE*0.5f;
				return DT_Shape;
			}
			else { return DT_None; }
			break;

	case 29:if (X_FADER_SUBMODE > 0)
	{
		SHAPE_ELMT = ELMT.X_FADER_BAR;
		SHAPE_ELMT.FILL_COLOR = COLOR.Xfader.OUT;
		SHAPE_ELMT.BORDER_COLOR = COLOR.Xfader_Border.OUT;
		SHAPE_ELMT.SHOW_BORDER = ELMT.GLOBAL.SHOW_BORDER;
		SHAPE_ELMT.BORDER_SIZE = ELMT.X_FADER_BAR.BORDER_SIZE*0.5f;
		return DT_Shape;
	}
			else { return DT_None; }
			break;

	case 30: if (X_FADER_SUBMODE > 0 && TFParams->SHOW_VALUE)
	{

		VALUE_ELMT.RECT = ELMT.X_FADER_BG.RECT;
		VALUE_ELMT.VALUE_STRING = X_FADER_VALUE_STRING;
		VALUE_ELMT.H_Align = taCenter;
		VALUE_ELMT.V_Align = taCenter;
		
		return DT_Text_Value;
	}
			 else { return DT_None; }
			 break;


	case 31: if (TFParams->SHOW_VALUE)
	{
		VALUE_ELMT.VALUE_STRING = FADER_VALUE_STRING;
				if (VALUE_IN_CAP)
				{
					VALUE_ELMT.RECT = ELMT.CAP.RECT;					
					VALUE_ELMT.H_Align = taCenter;
					VALUE_ELMT.V_Align = taCenter;
					//VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
				}
				else 
				{ 
					VALUE_ELMT.RECT = TFParams->VALUE_RECT;
					VALUE_ELMT.H_Align = taCenter;
					VALUE_ELMT.V_Align = taLeading;
					//VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
				}
				return DT_Text_Value;
	}
			 else { return DT_None; }
			 break;

			

	default:return DT_None;	break;
	}




}////end get paint layer
//////////////////////////////////////////
void Fader::UPD_PAINT_POINTERS()
{
	TFPaintP->COLOR_Grid_OUT = &COLOR.Grid.OUT;
	TFPaintP->FX_FLARE = &FX.FLARE;
	TFPaintP->FX_FLARES_COLORS = FX.FLARES_COLORS;
	TFPaintP->FX_FLARES_SIZES = FX.FLARES_SIZES;
	TFPaintP->FX_FLARES_SYM_COLORS = FX.FLARES_SYM_COLORS;
	TFPaintP->FX_FLARES_SYM_SIZES = FX.FLARES_SYM_SIZES;
	TFPaintP->FX_FLARE_SYM = &FX.FLARE_SYM;
	TFPaintP->FX_NB_FLARES = FX.NB_FLARES;
	TFPaintP->FX_REFLEXIONS_EACH = &FX.REFL_DRAW_EACH;
	TFPaintP->FX_REFLEXIONS_TYPE = &FX.REFLEXIONS_TYPE;
	TFPaintP->GRID_GRID_FROM = GRID.GRID_FROM;
	TFPaintP->GRID_GRID_TO = GRID.GRID_TO;
	TFPaintP->GRID_SUBDIVS = &GRID.GRID_SUBDIVS;
	TFPaintP->LINE_ELMT = &LINE_ELMT;
	TFPaintP->LINE_SIZE = &LINE_SIZE;
	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	TFPaintP->SHAPE = &SHAPE_ELMT;
	TFPaintP->SHAPE_NB_SEGS = &SHAPE_NB_SEGS;
	TFPaintP->SHAPE_SEG_FROM = &SHAPE_SEG_FROM;
	TFPaintP->STRIPES_DUAL_DRAW = &STRIPES.DUAL_DRAW;
	TFPaintP->STRIPES_MAX_PIX = &STRIPES.MAX_PIX;
	TFPaintP->STRIPES_STRIPES_COLORS = STRIPES.STRIPES_COLORS;
	TFPaintP->STRIPES_STRIPES_LEDS = STRIPES.STRIPES_LEDS;
	TFPaintP->STRIPES_STRIPES_LEDS_SYM = STRIPES.STRIPES_LEDS_SYM;
	TFPaintP->VALUE_ELMT = &VALUE_ELMT;
//	TFPaintP->STRIPES_MODULO = &STRIPES.MODULO;
}
///////////////////////////////////////////////////////////////////////
void Fader::UPD_STRINGS()
{
	if (TFParams->SHOW_VALUE)
	{
		FADER_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(TFParams->ITEM_VALUE_Y, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);

		if (X_FADER_SUBMODE > 0)
		{
			X_FADER_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(X_FADER.VAL, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
		}
	}

}// upd strings
///////////////////////////////////////////
////////////////////////////////////////////
void Fader::PRESET_LOAD(int preset)
{
	

	GRID.QUANTIZE_ON_GRID = FALSE; //all exept if specified

	switch (preset)
	{

	case 0:                             //DFT skin

		//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 6;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 1;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = TRUE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 4.0f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = TRUE;
		SHOW.STRIPES = TRUE;
		SHOW.BAR = TRUE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;		
		//------FX--------		
		FX.LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.33f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 4; // draw nth pixels
		STRIPES.WIDTH = 0.4f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
		//------------------
		LINE_SIZE = 1.0;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
	case 1:                             //MINIMAL skin

										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 1;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = FALSE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 1.5f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = FALSE;
		SHOW.STRIPES = FALSE;
		SHOW.BAR = TRUE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.33f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 4; // draw nth pixels
		STRIPES.WIDTH = 0.4f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 2:                             //Stripes skin

										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 8;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = FALSE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 4.0f;
		ELMT.GLOBAL.RATIO = 1.0;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = FALSE;
		SHOW.STRIPES = TRUE;
		SHOW.BAR = FALSE;
		SHOW.BG = FALSE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.0;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.33f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 4; // draw nth pixels
		STRIPES.WIDTH = 0.4f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 3:                             //3D hexa skin

										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 8;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 2;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = TRUE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 0.5f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = TRUE;
		SHOW.STRIPES = TRUE;
		SHOW.BAR = TRUE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 3; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.5f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.40f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 16; // draw nth pixels
		STRIPES.WIDTH = 0.8f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 4:                             //3D Reflections skin
										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 6;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 1;
		ELMT.GLOBAL.ROUND_SIZE = 0.04f;
		ELMT.GLOBAL.IS_ROUND = TRUE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 4.0f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = TRUE;
		SHOW.STRIPES = FALSE;
		SHOW.BAR = TRUE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 5; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.30f;
		FX.SHADOW_OPACITY = 0.50f;
		FX.SPECULAR_OPACITY = 0.50f;
		FX.REFLEXIONS_OPACITY = 0.50f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.3f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 16; // draw nth pixels
		STRIPES.WIDTH = 0.8f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 5:                             //CrossFader skin
										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 6;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.04f;
		ELMT.GLOBAL.IS_ROUND = FALSE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 2.0f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = TRUE;
		SHOW.CAP = TRUE;
		SHOW.STRIPES = TRUE;
		SHOW.BAR = FALSE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xC03984AD;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x4C4A5E6F;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 2; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0;
		FX.OCLLUSION_OPACITY = 0.30f;
		FX.SHADOW_OPACITY = 0.50f;
		FX.SPECULAR_OPACITY = 0.50f;
		FX.REFLEXIONS_OPACITY = 0.50f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.03f;
		FAD_BOX_W = 0.88f;
		FAD_BAR_W = 0.45f;
		//------STRIPES
		STRIPES.MODE = 3; //0 center 1 side ect
		STRIPES.MODULO = 5; // draw nth pixels
		STRIPES.WIDTH = 0.2f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 6:                             //Diagonal raise skin
										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 6;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.04f;
		ELMT.GLOBAL.IS_ROUND = FALSE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 2.0f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = FALSE;
		SHOW.STRIPES = TRUE;
		SHOW.BAR = FALSE;
		SHOW.BG = FALSE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.1f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xC03984AD;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4CB3FFCA;
		COLOR.Stripes_OFF.IN = 0x4C2D3944;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.30f;
		FX.SHADOW_OPACITY = 0.50f;
		FX.SPECULAR_OPACITY = 0.50f;
		FX.REFLEXIONS_OPACITY = 0.50f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.03f;
		FAD_BOX_W = 0.88f;
		FAD_BAR_W = 0.45f;
		//------STRIPES
		STRIPES.MODE = 4; //0 center 1 side ect
		STRIPES.MODULO = 16; // draw nth pixels
		STRIPES.WIDTH = 0.7f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = -0.5f;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0;
		GRID.GRID_SUBDIVS = 11;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 7:                             //Tube skin

										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 4;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 2;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = TRUE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 0.5f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = FALSE;
		SHOW.STRIPES = FALSE;
		SHOW.BAR = TRUE;
		SHOW.BG = TRUE;
		SHOW.GRID = FALSE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 4; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.5f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.40f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 1;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 16; // draw nth pixels
		STRIPES.WIDTH = 0.8f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 1.0f;
		GRID.GRID_SUBDIVS = 12;
		break;
		/////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////
	case 8:                             //Grid Quantize skin

		GRID.QUANTIZE_ON_GRID = TRUE;
										//Sub modes
		FADER_SWITCH_SUBMODE = 1;
		SWITCH_TARGET = 6;
		//----------Layout
		ELMT.GLOBAL.ROUND_TYPE = 0;
		ELMT.GLOBAL.ROUND_SIZE = 0.05f;
		ELMT.GLOBAL.IS_ROUND = FALSE;
		ELMT.GLOBAL.SHOW_BORDER = TRUE;
		ELMT.GLOBAL.BORDER_SIZE = 0.5f;
		ELMT.GLOBAL.RATIO = 1.0f;
		ELMT.GLOBAL.ROUND_SMOOTHNESS = 0;
		// Show--
		SHOW.LINE = FALSE;
		SHOW.CAP = TRUE;
		SHOW.STRIPES = FALSE;
		SHOW.BAR = FALSE;
		SHOW.BG = TRUE;
		SHOW.GRID = TRUE;
		//-----------------------
		//-------TFPColors behaviour
		COLOR.Global.HUE_MODE = 2;//full overide
		TFPColors->HUE_BEAVIOUR = 2;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.5f;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.2f;
		//------------COLORS
		COLOR.BG.IN = 0x5C21282C;
		COLOR.BG_Border.IN = 0xA641708E;
		COLOR.Bar.IN = 0xDA29434F;
		COLOR.Bar_Border.IN = 0xAA5992BF;
		COLOR.Line.IN = 0xFF1B1B1C;
		COLOR.Cap.IN = 0x83276688;
		COLOR.Cap_Border.IN = 0xC03984AD;
		COLOR.Stripes_ON.IN = 0x4C4A5E6F;
		COLOR.Stripes_OFF.IN = 0x431C2122;
		COLOR.Switch.IN = 0x8378FF21;
		COLOR.Xfader.IN = 0xFF1E4857;
		COLOR.Xfader_Border.IN = 0xCC217A99;
		COLOR.Grid.IN = 0x6D4E5889;
		//------FX--------		
		FX.LEVEL = 0; //02d 1shadow 2 specs 3 reflections 4 flares
		FX.LIGHTING_ROLL = 0.38f;
		FX.LIGHTING_DEPTH = 1.0f;
		FX.OCLLUSION_OPACITY = 0.20f;
		FX.SHADOW_OPACITY = 0.33f;
		FX.SPECULAR_OPACITY = 0.33f;
		FX.REFLEXIONS_OPACITY = 0.33f;
		FX.REFLEXIONS_TYPE_IN = 0;
		FX.FLARES_OPACITY = 0.33f;
		//--CAP------------------
		FAD_BOX_H = 0.1f;
		FAD_BOX_W = 0.9f;
		FAD_BAR_W = 0.1f;
		//------STRIPES
		STRIPES.MODE = 0; //0 center 1 side ect
		STRIPES.MODULO = 4; // draw nth pixels
		STRIPES.WIDTH = 0.4f; // width
		STRIPES.H_REDUX = 0.12f;
		STRIPES.GRADIENT = 0;  // ad lum gradient toward range
							   //------------------
		LINE_SIZE = 5.0f;
		GRID.GRID_SUBDIVS = 4;
		break;
		/////////////////////////////////////////////////////////////////////////////////




	default:
		break;
	}




}//end preset
/////////////////////////////////////////////////
void Fader::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.BG.IN = 0x5C21282C;
	COLOR.BG_Border.IN = 0xA641708E;
	COLOR.Bar.IN = 0xDA29434F;
	COLOR.Bar_Border.IN = 0xAA5992BF;
	COLOR.Line.IN = 0xFF1B1B1C;
	COLOR.Cap.IN = 0x83276688;
	COLOR.Cap_Border.IN = 0xC03984AD;
	COLOR.Stripes_ON.IN = 0x4C4A5E6F;
	COLOR.Stripes_OFF.IN = 0x431C2122;
	COLOR.Switch.IN = 0x8378FF21;
	COLOR.Xfader.IN = 0xFF1E4857;
	COLOR.Xfader_Border.IN = 0xCC217A99;
	COLOR.Grid.IN = 0x6D4E5889;
	///////////////
	
	TFPColors->COLOR_CORRECT = TRUE;
	APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
	//TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum

	
		for (int i = 0; i < NB_FADER_COLORS; i++)
		{
			FADER_COLORS[i]->OnSetQuickColor();
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
