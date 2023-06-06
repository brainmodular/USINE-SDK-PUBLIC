//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "ListBox.h"

ListBox::ListBox()
{

}

ListBox::ListBox(TFPStruct *TFParams_val, TFPaint *TFPaintP_val)
{
	TFParams = TFParams_val;
	TFPaintP = TFPaintP_val;
	TFPColors = new TFPColor();

	UPD_COLOR_ARRAY();

	for (int i = 0; i < NB_LB_COLORS; i++)
	{
		*LB_COLORS[i] = TFColor(TFParams, TFPColors);
	}


	SHOW.LIST_HEADER = TRUE;
	LIST_HEADER_H = 0.1f;
	LIST_HEADER.top = 0;
	LIST_HEADER.bottom = LIST_HEADER_H;
	LIST_HEADER.left = 0;
	LIST_HEADER.right = LIST_HEADER_W;
	LIST_CONTAINER.top = LIST_HEADER_H;
	LIST_SCROLLER_CONTAINER.top = LIST_CONTAINER.top;
	LIST_CONTAINER.bottom = 1.0f;
	LIST_SCROLLER_CONTAINER.bottom = LIST_CONTAINER.bottom;
	LIST_CONTAINER.left = 0;
	LIST_CONTAINER.right = 1 - LIST_SCROLLER_W;
	LIST_SCROLLER.left = 1 - LIST_SCROLLER_W;
	LIST_SCROLLER.right = 1.0;
	LIST_SCROLLER.top = LIST_CONTAINER.top;
	LIST_SCROLLER.bottom = LIST_SCROLLER.top + LIST_SCROLLER_H;
	LIST_SCROLLER_CONTAINER.left = LIST_SCROLLER.left;
	LIST_SCROLLER_CONTAINER.right = 1.0;
	LIST_SCROLL_VAL01 = 0;
	LIST_SCROLL_VAL01 = 0;
	LIST_SCROLL_VAL = 0;
	MAX_SCROLL = 0;
	INERTIA_MODE = FALSE;


	PRESET_LOAD(0);

	UPD_FIXED_LAYOUT();
	UPD_LAYOUT();////////////////////////////////


}

void ListBox::onMouseDown()
{
	TFParams->hilight = 1;
	LIST_SCROLL_ACTIVE = FALSE;
	SCROLL_CHANGED = FALSE;
	ZOOM_CHANGED = FALSE;
	SCROLL_ON_CLIC_VAL = LIST_SCROLL_VAL;
	ZOOM_ON_CLIC_VAL = LIST_ZOOM_01;
	ON_CLIC_SEL = LIST_ITEM_SEL;

	CheckCursorPos(); /// check in wich rects is mouse

	Y_MODE = FALSE;  // default X (xfader) and Y (scroll) are false, witing to check main move from TF
	X_MODE = FALSE;  //
	
	if ( (SHOW.LIST_SCROLLER && INSIDE.LIST_SCROLLER) || (SHOW.LIST_SCROLLER_CONTAINER && INSIDE.LIST_SCROLLER_CONTAINER) )  //mouse in scrollers
		{
			LISTBOX_ALLOW_SEL = FALSE;
			LIST_SCROLL_ACTIVE = TRUE;

			if (SHOW.LIST_SCROLLER_CONTAINER && INSIDE.LIST_SCROLLER_CONTAINER)	{UPD_SCROLL_CONTAINER();}

		}
		else  if (INSIDE.LIST_CONTAINER)
		{
			LIST_SCROLL_ACTIVE = FALSE;
			//LIST_MS_IN_ITEM_ZONE = TRUE; 	
			LISTBOX_ALLOW_SEL = TRUE;

			if (!INERTIA_MODE){ UPD_SELECTION(); }
		}

	UPD_COLORS();
	UPD_SRC_POINTERS();

}/////////////////////////////////////////////on msdwn

void ListBox::onInputChange()
{
//	LIST_ITEM_SEL = (int)TFParams->ITEM_VALUE_Y * (NB_LIST_ITEMS-1);

	LIST_ITEM_SEL = (int)TFParams->ITEM_VALUE_W;
	LIST_ITEM_SEL = min(LIST_ITEM_SEL, NB_LIST_ITEMS - 1);

//	sel_item_string = COMMA_LIST_LOCAL[LIST_ITEM_SEL];
	

}

void ListBox::onPreset()
{
	LIST_ITEM_SEL = (int)TFParams->ITEM_VALUE_W;
	LIST_ITEM_SEL = min(LIST_ITEM_SEL, NB_LIST_ITEMS - 1);

}

void ListBox::onMouseUp()
{
	TFParams->hilight = 0;
		LIST_SCROLL_ACTIVE = FALSE;
		INSIDE.LIST_ACTIVE_ITEM = FALSE;
		INSIDE.LIST_HEADER = FALSE;

		if (INERTIA_MODE)
		{
			if (!SCROLL_CHANGED && !ZOOM_CHANGED && INSIDE.LIST_ITEM_ZONE)
			{
				last_scroll_delta = 0;
				UPD_SELECTION();
			}
		 }
		else
		{
			INSIDE.LIST_ITEM_ZONE = FALSE;
		}
		UPD_COLORS();
		UPD_SRC_POINTERS();
}

