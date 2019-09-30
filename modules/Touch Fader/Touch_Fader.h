//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// include once, no more
#ifndef __RGBA_COLOR_MODULE_H__
#define __RGBA_COLOR_MODULE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <iostream>
#include <iomanip> ///////////////////prec
#include <cmath>
#include <limits>
#include <string>
#include <algorithm>
//#include <fstream>
#include "Touch_Fader_Tools.h"
#include "TFParamTypes.h"
#include "Item.h"
#include <vector>

using namespace std;

#define CANVAS_DEFAULT_W 150   // default width of module on creation
#define CANVAS_DEFAULT_H 300   // dft height
#define LB_DISPLAYED_MAX 32    // max list box displayed items
#define MAX_LIST_ITEMS 127     // max listbox/matrix comma list vectors size
#define MAX_ARRAY_SIZE 127     // max X Y Z arrays size
#define DELTA_ARRAY_SIZE 10    // mouse XY deltas are sampled to see if X or Y is main direction checking average, and velocity on relase for inertia
#define FX_GRAD_MAX 24         // max subdivisons of gradients FX
#define MAX_ITEMS 8 //nb items 0fade 1butt 2swich 3enc 4xy 5range 6lb 7mx 
//#define M_PI  3.14159265358        //pi  for radial stuf computations

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

class Touch_Fader : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	Touch_Fader(){};
	

    // destructor
	virtual ~Touch_Fader(){};

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	//-----------------------------------------------------------------------------
	// query system and init
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	//-----------------------------------------------------------------------------
	void onAfterQuery(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
	//void onCreateCommands();
	void onPaint();
	void onResize(float contentWidth, float contentHeight);
	//--------------------------------------------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen(LongBool Preset);
	void onGetChunk(void* chunk, LongBool Preset);
	void onSetChunk(const void* chunk, int sizeInBytes, LongBool Preset);
	//--------------------------------------------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y);
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y);
	void onMouseWheel(TShiftState Shift, int WheelDelta);
	//-----------------------------------------------------------------------------
	//void onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	//-------------------------------------------------------------------------------
	//Automations
	void onSetRecordedValue(TPrecision 	X, TPrecision 	Y, TPrecision 	Z);
	//void sdkStopRecord();
	//void sdkProcessRecord(TPrecision X, TPrecision Y, TPrecision Z);
	///----------------------------------------------------------------
	void onBlocSizeChange(int BlocSize);

	void onSetQuickColor(TColorUsine color);
	//
	void onProcess();
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------

	

private:
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr pMODE;
	UsineEventPtr pIn_W;
	UsineEventPtr pIn_X;
	UsineEventPtr pIn_Y;
	UsineEventPtr pIn_Z;
	UsineEventPtr pIn_SW;
	UsineEventPtr pRESET;
	UsineEventPtr pRESET_VALUE;
	UsineEventPtr pCAPTION;
	UsineEventPtr pCAPTION_ON;
	UsineEventPtr pCOMMA_TEXT;
	//UsineEventPtr pHUE;
	UsineEventPtr pOUT_MIN;
	UsineEventPtr pOUT_MAX;
	//-----------------
