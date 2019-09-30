//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "Encoder.h"

Encoder::Encoder()
{

}

Encoder::Encoder(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{
	TFParams = TFParams_val;
	TFPaintP = TFPaintP_val;
	TFPColors = new TFPColor();   //destruct??

	Encoder_Rect.left = 0;
	Encoder_Rect.right = 1;
	Encoder_Rect.top = 0;
	Encoder_Rect.bottom = 1;

	UPD_COLOR_ARRAY();

	for (int i = 0; i < NB_ENCODER_COLORS; i++)
	{
		*ENCODER_COLORS[i] = TFColor(TFParams, TFPColors);
	}

	PRESET_LOAD(0);

	onSettingsHasChanged(); //import settings
}

void Encoder::onMouseDown()
{
	TFParams->hilight = 1.0;

	if (INPUT_TYPE == 2)  //angular reset delta angle
	{
		ANGLE_RESET_DELTA = TRUE;
	}


	UPD_FROM_MOUSE();


		if (ENCODER_SUBMODE == 1 ) // enc button
		{
			if (ENCODER_SUBMODE_BEHAVIOUR == 0) //if need restore prev val
			{
				encoder_val = MSD_ENC_VAL;
				TFParams->ITEM_VALUE_Y = encoder_val;
			}
		 TFParams->ITEM_VALUE_SWITCH = TRUE;
		}
		if (ENCODER_SUBMODE == 2 ) //swich encoder
		{
			MSD_ENC_VAL = encoder_val;
			if (TFParams->ITEM_VALUE_SWITCH){ ENCODER_WILL_SW_OFF = TRUE; }else{ ENCODER_WILL_SW_OFF = FALSE;} //if is on will potentiall  sw off if no moved performed
			TFParams->ITEM_VALUE_SWITCH = TRUE;
			if (ENCODER_SUBMODE_BEHAVIOUR == 0) //if need restore prev val
			{
				encoder_val = encoder_memo;
				TFParams->ITEM_VALUE_Y = encoder_val;
			}
		}

		UPD_COLORS();
		UPD_PAINT();
		UPD_STRINGS();
}

void Encoder::onInputChange()
{

	encoder_val = TFParams->ITEM_VALUE_Y;

	//encoder_val =  TFParams->input_val;
	//TFParams->ITEM_VALUE_Y = encoder_val;

	UPD_COLORS();
	UPD_PAINT();
	UPD_STRINGS();
}

void Encoder::onPreset()
{
	encoder_val = TFParams->ITEM_VALUE_Y;
	UPD_COLORS();
	UPD_PAINT();
	UPD_STRINGS();
}

void Encoder::onMouseUp()
{
	TFParams->hilight = 0;

		if (ENCODER_SUBMODE == 1 ) // enc button
		{
			MSD_ENC_VAL = encoder_val;  //use this var to remeber value back on before applying reset
			TFParams->ITEM_VALUE_SWITCH = FALSE;
			if (ENCODER_SUBMODE_BEHAVIOUR != 2)  //reset exepts if switch only mode
			{
				TFParams->ITEM_VALUE_Y = TFParams->RESET_VALUE_01;
				encoder_val = TFParams->RESET_VALUE_01;
			}
		}

		if (ENCODER_SUBMODE == 2 ) //swich encoder
		{
			if (MSD_ENC_VAL != encoder_val)   //won't swict off if val chgd
			{
				encoder_memo = encoder_val;
				ENCODER_WILL_SW_OFF = FALSE;
			}

			if (ENCODER_WILL_SW_OFF)
			{
				//memo
				TFParams->ITEM_VALUE_SWITCH = FALSE;
				if (ENCODER_SUBMODE_BEHAVIOUR != 2)
				{
					TFParams->ITEM_VALUE_Y = TFParams->RESET_VALUE_01;
					encoder_val = TFParams->RESET_VALUE_01;
				}
			}
			
		}

		UPD_COLORS();
		UPD_PAINT();
		UPD_STRINGS();

}

void Encoder::onMouseMove()
{

	UPD_FROM_MOUSE();

}

void Encoder::UPD_FROM_MOUSE()
{


	if (RELATIVE_MODE)
	{
		if (INPUT_TYPE==0) //y as input
		{
			encoder_val -= (TFParams->DELTA_Y*ENCODER_SPEED);
		}
		else if (INPUT_TYPE == 1) // x as input
		{ 
			encoder_val += (TFParams->DELTA_X*ENCODER_SPEED);
		}
		else if (INPUT_TYPE == 2)
		{
			GET_ENCODER_ANGLE();
			encoder_val += angle_delta * ENCODER_SPEED;
		}
	}
	else //absolute mode
	{
		if (INPUT_TYPE != 2) //if not angular but x or y
		{
			if (ENCODER_LAYOUT_TYPE == 1 || EN_HORIZONTAL_MODE) { encoder_val = TFParams->MSX; } //horizontal bar //x	
			else { encoder_val = 1 - TFParams->MSY; } //y
			//encoder_val = RE_RANGE_NORM_OUT(encoder_val, start_offset, 1- start_offset);
		}

		else { GET_ENCODER_ANGLE(); encoder_val = angle; } // angle if input type3
	}

	encoder_val = CLAMP01(encoder_val);
	TFParams->ITEM_VALUE_Y = encoder_val;
	UPD_PAINT();
	UPD_STRINGS();
}

void Encoder::UPD_PAINT()
{
    
	DOT_X = 0;
	DOT_Y = 0;
	DOT_T = 0;

	if (SHOW.RING)
	{	
		QUADRA_RING_DUAL(encoder_val, ENCODER_LEDS_MODE,QUAD_ON_MAINS,QUAD_ON_CTRLS);

		if (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2) // bars
		{		
			BAR.RECT.bottom = 0.5f + BAR_V_SIZE;
			BAR.RECT.top = 0.5f - BAR_V_SIZE;
			BAR_OFF.RECT.left = start_offset;
			BAR_OFF.RECT.right = BAR_OFF.RECT.left + RING_LENGTH;
			BAR_OFF.RECT.top = BAR.RECT.top;
			BAR_OFF.RECT.bottom = BAR.RECT.bottom;
				
			switch (ENCODER_LEDS_MODE)
			{
			case 0: BAR.RECT.left = start_offset; BAR.RECT.right = RE_RANGE_NORM_IN(encoder_val, cap_width + start_offset, RING_LENGTH - cap_width + start_offset); break;   // norm
			case 1:	BAR.RECT.right = 1 - start_offset; BAR.RECT.left = 1 - RE_RANGE_NORM_IN(1 - encoder_val, cap_width + start_offset, RING_LENGTH - cap_width + start_offset); break; //reverse
			case 2: BAR.RECT.left = 0.5f - encoder_val*RING_LENGTH / 2.0f; BAR.RECT.right = BAR.RECT.left + encoder_val*RING_LENGTH;  break; // spread
			case 3: if (encoder_val < 0.5f){ BAR.RECT.right = 0.5f; BAR.RECT.left = start_offset + cap_width + encoder_val*(RING_LENGTH - cap_width*2.0f); } //pan L
					else{ BAR.RECT.left = 0.5f; BAR.RECT.right = start_offset + cap_width + encoder_val*(RING_LENGTH - cap_width*2.0f); } break; // pan R
			case 4: BAR.RECT.left = start_offset + cap_width + encoder_val*(RING_LENGTH - cap_width*2.0f) - 0.01f; BAR.RECT.right = BAR.RECT.left + 0.02f; //cursor
			default:	break;
			}

			if (ENCODER_LAYOUT_TYPE == 2)//v bar
			{
				SWAP_RECT_ORIENTATION(BAR.RECT);
				SWAP_RECT_ORIENTATION(BAR_OFF.RECT);
			}
		}///bars quad
//leds		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////// leds done
		if (SHOW.LEDS_RING && ENCODER_LAYOUT_TYPE <3)
		{float led_seg_width = (1.0f / (float)RING_LEDS)*0.75f;

			for (int j = 0; j < RING_LEDS; j++)
			{
				DOT_T = 1 - (float(j) / float(RING_LEDS - 1));

				if (ENCODER_LAYOUT_TYPE == 0){ DOT_T = RE_RANGE_NORM_IN(DOT_T, led_seg_width, 1 - led_seg_width); }

				ring_on_part = LED_TEST(encoder_val);
				//////////////////////////leds color
				if (!ring_on_part){ ENCODER_RING_COLOR = COLOR.Leds_Off.OUT; }
				else{
					if (LEDS_GRADIENT == 0){ ENCODER_RING_COLOR = COLOR.Leds_On.OUT; }
					else{
						COLOR.Leds_On.OUT_HSLA.l = CLAMP01(COLOR.Leds_On.IN_HSLA.l + (GET_GRADIENT(DOT_T, ENCODER_LEDS_MODE)*LEDS_GRADIENT));
						ENCODER_RING_COLOR = sdkColorAhslToAlphaColor(COLOR.Leds_On.OUT_HSLA);
					}
				}
				DOTS_COLOR[j] = ENCODER_RING_COLOR;
				////////////////////////
				DOT_T = DOT_T*RING_LENGTH + start_offset;

				if (ENCODER_LAYOUT_TYPE == 0) //ring
				{	DOT_T += 0.25f;
					DOT_X = ((cos(DOT_T*M2PI)*RING_RADIUS*LEDS_RADIUS)*TFParams->CANVAS_RATIO) + 0.5f;
					DOT_Y = (sin(DOT_T*M2PI)*RING_RADIUS*LEDS_RADIUS) + 0.5f;
					DOTS[j].x = DOT_X; DOTS[j].y = DOT_Y;
				}
				else //if (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2) // horizontal & vertical bar
				{SET_RECT_ORIENTATION(LEDS_RECTS[j], DOT_T, 0.5f, BAR_H_SIZE, RING_RADIUS, ENCODER_LAYOUT_TYPE - 1);
				} //fuction to set h or v rect
				//end bars leds upd
			}//loop
		}//end leds
	}//rings & bars/////
	///////////////////////////////////////////////////////
	//// CENTERS - CAP
	/////////////////////////////////////////////////////////////
	if (SHOW.CENTER && ENCODER_LAYOUT_TYPE<3) //only rings & bars centers
	{
		CENTER_A_SIZE = RING_RADIUS*CENTER_A_RADIUS;////////////////////cap radius

		if (ENCODER_LAYOUT_TYPE == 0)   //ring
		{	CENTER.x = 0.5 ; CENTER.y = 0.5 ;			
			if (CENTER_DRAW_TYPE > 0){ CENTER_TURN(encoder_val); } //ngon turn			
		}
		else //if (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2) //bars
		{
			cap_width = CENTER_A_SIZE;
			cap_len = RING_RADIUS;

			SET_RECT_ORIENTATION(Center_Led, RE_RANGE_NORM_IN(encoder_val, start_offset+ cap_width , 1 - start_offset-cap_width), 0.5f, cap_width, cap_len, ENCODER_LAYOUT_TYPE - 1);

			if (ENCODER_LEDS_MODE == 2) //spread keep cap centered
			{	if (ENCODER_LAYOUT_TYPE == 1)
				{	Center_Led.left = 0.5f - cap_width;Center_Led.right = 0.5f + cap_width;}
				else {Center_Led.top = 0.5f - cap_width;Center_Led.bottom = 0.5f + cap_width;}
			}
		}//bars
		/////////////////////////////////////////////////////////////////////////////////
	}//// end center//
	else if (!SHOW.CENTER){cap_width = 0;} //if no center wont compensate
	//////line CURSOR
	if (SHOW.CURSOR_ && ENCODER_LAYOUT_TYPE < 3) //cursor and ring or bars
	{
		CURSOR_SIZE_USED = CURSOR_SIZE*RING_RADIUS*5.0f;

		if (ENCODER_LAYOUT_TYPE == 0) //ring dot or line compute
		{
			DOT_T = encoder_val;
			DOT_T = DOT_T*RING_LENGTH + 0.25f + start_offset;

			LINE_FROM.x = ((cos(DOT_T*M2PI)*RING_RADIUS*CURSOR_LENGTH)*TFParams->CANVAS_RATIO) + 0.5f ;
			LINE_FROM.y = (sin(DOT_T*M2PI)*RING_RADIUS*CURSOR_LENGTH) + 0.5f ;
			LINE_TO.x = ((cos(DOT_T*M2PI)*RING_RADIUS*CURSOR_LENGTH_B)*TFParams->CANVAS_RATIO) + 0.5f;
			LINE_TO.y = (sin(DOT_T*M2PI)*RING_RADIUS*CURSOR_LENGTH_B) + 0.5f ;

			if (CURSOR_TYPE > 1) //shape cursor
			{
				SHAPE_H_OFFSET = 0;//RING_H_OFFSET;
				SHAPE_V_OFFSET = 0;// RING_V_OFFSET;
				SHAPE_TURN(encoder_val);
			}
		}
		else if (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2) //bars
		{
			float LINE_RADIUS;
			if (CURSOR_LINKED_TO_RADIUS){LINE_RADIUS = RING_RADIUS;}else{ LINE_RADIUS = 0.25; }


			 if (ENCODER_LAYOUT_TYPE == 1) // H bar
			{
				LINE_FROM.y = 0.5f - CURSOR_LENGTH*LINE_RADIUS  + CURSOR_EXTRA_OFFSET;
				LINE_TO.y = 0.5f + CURSOR_LENGTH_B*LINE_RADIUS  + CURSOR_EXTRA_OFFSET;

				LINE_FROM.x = RE_RANGE_NORM_IN(encoder_val, start_offset + cap_width, (1 - start_offset) - cap_width) ;
				LINE_TO.x = LINE_FROM.x;

				if (ENCODER_LEDS_MODE == 2) //spread keep cap centered
				{
					LINE_FROM.x = 0.5f + RING_H_OFFSET;
					LINE_TO.x = LINE_FROM.x;
				}

				if (CURSOR_TYPE > 1) //shape cursor
				{
				SHAPE_V_OFFSET = RING_RADIUS*0.5f ;
				SHAPE_H_OFFSET = -0.5f + encoder_val*(RING_LENGTH-cap_width*2.0f) + (1 - RING_LENGTH)*0.5f  + cap_width;
				if (ENCODER_LEDS_MODE == 2) //spread keep cap centered
				{SHAPE_H_OFFSET = RING_H_OFFSET;}
				SHAPE_TURN(0);
				}

			}//hbar
			else if (ENCODER_LAYOUT_TYPE == 2) // V bar
			{
				LINE_FROM.x = 0.5f - CURSOR_LENGTH*LINE_RADIUS  + CURSOR_EXTRA_OFFSET;
				LINE_TO.x = 0.5f + CURSOR_LENGTH_B*LINE_RADIUS  + CURSOR_EXTRA_OFFSET;

				LINE_FROM.y = RE_RANGE_NORM_IN((1-encoder_val), start_offset + cap_width, (1 - start_offset) - cap_width);
				LINE_TO.y = LINE_FROM.y;

				if (ENCODER_LEDS_MODE == 2) //spread keep shp centered
				{
					LINE_FROM.y = 0.5f + RING_V_OFFSET;
					LINE_TO.y = LINE_FROM.y;
				}

			if (CURSOR_TYPE > 1) //shape cursor
			{
				SHAPE_V_OFFSET = -0.5f + (1 - encoder_val)*(RING_LENGTH - cap_width*2.0f) + (1 - RING_LENGTH)*0.5f + cap_width;
				if (ENCODER_LEDS_MODE == 2) //spread keep shp centered
				//{	SHAPE_V_OFFSET = RING_V_OFFSET;}
				SHAPE_TURN(0);
			}

			}
		}//bars

		
	}/////end line
	//////////////////////////
	// middle indicator
/*	if (SHOW.MIDDLE_INDICATOR)
	{

		switch (ENCODER_LAYOUT_TYPE)
		{
			
		case 0: DOT_T = 0.5* RING_LENGTH  + start_offset + 0.25;
			MIDDLE_INDICATOR_FROM.x = ((cos(DOT_T*M2PI)*RING_RADIUS*MIDDLE_INDICATOR_RADIUS_FROM)*TFParams->CANVAS_RATIO) + 0.5;
			MIDDLE_INDICATOR_FROM.y = (sin(DOT_T*M2PI)*RING_RADIUS*MIDDLE_INDICATOR_RADIUS_FROM) + 0.5;
			MIDDLE_INDICATOR_TO.x = ((cos(DOT_T*M2PI)*RING_RADIUS*MIDDLE_INDICATOR_RADIUS_TO)*TFParams->CANVAS_RATIO) + 0.5 ;
			MIDDLE_INDICATOR_TO.y = (sin(DOT_T*M2PI)*RING_RADIUS*MIDDLE_INDICATOR_RADIUS_TO) + 0.5 ;
			break; //ring
		case 1: MIDDLE_INDICATOR_FROM.x = 0.5 ; MIDDLE_INDICATOR_FROM.y = 0.5 - (RING_RADIUS*MIDDLE_INDICATOR_RADIUS_FROM);
			MIDDLE_INDICATOR_TO.x = 0.5  ; MIDDLE_INDICATOR_TO.y = 0.5 + (RING_RADIUS*MIDDLE_INDICATOR_RADIUS_TO) ; break; //hbar
		case 2: MIDDLE_INDICATOR_FROM.x = 0.5 - (RING_RADIUS*MIDDLE_INDICATOR_RADIUS_FROM); MIDDLE_INDICATOR_FROM.y = 0.5 ;
			MIDDLE_INDICATOR_TO.x = 0.5 + (RING_RADIUS*MIDDLE_INDICATOR_RADIUS_TO) ; MIDDLE_INDICATOR_TO.y = 0.5 ; break; //vbar
		default:
			break;
		}		
	}//end indicator
	*/
	/////////////////////////
	if (ENCODER_LAYOUT_TYPE == 3 || ENCODER_LAYOUT_TYPE == 4) //color panel or fader
	{
		//panel_lum = RE_RANGE_NORM_IN(GET_GRADIENT(encoder_val,ENCODER_LEDS_MODE), PANEL_MIN_LUM, PANEL_MAX_LUM);

		if (ENCODER_LAYOUT_TYPE == 3)
		{
			Encoder_Rect.top = 0; Encoder_Rect.bottom = 1;
		}

		switch (ENCODER_LEDS_MODE)
		{
		case 0: panel_lum = encoder_val; break; //norm
		case 1: panel_lum = 1-encoder_val; break;//rev
		case 2: panel_lum = encoder_val; break; //spread
		case 3: panel_lum = abs(0.5f- encoder_val)*2.0f; break; //pan
		case 4: panel_lum = encoder_val; break;//cursor
		default:
			break;
		}

		panel_lum = RE_RANGE_NORM_IN(panel_lum, PANEL_MIN_LUM, PANEL_MAX_LUM);

		CURSOR_lum = (2.0f - encoder_val)*PANEL_MAX_LUM*0.5f;
		fader_lum = panel_lum / 2.0f;
		fader_sat = 0;
		PANEL_COLOR = HSLA(COLOR.Panel.OUT_HSLA.h, COLOR.Panel.OUT_HSLA.s, panel_lum, COLOR.Panel.OUT_HSLA.a);
		FADER_COLOR = HSLA(COLOR.Panel.OUT_HSLA.h, fader_sat, fader_lum, COLOR.Panel.OUT_HSLA.a);

		if (ENCODER_LAYOUT_TYPE == 4) //fader panel
		{
			Cusor_Fader.left = 0.01f;
			Cusor_Fader.right = 0.98f;

			switch (ENCODER_LEDS_MODE)
			{
			case 0: Cusor_Fader.top = 0.01f; Cusor_Fader.bottom = 1 - encoder_val; Encoder_Rect.top = 0; Encoder_Rect.bottom = 1; break; //norm
			case 1: Cusor_Fader.bottom = 0.98f; Cusor_Fader.top = encoder_val; Encoder_Rect.top = 0; Encoder_Rect.bottom = 1; break;//rev
			case 2: Encoder_Rect.top = 0.5f - encoder_val*0.5f; Encoder_Rect.bottom = 0.5f + encoder_val*0.5f; Cusor_Fader.top = 0; Cusor_Fader.bottom = 1; break; //spread
			case 3: if (encoder_val < 0.5f){ Encoder_Rect.top = 0.5f; Encoder_Rect.bottom = 1 - encoder_val; }
					else{ Encoder_Rect.bottom = 0.5f; Encoder_Rect.top = 1 - encoder_val; }
					Cusor_Fader.top = 0; Cusor_Fader.bottom = 1; break; //pan
			case 4: Encoder_Rect.top = 1 - encoder_val - 0.01f; Encoder_Rect.bottom = Encoder_Rect.top + 0.02f; Cusor_Fader.top = 0; Cusor_Fader.bottom = 1;  break;//cursor
			default:
				break;
			}
			if (ADD_PANEL_CURSOR_)
			{
				CURSOR_FROM.x = 0.05f; 
				CURSOR_TO.x = 0.95f;

				switch (ENCODER_LEDS_MODE)
				{
				case 0: CURSOR_FROM.y = 1 - encoder_val; break; //norm
				case 1: CURSOR_FROM.y = encoder_val; break;//rev
				case 2: CURSOR_FROM.y = 0.5 ; break; //spread
				case 3: CURSOR_FROM.y = 1 - encoder_val; break; //pan
				case 4: CURSOR_FROM.y = 1 - encoder_val; break; //cursor
				default:
					break;
				}

				CURSOR_TO.y = CURSOR_FROM.y;
				COLOR.Cursor.OUT = HSLA(COLOR.Panel.OUT_HSLA.h, COLOR.Panel.OUT_HSLA.s, CURSOR_lum, COLOR.Panel.OUT_HSLA.a);
			}

		}//fader panel
	}//panels types

}

bool Encoder::LED_TEST(float test_val)
{
	bool IS_ON = FALSE;    //leds are off by default, switch on depending mode

	float w = 1.0f / (float)(RING_LEDS);
	float a = 1.0f;

	switch (LEDS_TYPE)
	{
	case 0: a = 0.5f; break; //single led on within 0.5 part of segment width
	case 1: a = 1.0f; w *= 2.0f; break; //segment /dual lead need more tolerance
	case 2: a = 0.5f; break; //to center led on within 0.5 part of segment width
	case 3: a = 0.5f; break; //3d led on within 0.5 part of segment width
	default:
		break;
	}


	switch (ENCODER_LEDS_MODE)
	{
	case 0: IS_ON = DOT_T <= test_val; break;  //norm
	case 1: IS_ON = DOT_T >= test_val; break;  //reverse
	case 2: IS_ON = abs(DOT_T - 0.5) * 2 < test_val; break; //spread
	case 3: IS_ON = (DOT_T > test_val  && DOT_T < 0.5) || (DOT_T < test_val  && DOT_T > 0.5); break; //pan
	case 4: IS_ON = (abs(DOT_T - test_val) <= (a / ((float)RING_LEDS - (test_val == 0))))
					|| (DOT_T < w && test_val < w) || (DOT_T>(1-w) && test_val > (1-w))	; 
						break; //single-CURSOR_ <=
	default:
		break;
	}

	return IS_ON;
}

void Encoder::QUADRA_RING(float inval, int draw_mode)
{
	//single circle part path

	QUAD_NBP = MAX_QUADS; //16max path points (mains & quadratic controls)


	float start_offset = 0; //global rotation offset
	float CURSOR_val   = 0; // used val for end of ring
	float DIR = 0;         // direction 1 clockwise -1 inverse clockwise
	float global_offset = 0.25f;   // by dft start a quarter circle late correction
	float width = ((1.0f / (float)(QUAD_NBP - 1))*RING_LENGTH) / 2.0f; //segment half width to add radius mult for ctrl point 
	float t = 0;    //time when operating 0..1 of main point
	float t_ctrl = 0;   //time of ctrl point
	float CTRL_RADIUS = 0;  //radius of ctrl point

	start_offset = ((1.0f - RING_LENGTH) / 2.0f) + RING_ROTATION_OFFSET;///;
	//////////////////////////////////////// draw modes 
	switch (draw_mode)
	{
	case 0:  CURSOR_val = inval*RING_LENGTH; DIR = 1; break;  //norm
	case 1:  CURSOR_val = (1 - inval)*RING_LENGTH; start_offset += RING_LENGTH; DIR = -1;  break;  //reverse
	case 2:  CURSOR_val = inval*RING_LENGTH; start_offset += RING_LENGTH*0.5f - (CURSOR_val / 2.0f); DIR = 1; break; //spread
	case 3: if (inval < 0.5f){ CURSOR_val = (0.5f - inval)*RING_LENGTH; start_offset += RING_LENGTH*0.5f; DIR = -1; }
			else{ CURSOR_val = (inval - 0.5f)*RING_LENGTH; start_offset += RING_LENGTH*0.5f; DIR = 1; }break; //pan
	case 4: CURSOR_val = 0; start_offset +=  inval*RING_LENGTH ; DIR = 1; break; //single-CURSOR_
	default:break;
	}
	//////////////////////////////////////////////////////////
	start_offset += global_offset;

	CTRL_RADIUS = RING_RADIUS + (width*inval*0.1f); //maybe no need inval

	for (int i = 0; i < QUAD_NBP; i++)
	{
		t = (float)(i) / (float)(QUAD_NBP - 1);

		t = (t*CURSOR_val*DIR) + start_offset;
	
		if (i == 0){ t_ctrl = t; }else{ t_ctrl = t - (width*inval*DIR); }; //maybe no need inval


		QUAD_DOT.x = cos(t*M2PI)*RING_RADIUS*TFParams->CANVAS_RATIO + 0.5f;
		QUAD_DOT.y = sin(t*M2PI)*RING_RADIUS + 0.5f;
		QUAD_CTRL.x = cos(t_ctrl*M2PI)*CTRL_RADIUS *TFParams->CANVAS_RATIO + 0.5f;
		QUAD_CTRL.y = sin(t_ctrl*M2PI)*CTRL_RADIUS + 0.5f;

		QUAD_MAIN_DOTS[i] = QUAD_DOT;		QUAD_CTRL_DOTS[i] = QUAD_CTRL;
	}//loop

}///end quadra ring

void Encoder::QUADRA_RING_DUAL(float inval, int draw_mode, TPointF MAINS[], TPointF  CTRLS[])
{
	QUAD_NBP = MAX_QUADS; //16max path points (mains & quadratic controls)

	//dual path

	float Radius_low_mult = QUAD_INNER_RADIUS; //low radius mult
	float start_offset = 0.0f; //global rotation offset
	float CURSOR_val = 0.0f; // 
	float DIR = 0.0f;         // direction 1 clockwise -1 inverse clockwise
	float global_offset = 0.25f;   // by dft start a quarter circle late correction
	float width = ((1.0f / (float)(QUAD_NBP - 1))*RING_LENGTH) / 2.0f; //segment half width to add radius mult for ctrl point 
	float t = 0.0f;    //time when operating 0..1 of main point
	float t_ctrl = 0.0f;   //time of ctrl point
	float CTRL_RADIUS = 0.0f;  //radius of ctrl point
	float cursor_len = 0.025f;

	start_offset = ((1 - RING_LENGTH) / 2.0f) + RING_ROTATION_OFFSET;///;
	//////////////////////////////////////// draw modes 
	switch (draw_mode)
	{
	case 0:  CURSOR_val = inval*RING_LENGTH; DIR = 1; break;  //norm
	case 1:  CURSOR_val = (1 - inval)*RING_LENGTH; start_offset += RING_LENGTH; DIR = -1;  break;  //reverse
	case 2:  CURSOR_val = inval*RING_LENGTH; start_offset += RING_LENGTH*0.5f - (CURSOR_val / 2.0f); DIR = 1; break; //spread
	case 3: if (inval < 0.5f){ CURSOR_val = (0.5f - inval)*RING_LENGTH; start_offset += RING_LENGTH*0.5f; DIR = -1; }
			else{ CURSOR_val = (inval - 0.5f)*RING_LENGTH; start_offset += RING_LENGTH*0.5f; DIR = 1; }break; //pan
	case 4: CURSOR_val = cursor_len; start_offset += inval*(RING_LENGTH - cursor_len); DIR = 1; break; //single-CURSOR_
	default:break;
	}
	//////////////////////////////////////////////////////////
	start_offset += global_offset;

	CTRL_RADIUS = RING_RADIUS + (width*CURSOR_val*0.1f); //maybe no need inval

	for (int i = 0; i < QUAD_NBP; i++)
	{
		t = (float)(i) / (float)(QUAD_NBP - 1);

		t = (t*CURSOR_val*DIR) + start_offset;

		if (i == 0 ){ t_ctrl = t; }
		else{ t_ctrl = t - (width*CURSOR_val*DIR); }; //maybe no need inval

		QUAD_DOT.x = cos(t*M2PI)*RING_RADIUS*TFParams->CANVAS_RATIO + 0.5f;
		QUAD_DOT.y = sin(t*M2PI)*RING_RADIUS + 0.5f;
		QUAD_CTRL.x = cos(t_ctrl*M2PI)*CTRL_RADIUS *TFParams->CANVAS_RATIO + 0.5f;
		QUAD_CTRL.y = sin(t_ctrl*M2PI)*CTRL_RADIUS + 0.5f;

		MAINS[i] = QUAD_DOT;		CTRLS[i] = QUAD_CTRL;
		///////////////////////////////////////////////////dwn part
		if (i == QUAD_NBP-1){ t_ctrl = t; }
		else{ t_ctrl = t - (width*CURSOR_val*-DIR); }; //maybe no need inval
		
		QUAD_DOT.x = cos(t*M2PI)*RING_RADIUS*Radius_low_mult*TFParams->CANVAS_RATIO + 0.5f;
		QUAD_DOT.y = sin(t*M2PI)*RING_RADIUS*Radius_low_mult + 0.5f ;
		QUAD_CTRL.x = cos(t_ctrl*M2PI)*CTRL_RADIUS*Radius_low_mult *TFParams->CANVAS_RATIO + 0.5f;
		QUAD_CTRL.y = sin(t_ctrl*M2PI)*CTRL_RADIUS*Radius_low_mult + 0.5f ;
		
		MAINS[QUAD_NBP_B - 1 - i] = QUAD_DOT;		CTRLS[QUAD_NBP_B - 1 - i] = QUAD_CTRL;
		
	}//loop

}///end quadra ring

void Encoder::SHAPE_TURN(float inval)
{
	//hegon turn
	float curs_w = 0.0125f;

	int NBP =  SHAPE_NB_POINTS+2;
	float t = 0;
	float HX = 0;
	float HY = 0;
	float Shape_Radius = 0;
	float Shape_Radius_A = 0;
	float Shape_Radius_B = 0;
	float start_offset = ((1 - RING_LENGTH) / 2.0f) + RING_ROTATION_OFFSET + 0.25f;
	float flip_mult = 0;

	if (CURSOR_LINKED_TO_RADIUS)
	{
		Shape_Radius_A =  RING_RADIUS*CURSOR_LENGTH;
		Shape_Radius_B =  RING_RADIUS*CURSOR_LENGTH_B;
	}
	else
	{
		Shape_Radius_A = 0.25f*CURSOR_LENGTH;
		Shape_Radius_B = 0.25f*CURSOR_LENGTH_B;
	}

	if (CURSOR_FLIP){flip_mult = -1;}else{	flip_mult = 1;}

	switch (ENCODER_LAYOUT_TYPE)
	{
	case 0: break; //ring
	case 1: start_offset = 0.75f - (CURSOR_FLIP*0.5f); SHAPE_V_OFFSET = CURSOR_EXTRA_OFFSET*flip_mult + (1 - RING_LENGTH)*(0.5f*flip_mult); break; //hbar
	case 2: start_offset = 0 + (CURSOR_FLIP*0.5f); SHAPE_H_OFFSET = CURSOR_EXTRA_OFFSET*flip_mult  -(1-RING_LENGTH)*(0.5f*flip_mult); break; //vbar
	default:
		break;
	}


	float prev_t = 0;

	for (int i = 0; i < NBP; i++)
	{
		if (i == 0 || i == 1 || i == NBP - 1){ Shape_Radius = Shape_Radius_A; }
		else{ Shape_Radius = Shape_Radius_B; }

		t = (float)(i) / (float)(NBP - 1);

		t -= curs_w;

		t += start_offset + inval*RING_LENGTH;

		if (i == 1 || i == NBP - 2){ t = prev_t + curs_w*2.0f; }

		HX = cos(t*M2PI)*Shape_Radius*TFParams->CANVAS_RATIO + 0.5f;
		HY = sin(t*M2PI)*Shape_Radius + 0.5f;

		SHAPE[i].x = HX + SHAPE_H_OFFSET;
		SHAPE[i].y = HY + SHAPE_V_OFFSET;

		prev_t = t;
	}


}//end shape

void Encoder::CENTER_TURN(float inval)
{
	//hegon turn
	int NBP_CENTER = CENTER_NBP+1;
	float t = 0;
	float Shape_Radius = CENTER_A_RADIUS*(RING_RADIUS*0.5f);
	float start_offset = ((1 - RING_LENGTH) / 2.0f) + RING_ROTATION_OFFSET + 0.25f;

	for (int i = 0; i < NBP_CENTER; i++)
	{
		t = float(i) / float(NBP_CENTER-1);

		t += inval*RING_LENGTH;
		t += start_offset;

		CENTER_POINTS[i].x = cos(t*M2PI)*Shape_Radius*TFParams->CANVAS_RATIO + 0.5f;
		CENTER_POINTS[i].y = sin(t*M2PI)*Shape_Radius + 0.5f;
	}

}//end center

float Encoder::GET_GRADIENT(float in_val, int draw_mode)
{
	float result = 0;

	switch (draw_mode)
	{
	case 0: result = in_val;  break;  //normal
	case 1: result = (1-in_val);  break;  //reverse
	case 2: result = abs(0.5f - in_val)*2.0f;  break;  //spread
	case 3: result = abs(0.5f - in_val)*2.0f; break; //pan
	case 4: result = 0.5f;  break;  //cursor
	default:break;
	}
	return result;
}/////////////get grad

void Encoder::onSettingsHasChanged()
{

	EN_HORIZONTAL_MODE = (INPUT_TYPE == 1); //x as input

	SHOW.QUAD_RING = (SHOW.FILLED_RING || SHOW.RING_BORDER);
	 start_offset = (1 - RING_LENGTH) / 2.0f;// +(RING_ROTATION_OFFSET*(ENCODER_LAYOUT_TYPE == 0));

	COLOR.Global.update_in(0, TRUE, FALSE, 0.2f);
	COLOR.Switch.update_in(1, ENCODER_SUBMODE>0, FALSE, 0.2f);
	COLOR.Ring_Off.update_in(2, SHOW.QUAD_RING, FALSE, 0.05f);
	COLOR.Ring_Off_Border.update_in(3, SHOW.QUAD_RING, FALSE, 0.05f);
	COLOR.Ring_On.update_in(4, SHOW.QUAD_RING, FALSE, 0.4f);
	COLOR.Ring_On_Border.update_in(5, SHOW.QUAD_RING, FALSE, 0.4f);
	COLOR.Leds_Off.update_in(6, SHOW.LEDS_RING, FALSE, 0.05f);
	COLOR.Leds_On.update_in(7, SHOW.LEDS_RING, FALSE, 0.4f);
	COLOR.Cursor.update_in(8, SHOW.CURSOR_, FALSE, 0.4f);
	COLOR.Center_Cap.update_in(9, SHOW.CENTER, FALSE, 0.1f);
	COLOR.Center_Cap_Border.update_in(10, SHOW.CENTER, FALSE, 0.1f);
	COLOR.Middle_indicator.update_in(11,SHOW.MIDDLE_INDICATOR, FALSE, 0.1f);
	COLOR.Panel.update_in(12, ENCODER_LAYOUT_TYPE>2, FALSE, 0.1f);


	TFPColors->TARGET_COLOR = COLOR.Switch.IN;

	UPD_COLOR_ARRAY();

	////////////////////////////////////////////////////////////////////////
	if (APPLY_COLOR_CORRECT)     // if clicing on apply color correct, it will make change to all colors set 
	{
		for (int i = 0; i < NB_ENCODER_COLORS; i++)
		{
			ENCODER_COLORS[i]->update_out();
			ENCODER_COLORS[i]->IN = ENCODER_COLORS[i]->OUT;
			ENCODER_COLORS[i]->IN_HSLA = ENCODER_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	}

	//////////////////////////////



	QUAD_SIZE_USED = ((1 - QUAD_INNER_RADIUS)*RING_RADIUS) / TFParams->CANVAS_S;

	TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE; // use gloabal from 0 tho individual are avaible

	UPD_COLORS();
	UPD_PAINT();
	TFParams->ITEM_MAIN_OUT = MAIN_OUTPUT;

	SHADOW_COLOR = HSLA(0.0f, 0.15f, 0.1f, SHADOW_OPACITY);
	SHADOW_COLOR_LO = HSLA(0.0f, 0.15f, 0.1f, SHADOW_OPACITY / 2.0f);
	SPECULAR_COLOR = HSLA(0.0f, 0.35f, 0.7f, SPECULAR_OPACITY);
	SPECULAR_COLOR_LO = HSLA(0.0f, 0.35f, 0.7f, SPECULAR_OPACITY / 2.0f);
	SPECULAR_COLOR_VLO = HSLA(0.0f, 0.35f, 0.7f, SPECULAR_OPACITY / 4.0f);
	SPECULAR_COLOR_HI = HSLA(0.0f, 0.35f, 0.7f, SPECULAR_OPACITY*2.0f);

	BAR_V_SIZE = RING_RADIUS *(1 + (QUAD_SIZE_USED / 50.0f));
	BAR_H_SIZE = DOT_SIZE_USED / 20 / RING_LEDS;
	DOT_SIZE_USED = LEDS_SIZE*RING_RADIUS * 5.0f;

	
	QUADRA_RING_DUAL(1.0, 0, QUAD_OFF_MAINS, QUAD_OFF_CTRLS);

	BAR.SHOW_FILL = SHOW.FILLED_RING;
	BAR_OFF.SHOW_FILL = SHOW.FILLED_RING;
	BAR.SHOW_BORDER = SHOW.RING_BORDER;
	BAR_OFF.SHOW_BORDER = SHOW.RING_BORDER;
	BAR.IS_ROUND = BAR_IS_ROUND;
	BAR_OFF.IS_ROUND = BAR_IS_ROUND;
	BAR.ROUND_SIZE = BAR_ROUND_SIZE;
	BAR_OFF.ROUND_SIZE = BAR_ROUND_SIZE;

	UPD_PAINT_POINTERS();

	UPD_STRINGS();
	
}

void Encoder::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier manips
	ENCODER_COLORS[0] = &COLOR.Global;
	ENCODER_COLORS[1] = &COLOR.Switch;
	ENCODER_COLORS[2] = &COLOR.Ring_Off;
	ENCODER_COLORS[3] = &COLOR.Ring_Off_Border;
	ENCODER_COLORS[4] = &COLOR.Ring_On;
	ENCODER_COLORS[5] = &COLOR.Ring_On_Border;
	ENCODER_COLORS[6] = &COLOR.Leds_Off;
	ENCODER_COLORS[7] = &COLOR.Leds_On;
	ENCODER_COLORS[8] = &COLOR.Cursor;
	ENCODER_COLORS[9] = &COLOR.Center_Cap;
	ENCODER_COLORS[10] = &COLOR.Center_Cap_Border;
	ENCODER_COLORS[11] = &COLOR.Middle_indicator;
	ENCODER_COLORS[12] = &COLOR.Panel;
	///////////////////////////////////////
}///end upd_color_array

void Encoder::UPD_COLORS()
{
	if (ENCODER_SUBMODE > 0){TFPColors->USE_TARGET_COLOR = !TFParams->ITEM_VALUE_SWITCH;}
	else  {TFPColors->USE_TARGET_COLOR = FALSE; }

	for (int i = 0; i < NB_ENCODER_COLORS; i++)	{ENCODER_COLORS[i]->update_out();}

	if (TFParams->ITEM_VALUE_SWITCH)
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;
	}
	else
	{
		VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_OFF_COLOR;
	}
}

