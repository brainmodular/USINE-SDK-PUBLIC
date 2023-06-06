//-----------------------------------------------------------------------------
// LIST BOX ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __LISTBOX_H__
#define __LISTBOX_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"

using namespace std;
//TFParams_struct TFParams;

class ListBox
{
public:
	//contructeur
	ListBox();
	ListBox(TFPStruct *TFParams, TFPaint *TFPaintP_val);

	void onSettingsHasChanged();
	void onPreset();
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();
	void onSetQuickColor();
	void UPD_PAINT();
	void CheckCursorPos();

	LongBool getLongBool(string name);
	void setParam(string name, bool value);

	void UPD_SCROLL_CONTAINER();
	void UPD_SCROLL(float divisor);
	void UPD_SELECTION();
	void RESET_INERTIA();
	void UPD_XFADER();
	void UPD_ZOOM();
	void UPD_LAYOUT();
	void UPD_FIXED_LAYOUT();

	void UPD_SRC_POINTERS();
	void PRESET_LOAD(int preset);
	void UPD_COLOR_ARRAY();
	void UPD_COLORS();

	//float * SRC[10];  //// Matrix to route some sources to destinations temp disabled
	//int SRC_ID = 0;
	//float TGT =0;

	int USED_DISPLAY_NB = 0;
	TRectF LIST_KH_UP;
	TRectF LIST_KH_DWN;
	float LIST_NB_OF_PAGES = 0;
	int   LAST_PAGE = 0; 
	float zoom_start = 0; 
	float zoom_width = 0; 
	float zoom_up = 0;
	float zoom_dwn = 0; 
	float SCROLL_RATIO = 0;// (float)(TFItem.IListBox.NB_LIST_ITEMS - USED_DISPLAY_NB) / (float)USED_DISPLAY_NB;
	//SCROLL_RATIO *= TFItem.IListBox.LIST_CONTAINER_H;

	TFPStruct *TFParams;
	TFPColor *TFPColors;
	TFPaint *TFPaintP;

	LongBool Alphabetical_Sort = TRUE;

	bool SCROLL_CHANGED = FALSE;
	bool ZOOM_CHANGED = FALSE;
	float SCROLL_ON_CLIC_VAL = 0;
	float ZOOM_ON_CLIC_VAL = 0;

	float Y_SCL = 0; 

	// variables
	int count_inertia = -1;
	float last_scroll_delta = 0;
	float DELTA_Y = 0;
	float DELTA_X = 0;
	float RATIO_DY = 1;
	float INERTIA_FACTOR = 0.25;
	int   INERTIA_DURATION = 300; //at 128 blocs size  what will be used
	int   INERTIA_DURATION_IN = 300;  // received for setting input
	float INERTIA_DURATION_RATIO = 1.0;// used to scle duration if bloc size change
	float SCROLL_DELTA = 0;
	float LIST_SCROLL_RESCL = 0;
	//////////////////
	float fad_val = 0.5;
	float real_val_Y = 0.5;
	float real_val_X = 0.5;
	LongBool INERTIA_MODE;// = LISTBOX_INERTIA_MODE;
	bool X_IS_MAIN = FALSE;
	bool Y_IS_MAIN = FALSE;
	bool MS_UP = FALSE;
	////////////////
	TRectF LIST_HEADER;
	TRectF LIST_ITEM_RECT;
	TRectF LIST_SCROLLER;
	TRectF LIST_SCROLLER_CONTAINER;
	TRectF LIST_CONTAINER;
	TRectF LIST_ACTIVE_ITEM_RECT;

	float LIST_HEADER_H = 0.1f;//0.1
	float LIST_HEADER_H_IN = 0.1f;
	float LIST_HEADER_W = 1.0f;

	float LIST_BORDER_SIZE = 4.0f;
	float LIST_CONTAINER_H = 0.9f;
	float LIST_ITEM_H;
	bool  LIST_SCROLL_ACTIVE = FALSE;
	float LIST_SCROLL_VAL01 = 0;
	float LIST_SCROLL_VAL = 0;
	float LIST_SCROLLER_W = 0.35f;
	float LIST_SCROLLER_H = 0.1f;
	float LIST_SCROLLER_H_IN = 0.1f;
	float MAX_SCROLL;

	float LIST_ITEM_BORDER_SIZE = 2.0f;
	LongBool LIST_ITEM_IS_ROUND = TRUE;
	float LIST_ITEM_ROUND_SIZE = 0.01f;
	float LIST_ITEM_GAP = 0.05f;
	float LIST_ITEM_H_GAP = 0.05f;
	float LIST_ITEM_XFADER_VAL = 0;