//	UsineEventPtr pOutW;
	UsineEventPtr pOut_X;
	UsineEventPtr pOut_Y;
	UsineEventPtr pOut_Z;
	UsineEventPtr pOut_SW;
	//-----
	UsineEventPtr pOut_LB_Text;
	UsineEventPtr pOut_LB_Selection;
	UsineEventPtr pOut_LB_Original_ID;
	UsineEventPtr pOut_LB_Assigned_Float;
	UsineEventPtr pOut_LB_Zoom;
	UsineEventPtr pOut_LB_Scroll;
	UsineEventPtr pMidi_Out;
	UsineEventPtr pMidi_In;



	 
	//-------------------------------------------------------------------------
	// custom variables
	//-------------------------------------------------------------------------
	//
	int init_MODE = 0; // init mode received from query

	Item TFItem;		// each mode 'fader, button ect  is defined as an Item, with it's own external code, communicating with main TF code
	string ITEMS_NAMES[MAX_ITEMS] = { "Fader", "Button", "Switch", "Encoder", "XYpad", "RangeFader", "ListBox", "Matrix" }; // differents modes listing

	//enum TMode { MFader, MButton, MSwitch, MEncoder, MXYpad, MRangeFader, MListBox, MMatrix };
	TMode MODE = MFader;    // mode is the current operating item/mode (fader, button ect..) so can be check by enum name or int deft is fader

	// pointers arrays
	TFPStruct *TFParams;  // For items 'local' code and main code to be able to read/write same set of datas a TFparams pointer is passed on item creation (ie mouse down, x value, switch status ect)
	TFPaint *TFPaintP;  // custom set of useful paint elements passed as well
	//Preset TFPreset[MAX_ITEMS];  // not used yet for custom UI presets store/recall

	int USINE_BLOC_SIZE = 128;			// some process computation like flashes/inertia counters ect may rely on a a bloc size variable	
	float USINE_BLOC_SIZE_RATIO = 1.0; // counters were calibrated for a 128 bloc size, if change apply the ratio to maintain time corellation

	float PRESET_MEM[5]; //array for chunk presets store/recall will have w,x,y,z and swich values

	//vector<float>Y_VALUES[MAX_ARRAY_SIZE];     // not used yet arrays 
	//vector<float>X_VALUES[MAX_ARRAY_SIZE];
	//vector<float>Z_VALUES[MAX_ARRAY_SIZE];

	string COMMA_IN ="";          // string of the comma text input

	vector<Tlist_item> LIST_ITEMS;    // will create a vector of custom type elemnt of list including .name .id  .value subset
	
	LongBool SkinLoad = FALSE;    // for custom Ui per item skin handling
	LongBool SkinSave = FALSE;    //
	int SkinNb = 0;               //

	int User_Skin = 0; //1load 2 save;

//	int User_SkinNb[MAX_ITEMS];   //for user Skin load/save
	/////////////////////////////////////// DFT common params like text sizes, box containers, and canvas	
	float CANVAS_H = CANVAS_DEFAULT_H;
	float CANVAS_W = CANVAS_DEFAULT_W;
	float CANVAS_S = CANVAS_DEFAULT_H;
	float NAME_SIZE = 0.06f;
	float VALUE_SIZE = 0.06f;
	TRectF NAME_RECT;// = { 0,0.90f,1,1 };
	LongBool SHOW_NAME = TRUE;

	
	////////////////////////