void Encoder::SET_RECT_ORIENTATION(TRectF &RECT, float invalA, float centerB, float symX, float symY, bool is_vertical)
{
	//SET_RECT_ORIENTATION(rect,DOT_T,BAR_H_SIZE,RING_RADIus, h v)

	float L,  R, T, B;

	B = centerB + symY;
	T = centerB - symY;
	L = invalA - symX;
	R = invalA + symX;

	RECT.left = L;
	RECT.right = R;
	RECT.bottom = B;
	RECT.top = T;

	if (is_vertical)
	{
		RECT.left = T;
		RECT.right = B;
		RECT.bottom = 1 - L;
		RECT.top = 1 - R;
	}
}

void Encoder::SWAP_RECT_ORIENTATION(TRectF &RECT)
{
	TRectF RECT_TMP;

	RECT_TMP = RECT;

	RECT.left = RECT_TMP.top;
	RECT.right = RECT_TMP.bottom;
	RECT.top = 1 - RECT_TMP.right;
	RECT.bottom = 1 - RECT_TMP.left;
}

TDrawType Encoder::GET_PAINT_LAYER(int layer)
{
//	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;

	switch (layer)
	{

	case 0: if (SHOW.RING && SHOW.QUAD_RING)
	{
		if (ENCODER_LAYOUT_TYPE == 0){ return DT_Ring; }
		else if (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2) //bars
		{
			SHAPE_ELMT = BAR_OFF;
			SHAPE_ELMT.FILL_COLOR = COLOR.Ring_Off.OUT;
			SHAPE_ELMT.BORDER_COLOR = COLOR.Ring_Off_Border.OUT;
			SHAPE_ELMT.BORDER_SIZE = RING_BORDER_SIZE;
			return DT_Shape;
		}
		else { return DT_None; }
	}
	else { return DT_None; }
	break;

	case 1: if ((SHOW.RING && SHOW.QUAD_RING) && (ENCODER_LAYOUT_TYPE == 1 || ENCODER_LAYOUT_TYPE == 2)) //bars
				{
				SHAPE_ELMT = BAR;
				SHAPE_ELMT.FILL_COLOR = COLOR.Ring_On.OUT;
				SHAPE_ELMT.BORDER_COLOR = COLOR.Ring_On_Border.OUT;
				SHAPE_ELMT.BORDER_SIZE = RING_BORDER_SIZE;
				return DT_Shape;
				}
				else { return DT_None; }
				break;

	case 2: return DT_Enc_Leds; break;


	case 3: if (CENTER_LAYER_POSITION == 0)
			{return DT_Enc_Cursor;}
			else
			{return DT_Enc_Center;}
			break;

	case 4: if (CENTER_LAYER_POSITION == 1)
			{return DT_Enc_Cursor;}
			else{return DT_Enc_Center;}
			break;


	case 5: return DT_Enc_Panels; break;


	case 6: if (TFParams->SHOW_VALUE)
			{
			VALUE_ELMT.VALUE_STRING = ENCODER_VALUE_STRING;		
			VALUE_ELMT.RECT = TFParams->VALUE_RECT;
			VALUE_ELMT.H_Align = taCenter;
			VALUE_ELMT.V_Align = taLeading;
			VALUE_ELMT.VALUE_COLOR = TFParams->VALUE_ON_COLOR;		
			return DT_Text_Value;
			}
			 else { return DT_None; }
			 break;

	default:return DT_None; break;
	}


}