void ListBox::onMouseMove()
{
	if (((int)LIST_SCROLL_VAL != SCROLL_ON_CLIC_VAL))
	{
		SCROLL_CHANGED = TRUE;
	}

	if (LIST_ZOOM_01 != ZOOM_ON_CLIC_VAL)
	{
		ZOOM_CHANGED = TRUE;
	}


	
	USED_DISPLAY_NB = min(NB_LIST_ITEMS, NB_LIST_DISPLAYED);

	RATIO_DY = ((float)USED_DISPLAY_NB / (float)NB_LIST_ITEMS) * (1.0f / LIST_CONTAINER_H); // for inertia scroll
	
	if (TFParams->MSDWN && INSIDE.LIST_CONTAINER) /////// once clicked were 0 and if move pass wich direction is main from tfmain
	{
		Y_MODE = Y_IS_MAIN;
		X_MODE = X_IS_MAIN;
	}
	/////// FILER XY MODES
	if (INERTIA_MODE)
	{
		if (TFParams->MSDWN && Y_IS_MAIN && !X_MODE)
		{
			LISTBOX_ALLOW_SEL = FALSE;			

				SCROLL_DELTA = (TFParams->DELTA_Y*RATIO_DY);

				if (LIST_SCROLL_ACTIVE)              // direction if in scroller reverse of if in list
				{
					last_scroll_delta = TFParams->DELTA_Y;
				}
				else { last_scroll_delta = -SCROLL_DELTA; }

				if (!INSIDE.LIST_SCROLLER) { UPD_SCROLL(SCROLL_DELTA); }
		}
	}
	//////////////////////////////////////////////////////
	if (LIST_USE_PAGES && LIST_SCROLLER_FIT_PAGE){ LIST_SCROLLER_H = LIST_PAGE_W*LIST_CONTAINER_H; } else { LIST_SCROLLER_H = LIST_SCROLLER_H_IN; }
	////////////////////////////////////////////////////////////
	if ( !LIST_SCROLL_ACTIVE && ((X_MODE && INSIDE.LIST_ACTIVE_ITEM) || INSIDE.LIST_HEADER)) { UPD_XFADER(); }        // X fader
	///////////////////////////////////////////////////////////
	if (LIST_USE_X_ZOOM && LIST_SCROLL_ACTIVE && X_IS_MAIN) { UPD_ZOOM(); }  // zoom
	///////////////////////////////////////////////////////////////////
	if (LIST_SCROLL_ACTIVE && !LIST_USE_PAGES) { UPD_SCROLL(-TFParams->DELTA_Y*(1.0f / LIST_CONTAINER_H)); } // slider scroll
	////////////////////////////////////////
	// SELECT ITEM if slide mode
	if (LIST_ALLOW_SLIDE && !INERTIA_MODE && TFParams->MSDWN && INSIDE.LIST_ITEM_ZONE ){ UPD_SELECTION(); }
	///////////////////////////////////////////////////////////////////////


	//UPD_SRC_POINTERS();
}////////////// ms move

LongBool ListBox::getLongBool(string name)
{
	if (name == "INERTIA_MODE") { return INERTIA_MODE; }
	return FALSE;
}

void ListBox::setParam(string name, bool value)
{
	if (name == "X_IS_MAIN") { X_IS_MAIN = value; }
	else if (name == "Y_IS_MAIN") { Y_IS_MAIN = value; }
}

void ListBox::UPD_SCROLL_CONTAINER()
{
	if (IN_01_RANGE(TFParams->MSY) && LIST_SCROLL_ACTIVE)
	{
		
		if (SHOW.LIST_SCROLLER_CONTAINER && INSIDE.LIST_SCROLLER_CONTAINER && !INSIDE.LIST_SCROLLER) // in container scroll
		{

			if (LIST_USE_PAGES && LIST_SCROLLER_FIT_PAGE) { LIST_SCROLLER_H = LIST_PAGE_W*LIST_CONTAINER_H; } /////////////added
			else { LIST_SCROLLER_H = LIST_SCROLLER_H_IN; }

			Y_SCL = RE_RANGE_NORM_OUT(TFParams->MSY, LIST_HEADER_H, 1.0);
			if (!LIST_USE_PAGES) { LIST_SCROLL_VAL01 = Y_SCL; }
			else //pages mode
			{
				//float LIST_NB_OF_PAGES = 0;
				LIST_NB_OF_PAGES = ((float)NB_LIST_ITEMS / (float)USED_DISPLAY_NB);
				LAST_PAGE = (int)trunc(LIST_NB_OF_PAGES);
				LIST_PAGE_W = 1.0f / LIST_NB_OF_PAGES;
				LIST_PAGE_W_MIN = 1.0f - (((LAST_PAGE * LIST_PAGE_W)*LIST_CONTAINER_H) + LIST_HEADER_H); ///size for last page		
				int page_num = 0;
				float temp_01 = 0;

				float max = 0;
				max = 1 + 1.0f / ((float)LIST_NB_OF_PAGES - 1);

				page_num = (int)(Y_SCL * LIST_NB_OF_PAGES);

				LIST_PAGE_STRING = "P";
				LIST_PAGE_STRING.append(sdkIntToString(page_num));

				temp_01 = (float)(USED_DISPLAY_NB  * page_num) / (float)NB_LIST_ITEMS;;

				LIST_SCROLL_VAL01 = RE_RANGE_NORM_IN(temp_01, 0, max);


				if (LIST_USE_PAGES && LIST_SCROLLER_FIT_PAGE)      ///extra rescale last page scroller
				{
					if ((int)page_num == (int)LIST_NB_OF_PAGES)
					 { LIST_SCROLLER_H = LIST_PAGE_W_MIN*LIST_CONTAINER_H; } /////////////added
				}

			}
			//end page
		}
		UPD_SCROLL(0);
		//////////////			
	} //Y in range

}/// scroll container
///////////////////////////
void ListBox::UPD_SCROLL(float DELTA_IN)
		{
			USED_DISPLAY_NB = min(NB_LIST_ITEMS, NB_LIST_DISPLAYED);
			LIST_SCROLL_VAL01 = LIST_SCROLL_VAL01 - (DELTA_IN);
			LIST_SCROLL_VAL01 = CLAMP01(LIST_SCROLL_VAL01);

			MAX_SCROLL = (float)(NB_LIST_ITEMS - USED_DISPLAY_NB);
			LIST_SCROLL_VAL = (LIST_SCROLL_VAL01*MAX_SCROLL);

			LIST_SCROLL_RESCL = LIST_SCROLL_VAL01*(1 - LIST_SCROLLER_H) + LIST_SCROLLER_H / 2.0f;
			LIST_SCROLL_RESCL *= LIST_CONTAINER_H;
			LIST_SCROLL_RESCL += LIST_HEADER_H;
			LIST_SCROLLER.top = (LIST_SCROLL_RESCL - LIST_SCROLLER_H / 2.0f);
			LIST_SCROLLER.bottom = LIST_SCROLLER.top + LIST_SCROLLER_H;
		}