//	int INPUTS_NORMALIZE = 0; // if 0 inputs are supposed to be ranged to min/max, if 1 should be normalised in 0..1 range
//	int OUTPUTS_NORMALIZE = 0;
	//LongBool USE_NORMALIZED_INPUTS = FALSE;  // switch btw 0..1  or ranged inputs
	float in_val = 0;
	/////////////////////////////////////// Used float variables scaled to output min max range

	struct TOutput_Value
	{
//		float W_RANGED = 0.5;
		float X_RANGED = 0.5;
		float Y_RANGED = 0.5;
		float Z_RANGED = 0.5;
		float SWITCH_RANGED = 0.5;
		float MAIN_RANGED = 0;
		//// ranged values^ Normalised bellow
//		float W_01 = 0.5;
		float X_01 = 0.5;
		float Y_01 = 0.5;
		float Z_01 = 0.5;
		float SWITCH_01 = 0;
		float MAIN_01 = 0.5;
	};

	 TOutput_Value OUT_VAL;

	 //-----------------------------midi
	 struct TCC_Value
	 {
		 LongBool ENABLED = FALSE;
		 int CHANNEL = 1;
		 int NUMBER = 10;
		 int VALUE;
		 int VALUE_LAST;
		 LongBool LEARN = FALSE;
	 };



	 //TCC_Value W_CC;
	 TCC_Value X_CC;
	 TCC_Value Y_CC;
	 TCC_Value Z_CC;
	 TCC_Value SW_CC;


	 vector<TCC_Value*> ALL_CC_OUT;

	 int NB_USED_CC = 0;
	 int NB_MAX_CC = 4; ///(w,x,y,z,sw)

	 UsineMidiCode IN_MIDI_CODE;
	 UsineMidiCode OUT_MIDI_CODE;
	 UsineMidiCode LEARN_MIDI_CODE;


	 int count_midi = -1;

	 LongBool MIDI_LEARN = FALSE;

	 LongBool MIDI_ENABLED = TRUE;

	 //-------------------------------
	 LongBool RECORD_AUTOMATION = FALSE;  //so that item know has to pass XYZ values to usine
	 LongBool RECORD_AUTO_STOP = TRUE; // will autostop automation on mouse relase, should be off for button type automs
	 LongBool RECORD_AUTO_START = FALSE; //if on will auto start rec on mouse down

	float ITEM_HUE = 200;		// global hue variable
	float RANDOM_PATERN[128];  //on init generate a 128 random values array ie for listbox luminosity variation for better scroll view
	//////////////////////////////////////// Mouse XY & delta
	float PREV_X = 0;                   // store previous XY for delta computation 
	float PREV_Y = 0;					//
	float DELTA_MEM[DELTA_ARRAY_SIZE];  // mouse delta is sampled to an array for tracking average if X or Y is main move and velocity on release for inertia
	int	  DELTA_MEM_ID = 0;             // iD of write index
	float DELTA_MEM_SUM = 0;            // Sum of array
	bool X_IS_MAIN = FALSE;             // return wich direction is main currently
	bool Y_IS_MAIN = FALSE;
	/////////////////////////////////// process used variables
	int count_inertia = -1;            // inertia needs a counter
	float normalise_count = 0;         // counter value normalised
	bool MS_CLIC = FALSE;
	bool MS_UP = FALSE;
	int MS_CLIC_count = -1;            // on ms clic counter
	int MS_UP_count = -1;              // on mse up counter
	/////////////////////////////////

	enum T_IN_TYPE{ internal, external };  // for item code, an internal update is usally something trigged by mouse, external is trigged by inputs

	/////////////////////////
	string item_name;
	string item_name_off;
	string item_name_on;
	LongBool NAME_IS_BOLD = FALSE;
	LongBool VALUE_IS_BOLD = FALSE;
	//////////////////////
	int PAINT_LAYER_TYPE;  // main paint proc asks for local item code that returns what type of element it should draw in actual requested layer
	int PAINT_NB_ELMTS_TO_DRAW; // nb of element (layers) to draw from each item to loop for
	TFShape *SHAPE; //to draw shape pointer
	TFValue *STRVAL; //string val to paint

	//LongBool FORMAT_OUTPUTS = TRUE; //apply float formating of outputs same way as visual

	TColorUsine NAME_COLOR;

	//------------------------------------------------------------------
	// Procedures && Functions
	//------------------------------------------------------------------
	void SET_INPUTS_CAPTION(TMode MODE);

	void UPD_OUT();
	void UPD_NAME();
	void RELAYOUT(float W, float H);
	bool XISMAIN();
	void IMPORT_COMMA_TEXT();
	float RANGE_OUT(float in_val);
	string VALUE_TO_STRING(float in_val);
	void UPD_QUANTIZE();
	void UPD_MIDI_OUT();
	void UPD_MIDI_IN();
	void UPD_MIDI_LEARN();
	float GET_IN_VALUE(float in_val);

	//--------------Settings Tabs
	void FADER_SETTINGS();
	void BUTTON_SETTINGS();
	void ENCODER_SETTINGS();
	void XYPAD_SETTINGS();
	void RANGE_FADER_SETTINGS();
	void LISTBOX_SETTINGS();
	void MATRIX_SETTINGS();
	void MIDI_SETTINGS();
	void GLOBAL_SETTINGS();
	//--------------Draw && Paint Tools that must remain in main code as using path/usine paint elements
	void PAINT_ITEM();
	void DRAW_PATH_FROM_TO(TPointF P_from, TPointF P_to);
	void DRAW_CIRCLE(TPointF Center, float Radius);
	void DRAW_ROUND_PATH(TRectF IN_RECT, LongBool ROUNDED, float ROUND_SIZE, float RATIO, float ROUND_SMOOTHNESS, float OFFSET_X = 0, float OFFSET_Y = 0, bool UNCLOSED = FALSE, int START_POS = 0, int NB_SEG = 4, bool ONLY_SHORT_SEG = FALSE);
	void DRAW_SHAPE_PATH(TFShape IN_RECT, float OFFSET_X = 0, float OFFSET_Y = 0, bool UNCLOSED = FALSE, int START_POS = 0, int NB_SEG = 4, bool ONLY_SHORT_SEG = FALSE);
	void DRAW_QUAD_RING(TPointF MAINS[], TPointF CTRLS[], int size);
	void RECT_REFLECTIONS(TRectF InRect, bool IS_ROUND, float ROUND_SIZE, float CANVAS_RATIO, float ROUND_SMOOTHNESS, float each, float hue, float sat, float opa, int Draw_mode);
	void SHAPE_REFLECTIONS(TFShape InRect, float each, TColorUsine COLOR, int DrawMode);
	void BEVEL_REFLECTIONS(TFShape InRect, float opa);
	///////////////////////////////


	//Automation
	void RECORD_XYZ();

	////////////////////
	

}; // class Touch_Fader

#endif //__RGBA_COLOR_MODULE_H__