void Encoder::UPD_PAINT_POINTERS()
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
	TFPaintP->GRID_GRID_FROM = nullptr;//GRID.GRID_FROM;
	TFPaintP->GRID_GRID_TO = nullptr;//GRID.GRID_TO;
	TFPaintP->GRID_SUBDIVS = nullptr;//&GRID.GRID_SUBDIVS;
	TFPaintP->LINE_ELMT = nullptr;//&LINE_ELMT;
	TFPaintP->LINE_SIZE = nullptr;//&LINE_SIZE;
	TFPaintP->PAINT_NB_ELMTS_TO_DRAW = &PAINT_NB_ELMTS_TO_DRAW;
	TFPaintP->SHAPE = &SHAPE_ELMT;
	TFPaintP->SHAPE_NB_SEGS = nullptr;//&SHAPE_NB_SEGS;
	TFPaintP->SHAPE_SEG_FROM = nullptr;//&SHAPE_SEG_FROM;
	TFPaintP->STRIPES_DUAL_DRAW = nullptr;//&STRIPES.DUAL_DRAW;
	TFPaintP->STRIPES_MAX_PIX = nullptr;//&STRIPES.MAX_PIX;
	TFPaintP->STRIPES_STRIPES_COLORS = nullptr;//STRIPES.STRIPES_COLORS;
	TFPaintP->STRIPES_STRIPES_LEDS = nullptr;//STRIPES.STRIPES_LEDS;
	TFPaintP->STRIPES_STRIPES_LEDS_SYM = nullptr;//STRIPES.STRIPES_LEDS_SYM;
	TFPaintP->VALUE_ELMT = &VALUE_ELMT;

}