//////////////////////////////////////////////////////////////////////
void ListBox::UPD_SELECTION()
{
		Y_SCL = RE_RANGE_NORM_OUT(TFParams->MSY, LIST_HEADER_H, 1.0);
		LIST_ITEM_SEL = (int)((Y_SCL*USED_DISPLAY_NB) + LIST_SCROLL_VAL);
		LIST_ITEM_SEL = (int)CLAMP((float)LIST_ITEM_SEL, 0, (float)(NB_LIST_ITEMS - 1));////////////////////////////
	//	LISTBOX_SETTING_ITEM = TRUE;	

		if (SHOW.LIST_PULSE && LIST_ITEM_SEL != ON_CLIC_SEL)
		{
			TFParams->PULSE = TRUE;
			TFParams->PULSE_count = 0;
		}

		//TFParams->ITEM_VALUE_Y = (float)LIST_ITEM_SEL / (float)(NB_LIST_ITEMS-1); // just for midi
}
//////////////////////////////////////////
void ListBox::RESET_INERTIA()
{
	last_scroll_delta = 0;
	count_inertia = -1;
}
///////////////////////////////////////////
void ListBox::UPD_XFADER()
{
	LIST_ITEM_XFADER_VAL += TFParams->DELTA_X;
	LIST_ITEM_XFADER_VAL = CLAMP01(LIST_ITEM_XFADER_VAL);
	TFParams->ITEM_VALUE_X = LIST_ITEM_XFADER_VAL;
}
/////////////////////////////////////////
void ListBox::UPD_ZOOM()
{
	NB_LIST_DISPLAYED_ += TFParams->DELTA_X * NB_LIST_ITEMS * LIST_X_ZOOM_SPEED;
	NB_LIST_DISPLAYED_ = min((float)NB_LIST_ITEMS, max(1.0f, (float)NB_LIST_DISPLAYED_));
	NB_LIST_DISPLAYED = (int)NB_LIST_DISPLAYED_;
	LIST_ZOOM_01 = RE_RANGE_NORM_OUT((float)NB_LIST_DISPLAYED, 1, (float)NB_LIST_ITEMS);
}
////////////////////////////////////////////////////////////////////////
void ListBox::UPD_PAINT()
{}
/////////////////////////////////////////////////
void ListBox::onSettingsHasChanged()
{

	
	//update layout
	UPD_FIXED_LAYOUT();

	COLOR.Global.update_in(0, true, false, 0.2f);
	COLOR.Zoom_Line.update_in(1, (bool)SHOW.LIST_ZOOM, false, 0.2f);
	COLOR.Border.update_in(2, (bool)SHOW.LIST_BORDER, false, 0.1f);
	COLOR.Header.update_in(3, (bool)SHOW.LIST_HEADER, false, 0);
	COLOR.Header_Border.update_in(4, (bool)SHOW.LIST_HEADER_BORDER, FALSE, 0);
	COLOR.KH.update_in(5, (bool)SHOW.LIST_KH, FALSE, 0);
	COLOR.Scroller.update_in(6, (bool)SHOW.LIST_SCROLLER, FALSE, 0.4f);
	COLOR.Scroller_Border.update_in(7, (bool)SHOW.LIST_SCROLLER_BORDER, FALSE, 0.4f);
	COLOR.Scroller_Shadow.update_in(8, (bool)SHOW.LIST_SCROLLER, FALSE, 0.1f);
	COLOR.Scroller_Container.update_in(9, (bool)SHOW.LIST_SCROLLER_CONTAINER, FALSE, 0.1f);
	COLOR.Scroller_Container_Border.update_in(10, (bool)SHOW.LIST_SCROLLER_CONTAINER, FALSE, 0.1f);
	COLOR.Item_Border_Off.update_in(11, (bool)SHOW.LIST_ITEM_BORDER, false, 0);
	COLOR.Item_Border_On.update_in(12, (bool)SHOW.LIST_ITEM_BORDER, FALSE, 0);
	COLOR.Item_Off.update_in(13, true, false, 0);
	COLOR.Item_On.update_in(14, true, false, 0);
	COLOR.Xfader.update_in(15, (bool)SHOW.LIST_ITEM_XFADER, false, 0);
	COLOR.Xfader_Border.update_in(16, (bool)SHOW.LIST_ITEM_XFADER, false, 0);
	COLOR.Xfader_Value.update_in(17, (bool)SHOW.LIST_ITEM_XFADER_VALUE, false, 0);
	COLOR.Item_Name.update_in(18, (bool)SHOW.LIST_ITEMS_NAMES, false, 0);

	UPD_COLOR_ARRAY();

	////////////////////////////////////////////////////////////////////////
	if (APPLY_COLOR_CORRECT)     // if clicing on apply color correct, it will make change to all colors set 
	{
		for (int i = 0; i < NB_LB_COLORS; i++)
		{
			LB_COLORS[i]->update_out();
			LB_COLORS[i]->IN = LB_COLORS[i]->OUT;
			LB_COLORS[i]->IN_HSLA = LB_COLORS[i]->OUT_HSLA;
		}

		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		APPLY_COLOR_CORRECT = FALSE;            // and swich back off color correct till a new press
	}

	//////////////////////////////
	UPD_COLORS();

}