	float LIST_HEADER_BORDER_SIZE = 2.0f;
	LongBool LIST_HEADER_IS_ROUND = TRUE;
	float LIST_HEADER_ROUND_SIZE = 0.01f;

	float LIST_SCROLLER_BORDER_SIZE = 4.0f;
	LongBool LIST_SCROLLER_IS_ROUND = TRUE;
	float LIST_SCROLLER_ROUND_SIZE = 0.01f;
	float LIST_LUM_MOD = 0.25;  ////patern lum add

	LongBool LIST_USE_PAGES = FALSE;
	LongBool LIST_BORDER_IS_ROUND = TRUE;
	LongBool LIST_USE_X_ZOOM = TRUE;
	LongBool LIST_SCROLLER_FIT_PAGE = TRUE;

	float LIST_ZOOM_LINE_WIDTH = 5.0f;
	float LIST_X_ZOOM_SPEED = 0.5f;
	float LIST_BG_ROUND_SIZE = 0.03f;
	string sel_item_string = "";
	string item_string = "";
	string LIST_PAGE_STRING = "";
	float LIST_ITEM_NAME_SIZE = 0.03f;
	float LIST_HEADER_NAME_SIZE = 0.03f;
	//string item_number_string = "";
	LongBool LIST_ALLOW_SLIDE = FALSE;
	int LISTBOX_INERTIA_MODE = 0; //0slide 1inertia
	bool LISTBOX_ALLOW_SEL = FALSE;
	bool LISTBOX_ALLOW_XFADER = FALSE;
	bool Y_MODE = FALSE;
	bool X_MODE = FALSE;
	float LIST_ZOOM_01 = 0;
	float LIST_PAGE_W = 0.1f;
	float LIST_PAGE_W_MIN = 0.1f;
	float NB_LIST_DISPLAYED_ = 8;
	int NB_LIST_DISPLAYED = 12;
	int LIST_ITEM_SEL = 0;
	int ON_CLIC_SEL = 0;
	int NB_LIST_ITEMS = 12;

	float LIST_PULSE_AMOUNT = 0.5f;

	struct TSHOW
	{
		LongBool LIST_SCROLLER = TRUE;
		LongBool LIST_SCROLLER_CONTAINER = FALSE;
		LongBool LIST_SCROLLER_VALUE = TRUE;
		LongBool LIST_ITEMS_NAMES = TRUE;
		LongBool LIST_ITEMS_NUMBERS = FALSE;
		LongBool LIST_BORDER = TRUE;
		LongBool LIST_ZOOM = TRUE;
		LongBool LIST_ITEM_XFADER = TRUE;
		LongBool LIST_SCROLLER_BORDER = TRUE;
		LongBool LIST_KH = TRUE;
		LongBool LIST_HEADER = TRUE;
		LongBool LIST_HEADER_BORDER = TRUE;
		LongBool LIST_ITEM_BORDER = TRUE;
		LongBool LIST_ITEM_XFADER_VALUE = TRUE;
		//LongBool LIST_ORIGINAL_ID = FALSE;
		int		 ITEMS_NUMBERS_TYPE = 0; //0 none 1: 'visual id' 2 'original pre-sorted id'
		int		 ITEMS_ASSIGNED_VALUE = 0; //0 none, int, .x, .xx, .xxx
		LongBool LIST_PULSE = TRUE;

	};
	TSHOW SHOW;

	struct TINSIDE
	{
		bool LIST_HEADER = FALSE;
		bool LIST_SCROLLER = FALSE;
		bool LIST_SCROLLER_CONTAINER = FALSE;
		bool LIST_CONTAINER = FALSE;
		bool LIST_ITEM_ZONE = FALSE;
		bool LIST_ACTIVE_ITEM = FALSE;
	};
	TINSIDE INSIDE;

	static const int NB_LB_COLORS = 19;
	TFColor *LB_COLORS[NB_LB_COLORS];  //pointer array to Tfcolors

	LongBool APPLY_COLOR_CORRECT = FALSE;

	struct TCOLOR
	{
		TFColor Global;  //0
		TFColor Zoom_Line; //1
		TFColor Border; //2
		TFColor Header; //3
		TFColor Header_Border; //4
		TFColor KH; //5
		TFColor Scroller; //6
		TFColor Scroller_Border; //7
		TFColor Scroller_Shadow; //8
		TFColor Scroller_Container; //9
		TFColor Scroller_Container_Border; //10
		TFColor Item_Border_Off; //11
		TFColor Item_Border_On; //12
		TFColor Item_Off; //13
		TFColor Item_On; //14
		TFColor Xfader; //15
		TFColor Xfader_Border; //16
		TFColor Xfader_Value; //17
		TFColor Item_Name; //18
		
	};
	TCOLOR COLOR;


};
#endif //__LISTBOX_H__