void Encoder::UPD_STRINGS()
{

	if (TFParams->SHOW_VALUE)
	{

		ENCODER_VALUE_STRING = sdkFloatToString(RE_RANGE_NORM_IN(TFParams->ITEM_VALUE_Y, TFParams->OUT_MIN, TFParams->OUT_MAX), TFParams->VALUE_PRECISION, 0);
	}
	
}

void Encoder::GET_ENCODER_ANGLE()
{

	TPointF center;
	TPointF mouse;

	center.x = 0.5;
	center.y = 0.5;
	mouse.x = TFParams->MSX;
	mouse.y = TFParams->MSY;


	angle = GET_ANGLE(center, mouse, 0.25);
	angle = RE_RANGE_NORM_OUT(angle,start_offset, 1- start_offset);
	angle_delta = angle - angle_prev;

	if(ANGLE_RESET_DELTA || abs(angle_delta)>0.75)
	{
		angle_delta = 0;
		ANGLE_RESET_DELTA = FALSE;
	}

	angle_prev = angle;
//	return angle;

}

void Encoder::PRESET_LOAD(int preset)

{


	switch (preset)
	{
		/////////////////////////// 
	case 0:					///0 DFT SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5; //bypass as swich will hilight itself
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.22f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = TRUE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = TRUE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xFF629AB6;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.55f;
		RING_BORDER_SIZE = 4.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.25f;
		LEDS_SIZE = 8.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 0;
		CURSOR_SIZE = 4.0f;
		CURSOR_LENGTH = 0.25f;
		CURSOR_LENGTH_B = 0.01f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0 ;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.42f;
		CENTER_BORDER_SIZE = 0.20f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.05f;
		SHADOW_OPACITY = 0.5f;
		FX_SHOW_REFLEXIONS = TRUE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////// 
	case 1:					///1 Minimalist--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5; 
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.22f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = FALSE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xFF629AB6;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.55f;
		RING_BORDER_SIZE = 4.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.25f;
		LEDS_SIZE = 8.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 0;
		CURSOR_SIZE = 4.0f;
		CURSOR_LENGTH = 0.25f;
		CURSOR_LENGTH_B = 0.01f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.42f;
		CENTER_BORDER_SIZE = 0.20f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.05f;
		SHADOW_OPACITY = 0.5f;
		FX_SHOW_REFLEXIONS = TRUE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////// 
	case 2:					///2 Leds--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.22f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = FALSE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = FALSE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xFF629AB6;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.55f;
		RING_BORDER_SIZE = 4.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 24;
		LEDS_RADIUS = 0.80f;
		LEDS_SIZE = 6.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 0;
		CURSOR_SIZE = 4.0f;
		CURSOR_LENGTH = 0.25f;
		CURSOR_LENGTH_B = 0.01f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.42f;
		CENTER_BORDER_SIZE = 0.20f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.05f;
		SHADOW_OPACITY = 0.5f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////// 
	case 3:					///3 Leds Segments--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.22f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = TRUE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xFF629AB6;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.75f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 1;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.88f;
		LEDS_SIZE = 5.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 0;
		CURSOR_SIZE = 4.0f;
		CURSOR_LENGTH = 0.25f;
		CURSOR_LENGTH_B = 0.01f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.30f;
		CENTER_BORDER_SIZE = 3.0f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.05f;
		SHADOW_OPACITY = 0.5f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
	case 4:					///4 Round--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = TRUE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = FALSE;
		SHOW.CURSOR_ = TRUE;
		SHOW.CENTER = TRUE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.95f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 1;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.80f;
		LEDS_SIZE = 8.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 0;
		CURSOR_SIZE = 10.0f;
		CURSOR_LENGTH = 0.50f;
		CURSOR_LENGTH_B = 0.01f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.78f;
		CENTER_BORDER_SIZE = 3.44f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 1;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
	case 5:					///5 CursorLine--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = TRUE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = TRUE;
		SHOW.CENTER = TRUE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.82f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 64;
		LEDS_RADIUS = 0.90f;
		LEDS_SIZE = 8.0f;
		LEDS_GRADIENT = 0.50f;
		//------------------------
		CURSOR_TYPE = 1;
		CURSOR_SIZE = 7.25f;
		CURSOR_LENGTH = 0.84f;
		CURSOR_LENGTH_B = 0.03f;
		SHAPE_NB_POINTS = 3;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.79f;
		CENTER_BORDER_SIZE = 4.01f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 1;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = FALSE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
	case 6:					///6 CursorShape--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 0;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = TRUE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = TRUE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = TRUE;
		SHOW.CENTER = TRUE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.82f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 64;
		LEDS_RADIUS = 1.06f;
		LEDS_SIZE = 3.63f;
		LEDS_GRADIENT = -0.50f;
		//------------------------
		CURSOR_TYPE = 3;
		CURSOR_SIZE = 7.25f;
		CURSOR_LENGTH = 0.94f;
		CURSOR_LENGTH_B = 0.27f;
		SHAPE_NB_POINTS = 7;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.16f;
		CENTER_BORDER_SIZE = 3.25f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = TRUE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
		/////////////////////////////////////////////////////
	case 7:					///7 VBar_Leds--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 2;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = FALSE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = FALSE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.82f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.68f;
		LEDS_SIZE = 5.15f;
		LEDS_GRADIENT = 0.0f;
		//------------------------
		CURSOR_TYPE = 3;
		CURSOR_SIZE = 7.25f;
		CURSOR_LENGTH = 0.94f;
		CURSOR_LENGTH_B = 0.27f;
		SHAPE_NB_POINTS = 7;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.16f;
		CENTER_BORDER_SIZE = 3.25f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0;
		//------------------
		FX_SHOW_SHADOWS = TRUE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
	case 8:					///8 Color Panel--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 3;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = FALSE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = FALSE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.82f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.68f;
		LEDS_SIZE = 5.15f;
		LEDS_GRADIENT = 0.0;
		//------------------------
		CURSOR_TYPE = 3;
		CURSOR_SIZE = 7.25f;
		CURSOR_LENGTH = 0.94f;
		CURSOR_LENGTH_B = 0.27f;
		SHAPE_NB_POINTS = 7;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.16f;
		CENTER_BORDER_SIZE = 3.25f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0f;
		//------------------
		FX_SHOW_SHADOWS = TRUE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50;
		//----------------------------
		break;
		/////////////////////////////////////////////////////
	case 9:					///9 PanelFader--- SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f;
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------
		ENCODER_LAYOUT_TYPE = 4;
		ENCODER_LEDS_MODE = 0;
		RING_RADIUS = 0.20f;
		RING_LENGTH = 0.80f;
		//---------Show
		SHOW.RING = TRUE;
		SHOW.FILLED_RING = FALSE;
		SHOW.RING_BORDER = FALSE;
		SHOW.QUAD_RING = FALSE;
		SHOW.LEDS_RING = TRUE;
		SHOW.CURSOR_ = FALSE;
		SHOW.CENTER = FALSE;
		SHOW.MIDDLE_INDICATOR = FALSE;
		//------------Colors
		COLOR.Switch.IN = 0xFF433939;
		COLOR.Ring_Off.IN = 0xCC373737;
		COLOR.Ring_Off_Border.IN = 0xD4404040;
		COLOR.Ring_On.IN = 0x675AAFDC;
		COLOR.Ring_On_Border.IN = 0xAC64BBE0;
		COLOR.Leds_On.IN = 0xBE2BACD0;
		COLOR.Leds_Off.IN = 0xCC434343;
		COLOR.Cursor.IN = 0xCC8D9EA8;
		COLOR.Center_Cap.IN = 0xCC3C3C3C;
		COLOR.Center_Cap_Border.IN = 0xCC597485;
		COLOR.Panel.IN = 0xCC1DCADB;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5;
		//-------------------------
		//ring
		QUAD_INNER_RADIUS = 0.82f;
		RING_BORDER_SIZE = 2.0f;
		//----------
		BAR_ROUND_SIZE = 0.03f;
		BAR_IS_ROUND = TRUE;
		//--------------------
		LEDS_TYPE = 0;
		RING_LEDS = 32;
		LEDS_RADIUS = 0.68f;
		LEDS_SIZE = 5.15f;
		LEDS_GRADIENT = 0.0;
		//------------------------
		CURSOR_TYPE = 3;
		CURSOR_SIZE = 7.25f;
		CURSOR_LENGTH = 0.94f;
		CURSOR_LENGTH_B = 0.27f;
		SHAPE_NB_POINTS = 7;
		CURSOR_FLIP = FALSE;
		CURSOR_EXTRA_OFFSET = 0;
		CURSOR_LINKED_TO_RADIUS = TRUE;
		//------------------
		CENTER_A_RADIUS = 0.16f;
		CENTER_BORDER_SIZE = 3.25f;
		CENTER_DRAW_TYPE = 0;
		CENTER_NBP = 7;
		CENTER_LAYER_POSITION = 0;
		//------------------------
		PANEL_MIN_LUM = 0.1f;
		PANEL_MAX_LUM = 0.75f;
		PANEL_BORDER_SIZE = 6.0f;
		PANEL_IS_ROUND = TRUE;
		PANEL_ROUND_SIZE = 0.10f;
		ADD_PANEL_CURSOR_ = TRUE;
		PANEL_CURSOR_WIDTH = 4.0;
		//------------------
		FX_SHOW_SHADOWS = TRUE;
		SHADOW_X = -0.03f;
		SHADOW_Y = 0.03f;
		SHADOW_OPACITY = 0.3f;
		FX_SHOW_REFLEXIONS = FALSE;
		REFLEXIONS_OPACITY = 0.50f;
		//----------------------------
		break;
		/////////////////////////////////////////////////////

	default:
		break;
	}



}

void Encoder::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.Switch.IN = 0xFF433939;
	COLOR.Ring_Off.IN = 0xCC373737;
	COLOR.Ring_Off_Border.IN = 0xD4404040;
	COLOR.Ring_On.IN = 0x675AAFDC;
	COLOR.Ring_On_Border.IN = 0xAC64BBE0;
	COLOR.Leds_On.IN = 0xBE2BACD0;
	COLOR.Leds_Off.IN = 0xCC434343;
	COLOR.Cursor.IN = 0xFF629AB6;
	COLOR.Center_Cap.IN = 0xCC3C3C3C;
	COLOR.Center_Cap_Border.IN = 0xCC597485;
	COLOR.Panel.IN = 0xCC1DCADB;
	///////////////

	TFPColors->COLOR_CORRECT = TRUE;
	APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
	//TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum


	for (int i = 0; i < NB_ENCODER_COLORS; i++)
	{
		ENCODER_COLORS[i]->OnSetQuickColor();
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