void ListBox::CheckCursorPos()
{
	INSIDE.LIST_HEADER			   = sdkPointInRect(TFParams->MSX, TFParams->MSY, LIST_HEADER);
	INSIDE.LIST_SCROLLER		   = sdkPointInRect(TFParams->MSX, TFParams->MSY, LIST_SCROLLER);
	INSIDE.LIST_SCROLLER_CONTAINER = sdkPointInRect(TFParams->MSX, TFParams->MSY, LIST_SCROLLER_CONTAINER);
	INSIDE.LIST_CONTAINER		   = sdkPointInRect(TFParams->MSX, TFParams->MSY, LIST_CONTAINER);

	INSIDE.LIST_ITEM_ZONE		   = INSIDE.LIST_CONTAINER && !INSIDE.LIST_SCROLLER && !INSIDE.LIST_SCROLLER_CONTAINER;

	INSIDE.LIST_ACTIVE_ITEM		   = sdkPointInRect(TFParams->MSX, TFParams->MSY, LIST_ACTIVE_ITEM_RECT);

}

void ListBox::UPD_FIXED_LAYOUT()
{

	INERTIA_DURATION = (int)(INERTIA_DURATION_IN * INERTIA_DURATION_RATIO);

	//////// params that only need update if tweaking settings, then set once, no need when processing

	if (SHOW.LIST_HEADER){LIST_HEADER_H = LIST_HEADER_H_IN;}else{LIST_HEADER_H = 0;}

	//LIST_HEADER_H = LIST_HEADER_H_IN;
	LIST_HEADER.top = 0;
	LIST_HEADER.bottom = LIST_HEADER_H;
	LIST_HEADER.bottom -= (LIST_HEADER_H*0.2f);
	LIST_HEADER.left = 0;
	LIST_HEADER.right = LIST_HEADER_W;
	LIST_CONTAINER.top = LIST_HEADER_H;
	LIST_CONTAINER.bottom = 1;
	LIST_CONTAINER.left = 0;
	LIST_CONTAINER.right = 1;

	if (SHOW.LIST_SCROLLER_CONTAINER) { LIST_CONTAINER.right = 1 - LIST_SCROLLER_W; }

	LIST_CONTAINER_H = LIST_CONTAINER.bottom - LIST_CONTAINER.top;
	LIST_SCROLLER_CONTAINER.left = LIST_CONTAINER.right + 0.025f;  //margin
	LIST_SCROLLER_CONTAINER.right = 1.0f;
	LIST_SCROLLER_CONTAINER.top = LIST_CONTAINER.top;
	LIST_SCROLLER_CONTAINER.bottom = 1;

	LIST_ITEM_RECT.left = LIST_CONTAINER.left + LIST_ITEM_H_GAP;
	LIST_ITEM_RECT.right = LIST_CONTAINER.right - LIST_ITEM_H_GAP;

	LIST_SCROLLER.left = 1 - LIST_SCROLLER_W;
	LIST_SCROLLER.right = 1.0;

	if (SHOW.LIST_KH)
	{
		LIST_KH_UP.left = -1.0; LIST_KH_UP.right = 2.0; LIST_KH_UP.top = -1.0; LIST_KH_UP.bottom = LIST_CONTAINER.top;
		LIST_KH_DWN.left = -1.0f; LIST_KH_DWN.right = 2.0f; LIST_KH_DWN.top = 1.0f; LIST_KH_DWN.bottom = 2.0f;
	}

}

void ListBox::UPD_LAYOUT()
{

	USED_DISPLAY_NB = min(NB_LIST_ITEMS, NB_LIST_DISPLAYED);

	SHOW.LIST_ITEMS_NUMBERS = SHOW.ITEMS_NUMBERS_TYPE > 0; //if show numbers type >0 one then display numbers, 1 = visual, 2= original id if was sorted


	LIST_NB_OF_PAGES = ((float)NB_LIST_ITEMS / (float)USED_DISPLAY_NB);
	LAST_PAGE = (int)trunc(LIST_NB_OF_PAGES);
	LIST_PAGE_W = 1.0f / LIST_NB_OF_PAGES;
	LIST_PAGE_W_MIN = 1.0f - (((LAST_PAGE * LIST_PAGE_W)*LIST_CONTAINER_H) + LIST_HEADER_H); ///size for last page	

	LIST_ITEM_H = 1.0f / (float)(USED_DISPLAY_NB);
	LIST_ITEM_H *= LIST_CONTAINER_H;
	LIST_ITEM_H = LIST_ITEM_H - (LIST_ITEM_GAP*LIST_ITEM_H);

	LIST_SCROLLER.bottom = LIST_SCROLLER.top + LIST_SCROLLER_H;

	zoom_start = LIST_SCROLLER.top + LIST_SCROLLER_H / 2.0f;
	zoom_width = ((LIST_ZOOM_01*LIST_CONTAINER_H) / 2.0f);
	zoom_up = fmaxf(LIST_HEADER_H, zoom_start - zoom_width);
	zoom_dwn = fminf(1, zoom_start + zoom_width);

	SCROLL_RATIO = (float)(NB_LIST_ITEMS - USED_DISPLAY_NB) / (float)USED_DISPLAY_NB;
	SCROLL_RATIO *= LIST_CONTAINER_H;
}

void ListBox::UPD_SRC_POINTERS()
{
	//matrix to toute src to tgt temp disabled
/*
	switch (SRC_ID)
	{

	case 0: TGT = LIST_SCROLL_VAL01; break;
	case 1: TGT = LIST_ZOOM_01; break;
	case 2: TGT = LIST_ITEM_SEL; break;

	default:
		break;
	}
	*/

}

void ListBox::UPD_COLOR_ARRAY()
{	///////////////////////////////////// create colors pointer array for easier manips

	LB_COLORS[0] = &COLOR.Global;  //0
	LB_COLORS[1] = &COLOR.Zoom_Line; //1
	LB_COLORS[2] = &COLOR.Border; //2
	LB_COLORS[3] = &COLOR.Header; //3
	LB_COLORS[4] = &COLOR.Header_Border; //4
	LB_COLORS[5] = &COLOR.KH; //5
	LB_COLORS[6] = &COLOR.Scroller; //6
	LB_COLORS[7] = &COLOR.Scroller_Border; //7
	LB_COLORS[8] = &COLOR.Scroller_Shadow; //8
	LB_COLORS[9] = &COLOR.Scroller_Container; //9
	LB_COLORS[10] = &COLOR.Scroller_Container_Border; //10
	LB_COLORS[11] = &COLOR.Item_Border_Off; //11
	LB_COLORS[12] = &COLOR.Item_Border_On; //12
	LB_COLORS[13] = &COLOR.Item_Off; //13
	LB_COLORS[14] = &COLOR.Item_On; //14
	LB_COLORS[15] = &COLOR.Xfader; //15
	LB_COLORS[16] = &COLOR.Xfader_Border; //16
	LB_COLORS[17] = &COLOR.Xfader_Value; //17
	LB_COLORS[18] = &COLOR.Item_Name; //18
	///////////////////////////////////////
}///end upd_color_array

void ListBox::UPD_COLORS()
{


		COLOR.Scroller.HILIGHT_MULT = LIST_SCROLL_ACTIVE *0.5f;
		COLOR.Scroller_Border.HILIGHT_MULT = LIST_SCROLL_ACTIVE *0.5f;

	for (int i = 0; i < NB_LB_COLORS; i++) { LB_COLORS[i]->update_out(); }
}

void ListBox::PRESET_LOAD(int preset)

{
	

	switch (preset)
	{
	case 0:  /////////////////////////// 0-DFT SKIN

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
		//---------------------------------
		LIST_USE_X_ZOOM = TRUE;
		NB_LIST_DISPLAYED_ = 8;
		NB_LIST_DISPLAYED = 12;
		LIST_ZOOM_LINE_WIDTH = 5.0;
		//---------------------------
		SHOW.LIST_BORDER = TRUE;
		SHOW.LIST_HEADER = TRUE;
		SHOW.LIST_HEADER_BORDER = TRUE;
		SHOW.LIST_SCROLLER = TRUE;
		SHOW.LIST_SCROLLER_BORDER = TRUE;
		SHOW.LIST_SCROLLER_VALUE = TRUE;
		SHOW.LIST_SCROLLER_CONTAINER = FALSE;
		SHOW.LIST_ZOOM = TRUE;
		SHOW.LIST_ITEM_BORDER = TRUE;
		SHOW.LIST_ITEMS_NAMES = TRUE;
		SHOW.ITEMS_NUMBERS_TYPE = 0;
		SHOW.ITEMS_ASSIGNED_VALUE = 0;
		SHOW.LIST_PULSE = TRUE;
		SHOW.LIST_KH = TRUE;
		SHOW.LIST_ITEM_XFADER = TRUE;
		SHOW.LIST_ITEM_XFADER_VALUE = TRUE;
		//---------------------------
		COLOR.Zoom_Line.IN = 0x7A5A82A2;
		COLOR.Xfader.IN = 0x7A5A82A2;
		COLOR.Xfader_Border.IN = 0x7A5A82A2;
		COLOR.Xfader_Value.IN = 0x7AFFFFFF;
		COLOR.Item_Off.IN = 0x7A323F42;
		COLOR.Item_On.IN = 0x666D8699;
		COLOR.Item_Border_Off.IN = 0x891F1F1F;
		COLOR.Item_Border_On.IN = 0x7A8EB0CC;
		COLOR.Item_Name.IN = 0x7AFFFFFF;
		COLOR.Border.IN = 0x7A5A82A2;
		COLOR.Scroller.IN = 0x7A5A82A2;
		COLOR.Scroller_Border.IN = 0x7A5A82A2;
		COLOR.Scroller_Shadow.IN = 0x23000000;
		COLOR.Scroller_Container.IN = 4281282351;
		COLOR.Scroller_Container_Border.IN = 0xFF4B4747;
		COLOR.Header.IN = 0x7A323F42;
		COLOR.Header_Border.IN = 0x7A5A82A2;
		COLOR.KH.IN = 4281282351;
		//--------------------------------
		LIST_LUM_MOD = 0.25f;
		LIST_ITEM_IS_ROUND = TRUE;
		LIST_ITEM_ROUND_SIZE = 0.01f;
		LIST_ITEM_BORDER_SIZE = 2.0f;
		LIST_ITEM_NAME_SIZE = 0.03f;
		LIST_ITEM_GAP = 0.05f;
		LIST_ITEM_H_GAP = 0.05f;
		LIST_PULSE_AMOUNT = 0.5f;
		//-----------------------------
		LIST_BORDER_SIZE = 4.0f;
		LIST_BORDER_IS_ROUND = TRUE;
		LIST_BG_ROUND_SIZE = 0.03f;
		//---------------------------
		LIST_SCROLLER_W = 0.35f;
		LIST_SCROLLER_H_IN = 0.1f;
		LIST_SCROLLER_BORDER_SIZE = 4.0f;
		LIST_SCROLLER_IS_ROUND = TRUE;
		LIST_SCROLLER_ROUND_SIZE = 0.01f;
		//-----------------------------
		LIST_USE_PAGES = FALSE;
		LIST_SCROLLER_FIT_PAGE = TRUE;
		//-----------------------------
		LIST_HEADER_NAME_SIZE = 0.03f;
		LIST_HEADER_W = 1.0f;
		LIST_HEADER_H_IN = 0.10f;
		LIST_HEADER_BORDER_SIZE = 2.0f;
		LIST_HEADER_IS_ROUND = TRUE;
		LIST_HEADER_ROUND_SIZE = 0.01f;
		//----------------
		break;
		////////////////////////////////////////////////////
	case 1:  /////////////////////////// 0-Minimalist SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f; //bypass as swich will hilight itself
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------------------
		LIST_USE_X_ZOOM = TRUE;
		NB_LIST_DISPLAYED_ = 8;
		NB_LIST_DISPLAYED = 12;
		LIST_ZOOM_LINE_WIDTH = 5.0f;
		//---------------------------
		SHOW.LIST_BORDER = FALSE;
		SHOW.LIST_HEADER = TRUE;
		SHOW.LIST_HEADER_BORDER = FALSE;
		SHOW.LIST_SCROLLER = TRUE;
		SHOW.LIST_SCROLLER_BORDER = FALSE;
		SHOW.LIST_SCROLLER_VALUE = TRUE;
		SHOW.LIST_SCROLLER_CONTAINER = FALSE;
		SHOW.LIST_ZOOM = TRUE;
		SHOW.LIST_ITEM_BORDER = FALSE;
		SHOW.LIST_ITEMS_NAMES = TRUE;
		SHOW.ITEMS_NUMBERS_TYPE = 0;
		SHOW.ITEMS_ASSIGNED_VALUE = 0;
		SHOW.LIST_PULSE = FALSE;
		SHOW.LIST_KH = TRUE;
		SHOW.LIST_ITEM_XFADER = FALSE;
		SHOW.LIST_ITEM_XFADER_VALUE = TRUE;
		//---------------------------
		COLOR.Zoom_Line.IN = 0x7A5A82A2;
		COLOR.Xfader.IN = 0x7A5A82A2;
		COLOR.Xfader_Border.IN = 0x7A5A82A2;
		COLOR.Xfader_Value.IN = 0x7AFFFFFF;
		COLOR.Item_Off.IN = 0x7A323F42;
		COLOR.Item_On.IN = 0x666D8699;
		COLOR.Item_Border_Off.IN = 0x891F1F1F;
		COLOR.Item_Border_On.IN = 0x7A8EB0CC;
		COLOR.Item_Name.IN = 0x7AFFFFFF;
		COLOR.Border.IN = 0x7A5A82A2;
		COLOR.Scroller.IN = 0x7A5A82A2;
		COLOR.Scroller_Border.IN = 0x7A5A82A2;
		COLOR.Scroller_Shadow.IN = 0x23000000;
		COLOR.Scroller_Container.IN = 4281282351;
		COLOR.Scroller_Container_Border.IN = 0xFF4B4747;
		COLOR.Header.IN = 0x7A323F42;
		COLOR.Header_Border.IN = 0x7A5A82A2;
		COLOR.KH.IN = 4281282351;
		//--------------------------------
		LIST_LUM_MOD = 0;
		LIST_ITEM_IS_ROUND = FALSE;
		LIST_ITEM_ROUND_SIZE = 0.01f;
		LIST_ITEM_BORDER_SIZE = 2.0f;
		LIST_ITEM_NAME_SIZE = 0.03f;
		LIST_ITEM_GAP = 0.05f;
		LIST_ITEM_H_GAP = 0.05f;
		LIST_PULSE_AMOUNT = 0.5f;
		//-----------------------------
		LIST_BORDER_SIZE = 4.0f;
		LIST_BORDER_IS_ROUND = FALSE;
		LIST_BG_ROUND_SIZE = 0.03f;
		//---------------------------
		LIST_SCROLLER_W = 0.35f;
		LIST_SCROLLER_H_IN = 0.1f;
		LIST_SCROLLER_BORDER_SIZE = 4.0f;
		LIST_SCROLLER_IS_ROUND = FALSE;
		LIST_SCROLLER_ROUND_SIZE = 0.01f;
		//-----------------------------
		LIST_USE_PAGES = FALSE;
		LIST_SCROLLER_FIT_PAGE = TRUE;
		//-----------------------------
		LIST_HEADER_NAME_SIZE = 0.03f;
		LIST_HEADER_W = 1.0f;
		LIST_HEADER_H_IN = 0.10f;
		LIST_HEADER_BORDER_SIZE = 2.0f;
		LIST_HEADER_IS_ROUND = FALSE;
		LIST_HEADER_ROUND_SIZE = 0.01f;
		//----------------
		break;
		////////////////////////////////////////////////////
	case 2:  /////////////////////////// 2-PAGES SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f; //bypass as swich will hilight itself
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------------------
		LIST_USE_X_ZOOM = FALSE;
		NB_LIST_DISPLAYED_ = 8;
		NB_LIST_DISPLAYED = 12;
		LIST_ZOOM_LINE_WIDTH = 5.0f;
		//---------------------------
		SHOW.LIST_BORDER = TRUE;
		SHOW.LIST_HEADER = TRUE;
		SHOW.LIST_HEADER_BORDER = TRUE;
		SHOW.LIST_SCROLLER = TRUE;
		SHOW.LIST_SCROLLER_BORDER = TRUE;
		SHOW.LIST_SCROLLER_VALUE = TRUE;
		SHOW.LIST_SCROLLER_CONTAINER = TRUE;
		SHOW.LIST_ZOOM = TRUE;
		SHOW.LIST_ITEM_BORDER = TRUE;
		SHOW.LIST_ITEMS_NAMES = TRUE;
		SHOW.ITEMS_NUMBERS_TYPE = 0;
		SHOW.ITEMS_ASSIGNED_VALUE = 0;
		SHOW.LIST_PULSE = TRUE;
		SHOW.LIST_KH = TRUE;
		SHOW.LIST_ITEM_XFADER = TRUE;
		SHOW.LIST_ITEM_XFADER_VALUE = TRUE;
		//---------------------------
		COLOR.Zoom_Line.IN = 0x7A5A82A2;
		COLOR.Xfader.IN = 0x7A5A82A2;
		COLOR.Xfader_Border.IN = 0x7A5A82A2;
		COLOR.Xfader_Value.IN = 0x7AFFFFFF;
		COLOR.Item_Off.IN = 0x7A323F42;
		COLOR.Item_On.IN = 0x666D8699;
		COLOR.Item_Border_Off.IN = 0x891F1F1F;
		COLOR.Item_Border_On.IN = 0x7A8EB0CC;
		COLOR.Item_Name.IN = 0x7AFFFFFF;
		COLOR.Border.IN = 0x7A5A82A2;
		COLOR.Scroller.IN = 0x7A5A82A2;
		COLOR.Scroller_Border.IN = 0x7A5A82A2;
		COLOR.Scroller_Shadow.IN = 0x23000000;
		COLOR.Scroller_Container.IN = 4281282351;
		COLOR.Scroller_Container_Border.IN = 0xFF4B4747;
		COLOR.Header.IN = 0x7A323F42;
		COLOR.Header_Border.IN = 0x7A5A82A2;
		COLOR.KH.IN = 4281282351;
		//--------------------------------
		LIST_LUM_MOD = 0.25f;
		LIST_ITEM_IS_ROUND = TRUE;
		LIST_ITEM_ROUND_SIZE = 0.01f;
		LIST_ITEM_BORDER_SIZE = 2.0f;
		LIST_ITEM_NAME_SIZE = 0.03f;
		LIST_ITEM_GAP = 0.05f;
		LIST_ITEM_H_GAP = 0.05f;
		LIST_PULSE_AMOUNT = 0.5f;
		//-----------------------------
		LIST_BORDER_SIZE = 4.0f;
		LIST_BORDER_IS_ROUND = TRUE;
		LIST_BG_ROUND_SIZE = 0.03f;
		//---------------------------
		LIST_SCROLLER_W = 0.35f;
		LIST_SCROLLER_H_IN = 0.1f;
		LIST_SCROLLER_BORDER_SIZE = 4.0f;
		LIST_SCROLLER_IS_ROUND = TRUE;
		LIST_SCROLLER_ROUND_SIZE = 0.01f;
		//-----------------------------
		LIST_USE_PAGES = TRUE;
		LIST_SCROLLER_FIT_PAGE = TRUE;
		//-----------------------------
		LIST_HEADER_NAME_SIZE = 0.03f;
		LIST_HEADER_W = 1.0f;
		LIST_HEADER_H_IN = 0.10f;
		LIST_HEADER_BORDER_SIZE = 2.0f;
		LIST_HEADER_IS_ROUND = TRUE;
		LIST_HEADER_ROUND_SIZE = 0.01f;
		//----------------
		break;
		////////////////////////////////////////////////////
	case 3:  /////////////////////////// 3-BLACK SKIN

		COLOR.Global.HUE_MODE = 2;
		TFPColors->HUE_BEAVIOUR = COLOR.Global.HUE_MODE;
		TFPColors->COLOR_CORRECT = TRUE;
		TFPColors->BLEND_MODE = BM_Mix;
		TFPColors->BLEND_MIX = 0.75f;
		TFPColors->USE_TARGET_COLOR = FALSE;
		TFPColors->LUMINOSITY_HIGLIGHT_FORCE = 0.5f; //bypass as swich will hilight itself
		TFPColors->HUE_ROLL = 0;
		TFPColors->LUMINOSITY_CONTRAST = 0;     // then once colors have been modified reset settings to 0
		TFPColors->LUMINOSITY_OFFSET = 0;
		TFPColors->SATURATION_OVERIDE = 0;
		//---------------------------------
		LIST_USE_X_ZOOM = FALSE;
		NB_LIST_DISPLAYED_ = 8;
		NB_LIST_DISPLAYED = 12;
		LIST_ZOOM_LINE_WIDTH = 5.0f;
		//---------------------------
		SHOW.LIST_BORDER = TRUE;
		SHOW.LIST_HEADER = TRUE;
		SHOW.LIST_HEADER_BORDER = TRUE;
		SHOW.LIST_SCROLLER = TRUE;
		SHOW.LIST_SCROLLER_BORDER = TRUE;
		SHOW.LIST_SCROLLER_VALUE = TRUE;
		SHOW.LIST_SCROLLER_CONTAINER = FALSE;
		SHOW.LIST_ZOOM = TRUE;
		SHOW.LIST_ITEM_BORDER = TRUE;
		SHOW.LIST_ITEMS_NAMES = TRUE;
		SHOW.ITEMS_NUMBERS_TYPE = 0;
		SHOW.ITEMS_ASSIGNED_VALUE = 0;
		SHOW.LIST_PULSE = TRUE;
		SHOW.LIST_KH = TRUE;
		SHOW.LIST_ITEM_XFADER = TRUE;
		SHOW.LIST_ITEM_XFADER_VALUE = TRUE;
		//---------------------------
		COLOR.Zoom_Line.IN = 0x7A5A82A2;
		COLOR.Xfader.IN = 0x7A5A82A2;
		COLOR.Xfader_Border.IN = 0x7A5A82A2;
		COLOR.Xfader_Value.IN = 0x7AFFFFFF;
		COLOR.Item_Off.IN = 0xFF000000;
		COLOR.Item_On.IN = 0x666D8699;
		COLOR.Item_Border_Off.IN = 0xFF2F383E;
		COLOR.Item_Border_On.IN = 0x7A8EB0CC;
		COLOR.Item_Name.IN = 0x7AFFFFFF;
		COLOR.Border.IN = 0x7A5A82A2;
		COLOR.Scroller.IN = 0x7A5A82A2;
		COLOR.Scroller_Border.IN = 0x7A5A82A2;
		COLOR.Scroller_Shadow.IN = 0x23000000;
		COLOR.Scroller_Container.IN = 4281282351;
		COLOR.Scroller_Container_Border.IN = 0xFF4B4747;
		COLOR.Header.IN = 0x7A323F42;
		COLOR.Header_Border.IN = 0x7A5A82A2;
		COLOR.KH.IN = 0xFF000000;
		//--------------------------------
		LIST_LUM_MOD = 0;
		LIST_ITEM_IS_ROUND = FALSE;
		LIST_ITEM_ROUND_SIZE = 0.01f;
		LIST_ITEM_BORDER_SIZE = 2.0f;
		LIST_ITEM_NAME_SIZE = 0.03f;
		LIST_ITEM_GAP = 0.05f;
		LIST_ITEM_H_GAP = 0.05f;
		LIST_PULSE_AMOUNT = 0.5f;
		//-----------------------------
		LIST_BORDER_SIZE = 4.0f;
		LIST_BORDER_IS_ROUND = FALSE;
		LIST_BG_ROUND_SIZE = 0.03f;
		//---------------------------
		LIST_SCROLLER_W = 0.35f;
		LIST_SCROLLER_H_IN = 0.1f;
		LIST_SCROLLER_BORDER_SIZE = 4.0f;
		LIST_SCROLLER_IS_ROUND = TRUE;
		LIST_SCROLLER_ROUND_SIZE = 0.01f;
		//-----------------------------
		LIST_USE_PAGES = FALSE;
		LIST_SCROLLER_FIT_PAGE = FALSE;
		//-----------------------------
		LIST_HEADER_NAME_SIZE = 0.03f;
		LIST_HEADER_W = 1.0f;
		LIST_HEADER_H_IN = 0.10f;
		LIST_HEADER_BORDER_SIZE = 2.0f;
		LIST_HEADER_IS_ROUND = FALSE;
		LIST_HEADER_ROUND_SIZE = 0.01f;
		//----------------
		break;
		////////////////////////////////////////////////////

	default:
		break;
	}



}//////////////////////

void ListBox::onSetQuickColor()
{
	TColorAhsl	HSLA_TEMP_COLOR;  //concerts Quick color to HSLA
	HSLA_TEMP_COLOR = sdkAlphaColorToColorAhsl(*TFPaintP->QUICK_COLOR);

	// first equivalent to laod skin 0 base colors
	COLOR.Zoom_Line.IN = 0x7A5A82A2;
	COLOR.Xfader.IN = 0x7A5A82A2;
	COLOR.Xfader_Border.IN = 0x7A5A82A2;
	COLOR.Xfader_Value.IN = 0x7AFFFFFF;
	COLOR.Item_Off.IN = 0x7A323F42;
	COLOR.Item_On.IN = 0x666D8699;
	COLOR.Item_Border_Off.IN = 0x891F1F1F;
	COLOR.Item_Border_On.IN = 0x7A8EB0CC;
	COLOR.Item_Name.IN = 0x7AFFFFFF;
	COLOR.Border.IN = 0x7A5A82A2;
	COLOR.Scroller.IN = 0x7A5A82A2;
	COLOR.Scroller_Border.IN = 0x7A5A82A2;
	COLOR.Scroller_Shadow.IN = 0x23000000;
	COLOR.Scroller_Container.IN = 4281282351;
	COLOR.Scroller_Container_Border.IN = 0xFF4B4747;
	COLOR.Header.IN = 0x7A323F42;
	COLOR.Header_Border.IN = 0x7A5A82A2;
	COLOR.KH.IN = 4281282351;
	///////////////

	TFPColors->COLOR_CORRECT = TRUE;
	APPLY_COLOR_CORRECT = TRUE;

	TFPColors->HUE_BEAVIOUR = 1; //override hue
	COLOR.Global.HUE_MODE = 1;
//	TFParams->ITEM_HUE = 0; //base Hue = 0
	TFPColors->HUE_ROLL = (int)HSLA_TEMP_COLOR.h; //use base quick color hue
	TFPColors->SATURATION_OVERIDE = HSLA_TEMP_COLOR.s; // use quick color sat
	TFPColors->LUMINOSITY_OFFSET = HSLA_TEMP_COLOR.l - 0.5f; // use quick color overall lum


	for (int i = 0; i < NB_LB_COLORS; i++)
	{
		LB_COLORS[i]->OnSetQuickColor();
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


