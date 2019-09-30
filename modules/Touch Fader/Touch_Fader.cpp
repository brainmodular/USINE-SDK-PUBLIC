///////////////////////////////////////////////////////////////////////
//																	//
//	Touch Fader User Module	v1.00 HH3 version 23fx23 02-04-2017		//	
//																	//
//////////////////////////////////////////////////////////////////////
//	compiler: Visual Studio 2015   Used Sdk : 7.00.001		     	//		
//////////////////////////////////////////////////////////////////////
//																	//
//	Touch oriented swichable & customizable Multi-mode module	    //
//   Fader, Button, Switch, Encoder, XYpad, RangeFader, ListBox		//
//																	//
//	Warning: newbie to ccp & sdk, weird code^^					    //
//																	//
//////////////////////////////////////////////////////////////////////

#include "Touch_Fader.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new Touch_Fader();

	//TF_MODULE = static_cast <UserModuleBase*>(pModule);
}
void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete ((Touch_Fader*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Touch Fader";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Touch Fader by 23fx23";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";


// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// UserModuleBase Methods
//-----------------------------------------------------------------------------
void Touch_Fader::onGetModuleInfo(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	//identification of the module
	pModuleInfo->Name = "Touch Fader";
	pModuleInfo->Description = "Touch Fader by 23fx23";
	pModuleInfo->ModuleType = mtControl;
	pModuleInfo->BackColor = 0xFF999999;
	pModuleInfo->NumberOfParams    = 25; 
	pModuleInfo->Version = "1.2";
	pModuleInfo->DontProcess = FALSE;
	pModuleInfo->DefaultWidth = CANVAS_DEFAULT_W;
	pModuleInfo->DefaultHeight = CANVAS_DEFAULT_H;
	pModuleInfo->CanRecord = FALSE;
	pModuleInfo->CanBeSavedInPreset = true; 
	pModuleInfo->TransparentBackground = TRUE;
	
	
	// query for dft item 
	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryString = "Choose Item:";
		pModuleInfo->QueryListValues = "0 Fader,1 Button,2 Switch,3 Encoder,4 XYpad,5 Range Fader ,6 List Box";
		pModuleInfo->QueryDefaultIdx = 0;
	}
}

void Touch_Fader::onAfterQuery(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex)
{
	init_MODE = queryIndex;
}
void Touch_Fader::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
	// all parameters declared in the module class		
	switch (ParamIndex) 
	{
	// dtfInputA
	case 0:
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "mode";
		pParamInfo->ListBoxStrings = "fader,button,switch,encoder,XY_Pad,range fader,listBox";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 6;
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "mode";
		break;

	case 1:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "W in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 1;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "Inputs";
		pParamInfo->IsStoredInPreset = true;
		pParamInfo->DontSave = true;
		break;


	case 2:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "X in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 1;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsStoredInPreset = true;
		pParamInfo->DontSave = true;
		break;

	case 3:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "Y in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsStoredInPreset = true;
		pParamInfo->DontSave = true;
		break;

	case 4:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "Z in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 1;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsStoredInPreset = true;
		pParamInfo->DontSave = true;
		break;

	case 5:
		pParamInfo->ParamType = ptSwitch;
		pParamInfo->Caption = "switch in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 1;
		//pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsStoredInPreset = true;
		pParamInfo->DontSave = true;
		break;

	case 6:
		pParamInfo->ParamType = ptButton;
		pParamInfo->Caption = "reset";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 1;
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = " ";
		break;

	case 7:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "reset value";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctImmediate;
		break;

	case 8:
		pParamInfo->ParamType = ptTextField;
		pParamInfo->Caption = "caption";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->TextValue = "name";
		pParamInfo->CallBackType = ctNormal;
		break;

	case 9:
		pParamInfo->ParamType = ptTextField;
		pParamInfo->Caption = "on caption";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->TextValue = "name on";
		pParamInfo->CallBackType = ctNormal;
		break;

	case 10:
		pParamInfo->ParamType = ptTextField;
		//pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "comma text";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->DontSave = FALSE;
		pParamInfo->TextValue = "A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z" ;
		//pParamInfo->ListBoxStrings =  "A a,B, C,D " ;
		break;

	/*case 11:
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "hue";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 360;
		pParamInfo->Format = "%.0f";
		pParamInfo->DefaultValue = 200;
		pParamInfo->CallBackType	= ctNormal;
		break;*/

	case 11:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "min";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->Format = "%.3f";
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->DefaultValue = 0;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "ranges";
		break;

	case 12:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "max";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->DefaultValue = 1;
		pParamInfo->Format = "%.3f";
		pParamInfo->CallBackType = ctNormal;
		break;





	// dftOutputs///////////////////////////////////////////////////////

	//case 13:
	//	pParamInfo->ParamType = ptDataField;
	//	pParamInfo->Caption = "W out";
	//	pParamInfo->IsInput = FALSE;
	//	pParamInfo->IsOutput = TRUE;
	//	pParamInfo->DontSave = TRUE;
	//	//pParamInfo->MinValue = 0;
	//	//pParamInfo->MaxValue = 127;
	//	pParamInfo->Format = "%.2f";
	//	pParamInfo->DefaultValue = 0;
	//	pParamInfo->IsSeparator = TRUE;
	//	pParamInfo->SeparatorCaption = "outputs";
	//	break;


	case 13:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "X out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->DontSave = TRUE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "outputs";
		break;



	case 14:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "Y out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->DontSave = TRUE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		break;


	case 15:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "Z out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->DontSave = TRUE;
		//pParamInfo->MinValue = 0;
		//pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.2f";
		pParamInfo->DefaultValue = 0;
		break;

	case 16:
		pParamInfo->ParamType = ptSwitch;
		pParamInfo->Caption = "switch out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->CallBackType = ctImmediate;
		break;

	case 17:
		pParamInfo->ParamType = ptTextField;
		pParamInfo->Caption = "LB text";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->TextValue = "name";
		//pParamInfo->CallBackType = ctImmediate;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "listBox";
		break;

	case 18:
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "LB selection";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.0f";
		pParamInfo->DefaultValue = 0;
		break;

	case 19:
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "LB original ID";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 127;
		pParamInfo->Format = "%.0f";
		pParamInfo->DefaultValue = 0;
		break;

	case 20:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "LB float";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->Format = "%.3f";
		pParamInfo->DefaultValue = 0;
		break;

	case 21:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "LB zoom";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->Format = "%.03f";
		pParamInfo->DefaultValue = 0;
		break;

	case 22:
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "LB scroll";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->Format = "%.03f";
		pParamInfo->DefaultValue = 0;
		break;
		//---------------------
	case 23:
		pParamInfo->ParamType = ptMidi;
		pParamInfo->Caption = "midi out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "MIDI";
		break;

	case 24:
		pParamInfo->ParamType = ptMidi;
		pParamInfo->Caption = "midi in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		break;


	// default case
	default:
		break;
	}
}
//-----------------------------------------------------------------------------
// set the parameters events address
void Touch_Fader::onSetEventAddress(int ParamIndex, UsineEventPtr pEvent)
{
	
	//Initialyse all events adress declared in your module class
	switch (ParamIndex) 
    {

	case 0:
		pMODE = pEvent;
		break;

	case 1:
		pIn_W = pEvent;
		break;

	case 2:
		pIn_X = pEvent;
		break;

	case 3:
		pIn_Y = pEvent;
		break;

	case 4:
		pIn_Z = pEvent;
		break;

	case 5:
		pIn_SW = pEvent;
		break;

	case 6:
		pRESET = pEvent;
		break;

	case 7:
		pRESET_VALUE = pEvent;
		break;

	case 8:
		pCAPTION = pEvent;
		break;

	case 9:
		pCAPTION_ON = pEvent;
		break;

	case 10:
		pCOMMA_TEXT = pEvent;
		break;

	case 11:
		pOUT_MIN = pEvent;
		break;

	case 12:
		pOUT_MAX = pEvent;
		break;
//////////////////////////////////////////ouputs

	//case 13:
	//	pOutW = pEvent;
	//	break;

	case 13:
		pOut_X = pEvent;
		break;

	case 14:
		pOut_Y = pEvent;
		break;

	case 15:
		pOut_Z = pEvent;
		break;

	case 16:
		pOut_SW = pEvent;
		break;

	case 17:
		pOut_LB_Text = pEvent;
		break;

	case 18:
		pOut_LB_Selection = pEvent;
		break;

	case 19:
		pOut_LB_Original_ID = pEvent;
		break;

	case 20:
		pOut_LB_Assigned_Float = pEvent;
		break;

	case 21:
		pOut_LB_Zoom = pEvent;
		break;

	case 22:
		pOut_LB_Scroll = pEvent;
		break;

	case 23 :
		pMidi_Out = pEvent;
		break;

	case 24:
		pMidi_In = pEvent;
		break;

	// default case
	default:
		break;

	}
}
void Touch_Fader::onInitModule(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	///////////////////////////////// INIT
	TFParams = new TFPStruct();     // structure of main processed parameters like X,YZ values
	TFPaintP = new TFPaint();       // structure of some passed paint parameters like shapes, lines 

	TFItem = Item(TFParams,TFPaintP);   // Create a new item passing above pointers

//	ITEM_HUE = sdkGetEvtData(pHUE);       // most items get their own set of colors but sometimes an override of hue using input is possible


	item_name_off = sdkGetEvtPChar(pCAPTION);  // get input names
	item_name_on = sdkGetEvtPChar(pCAPTION_ON);
	item_name = item_name_off;
	COMMA_IN = sdkGetEvtPChar(pCOMMA_TEXT); IMPORT_COMMA_TEXT(); //get input comma text and parse to vector

	//////////////////////////////////  init name rect used to display name
	NAME_RECT.left = 0;
	NAME_RECT.right = 1.0;
	NAME_RECT.top = 0.9f;
	NAME_RECT.bottom = 1;
	///////////////////////////////////
	TFParams->OUT_MIN = sdkGetEvtData(pOUT_MIN);   // get ranges min max
	TFParams->OUT_MAX = sdkGetEvtData(pOUT_MAX);

	USINE_BLOC_SIZE = sdkGetBlocSize(); // get usine bloc size for counters duartion to adapt
	USINE_BLOC_SIZE_RATIO = 64.0f / (float)USINE_BLOC_SIZE;   // counters were calibrated for a 64 bloc, if bloc change generate a ratio to keep same durations

	TFItem.IListBox.INERTIA_DURATION_RATIO = USINE_BLOC_SIZE_RATIO;  // lisbox gets that ratio

	RELAYOUT(sdkWidthPercentToPixel(1.0f),sdkHeightPercentToPixel(1.0f)); // compute canvas datas

	sdkSetEvtData(pMODE, (TPrecision)init_MODE); //set to mode defined in query answer

	////////////////////////generate random pattern (ie for listbox slight lum var for better scroll view)
	srand(1);
	for (int i = 0; i < 128; i++)
	{
		RANDOM_PATERN[i] = (float)rand() / 65535.0f;
	}
	////////////////////

	ALL_CC_OUT.resize(NB_MAX_CC);

	//ALL_CC_OUT[0] = &W_CC;
	ALL_CC_OUT[0] = &X_CC;
	ALL_CC_OUT[1] = &Y_CC;
	ALL_CC_OUT[2] = &Z_CC;
	ALL_CC_OUT[3] = &SW_CC;

}/////////////////End Init



//-----------------------------------------------------------------------------
// Parameters callback
void Touch_Fader::onCallBack(UsineMessage* Message)
{
	try
	{
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
        {
            int paramIndex = (int)Message->wParam;

			if ((paramIndex == 0) && (Message->lParam == MSG_CHANGE))  // MODE
			{							
				MODE = (TMode)(int)(sdkGetEvtData(pMODE));
				SET_INPUTS_CAPTION(MODE);
				TFItem.onSettingsHasChanged(MODE);			
				UPD_OUT();
				RELAYOUT(sdkWidthPercentToPixel(1.0f), sdkHeightPercentToPixel(1.0f));
			}

			/////////////////////////////////////// INPUTS
			if ((paramIndex == 1) && (Message->lParam == MSG_CHANGE))  // W - Main
			{
				in_val = sdkGetEvtData(pIn_W);
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_W = in_val;
				TFItem.onInputChange(MODE);
				UPD_OUT();
			}
			/////////////////////////////////////// 
			if ((paramIndex == 2 ) && (Message->lParam == MSG_CHANGE))  // X
			{
				in_val = sdkGetEvtData(pIn_X);
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_X = in_val; 
				TFItem.onInputChange(MODE);
				UPD_OUT();
			}
			//////////////////////////////
			if ((paramIndex == 3) && (Message->lParam == MSG_CHANGE))  // Y
			{
				in_val = sdkGetEvtData(pIn_Y);
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_Y = in_val;
				TFItem.onInputChange(MODE);
				UPD_OUT();
			}
			//////////////////////////////
			if ((paramIndex == 4) && (Message->lParam == MSG_CHANGE))  // Z
			{
				in_val = sdkGetEvtData(pIn_Z);
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_Z = in_val;
				//TFParams->MAX_POS = in_val; ////////////////////rf z to maxpos
				TFItem.onInputChange(MODE);
				UPD_OUT();
			}
			//////////////////////////////
			if ((paramIndex == 5) && (Message->lParam == MSG_CHANGE))  // SW
			{
				in_val = sdkGetEvtData(pIn_SW);
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_SWITCH = in_val;
				//TFParams->MIN_POS = in_val;  /////-----rf sw to min pos //add W?
				TFItem.onInputChange(MODE);
				UPD_OUT();
			}
			//////////////////////////////
			/////////////////////////////////////////////////////////////////////////////
			if ((paramIndex == 6) && (Message->lParam == MSG_CHANGE))  // reset trigged
			{
				//TFParams->input_val = TFParams->RESET_VALUE_01;
				in_val = TFParams->RESET_VALUE;
				in_val = GET_IN_VALUE(in_val);
				TFParams->ITEM_VALUE_Y = in_val;
				TFItem.onInputChange(MODE);
				//OUT_VAL.Y_01 = TFParams->ITEM_VALUE_Y;
				UPD_OUT();
			}

			if ((paramIndex == 7) && (Message->lParam == MSG_CHANGE))  // reset value
			{
				TFParams->RESET_VALUE = sdkGetEvtData(pRESET_VALUE);
				//TFParams->RESET_VALUE = fmax(TFParams->RESET_VALUE, TFParams->OUT_MIN);
				//TFParams->RESET_VALUE = fmin(TFParams->RESET_VALUE, TFParams->OUT_MAX);
				//TFParams->RESET_VALUE_01 = (TFParams->RESET_VALUE - TFParams->OUT_MIN) / (TFParams->OUT_MAX - TFParams->OUT_MIN);
			}

			if ((paramIndex == 8) && (Message->lParam == MSG_CHANGE))  // caption off
			{
				item_name_off = sdkGetEvtPChar(pCAPTION);
				UPD_NAME();
			}
			if ((paramIndex == 9) && (Message->lParam == MSG_CHANGE))  // caption on
			{
				item_name_on = sdkGetEvtPChar(pCAPTION_ON);
				UPD_NAME();
			}

			if ((paramIndex == 10) && (Message->lParam == MSG_CHANGE))  // comma text
			{
				COMMA_IN = sdkGetEvtPChar(pCOMMA_TEXT);
				IMPORT_COMMA_TEXT();
				sdkRepaintPanel();
			}

			//if ((paramIndex == 11) && (Message->lParam == MSG_CHANGE))  // hue
			//{
			//	TFParams->ITEM_HUE = sdkGetEvtData(pHUE);
			//	TFItem.onSettingsHasChanged(MODE);
			//	TFItem.UPD_PAINT(MODE);
			//	sdkRepaintPanel();
			//}

			if ((paramIndex == 11) && (Message->lParam == MSG_CHANGE))  // out min
			{
				TFParams->OUT_MIN = sdkGetEvtData(pOUT_MIN);
				UPD_QUANTIZE();
				UPD_OUT();
				sdkRepaintPanel();
			}

			if ((paramIndex == 12) && (Message->lParam == MSG_CHANGE))  // out max
			{
				TFParams->OUT_MAX = sdkGetEvtData(pOUT_MAX);
				UPD_QUANTIZE();
				UPD_OUT();
				sdkRepaintPanel();
			}

			if ((paramIndex == 24) && (Message->lParam == MSG_CHANGE))  // Midi In
			{

				UPD_MIDI_IN();
				sdkRepaintPanel();
			}

        }
	}
	catch (...)
	{
		//sdkTraceErrorChar("error");
	}
}

void Touch_Fader::SET_INPUTS_CAPTION(TMode MODE)
{
	//sdkSetEvtData(pIn_W, 0);
	//sdkSetEvtData(pIn_X, 0);
	//sdkSetEvtData(pIn_Y, 0);
	//sdkSetEvtData(pIn_Z, 0);
	//sdkSetEvtData(pIn_SW, 0);
	//sdkSetEvtData(pOut_X, 0);
	//sdkSetEvtData(pOut_Y, 0);
	//sdkSetEvtData(pOut_Z, 0);
	//sdkSetEvtData(pOut_SW, 0);


	switch (MODE)
	{
	case MFader:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "aux fader value in");
		sdkSetParamCaption(3, "fader value in");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "-");
		sdkSetParamCaption(13, "aux fader out");
		sdkSetParamCaption(14, "fader value out");
		sdkSetParamCaption(15, "max on pressed");
		sdkSetParamCaption(16, "pressed");
		break;
	case MButton:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "-");
		sdkSetParamCaption(3, "-");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "button value in");
		sdkSetParamCaption(13, "aux X position out");
		sdkSetParamCaption(14, "aux Y position out");
		sdkSetParamCaption(15, "button value out");
		sdkSetParamCaption(16, "button out");
		break;
	case MSwitch:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "-");
		sdkSetParamCaption(3, "-");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "button value in");
		sdkSetParamCaption(13, "aux X position out");
		sdkSetParamCaption(14, "aux Y position out");
		sdkSetParamCaption(15, "switch value out");
		sdkSetParamCaption(16, "switch out");

		break;
	case MListBox:
		sdkSetParamCaption(1, "list value in");
		sdkSetParamCaption(2, "-");
		sdkSetParamCaption(3, "-");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "-");
		sdkSetParamCaption(13, "aux fader out");
		sdkSetParamCaption(14, "list value out");
		sdkSetParamCaption(15, "-");
		sdkSetParamCaption(16, "pressed");

		break;
	case MRangeFader:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "min value in");
		sdkSetParamCaption(3, "max value in");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "-");
		sdkSetParamCaption(13, "min value out");
		sdkSetParamCaption(14, "max value out");
		sdkSetParamCaption(15, "mid value out");
		sdkSetParamCaption(16, "-");

		break;
	case MEncoder:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "-");
		sdkSetParamCaption(3, "encoder value in");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "-");
		sdkSetParamCaption(13, "-");
		sdkSetParamCaption(14, "encoder value out");
		sdkSetParamCaption(15, "-");
		sdkSetParamCaption(16, "pressed");

		break;
	case MXYpad:
		sdkSetParamCaption(1, "-");
		sdkSetParamCaption(2, "X value in");
		sdkSetParamCaption(3, "Y value in");
		sdkSetParamCaption(4, "-");
		sdkSetParamCaption(5, "-");
		sdkSetParamCaption(13, "X value out");
		sdkSetParamCaption(14, "Y value out");
		sdkSetParamCaption(15, "max on pressed");
		sdkSetParamCaption(16, "pressed");

		break;
    case MMatrix:
        break;
        
	}

}
//---------------------------------------------------------------------------------
/////////////////////////////////////////
float Touch_Fader::GET_IN_VALUE(float in_val)
{   
	in_val = fmax(in_val, TFParams->OUT_MIN);
	in_val = fmin(in_val, TFParams->OUT_MAX);
//	in_val = RE_RANGE_NORM_OUT(in_val, TFParams->OUT_MIN, TFParams->OUT_MAX);
	return in_val;

}
//////////////////////////////////////
void Touch_Fader::onMouseWheel(TShiftState Shift, int WheelDelta)
{
	//sdkTraceInt(WheelDelta);

	if (MODE == MListBox)
	{	
		TFItem.IListBox.UPD_SCROLL((float)WheelDelta / (float)(TFItem.IListBox.NB_LIST_ITEMS-1));
	}

}
void Touch_Fader::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if(RECORD_AUTO_START)
	{
		RECORD_AUTOMATION = TRUE;
	}

	TFParams->X = X;
	TFParams->Y = Y;

	if (MouseButton == mbLeft){ TFParams->MSDWN = TRUE; MS_CLIC = TRUE; MS_CLIC_count = -1; }

	TFParams->DELTA_X = 0;
	TFParams->DELTA_Y = 0;

	PREV_X = X;
	PREV_Y = Y;

	if (TFParams->MSDWN)
	{
		TFParams->MSX = CLAMP01(X);
		TFParams->MSY = CLAMP01(Y);

		TFItem.onMouseDown(MODE);
		UPD_OUT();
	}//msdwn

	TFParams->hilight = TFParams->MSDWN;/// *GB_HILIGHT_LUM;


	if (TFParams->MSDWN && RECORD_AUTOMATION) ///////////////////////////////// to optimize get sdk record state?
	{
		RECORD_XYZ();
	}

	sdkRepaintPanel();
}
void Touch_Fader::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	TFParams->X = X;
	TFParams->Y = Y;
	TFParams->MSDWN = FALSE;
	MS_UP = TRUE; MS_UP_count = -1;

	TFParams->DELTA_X = 0;
	TFParams->DELTA_Y = 0;

	TFParams->hilight = 0;
	////////////////////////////////////////////////
	TFItem.onMouseUp(MODE);
	UPD_OUT();
	//////////
	if (MODE == MListBox)
	{
		if (TFItem.IListBox.INERTIA_MODE)
		{
			if (TFItem.IListBox.Y_MODE && TFItem.IListBox.INSIDE.LIST_ITEM_ZONE )
			{
				count_inertia = 0;
			}
		}
	}// LB
	/////////////
	sdkRepaintPanel();

	if (RECORD_AUTOMATION && RECORD_AUTO_STOP)
	{
		sdkStopRecord(); RECORD_AUTOMATION = FALSE;
	}
	if (RECORD_AUTOMATION) ///////////////////////////////// to optimize get sdk record state?
	{
		RECORD_XYZ();
	}
}//end ms up
void Touch_Fader::onMouseMove(TShiftState Shift, float X, float Y)
{

	TFParams->X = X;
	TFParams->Y = Y;
	TFParams->DELTA_X = X - PREV_X;
	TFParams->DELTA_Y = Y - PREV_Y;

	if (TFParams->MSDWN)
	{
		TFParams->MSX = CLAMP01(X);
		TFParams->MSY = CLAMP01(Y);

		if (MODE == MListBox) //listbox
		{
			//ListBox *LB = dynamic_cast<ListBox*>(Items[6]);
			TFItem.IListBox.X_IS_MAIN = XISMAIN();    //check wich direction of mouse is main for zoom/scroll, inertia and xfader stuff
			TFItem.IListBox.Y_IS_MAIN = !TFItem.IListBox.X_IS_MAIN;
			TFItem.onMouseMove(MODE);
			UPD_OUT();
		}
		else
		{
			TFItem.onMouseMove(MODE);
			UPD_OUT();
		}
	}

	PREV_X = X;
	PREV_Y = Y;

	if (RECORD_AUTOMATION) ///////////////////////////////// to optimize get sdk record state?
	{
		RECORD_XYZ();
	}

}//MS MOVE
////////////////////////////////////////////////////////
void Touch_Fader::onCreateSettings()
{
	// Items Specific Options
	GLOBAL_SETTINGS();
	MIDI_SETTINGS();
		FADER_SETTINGS();
		BUTTON_SETTINGS();
		ENCODER_SETTINGS();
		XYPAD_SETTINGS();
		RANGE_FADER_SETTINGS();
		LISTBOX_SETTINGS();
	//	MATRIX_SETTINGS();
}
void Touch_Fader::onSettingsHasChanged()
{
		

	UPD_QUANTIZE(); //should only if value prec chged track?
	
	TFItem.onSettingsHasChanged(MODE);  /// apply updates on local items 

	//-----------------------build in skin templates
	if (SkinNb != 0) //if skin was chosen
	{
		TFItem.onSkinLoad(MODE, SkinNb - 1); //pass to item
		SkinNb = 0;   //revert to 0 for no load on next
	}

	//-------------midi //////////////////////

	MIDI_LEARN = (X_CC.LEARN || Y_CC.LEARN || Z_CC.LEARN || SW_CC.LEARN);  //if any midi learn will process ot not

	ALL_CC_OUT.resize(NB_MAX_CC);

	//ALL_CC_OUT[0] = &W_CC;
	ALL_CC_OUT[0] = &X_CC;
	ALL_CC_OUT[1] = &Y_CC;
	ALL_CC_OUT[2] = &Z_CC;
	ALL_CC_OUT[3] = &SW_CC;

	UPD_MIDI_OUT();
	//-------------

	// USERS_SKIN
   // SKINS read write datas & pointers pairs to .txt file-------------

	//string SkinFileName;
	int skin_mode = MODE; // to force switch being button

	if (skin_mode == 2) //if switch
	{
		skin_mode = 1; //button
	}

	//SkinFileName = sdkGetGlobalApplicationPath();
	//SkinFileName.append("/Resources/Snapshots/TF_");
	//SkinFileName.append(ITEMS_NAMES[skin_mode]);
	//SkinFileName.append("_Skin_");
	//SkinFileName.append(sdkIntToString(User_SkinNb[skin_mode]));
	//SkinFileName.append(".txt");
	

	string extension = ".txt"; ///Resources/Snapshots/
	//AnsiCharPtr file_char = file.c_str();
	//AnsiCharPtr file = "";
/*
	if (User_Skin == 1)  ///(SkinLoad)
	{
		//SkinLoad = FALSE;	
		User_Skin = 0;
		 file = "";// ITEMS_NAMES[skin_mode] + ".txt";

			sdkProcessOpenDialog(&file, "","");

			//sdkTraceChar(file);
			TFPreset[skin_mode].ReadFromFile(file);
			//TFPreset[skin_mode].Apply();
			SkinLoad = FALSE;
		
	}
	else if (User_Skin == 2)//(SkinSave)
	{
		User_Skin = 0;
		//SkinSave = FALSE;
		file = "";// ITEMS_NAMES[skin_mode] + ".txt";
		sdkProcessSaveDialog(&file, "", "");
		TFPreset[skin_mode].SaveToFile(file);
		SkinSave = FALSE; 
	}
*/
	//////////////////////////////
	if (MODE == MListBox)
	{
		IMPORT_COMMA_TEXT(); // reimport /sort comma text in case sort type was changed maybe track sort chang to optimize
	}

	TFItem.UPD_PAINT(MODE);

	UPD_OUT();
	RELAYOUT(sdkWidthPercentToPixel(1), sdkHeightPercentToPixel(1));
	sdkRepaintPanel();

}
////////////////// SETTINGS
void Touch_Fader::GLOBAL_SETTINGS()
{
	/////////////// Txt val / name / global options					
	sdkAddSettingLineCaption("properties", "values Options");
	//sdkAddSettingLineCombobox("properties", &INPUTS_NORMALIZE, "inputs", "ranged,normalized");
	//sdkAddSettingLineCombobox("properties", &OUTPUTS_NORMALIZE, "outputs", "ranged,normalized");
	sdkAddSettingLineCombobox("properties", &TFParams->VALUE_PRECISION, "format", "integer,.x,.xx,.xxx,.xxxx,.xxxxx");
	//sdkAddSettingLineBoolean("properties", &FORMAT_OUTPUTS, "format outputs same way");

	sdkAddSettingLineCaption("design", "value");
	sdkAddSettingLineBoolean("design", &TFParams->SHOW_VALUE, "show value");
	sdkAddSettingLineSingle("design", &VALUE_SIZE, "value size", 0.01f, 1, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &VALUE_IS_BOLD, "value Is Bold");
	sdkAddSettingLineSingle("design", &TFParams->VALUE_RECT.top, "value vertical pos", -0.2f, 1.2f, scLinear, "units", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFParams->VALUE_RECT.right, "value horizontal pos", -0.2f, 2.2f, scLinear, "units", "%.2f", 0);
	sdkAddSettingLineColor("design", &TFParams->VALUE_ON_COLOR, "value ON color");
	sdkAddSettingLineColor("design", &TFParams->VALUE_OFF_COLOR, "value OFF color");
	sdkAddSettingLineCaption("design", "name");
	sdkAddSettingLineBoolean("design", &SHOW_NAME, "show name");
	sdkAddSettingLineSingle("design", &NAME_SIZE, "name size", 0.01f, 1, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &NAME_IS_BOLD, "name Is Bold");
	sdkAddSettingLineSingle("design", &NAME_RECT.top, "name vertical pos", -0.2f, 1.2f, scLinear, "units", "%.2f", 0);
	sdkAddSettingLineSingle("design", &NAME_RECT.right, "name horizontal pos", -0.2f, 2.2f, scLinear, "units", "%.2f", 0);
	sdkAddSettingLineColor("design", &TFParams->NAME_ON_COLOR, "name ON color");
	sdkAddSettingLineColor("design", &TFParams->NAME_OFF_COLOR, "name OFF color");
	//sdkAddSettingLineCaption("remote", "automation record");
	//sdkAddSettingLineBoolean("remote", &RECORD_AUTOMATION, "record automation ready");
	//sdkAddSettingLineBoolean("remote", &RECORD_AUTO_START, "record autostart on mouse dwn");
	//sdkAddSettingLineBoolean("remote", &RECORD_AUTO_STOP, "record autostop on release");

	sdkAddSettingLineCaption("properties", "Build In Skins");
	sdkAddSettingLineCombobox("properties", &SkinNb, "Fader Skin", " ,default,minimalist,stripes,3D hexa,3D reflections,crossfader,diagonal raise,tube,grid quantize");
	sdkAddSettingLineCombobox("properties", &SkinNb, "Button & Switch Skin", " ,default,minimalist,led_XY,slinding led");
	sdkAddSettingLineCombobox("properties", &SkinNb, "Encoder Skin", " ,default,minimalist,leds,leds segments,round,cursor line,cursor shape,V bar leds,color panel,fader panel");
	sdkAddSettingLineCombobox("properties", &SkinNb, "XY pad Skin", " ,default,minimalist");
	sdkAddSettingLineCombobox("properties", &SkinNb, "Range Fader Skin", " ,default,minimalist");
	sdkAddSettingLineCombobox("design", &SkinNb, "Listbox Skin", " ,default,minimalist,pages scroll,black");

}
void Touch_Fader::MIDI_SETTINGS()
{
	sdkAddSettingLineCaption("remote", "MIDI");
	sdkAddSettingLineBoolean("remote", &MIDI_ENABLED, "enable midi");

	
	//sdkAddSettingLineCaption("remote", "W");
	//sdkAddSettingLineBoolean("remote", &W_CC.ENABLED, "W midi");
	//sdkAddSettingLineInteger("remote", &W_CC.CHANNEL, "W midi channel", 1, 16, scLinear, "", 1);
	//sdkAddSettingLineInteger("remote", &W_CC.NUMBER, "W midi CC", 0, 127, scLinear, "", 1);
	//sdkAddSettingLineBoolean("remote", &W_CC.LEARN, "W Learn");

	sdkAddSettingLineCaption("remote", "X");
	sdkAddSettingLineBoolean("remote", &X_CC.ENABLED, "X midi");
	sdkAddSettingLineInteger("remote", &X_CC.CHANNEL, "X midi channel", 1, 16, scLinear, "", 1);
	sdkAddSettingLineInteger("remote", &X_CC.NUMBER, "X midi CC", 0, 127, scLinear, "", 1);
	sdkAddSettingLineBoolean("remote", &X_CC.LEARN, "X Learn");

	sdkAddSettingLineCaption("remote", "Y");
	sdkAddSettingLineBoolean("remote", &Y_CC.ENABLED, "Y midi");
	sdkAddSettingLineInteger("remote", &Y_CC.CHANNEL, "Y midi channel", 1, 16, scLinear, "", 1);
	sdkAddSettingLineInteger("remote", &Y_CC.NUMBER, "Y midi CC", 0, 127, scLinear, "", 1);
	sdkAddSettingLineBoolean("remote", &Y_CC.LEARN, "Y Learn");

	sdkAddSettingLineCaption("remote", "Z");
	sdkAddSettingLineBoolean("remote", &Z_CC.ENABLED, "Z midi");
	sdkAddSettingLineInteger("remote", &Z_CC.CHANNEL, "Z midi channel", 1, 16, scLinear, "", 1);
	sdkAddSettingLineInteger("remote", &Z_CC.NUMBER, "Z midi CC", 0, 127, scLinear, "", 1);
	sdkAddSettingLineBoolean("remote", &Z_CC.LEARN, "Z Learn");

	sdkAddSettingLineCaption("remote", "SW");
	sdkAddSettingLineBoolean("remote", &SW_CC.ENABLED, "switch midi");
	sdkAddSettingLineInteger("remote", &SW_CC.CHANNEL, "switch midi channel", 1, 16, scLinear, "", 1);
	sdkAddSettingLineInteger("remote", &SW_CC.NUMBER, "switch midi CC", 0, 127, scLinear, "", 1);
	sdkAddSettingLineBoolean("remote", &SW_CC.LEARN, "switch Learn");

}
void Touch_Fader::FADER_SETTINGS()
{

	//---------------------------------------------------------------------------------------------------
	//sdkAddSettingLineCaption("design", "FD Main Output");
	//sdkAddSettingLineCombobox("design", &TFItem.IFader.MAIN_OUTPUT, "FD Main Out will receive:", "X,Y,Z,switch");
	//
	sdkAddSettingLineCaption("design", "FD Mode");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.RELATIVE_MODE,  "FD Absolute, Relative");
	////////////////////
	sdkAddSettingLineBoolean("design", &TFItem.IFader.HORIZONTAL_MODE, "FD Verical, Horizontal");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.INVERT_MODE, "FD Normal, Inverted");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.AUTO_RESET_MODE, "FD Auto reset");
	sdkAddSettingLineCombobox("design", &(int&)TFItem.IFader.SCALE_TYPE, "FD Scale type", "linear,logarithmic,exponential");
	/////////////////////////////////
	sdkAddSettingLineCombobox("design", &TFItem.IFader.FADER_SWITCH_SUBMODE, "FD Use Switch:", "none,button,switch");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.SWITCH_TARGET, "FD Switch target:", "all,all borders,Background,background border,bar,bar border,caption,caption border,stripes on,stripes off,line");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.X_FADER_SUBMODE, "FD Exta X fader", "no,absolute,relative");
	sdkAddSettingLineSingle("design", &TFItem.IFader.RELATIVE_SPEED, "FD Relative speed", 0.1f, 1.0f, scLinear, "", "%.2f", 0);
	//
	sdkAddSettingLineCaption("design", "FD Layout");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.ELMT.GLOBAL.ROUND_TYPE, "FD Round type", "not rounded,round,hexagon");
	sdkAddSettingLineSingle("design", &TFItem.IFader.ELMT.GLOBAL.ROUND_SIZE, "FD Round size", 0.01f, 0.25f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IFader.ELMT.GLOBAL.SHOW_BORDER, "FD Show borders");
	sdkAddSettingLineSingle("design", &TFItem.IFader.ELMT.GLOBAL.BORDER_SIZE, "FD Borders size", 0.5f, 20, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FAD_BAR_W, "FD Width redux", 0, 0.50f, scLinear, "ratio", "%.2f", 0);
	//
	sdkAddSettingLineCaption("design", "FD Show");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.BG, "FD Show Fader BG");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.BAR, "FD Show Fader Bar");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.LINE, "FD Show Fader Line");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.CAP, "FD Show Fader Cap");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.STRIPES, "FD Show Fader Stripes");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.SHOW.GRID, "FD Show Fader Grid");
	//
	sdkAddSettingLineCaption("design", "FD Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.COLOR.Global.HUE_MODE, "FD Color Mode", "in Hue / cs SLA,in hue + cs hue / cs SLA,colorset HSLA");
	sdkAddSettingLineInteger("design",&TFItem.IFader.TFPColors->HUE_ROLL, "FD Hue Roll", 0, 360, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.TFPColors->SATURATION_OVERIDE, "FD Saturation Overide", 0, 1.0f, scLinear, "ratio", "%.2f", 0);

	sdkAddSettingLineSingle("design", &TFItem.IFader.TFPColors->LUMINOSITY_CONTRAST, "FD Luminosity Contrast", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.TFPColors->LUMINOSITY_OFFSET, "FD Luminosity Offset", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IFader.APPLY_COLOR_CORRECT, "FD Apply Color Correct to Palette");
	//not stored in preset
	//
	sdkAddSettingLineCaption("design", "FD Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.BG.IN, "FD BG color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.BG_Border.IN, "FD BG Border color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Bar.IN, "FD Bar color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Bar_Border.IN, "FD Bar Border color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Line.IN, "FD Line color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Cap.IN, "FD Cap color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Cap_Border.IN, "FD Cap Border color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Stripes_ON.IN, "FD Stripes on color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Stripes_OFF.IN, "FD Stripes off color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Switch.IN, "FD Switch Color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Xfader.IN, "FD Xfader color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Xfader_Border.IN, "FD Xfader Border color");
	sdkAddSettingLineColor("design", &TFItem.IFader.COLOR.Grid.IN, "FD Grid color");
	sdkAddSettingLineSingle("design", &TFItem.IFader.TFPColors->LUMINOSITY_HIGLIGHT_FORCE, "FD Highlight force", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);

	//
	sdkAddSettingLineCaption("design", "FD FX");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.FX.LEVEL, "FD Rendering type", "flat design,occlusion,shadows,speculars,reflexions,flares");
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.LIGHTING_ROLL, "FD Lighting Angle", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.LIGHTING_DEPTH, "FD Lighting Depth", 0, 2.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.OCLLUSION_OPACITY, "FD Occlusion opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.SHADOW_OPACITY, "FD Shadow opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.SPECULAR_OPACITY, "FD Specular opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.REFLEXIONS_OPACITY, "FD Reflexions opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCombobox("design", &TFItem.IFader.FX.REFLEXIONS_TYPE_IN, "FD Reflexion type", "0,1,2,3");
	sdkAddSettingLineSingle("design", &TFItem.IFader.FX.FLARES_OPACITY, "FD Flares opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "Cap settings");
	sdkAddSettingLineSingle("design", &TFItem.IFader.FAD_BOX_H, "FD Cap height", 0.01f, 0.20f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.FAD_BOX_W, "FD Cap width", 0.51f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IFader.VALUE_IN_CAP, "FD Value in fader cap");
	sdkAddSettingLineBoolean("design", &TFItem.IFader.NAME_IN_CAP, "FD Name in fader cap");
	sdkAddSettingLineCombobox("design", &TFItem.IFader.STRIPES.MODE, "FD Stripes Modes", "center,side,ruler quad,zip,diagonal raise,center raise,twist");
	sdkAddSettingLineInteger("design", &TFItem.IFader.STRIPES.MODULO, "FD Stripes modulo", 2, 16, scLinear, "draw each", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.STRIPES.WIDTH, "FD Stripes width", 0.01f, 2, scLinear, "ratio", "%.1f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IFader.STRIPES.GRADIENT, "FD Stripes gradient", -1.0f, 1.0f, scLinear, "", "%.1f", 0);
	sdkAddSettingLineCaption("design", "FD Line/Grid settings");
	sdkAddSettingLineSingle("design", &TFItem.IFader.LINE_SIZE, "FD Line / grid size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineInteger("design", &TFItem.IFader.GRID.GRID_SUBDIVS, "FD Grid subdivs", 1, 32, scLinear, "n", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IFader.GRID.QUANTIZE_ON_GRID, "FD Quantize value to grid");
		
	///---------------------------------------------------------------------------------------------------------
	// copied & replaced to store




}
void Touch_Fader::BUTTON_SETTINGS()
{
	
	
	sdkAddSettingLineCaption("design", "BT Layout");
	sdkAddSettingLineCombobox("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.ROUND_TYPE, "BT Round type", "not rounded,rounded,hexagon");
	sdkAddSettingLineSingle("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.ROUND_SIZE, "BT Round size", 0.01f, 0.50f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.SHOW_BORDER, "BT Show border");
	sdkAddSettingLineSingle("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.BORDER_SIZE, "BT Border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.Scale_X, "BT scale X", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.ELMT.BUTTON_SHAPE.Scale_Y, "BT scale Y", 0, 1.0f, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "BT Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IButton.COLOR.Global.HUE_MODE, "BT Color Behaviour", "in hue / SLA,shift in hue,full override");
	sdkAddSettingLineInteger("design", &TFItem.IButton.TFPColors->HUE_ROLL, "BT Hue Roll", 0, 360, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.TFPColors->SATURATION_OVERIDE, "BT Saturation Overide", 0, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.TFPColors->LUMINOSITY_CONTRAST, "BT Luminosity Contrast", -1.0f, 1.0, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.TFPColors->LUMINOSITY_OFFSET, "BT Luminosity Offset", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	//sdkAddSettingLineBoolean("design", &(LongBool&)TFItem.IButton.TFPColors->COLOR_CORRECT, "BT Apply Color Correct to Palette");
	sdkAddSettingLineBoolean("design", &TFItem.IButton.APPLY_COLOR_CORRECT, "BT Apply Color Correct to Palette");
	//not in preset
	sdkAddSettingLineCaption("design", "BT Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.OFF.IN, "BT Off color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.ON.IN, "BT On color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.OFF_Border.IN, "BT Off border color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.ON_Border.IN, "BT On border color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.XY_OFF.IN, "BT XY off color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.XY_ON.IN, "BT XY on color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.CONTAINER.IN, "BT Container color");
	sdkAddSettingLineColor("design", &TFItem.IButton.COLOR.CONTAINER_BORDER.IN, "BT Container Border color");

	sdkAddSettingLineCaption("design", "BT Extra XY");
	sdkAddSettingLineCombobox("design", &TFItem.IButton.XYLines_Nb, "BT Exta XY", "none,X,Y,both");
	sdkAddSettingLineSingle("design", &TFItem.IButton.XYLines_Sz, "BT XY size", 0, 50, scLinear, "pixels", "%.1f", 0);

	sdkAddSettingLineCaption("design", "BT Transform Effect");
	sdkAddSettingLineCombobox("design", &TFItem.IButton.TRANSFORM, "BT Transform Effect", "none,slide V,slide H,scale");
	sdkAddSettingLineSingle("design", &TFItem.IButton.REDUX, "BT Transform Amount", 0, 0.50f, scLinear, "", "%.2f", 0);
	
	sdkAddSettingLineCaption("design", "BT FX");
	sdkAddSettingLineCombobox("design", &TFItem.IButton.FX.LEVEL, "BT Rendering type", "flat design,occlusion,shadows,speculars,reflexions,flares");
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.LIGHTING_ROLL, "BT Lighting Angle", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.LIGHTING_DEPTH, "BT Lighting Depth", 0, 2.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.OCLLUSION_OPACITY, "BT Occlusion opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.SHADOW_OPACITY, "BT Shadow opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.SPECULAR_OPACITY, "BT Specular opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.REFLEXIONS_OPACITY, "BT Reflexions opacity", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCombobox("design", &TFItem.IButton.FX.REFLEXIONS_TYPE_IN, "BT Reflexion type", "0,1,2,3,4,5,6,bevel");
	sdkAddSettingLineSingle("design", &TFItem.IButton.FX.FLARES_OPACITY, "BT Flares opacity", 0.01f, 1, scLinear, "", "%.2f", 0);

	// ----------------- BUTTON & SWITCH ITEMS : Switch Caption
//	sdkAddSettingLineCaption("design", "BT Main Output");
//	sdkAddSettingLineCombobox("design", &TFItem.IButton.MAIN_OUTPUT, "BT main out", "X,Y,Z,switch");
}
void Touch_Fader::XYPAD_SETTINGS()
{
	
	sdkAddSettingLineCaption("design", "XY pad");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.RELATIVE_MODE, "XY Absolute / Relative");
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.ENCODER_COEFF_Y, "XY encoder speed Y", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.ENCODER_COEFF_X, "XY encoder speed X", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "XY Show");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.SHOW.BG, "XY Show BG");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.ELMT.BG.SHOW_BORDER, "XY Show BG Border");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.SHOW.CROSS, "XY Show cross");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.SHOW.DOT, "XY Show center dot");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.SHOW.FADERS, "XY Show side faders");
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.SHOW.RECTANGLE, "XY Show rectangle");
	sdkAddSettingLineCaption("design", "XY Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IXYpad.COLOR.Global.HUE_MODE, "XY Color Behaviour", "in hue / SLA,shift in hue,full override");
	sdkAddSettingLineInteger("design", &TFItem.IXYpad.TFPColors->HUE_ROLL, "XY Hue Roll", 0, 360, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.TFPColors->SATURATION_OVERIDE, "XY Saturation Overide", 0, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.TFPColors->LUMINOSITY_CONTRAST, "XY Luminosity Contrast", -1.0f, 1.0, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.TFPColors->LUMINOSITY_OFFSET, "XY Luminosity Offset", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.APPLY_COLOR_CORRECT, "XY Apply Color Correct to Palette");
	sdkAddSettingLineCaption("design", "XY Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.BG.IN, "XY BG Color");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.BG_BORDER.IN, "XY BG Border Color");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.Cross.IN, "XY Cross Color");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.Dot.IN, "XY Dot Color");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.Faders.IN, "XY Faders Color");
	sdkAddSettingLineColor("design", &TFItem.IXYpad.COLOR.Rect.IN, "XY Rect Color");
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.TFPColors->LUMINOSITY_HIGLIGHT_FORCE, "XY Highlight force", -1.0, 1.0, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineCaption("design", "XY Layout");
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.XYPAD_CROSS_SIZE, "XY cross size", 0, 20, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.XYPAD_DOT_SIZE, "XY dot size", 0, 50, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IXYpad.XYPAD_DOT_IS_ROUND, "XY square / circle");
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.XYPAD_FADERS_SIZE, "XY faders size", 0, 50, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.ELMT.BG.BORDER_SIZE, "XY BG Border size", 0.5f, 20, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineCombobox("design", &TFItem.IXYpad.ELMT.BG.ROUND_TYPE, "XY BG Round type", "not rounded,rounded,hexagon");
	sdkAddSettingLineSingle("design", &TFItem.IXYpad.ELMT.BG.ROUND_SIZE, "XY BG Round size", 0.01f, 0.25f, scLinear, "ratio", "%.2f", 0);
	//sdkAddSettingLineCaption("design", "XY Main Output");
	//sdkAddSettingLineCombobox("design", &TFItem.IXYpad.MAIN_OUTPUT, "XYpad main out", "X,Y,Z,switch");


} //end paint XYpad
void Touch_Fader::ENCODER_SETTINGS()
{
	
	sdkAddSettingLineCaption("design", "EN speed");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.RELATIVE_MODE, "EN absolute / relative");	
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.INPUT_TYPE, "EN Input Used", "Y,X,angular");	
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.ENCODER_SPEED, "EN relative speed", 0.01f, 2.0f, scLinear, "", "%.2f", 0);	
	sdkAddSettingLineCaption("design", "EN Sub Mode");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.ENCODER_SUBMODE, "EN Sub mode", "simple enc,button enc,swich enc");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.ENCODER_SUBMODE_BEHAVIOUR, "EN SW Behaviour", "tab reset/last,reset,switch only");
	sdkAddSettingLineCaption("design", "EN Layout");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.ENCODER_LAYOUT_TYPE, "EN Layout style", "ring knob,horizontal bar,vertical bar,color panel,color panel + Fader,only value");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.ENCODER_LEDS_MODE, "EN Draw mode", "normal,reverse,spread,pan,cursor");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.RING_RADIUS, "EN Global radius", 0.01f, 0.6f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.RING_LENGTH, "EN Global length", 0.1f, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "EN Show");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.FILLED_RING, "EN Show filled ring");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.RING_BORDER, "EN Show Ring Border");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.LEDS_RING, "EN Show leds");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.CURSOR_, "EN Show cursor");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.CENTER, "ENShow center");

	sdkAddSettingLineCaption("design", "EN Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.COLOR.Global.HUE_MODE, "EN Color Behaviour", "in hue / SLA,shift in hue,full override");
	sdkAddSettingLineInteger("design", &TFItem.IEncoder.TFPColors->HUE_ROLL, "EN Hue Roll", 0, 360, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.TFPColors->SATURATION_OVERIDE, "EN Saturation Overide", 0, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.TFPColors->LUMINOSITY_CONTRAST, "EN Luminosity Contrast", -1.0f, 1.0, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.TFPColors->LUMINOSITY_OFFSET, "EN Luminosity Offset", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.APPLY_COLOR_CORRECT, "EN Apply Color Correct to Palette");
	//not in preset


	sdkAddSettingLineCaption("design", "EN Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Switch.IN, "En Switch off color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Ring_Off.IN, "EN Ring off color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Ring_On.IN, "EN Ring on color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Ring_Off_Border.IN, "EN Ring off Border color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Ring_On_Border.IN, "EN Ring on Border color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Leds_Off.IN, "EN Leds off color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Leds_On.IN, "EN Leds on color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Cursor.IN, "EN Cursor color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Center_Cap.IN, "EN Center cap color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Center_Cap_Border.IN, "EN Center cap border color");
	sdkAddSettingLineColor("design", &TFItem.IEncoder.COLOR.Panel.IN, "EN Panel color");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.TFPColors->LUMINOSITY_HIGLIGHT_FORCE, "EN Highlight force", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);

	sdkAddSettingLineCaption("design", "EN Ring");
		//sdkAddSettingLineBoolean("design", &TFItem.IEncoder.SHOW.QUAD_RING, "Show Ring");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.QUAD_INNER_RADIUS, "EN Ring inner radius size", 0.01f, 1.0f, scLinear, "%", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.RING_BORDER_SIZE, "EN Ring border size", 0.01f, 40, scLinear, "pixels", "%.2f", 0);

	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.BAR_IS_ROUND, "EN (if bar) rounded");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.BAR_ROUND_SIZE, "EN (if bar) round size", 0, 0.25f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "EN Leds");

	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.LEDS_TYPE, "EN Leds type", "dots,segments,to center,3D");
	sdkAddSettingLineInteger("design", &TFItem.IEncoder.RING_LEDS, "EN nb of leds", 4, 128, scLinear, "leds", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.LEDS_RADIUS, "EN Leds radius size", 0.01f, 1.5f, scLinear, "%", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.LEDS_SIZE, "EN leds size", 0.01f, 40, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.LEDS_GRADIENT, "EN leds on gradient", -1.0f, 1.0f, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "EN Rotating Cursor");
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.CURSOR_TYPE, "EN cursor type", " dot,line,shape,shape fill");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CURSOR_SIZE, "EN cursor size", 0.01f, 40, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CURSOR_LENGTH, "EN cursor radius max", 0.05f, 2.0, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CURSOR_LENGTH_B, "EN cursor radius min", -1.0f, 2.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineInteger("design", &TFItem.IEncoder.SHAPE_NB_POINTS, "EN if shape nb of points", 3, 16, scLinear, "points", 0);

	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.CURSOR_FLIP, "EN flip cursor from/to");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CURSOR_EXTRA_OFFSET, "EN cursor extra offset", -0.5f, 0.5f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.CURSOR_LINKED_TO_RADIUS, "EN cursor size linked to radius");

	sdkAddSettingLineCaption("design", "EN Center / Cap");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CENTER_A_RADIUS, "EN A Radius", 0, 2.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.CENTER_BORDER_SIZE, "EN Center border size", 0.01f, 40, scLinear, "pixels", "%.2f", 0);
	
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.CENTER_DRAW_TYPE, "EN center draw type", "fixed circle,turning n-gon");
	sdkAddSettingLineInteger("design", &TFItem.IEncoder.CENTER_NBP, "EN if turning nb of points", 3, 16, scLinear, "points", 0);
	sdkAddSettingLineCombobox("design", &TFItem.IEncoder.CENTER_LAYER_POSITION, "EN Center Position", "over cursor,under cursor");

	sdkAddSettingLineCaption("design", "EN if colored panel type");

	sdkAddSettingLineSingle("design", &TFItem.IEncoder.PANEL_MIN_LUM, "EN panel min luminosity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.PANEL_MAX_LUM, "EN panel max luminosity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.PANEL_BORDER_SIZE, "EN panel border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.PANEL_IS_ROUND, "EN panel rounded");

	sdkAddSettingLineSingle("design", &TFItem.IEncoder.PANEL_ROUND_SIZE, "EN panel round size", 0.01f, 0.50, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.ADD_PANEL_CURSOR_, "EN add vertical cursor");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.PANEL_CURSOR_WIDTH, "EN panel cursor size", 0.01f, 20, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "EN Visual FX");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.FX_SHOW_REFLEXIONS, "EN show reflexions");
	sdkAddSettingLineBoolean("design", &TFItem.IEncoder.FX_SHOW_SHADOWS, "EN show shadows");
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.SHADOW_X, "EN shadow X ", -0.2f, 0.2f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.SHADOW_Y, "EN shadow Y ", -0.2f, 0.2f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.SHADOW_OPACITY, "EN Shadow opa ", 0.0f, 1.0f, scLinear, "", "%.2f", 0);
	//sdkAddSettingLineBoolean("design", &TFItem.IEncoder.FX_SHOW_SPECULAR, "EN show specular");
	//sdkAddSettingLineSingle("design", &TFItem.IEncoder.SPECULAR_OPACITY, "EN Specular opa ", 0.0, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IEncoder.REFLEXIONS_OPACITY, "EN Reflexions opa ", 0.0f, 1.0f, scLinear, "", "%.2f", 0);
	//sdkAddSettingLineSingle("design", &TFItem.IEncoder.DEBUG_TEST, "debug test ", -0.7, 1.0, scLinear, "", "%.2f", 0);
	//sdkAddSettingLineCaption("design", "EN Main Output");
	//sdkAddSettingLineCombobox("design", &TFItem.IEncoder.MAIN_OUTPUT, "EN Encoder main out", "X,Y,Z,switch");

}
void Touch_Fader::RANGE_FADER_SETTINGS()
{
	
	sdkAddSettingLineCaption("design", "RF Range Fader");
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.RF_ENCODER_COEFF_Y, "RF encoder speed Y", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.RF_ENCODER_COEFF_X, "RF encoder speed X", 0.01f, 1, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "RF Cap");
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.RANGE_BOX_H, "RF Cap height", 0.01f, 0.20f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.RANGE_BOX_W, "RF Cap width", 0.55f, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.RANGE_BAR_REDUX, "RF Bar width redux", 0, 0.20f, scLinear, "ratio", "%.2f", 0);
	//sdkAddSettingLineBoolean("Fader", &RF->SHOW_VALUES, "show range values");
	sdkAddSettingLineCaption("design", "RF Show");
	sdkAddSettingLineBoolean("design", &TFItem.IRangeFader.SHOW.BG, "RF Show BG");
	sdkAddSettingLineBoolean("design", &TFItem.IRangeFader.SHOW.BG_BORDER, "RF Show BG Border");
	sdkAddSettingLineBoolean("design", &TFItem.IRangeFader.SHOW.CAP_BORDER, "RF Show Cap Borders");
	sdkAddSettingLineBoolean("design", &TFItem.IRangeFader.SHOW.STRIPES, "RF Show Stripes");

	sdkAddSettingLineCaption("design", "RF Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IRangeFader.COLOR.Global.HUE_MODE, "RF Color Behaviour", "in hue / SLA,shift in hue,full override");
	sdkAddSettingLineInteger("design", &TFItem.IRangeFader.TFPColors->HUE_ROLL, "RF Hue Roll", 0, 360, scLinear, "", 0);

	sdkAddSettingLineCaption("design", "RF Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.BG.IN, "RF BG color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.BG_BORDER.IN, "RF BG Border color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_UP.IN, "RF Cap UP color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_UP_BORDER.IN, "RF Cap UP Border color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_DWN.IN, "RF Cap DWN color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_DWN_BORDER.IN, "RF Cap DWN Border color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_MID.IN, "RF Cap MID color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.CAP_MID_BORDER.IN, "RF Cap MID Border color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.BAR.IN, "RF Bar color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.BAR_BORDER.IN, "RF Bar Border color");
	sdkAddSettingLineColor("design", &TFItem.IRangeFader.COLOR.STRIPES.IN, "RF Stripes color");

	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.TFPColors->LUMINOSITY_HIGLIGHT_FORCE, "RF Hilight Strengh", -1.0f, 1.0f, scLinear, "ratio", "%.1f", 0.2f);

	sdkAddSettingLineCaption("design", "RF Stripes settings");
	sdkAddSettingLineInteger("design", &TFItem.IRangeFader.STRIPES.MODULO, "RF stripes modulo", 2, 16, scLinear, "draw each", 0);
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.STRIPES.WIDTH, "RF stripes width", 0.01f, 2, scLinear, "ratio", "%.1f", 0);

	sdkAddSettingLineCaption("design", "RF Round settings");
	sdkAddSettingLineCombobox("design", &TFItem.IRangeFader.ELMT.GLOBAL.ROUND_TYPE, "RF Round type", "not rounded,round,hexagon");
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.ELMT.GLOBAL.ROUND_SIZE, "RF Round size", 0.01f, 0.25f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IRangeFader.ELMT.GLOBAL.BORDER_SIZE, "RF Borders size", 0.5f, 20, scLinear, "pixels", "%.1f", 0);

	//sdkAddSettingLineCaption("design", "RF Input Mode");
	//sdkAddSettingLineBoolean("design", &TFItem.IRangeFader.WIDTH_INPUT_MODE, "XY=Up-Dwn | ZW=Mid-Width");


}
void Touch_Fader::LISTBOX_SETTINGS()
{


	sdkAddSettingLineCaption("design", "LB Sub Mode");
	sdkAddSettingLineCombobox("design", &TFItem.IListBox.INERTIA_MODE, "LB scroll type", "slider,inertia");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.INERTIA_FACTOR, "LB Inertia factor", 0.01f, 0.5f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineInteger("design", &TFItem.IListBox.INERTIA_DURATION_IN, "LB Inertia Duration", 100, 600, scLinear, "blocs", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_ALLOW_SLIDE, "LB allow slide selection in slider mode");
	sdkAddSettingLineCaption("design", "LB Zoom");
	sdkAddSettingLineInteger("design", &TFItem.IListBox.NB_LIST_DISPLAYED, "LB nb of displayed items", 1, LB_DISPLAYED_MAX, scLinear, "items", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_USE_X_ZOOM, "LB control zoom via X axis");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_X_ZOOM_SPEED, "LB X zoom speed", 0.01f, 1, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ZOOM_LINE_WIDTH, "LB zoom line size", 0, 50, scLinear, "pixels", "%.1f", 0);

	sdkAddSettingLineCaption("design", "LB Show");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_BORDER, "LB show border");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_HEADER, "LB show header");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_HEADER_BORDER, "LB show header border");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_SCROLLER, "LB show scroller");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_SCROLLER_BORDER, "LB show scroller border");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_SCROLLER_VALUE, "LB show scroller value");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_SCROLLER_CONTAINER, "LB show scroller container");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_ZOOM, "LB show zoom");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_ITEM_BORDER, "LB show item border");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_ITEMS_NAMES, "LB show items names");
	sdkAddSettingLineCombobox("design", &TFItem.IListBox.SHOW.ITEMS_NUMBERS_TYPE, "LB Show Items Id Number", "none,visual,ordinal pre-sort");
	sdkAddSettingLineCombobox("design", &TFItem.IListBox.SHOW.ITEMS_ASSIGNED_VALUE, "LB Show Items Assigned Value", "none,int,.x,.xx,.xxx");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_PULSE, "LB Show Light Pulse");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_KH, "LB hide items outside range ");
	sdkAddSettingLineCaption("design", "LB X fader");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_ITEM_XFADER, "LB Use item X fader");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.SHOW.LIST_ITEM_XFADER_VALUE, "LB Show item X fader value");

	sdkAddSettingLineCaption("design", "LB Colors Managment");
	sdkAddSettingLineCombobox("design", &TFItem.IListBox.COLOR.Global.HUE_MODE, "LB Color Behaviour", "in hue  / SLA,shift in hue,full override");
	sdkAddSettingLineInteger("design", &TFItem.IListBox.TFPColors->HUE_ROLL, "LB Hue Roll", 0, 360, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.TFPColors->SATURATION_OVERIDE, "LB Saturation Overide", 0, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.TFPColors->LUMINOSITY_CONTRAST, "LB Luminosity Contrast", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.TFPColors->LUMINOSITY_OFFSET, "LB Luminosity Offset", -1.0f, 1.0f, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.APPLY_COLOR_CORRECT, "LB Apply Color Correct to Palette");
	//not stored

	sdkAddSettingLineCaption("design", "LB Colors Set");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Zoom_Line.IN, "LB Zoom line color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Xfader.IN, "LB Xfader color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Xfader_Border.IN, "LB Xfader Border color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Xfader_Value.IN, "LB Xfader Value color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Item_Off.IN, "LB Item Off color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Item_On.IN, "LB Item On color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Item_Border_Off.IN, "LB Item Border Off color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Item_Border_On.IN, "LB Item Border On color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Item_Name.IN, "LB Item Name color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Border.IN, "LB Border color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Scroller.IN, "LB Scroller color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Scroller_Border.IN, "LB Scroller Border color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Scroller_Shadow.IN, "LB Scroller Shadow color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Scroller_Container.IN, "LB Scroller Container color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Scroller_Container_Border.IN, "LB Scroller Container Border color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Header.IN, "LB Header color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.Header_Border.IN, "LB Header Border color");
	sdkAddSettingLineColor("design", &TFItem.IListBox.COLOR.KH.IN, "LB hider color");

	sdkAddSettingLineCaption("design", "LB Items");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.Alphabetical_Sort, "LB Alphabetical Sort");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_LUM_MOD, "LB Pattern modulo luminosity", 0, 0.5f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_ITEM_IS_ROUND, "LB item rounded ");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ITEM_ROUND_SIZE, "LB item round size", 0.01f, 0.50f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ITEM_BORDER_SIZE, "LB item border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ITEM_NAME_SIZE, "LB item text size", 0.01f, 1, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ITEM_GAP, "LB item V gap", 0, 0.1f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_ITEM_H_GAP, "LB item H gap", 0, 0.1f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_PULSE_AMOUNT, "LB Light Pulse Amount", 0.01f, 1.0f, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "LB Global border");

	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_BORDER_SIZE, "LB border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_BORDER_IS_ROUND, "LB rounded border");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_BG_ROUND_SIZE, "LB border round size", 0.01f, 0.50f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "LB Scroller");

	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_SCROLLER_W, "LB scroller width", 0.1f, 0.5f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_SCROLLER_H_IN, "LB scroller height", 0.02f, 0.5f, scLinear, "", "%.2f", 0);

	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_SCROLLER_BORDER_SIZE, "LB scroller border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_SCROLLER_IS_ROUND, "LB scroller rounded ");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_SCROLLER_ROUND_SIZE, "LB scroller round size", 0.01f, 0.50, scLinear, "", "%.2f", 0);

	sdkAddSettingLineCaption("design", "LB Scroller Container");

	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_USE_PAGES, "LB container absolute / page mode");
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_SCROLLER_FIT_PAGE, "LB override & fit page");

	sdkAddSettingLineCaption("design", "LB LB Header");

	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_HEADER_NAME_SIZE, "LB header text size", 0.01f, 1, scLinear, "ratio", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_HEADER_W, "LB header width", 0.1f, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_HEADER_H_IN, "LB header height", 0.02f, 0.5f, scLinear, "", "%.2f", 0);

	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_HEADER_BORDER_SIZE, "LB header border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IListBox.LIST_HEADER_IS_ROUND, "LB header rounded ");
	sdkAddSettingLineSingle("design", &TFItem.IListBox.LIST_HEADER_ROUND_SIZE, "LB header round size", 0.01f, 0.50f, scLinear, "", "%.2f", 0);

}
void Touch_Fader::MATRIX_SETTINGS()
{
	/*sdkAddSettingLineCaption("design", "MX Matrix");
	sdkAddSettingLineInteger("design", &TFItem.IMatrix.MX.nb_cols, "MX nb cols", 1, 16, scLinear, "cols", 0);
	sdkAddSettingLineInteger("design", &TFItem.IMatrix.MX.nb_rows, "MX nb rows", 1, 8, scLinear, "rows", 0);
	sdkAddSettingLineCaption("design", "MX Sub Mode");
	sdkAddSettingLineCombobox("design", &TFItem.IMatrix.MODE, "MX Mode", "momentary,toggle,fade,slider button,slider Switch");
	sdkAddSettingLineCombobox("design", &TFItem.IMatrix.TOGGLE_SUB_MODE, "MX Toggle sub mode", "normal,exclusive,1 per col,1 per row");
	sdkAddSettingLineCombobox("design", &TFItem.IMatrix.SLIDE_MODE, "MX Slide mode", "none,both,only in col,only in row");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.RELATIVE_MODE, "MX absolute / relative ");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.ENCODER_COEFF_Y, "MX encoder speed Y", 0.1f, 4.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.ENCODER_COEFF_X, "MX encoder speed X", 0.1f, 4.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.SHOW_FADERS, "show faders");
	sdkAddSettingLineCaption("design", "MX Sliders options");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.SHOW_Y_FADER, "MX show Y fader");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.SHOW_X_FADER, "MX show X fader");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.AUTO_RESET_Y, "MX Reset Y on Release");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.Y_RESET_VAL, "MX Y reset value", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.AUTO_RESET_X, "MX Reset X on Release");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.X_RESET_VAL, "MX X reset value", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.FADERS_OPACITY, "MX faders opacity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.FADERS_LUMINOSITY, "MX faders luminosity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.FADERS_SATURATION, "MX faders saturation", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.FADERS_LINE_OPACITY, "MX faders line opacity", 0, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.FADERS_LINE_SIZE, "MX line size", 0, 50, scLinear, "pixels", "%.1f", 0);
	sdkAddSettingLineCaption("design", "MX cell options");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.MX.gap, "MX cells gap", 0, 0.1f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.MX.SHOW_NAME, "MX show cell name");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.NAME_SIZE_MULTIPLIER, "MX Name size multiplier", 0.01f, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.MX.SHOW_INDEX, "MX show cell index");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.MX.SHOW_X_VAL, "MX show X value");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.MX.SHOW_Y_VAL, "MX show Y value");
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.MX.SHOW_Z_VAL, "MX show Z value");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.VALUE_SIZE_MULTIPLIER, "MX Values size multiplier", 0.01f, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.ROUND_CELL, "Round Cell");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.ROUND_SIZE, "MX round size", 0.01f, 0.50f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_OFF_LUMINOSITY, "MX Cell off luminosity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_SATURATION, "MX Cell saturation", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_OPACITY, "MX Cell opacity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineBoolean("design", &TFItem.IMatrix.SHOW_CELL_BORDER, "MX show cell border");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.BORDER_SIZE, "MX cell border size", 0.5f, 20, scLinear, "pixels", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_BORDER_LUMINOSITY, "MX Cell border luminosity", 0, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_BORDER_SATURATION, "MX Cell border saturation", 0, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.CELL_BORDER_OPACITY, "MX Cell border opacity", 0, 1.0f, scLinear, "", "%.2f", 0);
	sdkAddSettingLineCaption("design", "MX Selected Cell");
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.SELECTED_HILIGHT, "MX selected higlight force", -1.0f, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineInteger("design", &TFItem.IMatrix.SELECTED_HUE_OFFSET, "MX selected hue offset", -180, 180, scLinear, "", 0);
	sdkAddSettingLineSingle("design", &TFItem.IMatrix.SELECTED_SAT_OFFSET, "MX selected sat offset", -1.0, 1.0, scLinear, "", "%.2f", 0);
	sdkAddSettingLineColor("design", &TFItem.IMatrix.ON_COLOR, "MX ON color");
	sdkAddSettingLineColor("design", &TFItem.IMatrix.OFF_COLOR, "MX OFF color");
	sdkAddSettingLineInteger("design", &TFItem.IMatrix.CELLS_HUE_OFFSET, "cells auto hue offset", -360, 360, scLinear, "", 0);
*/
}
///////////////////////////////////////
void Touch_Fader::onProcess()
{
	if (MS_CLIC) { MS_CLIC_count++;  }
	if (MS_CLIC_count == 1){ MS_CLIC = FALSE; MS_CLIC_count = -1;}

	if (MS_UP){ MS_UP_count++; }
	if (MS_UP_count == 1){ MS_UP = FALSE; 
	MS_UP_count = -1; }

	//-----------------------midi stuff
	if (MIDI_ENABLED)
	{
		if (count_midi > -1)
		{
			count_midi++;

			if (count_midi > 1)
			{
				UPD_MIDI_OUT(); //will copy current values to compare values.
				count_midi = -1; //and reset to sleeping state
			}
		}
	}//end midi
	//---------------------------------------

	if (TFParams->PULSE) { TFParams->PULSE_count++;
	TFParams->PULSE_RAMP = (float)TFParams->PULSE_count / (float)TFParams->PUSE_DURATION;
		sdkRepaintPanel(); }
	if (TFParams->PULSE_count == TFParams->PUSE_DURATION) { TFParams->PULSE = FALSE; TFParams->PULSE_count = -1; sdkRepaintPanel(); }


	if (MODE == MListBox) ///LB
	{
		if (TFItem.IListBox.INERTIA_MODE)  // list box inertia
		{
			if (count_inertia > -1) { count_inertia++; }
			if (count_inertia > TFItem.IListBox.INERTIA_DURATION) { count_inertia = -1; }

			if (count_inertia > -1 && count_inertia <= TFItem.IListBox.INERTIA_DURATION)
			{
				normalise_count = (CLAMP01(1 - (float(count_inertia) / float(TFItem.IListBox.INERTIA_DURATION))));
				TFItem.IListBox.UPD_SCROLL(float(TFItem.IListBox.last_scroll_delta*normalise_count*-TFItem.IListBox.INERTIA_FACTOR));
				sdkRepaintPanel();
			}
		}//end LB inertia
	}//lb

//	if (TFParams->MSDWN)
//	{
//		sdkProcessRecord(TFParams->ITEM_VALUE_X, TFParams->ITEM_VALUE_Y, TFParams->ITEM_VALUE_Z);
//	}

}//end process
//////////////////////////////////////////////////
bool Touch_Fader::XISMAIN()
{
	float DX = abs(TFParams->DELTA_X);
	float DY = abs(TFParams->DELTA_Y) * TFParams->CANVAS_RATIO;

	//////////////////////////// delta sampling /compare
	if ((DX != 0 || DY != 0) && TFParams->MSDWN)
	{
		DELTA_MEM_ID++;
		DELTA_MEM_ID = (int)fmod(DELTA_MEM_ID, DELTA_ARRAY_SIZE);

		if (DX > DY)
		{
			DELTA_MEM[DELTA_MEM_ID] = DX;
		}
		else { DELTA_MEM[DELTA_MEM_ID] = -DY; }

		DELTA_MEM_SUM = 0;
		for (int i = 0; i < DELTA_ARRAY_SIZE; i++)
		{
			DELTA_MEM_SUM += DELTA_MEM[i];
		}

	}// end delta
	///////////////////////////////////////
	if (MS_CLIC)       /// reset delta mem
	{
		memset(DELTA_MEM, 0, sizeof DELTA_MEM);
		//for (int i = 0; i < DELTA_MEM_SZ; i++){DELTA_MEM[0];}
		DELTA_MEM_SUM = 0;
	}
	//////////////////////////////
	return DELTA_MEM_SUM >= 0;
	//sdkTracePrecision(DELTA_MEM_SUM>0);
	////////////////////////////////////////
}
////////////////////////////////////////////////////////////////////////////
void Touch_Fader::UPD_OUT()
{
	//////////////////////////////////////////////////// get normalised items values to out structure for easier manips
	//OUT_VAL.W_01	  = TFParams->ITEM_VALUE_W;
	OUT_VAL.X_01	  = TFParams->ITEM_VALUE_X;
	OUT_VAL.Y_01	  = TFParams->ITEM_VALUE_Y;
	OUT_VAL.Z_01	  = TFParams->ITEM_VALUE_Z;
	OUT_VAL.SWITCH_01 = TFParams->ITEM_VALUE_SWITCH;
						
	/////////////////////////////////// re range normalised
	//OUT_VAL.W_RANGED = RANGE_OUT(OUT_VAL.W_01);
	OUT_VAL.X_RANGED = RANGE_OUT(OUT_VAL.X_01);
	OUT_VAL.Y_RANGED = RANGE_OUT(OUT_VAL.Y_01);
	OUT_VAL.Z_RANGED = RANGE_OUT(OUT_VAL.Z_01);
	OUT_VAL.SWITCH_RANGED = RANGE_OUT(OUT_VAL.SWITCH_01);
	/////////////////////////////////if asked format float precision
	//FORMAT_FLOAT_DECA(&OUT_VAL.W_RANGED, TFParams->VALUE_PRECISION_DECA);
	FORMAT_FLOAT_DECA(&OUT_VAL.X_RANGED, TFParams->VALUE_PRECISION_DECA);
	FORMAT_FLOAT_DECA(&OUT_VAL.Y_RANGED, TFParams->VALUE_PRECISION_DECA);
	FORMAT_FLOAT_DECA(&OUT_VAL.Z_RANGED, TFParams->VALUE_PRECISION_DECA);
	FORMAT_FLOAT_DECA(&OUT_VAL.SWITCH_RANGED, TFParams->VALUE_PRECISION_DECA);
	
	/////////////////////////////////  write ranged outs
	//sdkSetEvtData(pOutW, OUT_VAL.W_RANGED);
	sdkSetEvtData(pOut_X, OUT_VAL.X_RANGED);
	sdkSetEvtData(pOut_Y, OUT_VAL.Y_RANGED);
	sdkSetEvtData(pOut_Z, OUT_VAL.Z_RANGED);
	sdkSetEvtData(pOut_SW, OUT_VAL.SWITCH_01); //--switch remain bool
	
	//////////////
						//////////////////////////////////// matrix rounting for main out
	switch (TFParams->ITEM_MAIN_OUT)
	{
	case 0:	OUT_VAL.MAIN_01 = OUT_VAL.X_01;      OUT_VAL.MAIN_RANGED = OUT_VAL.X_RANGED; break; //route X to main out
	case 1: OUT_VAL.MAIN_01 = OUT_VAL.Y_01;      OUT_VAL.MAIN_RANGED = OUT_VAL.Y_RANGED; break;//route Y to main out
	case 2: OUT_VAL.MAIN_01 = OUT_VAL.Z_01;      OUT_VAL.MAIN_RANGED = OUT_VAL.Z_RANGED; break;//route Z to main out
	case 3: OUT_VAL.MAIN_01 = OUT_VAL.SWITCH_01; OUT_VAL.MAIN_RANGED = OUT_VAL.SWITCH_RANGED; break;//route switch to main out
	//case 4: OUT_VAL.MAIN_01 = OUT_VAL.W_01;		OUT_VAL.MAIN_RANGED = OUT_VAL.W_RANGED; break;//route W to main out
	default:break;
	}

	//sdkSetEvtData(pOutW, OUT_VAL.MAIN_RANGED);
	////////////////////////////////////////////////////////////////////////////////////////////////////


	if (MODE == MListBox)//listBox
	{	
		//sdkSetEvtData(pOutW01, OUT_VAL.Y_01);
		//sdkSetEvtData(pOutW, RANGE_OUT(OUT_VAL.Y_01));

		int LB_SEL = TFItem.IListBox.LIST_ITEM_SEL;
		TFParams->ITEM_VALUE_W = (float)LB_SEL; //for preset recall
		//int PID = 21; ///////////////////////////////////////////////////////<-params id of lbtext out for repaint
		//list[count].id
		sdkSetEvtData(pOut_LB_Selection, (TPrecision)LB_SEL); // actual 'visual' id
		sdkSetEvtData(pOut_LB_Original_ID, (TPrecision)LIST_ITEMS.at(LB_SEL).id); // if sorted 'original' id
		sdkSetEvtPChar(pOut_LB_Text, AnsiCharPtr(LIST_ITEMS.at(LB_SEL).name.c_str())); // selected name
		sdkSetEvtData(pOut_LB_Assigned_Float, LIST_ITEMS.at(LB_SEL).assigned_value); // if was assigned float value
		sdkRepaintParam(17);         ///////////////////////////////////////////////////////////////// repaint prams lb text not always refreshing
		sdkSetEvtData(pOut_LB_Zoom, TFItem.IListBox.NB_LIST_DISPLAYED_);
		sdkSetEvtData(pOut_LB_Scroll, TFItem.IListBox.LIST_SCROLL_VAL);
		//--custom tweak for midi to xyz x being x fader
		OUT_VAL.Y_RANGED = CLAMP((float)LIST_ITEMS.at(LB_SEL).id,0.0f,127.0f); //for sorted id as midi out 
		sdkSetEvtData(pOut_Y, OUT_VAL.Y_RANGED);
		//OUT_VAL.W_RANGED = CLAMP((float)LB_SEL, 0.0f, 127.0f); //for id as midi out 
		//sdkSetEvtData(pOutW, OUT_VAL.W_RANGED);
		OUT_VAL.Z_RANGED = CLAMP(LIST_ITEMS.at(LB_SEL).assigned_value,0.0f,127.0f); // if was assigned float value
		sdkSetEvtData(pOut_Z, OUT_VAL.Z_RANGED);
		OUT_VAL.SWITCH_01 = TFParams->MSDWN;
		sdkSetEvtData(pOut_SW, OUT_VAL.SWITCH_01);
		OUT_VAL.SWITCH_RANGED = (RE_RANGE_NORM_IN(OUT_VAL.SWITCH_01,TFParams->OUT_MIN,TFParams->OUT_MAX));

	}
	else if (MODE == MMatrix)//Matrix
	{
		sdkSetEvtData(pOut_LB_Selection, (TPrecision)TFItem.IMatrix.MX.cell);
		sdkSetEvtPChar(pOut_LB_Text, AnsiCharPtr(TFItem.IMatrix.MX_CELL_STRING.c_str()));
	}

	UPD_NAME(); // to do: could be optimized (only if swich status changed)

	//------------midi
//	W_CC.VALUE = (int)OUT_VAL.W_RANGED;
	X_CC.VALUE = (int)OUT_VAL.X_RANGED;
	Y_CC.VALUE = (int)OUT_VAL.Y_RANGED;
	Z_CC.VALUE = (int)OUT_VAL.Z_RANGED;
	SW_CC.VALUE = (int)OUT_VAL.SWITCH_RANGED;

	UPD_MIDI_OUT();

	//-------------

	sdkRepaintPanel();

}//end upd out
////////////////////////////////
void Touch_Fader::UPD_MIDI_LEARN()
{
	if (MIDI_LEARN)  //only if midi learn
	{
		LEARN_MIDI_CODE = sdkGetEvtArrayMidi(pMidi_In, 0); //get input message

		if (LEARN_MIDI_CODE.Msg == MIDI_CONTROLCHANGE) //only if is a CC
		{
			//if (W_CC.LEARN) { W_CC.CHANNEL = LEARN_MIDI_CODE.Channel;	W_CC.NUMBER = LEARN_MIDI_CODE.Data1; W_CC.LEARN = FALSE; } //apply to cc and quit learning mode
			if (X_CC.LEARN) { X_CC.CHANNEL = LEARN_MIDI_CODE.Channel;	X_CC.NUMBER = LEARN_MIDI_CODE.Data1; X_CC.LEARN = FALSE; } //apply to cc and quit learning mode
			if (Y_CC.LEARN) { Y_CC.CHANNEL = LEARN_MIDI_CODE.Channel;	Y_CC.NUMBER = LEARN_MIDI_CODE.Data1; Y_CC.LEARN = FALSE; }
			if (Z_CC.LEARN) { Z_CC.CHANNEL = LEARN_MIDI_CODE.Channel;	Z_CC.NUMBER = LEARN_MIDI_CODE.Data1; Z_CC.LEARN = FALSE; }
			if (SW_CC.LEARN) { SW_CC.CHANNEL = LEARN_MIDI_CODE.Channel;	SW_CC.NUMBER = LEARN_MIDI_CODE.Data1; SW_CC.LEARN = FALSE; }
		}
	}

}
/////////////////////////////////
void Touch_Fader::UPD_MIDI_OUT()
{
	
	if (MIDI_ENABLED)
	{
		bool has_chg = FALSE;

		NB_USED_CC = 0;
		sdkSetEvtMaxSize(pMidi_Out, NB_MAX_CC);


		 ///----------------
		for (int i = 0; i < NB_MAX_CC; i++)
		{
			if (count_midi >1)     ////// oncve value were set at bloc 0, at bloc 1 copy current values to comparevalues for next check if diffs
			{
				ALL_CC_OUT.at(i)->VALUE_LAST = ALL_CC_OUT.at(i)->VALUE;
			}
			//---------------------
			if ( (ALL_CC_OUT.at(i)->ENABLED) && (ALL_CC_OUT.at(i)->VALUE - ALL_CC_OUT.at(i)->VALUE_LAST != 0) ) //if any value changed
			{
				has_chg = TRUE;
				count_midi = 0;
				NB_USED_CC++;

				OUT_MIDI_CODE.Channel = ALL_CC_OUT.at(i)->CHANNEL;
				OUT_MIDI_CODE.Msg = MIDI_CONTROLCHANGE;
				OUT_MIDI_CODE.Data1 = ALL_CC_OUT.at(i)->NUMBER;

				OUT_MIDI_CODE.Data2 = int(CLAMP((float)ALL_CC_OUT.at(i)->VALUE,0.0f,127.0f));

				//MidiCodes_OUT.at(NB_USED_CC - 1 ) = MidiCode;

				sdkSetEvtArrayMidi(pMidi_Out, NB_USED_CC - 1, OUT_MIDI_CODE);
			}

		}//loop

		sdkSetEvtSize(pMidi_Out, NB_USED_CC);
		//--------------------------------
		if (!has_chg) // if no changed set size back to 0 to avaid unesuful msg emissions
		{
			sdkSetEvtSize(pMidi_Out, 0);
		}
	}
	
}
////////////////////////////////
void Touch_Fader::UPD_MIDI_IN()
{

	if (MIDI_ENABLED)  // will process only if midi feature is enabled 
	{

		UPD_MIDI_LEARN();

		int IN_MIDI_SIZE = 0;
		float CC_VALUE = 0;


		IN_MIDI_SIZE = sdkGetEvtSize(pMidi_In); //first get nb of message in input midi

		for (int i = 0; i < IN_MIDI_SIZE; i++)  /// scan incoming midi mess
		{
			IN_MIDI_CODE = sdkGetEvtArrayMidi(pMidi_In, i);   // get mess

			if (IN_MIDI_CODE.Msg == MIDI_CONTROLCHANGE)  //check if it's a CC type
			{
				CC_VALUE = RE_RANGE_NORM_OUT(IN_MIDI_CODE.Data2, 0, 127);

				//if ((W_CC.ENABLED) && (IN_MIDI_CODE.Channel == W_CC.CHANNEL) && (IN_MIDI_CODE.Data1 == W_CC.NUMBER))  //if matching cc num && channel
				//{
				//	TFParams->ITEM_VALUE_W = CC_VALUE; TFItem.onInputChange(MODE);  //pass value to item & request update
				//}

				if ( (X_CC.ENABLED) && (IN_MIDI_CODE.Channel == X_CC.CHANNEL) && (IN_MIDI_CODE.Data1 == X_CC.NUMBER))  //if matching cc num && channel
				{
					TFParams->ITEM_VALUE_X = CC_VALUE; TFItem.onInputChange(MODE);  //pass value to item & request update
				}
				if ( (Y_CC.ENABLED) && (IN_MIDI_CODE.Channel == Y_CC.CHANNEL) && (IN_MIDI_CODE.Data1 == Y_CC.NUMBER))  //if matching cc num && channel
				{
					TFParams->ITEM_VALUE_Y = CC_VALUE; TFItem.onInputChange(MODE);  //pass value to item & request update
				}
				if ( (Z_CC.ENABLED) && (IN_MIDI_CODE.Channel == Z_CC.CHANNEL) && (IN_MIDI_CODE.Data1 == Z_CC.NUMBER))  //if matching cc num && channel
				{
					TFParams->ITEM_VALUE_Z = CC_VALUE; TFItem.onInputChange(MODE); //pass value to item & request update
				}
				if ( (SW_CC.ENABLED) && (IN_MIDI_CODE.Channel == SW_CC.CHANNEL) && (IN_MIDI_CODE.Data1 == SW_CC.NUMBER))  //if matching cc num && channel
				{
					TFParams->ITEM_VALUE_SWITCH = CC_VALUE; TFItem.onInputChange(MODE);  //pass value to item & request update
				}


			}

			UPD_OUT(); sdkRepaintPanel();
		}

	}


}
//////////////////////////////////
void Touch_Fader::UPD_NAME()
{
	if (SHOW_NAME)  // text update
	{
		if (TFParams->ITEM_VALUE_SWITCH) { item_name = item_name_on; NAME_COLOR = TFParams->NAME_ON_COLOR; }
		else { item_name = item_name_off; NAME_COLOR = TFParams->NAME_OFF_COLOR; }
		sdkRepaintPanel();
	}
}// upd name
///////////////////////////////////////////////////////
void Touch_Fader::onResize(float W, float H)
{RELAYOUT(W,H);}
////////////////////////////////////////////////// recaliber & graphical Layout settings
void Touch_Fader::RELAYOUT(float W, float H)
{
	CANVAS_H = H;
	CANVAS_W = W;
	CANVAS_S = fmaxf(H, W);

	TFParams->CANVAS_H = CANVAS_H;
	TFParams->CANVAS_W = CANVAS_W;
	TFParams->CANVAS_S = CANVAS_S;
	TFParams->CANVAS_RATIO = H / W;  /// pixels
	TFItem.onSettingsHasChanged(MODE);

	sdkRepaintPanel();	
}
//////////////////////////////////////////////////////////
void Touch_Fader::IMPORT_COMMA_TEXT()
{
	istringstream ss(COMMA_IN);     // input 'comma text string' goes to string stream
	string token;                   // string var used to store splitted comma string elements string
	char marker_comma = ',';       // char marker to search for split comma text
	char marker_assign = '=';      // char marker to search for if user assigned value  to list element 
	char marker_space = ' ';       // space character marker to remove to correctly convert float to string

	LIST_ITEMS.clear();                  // first clear the vector 
	LIST_ITEMS.resize(MAX_LIST_ITEMS);  // then we assign max allowed size to vector
	

	int count = 0;                     //count will be pos variable iterating, starting at 0
	while (std::getline(ss, token, marker_comma) && count < MAX_LIST_ITEMS)   // get each 'line' of comma text separated by defined marker
	{
		LIST_ITEMS.at(count).id = count;                                      // assign iteration to ID (to keep trace of original id once sorted) 
		vector<string> parts = split(token, marker_assign);                // if in splitted element an assign marker is found, return a cextor of spitted between marker sub elements
		LIST_ITEMS.at(count).name = parts[0];                                 // first part is kept as name
		if (parts.size() > 1 && parts[1] != "")                            //if more than 1 part (there is an assign marker), and 2nd non null then is supposed to be an assignated value as a string, so process to convert
		{
			parts[1].erase(std::remove(parts[1].begin(), parts[1].end(), marker_space), parts[1].end());    // removing eventual spaces (space defined marker char) of second part                       
			LIST_ITEMS.at(count).assigned_value = (float)atof(parts[1].c_str());         // then convert second part string to a floating point  value
			//sdkTracePrecision(LIST_ITEMS[count].assigned_value);             // trace for debug
		}
		count++;                             // once done loop iteration var increments
	}// end of loop

	LIST_ITEMS.resize(count);                // max found count in max allowed range in done loop become 'final' size of list and vector
	TFItem.IListBox.NB_LIST_ITEMS = count ;   // pass it to listbox code to be aware of limits

	if (TFItem.IListBox.Alphabetical_Sort)
	{
		sort(LIST_ITEMS.begin(), LIST_ITEMS.end(), sortByName); // if requested triggs an alphabetical  sort function by element.name
	}

	sdkRepaintPanel();	  //ask for redraw
}//end import commatext
//////////////////////////////////////////////////////////////
string Touch_Fader::VALUE_TO_STRING(float in_val)
{
	return sdkFloatToString(in_val, TFParams->VALUE_PRECISION, 0);
}
///////////////////////////////////////////////////////
float Touch_Fader::RANGE_OUT(float in_val)
{
	return RE_RANGE_NORM_IN(in_val, TFParams->OUT_MIN, TFParams->OUT_MAX);
}
/////////////////////////////////////////////////////
void Touch_Fader::UPD_QUANTIZE()
{   // for quantize or float formattings
	TFParams->QUANTIZE_INT = (int)((TFParams->OUT_MAX - TFParams->OUT_MIN)*(pow(10, TFParams->VALUE_PRECISION)));
	TFParams->VALUE_PRECISION_DECA = (int)pow(10, TFParams->VALUE_PRECISION); //prepares a deca value according to prec 1,10,100,1000 ect for rounding formating floats commands
}
///////////////////////////// PRESETS /////////////////////////////
int Touch_Fader::onGetChunkLen(LongBool Preset)
{
	int ChunkLen = 20;// preset save and recall 5 floats * 4 bytes:  W,X,Y,Z, and Switch values
	return ChunkLen;
}
void Touch_Fader::onGetChunk(void* chunk, LongBool Preset)
{
	//prepare datas to array
	PRESET_MEM[0] = TFParams->ITEM_VALUE_W;
	PRESET_MEM[1] = TFParams->ITEM_VALUE_X;
	PRESET_MEM[2] = TFParams->ITEM_VALUE_Y;
	PRESET_MEM[3] = TFParams->ITEM_VALUE_Z;
	PRESET_MEM[4] = TFParams->ITEM_VALUE_SWITCH;
	//
	memcpy(chunk, &(PRESET_MEM), 20);// sizeof PRESET_MEM);
}
void Touch_Fader::onSetChunk(const void* chunk, int sizeInBytes, LongBool Preset)
{
	memcpy(&(PRESET_MEM), chunk, 20);

	 TFParams->ITEM_VALUE_W = PRESET_MEM[0];
	 TFParams->ITEM_VALUE_X = PRESET_MEM[1];
	 TFParams->ITEM_VALUE_Y = PRESET_MEM[2];
	 TFParams->ITEM_VALUE_Z = PRESET_MEM[3];
	 TFParams->ITEM_VALUE_SWITCH = PRESET_MEM[4];

	 TFItem.onPreset(MODE);
	 UPD_OUT();
	 sdkRepaintPanel();
	//get in data demultiplex
}
////////////////////////////////////////////////////////////////////////////////////////////
void Touch_Fader::RECORD_XYZ()
{
	if (MODE == MSwitch || MODE == MFader || MODE == MEncoder)   //rec switch status . to do: get info if item is in button or switch type
	{
		sdkProcessRecord(TFParams->ITEM_VALUE_X, TFParams->ITEM_VALUE_Y, TFParams->ITEM_VALUE_SWITCH);
	}
	else  // otherwise rec basically mouse down or assignated Z value
	{
		sdkProcessRecord(TFParams->ITEM_VALUE_X, TFParams->ITEM_VALUE_Y, TFParams->ITEM_VALUE_Z);
	}

}
void Touch_Fader::onSetRecordedValue(TPrecision 	X, TPrecision 	Y, TPrecision 	Z)
{
	TFParams->ITEM_VALUE_X = X;
	TFParams->ITEM_VALUE_Y = Y;
	TFParams->ITEM_VALUE_Z = Z;

	if (MODE != MRangeFader) //get switch exept for rf
	{
		if (MODE == MButton)
		{
			TFParams->ITEM_VALUE_SWITCH = trunc(Z);
		}
		else// if //(MODE == MSwitch)
		{
			if (Z == 0)
			{
				TFParams->ITEM_VALUE_SWITCH = 0;
			}
			else if (Z == 1)
			{
				TFParams->ITEM_VALUE_SWITCH = 1;
			}
		}
	}////////////////////switch

	TFItem.onPreset(MODE);
	UPD_OUT();
	sdkRepaintPanel();

}
void Touch_Fader::onBlocSizeChange(int 	BlocSize)
{
	USINE_BLOC_SIZE = BlocSize;
}
/////////////////////////////////////////////////////////////////////////
////////////////// DRAW SPECIALS ///////////////////////////////////////
void Touch_Fader::DRAW_ROUND_PATH(TRectF IN_RECT, LongBool ROUNDED, float ROUND_SIZE, float RATIO, float ROUND_SMOOTHNESS, float OFFSET_X, float OFFSET_Y, bool UNCLOSED, int START_POS, int NB_SEG, bool ONLY_SHORT_SEG)
{
	int to_draw = 0;

	float L, R, T, B;
	L = IN_RECT.left + OFFSET_X;
	R = IN_RECT.right + OFFSET_X;
	T = IN_RECT.top + OFFSET_Y;
	B = IN_RECT.bottom + OFFSET_Y;

	////////////////////////////////////////////////////////////
	if (!ROUNDED)
	{
		TPointF Corners[4];  //from down left  up clockwise
		Corners[0].x = L; Corners[0].y = B;
		Corners[1].x = L; Corners[1].y = T;
		Corners[2].x = R; Corners[2].y = T;
		Corners[3].x = R; Corners[3].y = B;
		sdkDrawPathStart();
		sdkDrawPathMoveTo(Corners[START_POS]);
		for (int i = 0; i < NB_SEG; i++) { sdkDrawPathLineTo(Corners[(START_POS + i) % 4]); }
		if (!UNCLOSED) { sdkDrawPathClose(); }
	}//not rounded
	 /////////////////////////////////////////////////////////////////
	else //rounded
	{
		float ROUND_SIZE_Y = ROUND_SIZE;
		float ROUND_SIZE_X = ROUND_SIZE_Y*RATIO;
		ROUND_SIZE_Y = fminf(ROUND_SIZE_Y, (B - T) / 2.0f);
		ROUND_SIZE_X = fminf(ROUND_SIZE_X, (R - L) / 2.0f);

		sdkDrawPathStart();
		switch (START_POS)
		{
		case 0: sdkDrawPathMoveTo(sdkPointF(L, B - ROUND_SIZE_Y)); break;
		case 1: sdkDrawPathMoveTo(sdkPointF(L + ROUND_SIZE_X, T)); break;
		case 2: sdkDrawPathMoveTo(sdkPointF(R, T + ROUND_SIZE_Y)); break;
		case 3: sdkDrawPathMoveTo(sdkPointF(R - ROUND_SIZE_X, B)); break;
		default: sdkDrawPathMoveTo(sdkPointF(L, B - ROUND_SIZE_Y)); break;
		}

		bool IS_LAST = FALSE;	bool SHORT_SEG = FALSE;

		to_draw = START_POS;
		for (int remaining_segments = NB_SEG; remaining_segments > 0; remaining_segments--)
		{
			IS_LAST = (remaining_segments == 1);   // in some case don't wana drw last sub segment
			if (ONLY_SHORT_SEG) { SHORT_SEG = IS_LAST && NB_SEG != 4; }
			switch (to_draw)
			{
			case 0: sdkDrawPathQuadCurveTo(sdkPointF(L, T + ROUND_SIZE_Y), sdkPointF(L, T + ROUND_SIZE_Y));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(L + (ROUND_SIZE_X * ROUND_SMOOTHNESS), T), sdkPointF(L + ROUND_SIZE_X, T)); }
				break;
			case 1: sdkDrawPathQuadCurveTo(sdkPointF(R - ROUND_SIZE_X, T), sdkPointF(R - ROUND_SIZE_X, T));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(R, T + (ROUND_SIZE_Y * ROUND_SMOOTHNESS)), sdkPointF(R, T + ROUND_SIZE_Y)); }
				break;
			case 2: sdkDrawPathQuadCurveTo(sdkPointF(R, B - ROUND_SIZE_Y), sdkPointF(R, B - ROUND_SIZE_Y));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(R - (ROUND_SIZE_X * ROUND_SMOOTHNESS), B), sdkPointF(R - ROUND_SIZE_X, B)); }
				break;
			case 3: sdkDrawPathQuadCurveTo(sdkPointF(L + ROUND_SIZE_X, B), sdkPointF(L + ROUND_SIZE_X, B));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(L, B - (ROUND_SIZE_Y * ROUND_SMOOTHNESS)), sdkPointF(L, B - ROUND_SIZE_Y)); }
				break;
			default: break;
			}
			to_draw = (to_draw + 1) % 4;
		}
		if (!UNCLOSED) { sdkDrawPathClose(); }
	}//rounded

}// end draw path
void Touch_Fader::DRAW_SHAPE_PATH(TFShape IN_RECT, float OFFSET_X, float OFFSET_Y, bool UNCLOSED, int START_POS, int NB_SEG, bool ONLY_SHORT_SEG)
{

	OFFSET_X = IN_RECT.offset_X;
	OFFSET_Y = IN_RECT.offset_Y;

	int to_draw = 0;

	float L, R, T, B;
	L = IN_RECT.RECT.left + OFFSET_X;
	R = IN_RECT.RECT.right + OFFSET_X;
	T = IN_RECT.RECT.top + OFFSET_Y;
	B = IN_RECT.RECT.bottom + OFFSET_Y;

	////////////////////////////////////////////////////////////
	if (!IN_RECT.IS_ROUND)
	{
		TPointF Corners[4];  //from down left  up clockwise
		Corners[0].x = L; Corners[0].y = B;
		Corners[1].x = L; Corners[1].y = T;
		Corners[2].x = R; Corners[2].y = T;
		Corners[3].x = R; Corners[3].y = B;
		sdkDrawPathStart();
		sdkDrawPathMoveTo(Corners[START_POS]);
		for (int i = 0; i < NB_SEG; i++) { sdkDrawPathLineTo(Corners[(START_POS + i) % 4]); }
		if (!UNCLOSED) { sdkDrawPathClose(); }
	}//not rounded
	 /////////////////////////////////////////////////////////////////
	else //rounded
	{
		float ROUND_SIZE_Y = IN_RECT.ROUND_SIZE;
		float ROUND_SIZE_X = ROUND_SIZE_Y*TFParams->CANVAS_RATIO;//ROUND_SIZE_Y*IN_RECT.RATIO;
		ROUND_SIZE_Y = fminf(ROUND_SIZE_Y, (B - T) / 2.0f);
		ROUND_SIZE_X = fminf(ROUND_SIZE_X, (R - L) / 2.0f);

		sdkDrawPathStart();
		switch (START_POS)
		{
		case 0: sdkDrawPathMoveTo(sdkPointF(L, B - ROUND_SIZE_Y)); break;
		case 1: sdkDrawPathMoveTo(sdkPointF(L + ROUND_SIZE_X, T)); break;
		case 2: sdkDrawPathMoveTo(sdkPointF(R, T + ROUND_SIZE_Y)); break;
		case 3: sdkDrawPathMoveTo(sdkPointF(R - ROUND_SIZE_X, B)); break;
		default: sdkDrawPathMoveTo(sdkPointF(L, B - ROUND_SIZE_Y)); break;
		}

		bool IS_LAST = FALSE;	bool SHORT_SEG = FALSE;

		to_draw = START_POS;
		for (int remaining_segments = NB_SEG; remaining_segments > 0; remaining_segments--)
		{
			IS_LAST = (remaining_segments == 1);   // in some case don't wana drw last sub segment
			if (ONLY_SHORT_SEG) { SHORT_SEG = IS_LAST && NB_SEG != 4; }
			switch (to_draw)
			{
			case 0: sdkDrawPathQuadCurveTo(sdkPointF(L, T + ROUND_SIZE_Y), sdkPointF(L, T + ROUND_SIZE_Y));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(L + (ROUND_SIZE_X * IN_RECT.ROUND_SMOOTHNESS), T), sdkPointF(L + ROUND_SIZE_X, T)); }
				break;
			case 1: sdkDrawPathQuadCurveTo(sdkPointF(R - ROUND_SIZE_X, T), sdkPointF(R - ROUND_SIZE_X, T));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(R, T + (ROUND_SIZE_Y * IN_RECT.ROUND_SMOOTHNESS)), sdkPointF(R, T + ROUND_SIZE_Y)); }
				break;
			case 2: sdkDrawPathQuadCurveTo(sdkPointF(R, B - ROUND_SIZE_Y), sdkPointF(R, B - ROUND_SIZE_Y));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(R - (ROUND_SIZE_X * IN_RECT.ROUND_SMOOTHNESS), B), sdkPointF(R - ROUND_SIZE_X, B)); }
				break;
			case 3: sdkDrawPathQuadCurveTo(sdkPointF(L + ROUND_SIZE_X, B), sdkPointF(L + ROUND_SIZE_X, B));
				if (!SHORT_SEG) { sdkDrawPathQuadCurveTo(sdkPointF(L, B - (ROUND_SIZE_Y * IN_RECT.ROUND_SMOOTHNESS)), sdkPointF(L, B - ROUND_SIZE_Y)); }
				break;
			default: break;
			}
			to_draw = (to_draw + 1) % 4;
		}
		if (!UNCLOSED) { sdkDrawPathClose(); }
	}//rounded

}// end draw path
void Touch_Fader::DRAW_PATH_FROM_TO(TPointF P_from, TPointF P_to)
{
	sdkDrawPathStart(); sdkDrawPathMoveTo(P_from); sdkDrawPathLineTo(P_to); sdkDrawPathClose();

}
void Touch_Fader::DRAW_CIRCLE(TPointF Center, float Radius)
{
	int NBP = 16;
	float ctrl_rad_mult = 1.05f; //ctr point are farest but whats math law? seg with link to find
	float t = 0;

	TPointF P, P_prev, P_MIX;
	P = Center;
	P_prev = Center;
	P_MIX = Center;

	sdkDrawPathStart();
	for (int i = 0; i < NBP; i++)
	{
		t = float(i) / float(NBP - 1);

		P.x = cos(t*M2PI)*Radius*TFParams->CANVAS_RATIO;// +0.5;
		P.y = sin(t*M2PI)*Radius;// +0.5;

		P_MIX.x = (P_prev.x + P.x) / 2.0f;  //ctrl at intermediate pos
		P_MIX.y = (P_prev.y + P.y) / 2.0f;

		P_prev = P;

		P_MIX.x *= ctrl_rad_mult;
		P_MIX.y *= ctrl_rad_mult;

		P_MIX.x += Center.x;
		P_MIX.y += Center.y;
		P.x += Center.x;
		P.y += Center.y;

		if (i == 0) { sdkDrawPathMoveTo(P); P_MIX = P; }

		sdkDrawPathQuadCurveTo(P_MIX, P);

	}

	sdkDrawPathClose();
}
void Touch_Fader::DRAW_QUAD_RING(TPointF MAINS[], TPointF CTRLS[], int size)
{
	sdkDrawPathStart();
	sdkDrawPathMoveTo(MAINS[0]);
	for (int i = 0; i < size - 1; i++)
	{
		sdkDrawPathQuadCurveTo(CTRLS[i + 1], MAINS[i + 1]);
	}
	sdkDrawPathLineTo(MAINS[0]);
	sdkDrawPathClose();	
}
////////////////// REFLECTION SPECIALS ///////////////////////////////////////////
void Touch_Fader::RECT_REFLECTIONS(TRectF InRect, bool IS_ROUND, float ROUND_SIZE, float CANVAS_RATIO, float ROUND_SMOOTHNESS, float each, float Hue, float Sat, float opa, int DrawMode)
{

	float offset = 0; 
	float off = 0;
	offset = float(IS_ROUND) * float(ROUND_SIZE) * (2.0f + ROUND_SMOOTHNESS*4.0f);
	float inv_offset = 1 - offset;
	int NBP = 0;
	float t = 0;
	float lum = 0;
	float PY = 0;
	float PX = 0;
	float WR = 0;
	float LEN = 0;
	float RATIO = 1.0;
	TPointF P_from, P_to;
	//float norm_off = 0;

	switch (DrawMode)
	{
	case 0: WR = each / CANVAS_H;
		LEN = (InRect.bottom - InRect.top) + WR;
		NBP = (int)((LEN*CANVAS_H) / float(each));
		RATIO = (LEN / (InRect.right - InRect.left)) * CANVAS_RATIO;
		break;

	case 1: WR = each / CANVAS_H;
		LEN = (InRect.bottom - InRect.top) + WR - 0.010f;
		NBP = (int)((LEN*CANVAS_H) / float(each));
		RATIO = (LEN / (InRect.right - InRect.left)) * CANVAS_RATIO;
		break;

	case 2: WR = each / CANVAS_W;
		LEN = (InRect.right - InRect.left) + WR;
		NBP = (int)((LEN*CANVAS_W) / float(each));
		RATIO = 1.0;
		break;

	case 3: WR = each / CANVAS_W;
		LEN = (InRect.right - InRect.left);
		NBP = (int)((LEN*CANVAS_W) / float(each));
		RATIO = 1.0;
		break;

	case 4: WR = each / CANVAS_H;
		LEN = (InRect.bottom - InRect.top) + WR;
		NBP = (int)((LEN*CANVAS_H) / float(each));
		RATIO = (LEN / (InRect.right - InRect.left)) * CANVAS_RATIO;
		break;

	case 5: WR = each / CANVAS_H;
		LEN = (InRect.bottom - InRect.top) + WR;
		NBP = (int)((LEN*CANVAS_H) / float(each));
		RATIO = (LEN / (InRect.right - InRect.left)) * CANVAS_RATIO;
		break;


	default: break;
	}


	for (int i = 0; i < NBP; i++)
	{
		t = float(i) / float(NBP - 1);

		if (IS_ROUND)  //reduce line near round edges
		{
			if (t < offset)
			{
				off = (offset - t);
			}
			else if (t > inv_offset)
			{
				off = (t - inv_offset);
			}
			else
			{
				off = 0;
			}

			//off = CLAMP01(off);
			off = off / CANVAS_RATIO / RATIO;

			off = (WSHAPE_VAL(off, 0, offset, 2.0f));
			//norm_off = off *(1.0 / offset);
			//off = GET_S_CURVE(norm_off, 0.85, 0)*offset;
		}


		switch (DrawMode)
		{

		case 0: if (t < 0.5f) { lum = 0.5f - t; }
				else { lum = t; }   //fad cap base
				PY = InRect.bottom - (WR*0.5f) - t*(LEN - WR);
				P_from.x = InRect.left + off; P_from.y = PY;
				P_to.x = InRect.right - off; P_to.y = PY;
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
				break;

		case 1: lum = (abs(0.5f - t)*2.0f);                            //variant
			lum = pow(lum, 2)*1.25f;
			PY = InRect.bottom - (WR*0.5f) - t*(LEN - WR - 0.010f) - 0.005f;
			P_from.x = InRect.left + off + 0.05f; P_from.y = PY;
			P_to.x = InRect.right - off - 0.05f; P_to.y = PY;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat*2.0f, 0.5f + lum*0.5f, (lum)*opa), each);
			break;

		case 2:	lum = abs(0.5f - t) * 2.0f;    // horiz baze
			lum = (pow(lum, 1.75f))*1.25f;
			//lum = pow(lum, 2);
			PX = InRect.left - (WR*0.5f) + 0.025f + (t*(LEN - (WR*0.5f) - 0.025f));
			P_from.x = PX; P_from.y = InRect.top + off;
			P_to.x = PX; P_to.y = InRect.bottom - off;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.7f - lum, opa), each);
			break;

			/*	case 2:	lum = abs(0.5 - t) * 2;    // horiz baze
			lum = pow(lum, 2);
			PX = InRect.left - (WR*0.5) + (t*(LEN - WR*0.5));
			P_from.x = PX; P_from.y = InRect.top + off;
			P_to.x = PX; P_to.y = InRect.bottom - off;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
			break;
			*/

		case 3:
			lum = 1 - (abs(0.75f - t)*3.0f);
			PX = InRect.left - (WR*0.5f) + (t*(LEN - (WR*0.5f)));
			P_from.x = PX; P_from.y = InRect.top + off;
			P_to.x = PX; P_to.y = InRect.bottom - off;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.25f + (lum*0.5f), opa), each);
			break;

		case 4:
			lum = 1 - (abs(0.5f - t) * 2.0f);
			PY = InRect.bottom - (WR*0.5f) - t*(LEN - WR*0.5f);
			P_from.x = InRect.left + off; P_from.y = PY;
			P_to.x = InRect.right - off; P_to.y = PY;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
			break;

		case 5: 	//ok		
			lum = (abs(0.75f - t) * 2.0f);
			PY = InRect.bottom - (WR*0.5f) - t*(LEN - WR*0.5f);
			P_from.x = InRect.left + off; P_from.y = PY;
			P_to.x = InRect.right - off; P_to.y = PY;
			sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
			break;

		default: break;
		}

	}//loop


}//ret reflexions
void Touch_Fader::SHAPE_REFLECTIONS(TFShape InRect, float each, TColorUsine COLOR, int DrawMode)
{
	float Hue, Sat, lum_offset, opa;

	TColorAhsl COLOR_HSLA = sdkAlphaColorToColorAhsl(COLOR);

	Hue = COLOR_HSLA.h;
	Sat = COLOR_HSLA.s;
	lum_offset = COLOR_HSLA.l;
	opa = COLOR_HSLA.a;


	if (DrawMode > 6) { BEVEL_REFLECTIONS(InRect, opa); }
	else
	{

		float offset = 0; float off = 0;
		offset = float(InRect.IS_ROUND) * float(InRect.ROUND_SIZE);// *(2.0 + InRect.ROUND_SMOOTHNESS*4.0);
																   //offset = offset*InRect.RATIO;
		//float inv_offset = 1 - offset;

		float RECRATIO = ((InRect.RECT.right - InRect.RECT.left) / (InRect.RECT.bottom - InRect.RECT.top));

		int NBP = 0;
		float t = 0;
		float lum = 0;
		float PY = 0;
		float PX = 0;
		float WR = 0;
		float LEN = 0;
		float RATIO = 1.0;
		TPointF P_from, P_to;
		//float norm_off = 0;

		switch (DrawMode)
		{
		case 0: WR = each / CANVAS_H;
			LEN = (InRect.RECT.bottom - InRect.RECT.top) + WR;
			NBP = (int)(ceil(LEN*CANVAS_H) / float(each));
			RATIO = (LEN / (InRect.RECT.right - InRect.RECT.left)) * InRect.RATIO;
			break;

		case 1: WR = each / CANVAS_H;
			LEN = (InRect.RECT.bottom - InRect.RECT.top) + WR - 0.010f;
			NBP = (int)(ceil(LEN*CANVAS_H) / float(each));
			RATIO = (LEN / (InRect.RECT.right - InRect.RECT.left)) * InRect.RATIO;
			break;

		case 2: WR = 0;// each / CANVAS_W;
			LEN = (InRect.RECT.right - InRect.RECT.left) + WR;
			NBP = (int)(ceil(LEN*CANVAS_W) / float(each));
			RATIO = 1.0;
			break;

		case 3: WR = each / CANVAS_W;
			LEN = (InRect.RECT.right - InRect.RECT.left);
			NBP = (int)(ceil(LEN*CANVAS_W) / float(each));
			RATIO = 1.0;
			break;

		case 4: WR = each / CANVAS_H;
			LEN = (InRect.RECT.bottom - InRect.RECT.top) + WR;
			NBP = (int)((LEN*CANVAS_H) / float(each));
			RATIO = (LEN / (InRect.RECT.right - InRect.RECT.left)) * InRect.RATIO;
			break;

		case 5: WR = each / CANVAS_H;
			LEN = (InRect.RECT.bottom - InRect.RECT.top) + WR;
			NBP = (int)(ceil(LEN*CANVAS_H) / float(each));
			RATIO = (LEN / (InRect.RECT.right - InRect.RECT.left)) * InRect.RATIO;
			break;

		case 6: WR = 0;// each / CANVAS_W;
			LEN = (InRect.RECT.right - InRect.RECT.left);// +WR;
			NBP = (int)(ceil(LEN*CANVAS_W) / float(each));
			//NBP = ceil(sdkWidthPercentToPixel(LEN));
			RATIO = 1.0;
			break;


		default: break;
		}

		RECRATIO = fmaxf(1, RECRATIO);

		float offsetB = offset*RECRATIO;
		float inv_offsetB = 1 - offsetB;


		for (int i = 0; i < NBP; i++)
		{
			t = float(i) / float(NBP - 1);

			if (InRect.IS_ROUND)  //reduce line near round edges
			{
				if (t < offsetB)
				{
					off = (offsetB - t);
				}
				else if (t > inv_offsetB)
				{
					off = (t - inv_offsetB);
				}
				else { off = 0; }

				off = off / RECRATIO;
				off = off*InRect.RATIO;

				//off = CLAMP01(off);
				if (InRect.ROUND_SMOOTHNESS == 0)
				{
					off = (WSHAPE_VAL(off, 0, offset*2.0f, 3.0f));
				}

				//norm_off = off *(1.0 / offsetB);
				//norm_off = CLAMP01(norm_off);
				//off = (1-GET_S_CURVE(1-norm_off, TFParams->input_val, 2))*(offsetB);
			}


			switch (DrawMode)
			{

			case 0: if (t < 0.5f) { lum = 0.5f - t; }
					else { lum = t; }   //fad cap base
					PY = InRect.RECT.bottom - (WR*0.5f) - t*(LEN - WR);
					P_from.x = InRect.RECT.left + off; P_from.y = PY;
					P_to.x = InRect.RECT.right - off; P_to.y = PY;
					lum = lum *(1 + lum_offset);
					sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
					break;

			case 1: lum = (abs(0.5f - t)*2.0f);                            //variant
				lum = pow(lum, 2)*1.25f;
				PY = InRect.RECT.bottom - (WR*0.5f) - t*(LEN - WR - 0.010f) - 0.005f;
				P_from.x = InRect.RECT.left + off + 0.05f; P_from.y = PY;
				P_to.x = InRect.RECT.right - off - 0.05f; P_to.y = PY;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat*2.0f, 0.5f + lum*0.5f, (lum)*opa), each);
				break;

			case 2:	lum = abs(0.5f - t) * 2.0f;    // horiz baze
				lum = (pow(lum, 1.75f))*1.25f;
				//lum = pow(lum, 2);
				//PX = InRect.RECT.left - (WR*0.5) + t*(LEN - WR);
				PX = InRect.RECT.left + t*(LEN);
				P_from.x = PX; P_from.y = InRect.RECT.top + off;
				P_to.x = PX; P_to.y = InRect.RECT.bottom - off;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.7f - lum, opa), each);
				break;

				/*	lum = (pow(lum, 1.75))*1.25;
				//lum = pow(lum, 2);
				PX = InRect.RECT.left - (WR*0.5) + 0.025 + (t*(LEN - (WR*0.5) - 0.025));
				P_from.x = PX; P_from.y = InRect.RECT.top + off;
				P_to.x = PX; P_to.y = InRect.RECT.bottom - off;
				lum = lum + lum_offset;
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.7 - lum, opa), each);
				break;
				*/

				/*	case 2:	lum = abs(0.5 - t) * 2;    // horiz baze
				lum = pow(lum, 2);
				PX = InRect.left - (WR*0.5) + (t*(LEN - WR*0.5));
				P_from.x = PX; P_from.y = InRect.top + off;
				P_to.x = PX; P_to.y = InRect.bottom - off;
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
				break;
				*/

			case 3:
				lum = 1 - (abs(0.75f - t)*3.0f);
				PX = InRect.RECT.left - (WR*0.5f) + (t*(LEN - (WR*0.5f)));
				P_from.x = PX; P_from.y = InRect.RECT.top + off;
				P_to.x = PX; P_to.y = InRect.RECT.bottom - off;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.25f + (lum*0.5f), opa), each);
				break;

			case 4:
				lum = 1 - (abs(0.5f - t) * 2.0f);
				PY = InRect.RECT.bottom - (WR*0.5f) - t*(LEN - WR*0.5f);
				P_from.x = InRect.RECT.left + off; P_from.y = PY;
				P_to.x = InRect.RECT.right - off; P_to.y = PY;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
				break;

			case 5: 	//ok		
				lum = (abs(0.75f - t) * 2);
				PY = InRect.RECT.bottom - (WR*0.5f) - t*(LEN - WR*0.5f);
				P_from.x = InRect.RECT.left + off; P_from.y = PY;
				P_to.x = InRect.RECT.right - off; P_to.y = PY;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, lum, opa), each);
				break;

			case 6:	lum = 1 - (abs(0.5f - t) * 2);    // horiz baze  2 inverted
													 //lum = (pow(lum, 1.75))*1.25;
				lum = pow(lum, 2);
				PX = InRect.RECT.left + t*(LEN);
				P_from.x = PX; P_from.y = InRect.RECT.top + off;
				P_to.x = PX; 
				P_to.y = InRect.RECT.bottom - off;
				lum = lum *(1 + lum_offset);
				sdkDrawLine(P_from, P_to, HSLA(Hue, Sat, 0.7f - lum, opa), each);
				break;

			default: break;
			}

		}//loop

	}
}//ret reflexions
void Touch_Fader::BEVEL_REFLECTIONS(TFShape InRect, float opa_in)

{
	float each = 4;
	TPointF P_from, P_to;

	float tn = 0; float t = 0; float lum = 0; float lum_bar_on = 0.5f; float lum_bar_off = 0.1f;
	float hue = ITEM_HUE; float opa = opa_in; float sat = 0.25f; float bar_size = 5; float bar_opa = opa_in*1.50f; float bar_sat = 0.25f;

	float Xstart = InRect.RECT.left + 0.05f;// 0.05;
	float Ystart = InRect.RECT.top - 0.025f;
	float Xend = InRect.RECT.right - 0.05f; float Yend = InRect.RECT.bottom + 0.025f;
	int NBV = (int)(sdkHeightPercentToPixel((1.0f - Ystart*2.0f) / float(each)));
	int NBH = (int)(sdkWidthPercentToPixel((1.0f - Xstart*2.0f) / float(each)));

	bool INVERSE_LUM = FALSE;

	float offset_bar = 0.05f;

	bar_size = (Xend - Xstart)*6.5f;

	if (TFParams->ITEM_VALUE_SWITCH)
	{
		lum_bar_on = 0.75f;  lum_bar_off = 0.75f;
		INVERSE_LUM = TRUE; hue = ITEM_HUE + TFItem.IButton.ColorEffect.H_OnOffset; //get hue on
		bar_opa = opa_in*2.0f; bar_sat = 0.75f; //bar_size = 5;
											  //offset_bar = TFItem.IButton.REDUX +0.05;// 0.0125;   //get redux
	}
	else
	{
		offset_bar = 0.05f;// +(TFParams->input_val / 5.0);
	}

	for (int i = 0; i < NBH; i++)
	{
		tn = float(i) / float(NBH - 1);

		t = abs(0.5f - tn)*2.0f;
		lum = pow(t, 4);

		if (INVERSE_LUM) {
			lum = fmaxf(0.35f, pow(t, 0.5f))*1.25f; opa = opa_in;// *lum;
			sat = 0.75f;
		}

		P_from.x = RE_RANGE_NORM_IN(tn, Xstart, Xend);
		P_from.y = Ystart + 0.125f;
		P_to.x = P_from.x;
		P_to.y = Yend - 0.125f;

		DRAW_PATH_FROM_TO(P_from, P_to);
		sdkDrawPathDraw(HSLA(hue, sat, lum, opa), each);
	}

	for (int i = 0; i < NBV; i++)
	{
		tn = float(i) / float(NBV - 1);

		t = abs(1 - pow(tn, 3));
		lum = 1 - t; opa = 0.0125f;

		if (INVERSE_LUM) { lum = pow((1 - t), 0.25f); opa = opa_in; sat = 0.9f; }

		P_from.x = Xstart + 0.15f;
		P_from.y = RE_RANGE_NORM_IN(tn, Ystart, Yend - 0.05f);
		P_to.x = Xend - 0.15f;
		P_to.y = P_from.y;

		DRAW_PATH_FROM_TO(P_from, P_to);
		sdkDrawPathDraw(HSLA(hue, sat, lum, opa), each);
	}

	//////////////////////lines
	P_from.x = Xstart + offset_bar;
	P_from.y = Ystart + offset_bar;
	P_to.x = P_from.x;
	P_to.y = Yend - offset_bar;
	sdkDrawLine(P_from, P_to, HSLA(hue, bar_sat, lum_bar_on, bar_opa), bar_size);
	P_to.y = Ystart + offset_bar;
	sdkDrawLine(P_from, P_to, HSLA(hue, bar_sat, lum_bar_off, bar_opa), bar_size);
	P_to.x = Xend - 0.01f - offset_bar;
	P_to.y = Yend - offset_bar;
	P_from.y = Yend - offset_bar;
	sdkDrawLine(P_from, P_to, HSLA(hue, bar_sat, lum_bar_off, bar_opa), bar_size);
	P_from.x = Xend - offset_bar;
	P_from.y = Ystart + offset_bar;
	sdkDrawLine(P_from, P_to, HSLA(hue, bar_sat, lum_bar_on, bar_opa), bar_size);

	//sdkDrawLine(sdkPointF(Xend - offset_bar, Ystart + offset_bar), sdkPointF(Xend - offset_bar, Yend - offset_bar), HSLA(hue, bar_sat, lum_bar_on, bar_opa), bar_size);
	//sdkDrawLine(sdkPointF(Xstart + 0.01 + offset_bar, Ystart + offset_bar), sdkPointF(Xend - 0.01 - offset_bar, Ystart + offset_bar), HSLA(hue, bar_sat, lum_bar_off, bar_opa), bar_size);
	//sdkDrawLine(sdkPointF(Xstart + 0.01 + offset_bar, Yend - offset_bar), sdkPointF(Xend - 0.01 - offset_bar, Yend - offset_bar), HSLA(hue, bar_sat, lum_bar_off, bar_opa), bar_size);

}//fx reflect bevel
 //////////////////////////////////////////////////
 ////////////////////////////////////////////////////
void Touch_Fader::onSetQuickColor(TColorUsine color)
{
	//sdkTraceChar("Setting QuickColor");
	//sdkSetEvtData(pHUE, 0);
	//TFParams->ITEM_HUE = 0;
	TFPaintP->QUICK_COLOR = &color;
	TFItem.onSetQuickColor(MODE);
	//TFItem.
}
void Touch_Fader::PAINT_ITEM()
{

	PAINT_NB_ELMTS_TO_DRAW = *TFPaintP->PAINT_NB_ELMTS_TO_DRAW;// items specify how many max paint layers they need, paint proc will loop tru and request each layer if it need paint and what to paint
															   //////////////////////////////////////////////////////////////////Loop thru item layers
	for (int i = 0; i < PAINT_NB_ELMTS_TO_DRAW; i++)
	{
		PAINT_LAYER_TYPE = TFItem.GET_PAINT_LAYER(MODE, i);   //first ask item layer type = what to do to item code file, item will answer a type of layer passing needed element pointer

		switch (PAINT_LAYER_TYPE)
		{////////////////////////////////////////// differents draw possible action relating types of layers & associated pointers returned from item
		 //0= shape rect type to draw rounded square path
		case DT_Shape: SHAPE = TFPaintP->SHAPE; if (SHAPE->SHOW) { DRAW_SHAPE_PATH(*SHAPE); if (SHAPE->SHOW_FILL) { sdkDrawPathFill(SHAPE->FILL_COLOR); } if (SHAPE->SHOW_BORDER) { sdkDrawPathDraw(SHAPE->BORDER_COLOR, SHAPE->BORDER_SIZE); } }; break; //shape
																																																														  //1 = reflexion on shape
		case DT_Shape_Reflexion: SHAPE = TFPaintP->SHAPE; 
			                     if (SHAPE->SHOW) { SHAPE_REFLECTIONS(*SHAPE, *TFPaintP->FX_REFLEXIONS_EACH, SHAPE->FILL_COLOR, *TFPaintP->FX_REFLEXIONS_TYPE); }break;
			//2 = specs partial border segs drw
		case DT_Shape_Partial: SHAPE = TFPaintP->SHAPE; if (SHAPE->SHOW) { DRAW_SHAPE_PATH(*SHAPE, 0, 0, TRUE, *TFPaintP->SHAPE_SEG_FROM, *TFPaintP->SHAPE_NB_SEGS, FALSE); sdkDrawPathDraw(SHAPE->BORDER_COLOR, SHAPE->BORDER_SIZE); }break;
			//3 = Trect array ie stripes leds
		case DT_TRect_Array:
			for (int i = 0; i < *TFPaintP->STRIPES_MAX_PIX; i++) 
			{
				//if (i % *TFPaintP->STRIPES_MODULO == 0)//no need as was optimized in item code
				{
					DRAW_ROUND_PATH(TFPaintP->STRIPES_STRIPES_LEDS[i], FALSE, 0.25, 1, 0); sdkDrawPathFill(TFPaintP->STRIPES_STRIPES_COLORS[i]); sdkDrawPathDraw(TFPaintP->STRIPES_STRIPES_COLORS[i], 1.0);
					if (TFPaintP->STRIPES_DUAL_DRAW) { DRAW_ROUND_PATH(TFPaintP->STRIPES_STRIPES_LEDS_SYM[i], FALSE, 0.25, 1, 0); sdkDrawPathFill(TFPaintP->STRIPES_STRIPES_COLORS[i]); sdkDrawPathDraw(TFPaintP->STRIPES_STRIPES_COLORS[i], 1.0); }
				}
			}
			break;
			//4 = flares or array of points
		case DT_Points_Array: { for (int i = 0; i < TFPaintP->FX_NB_FLARES; i++) { sdkDrawPoint(*TFPaintP->FX_FLARE, TFPaintP->FX_FLARES_COLORS[i], TFPaintP->FX_FLARES_SIZES[i], TRUE); sdkDrawPoint(*TFPaintP->FX_FLARE_SYM, TFPaintP->FX_FLARES_SYM_COLORS[i], TFPaintP->FX_FLARES_SYM_SIZES[i], TRUE); } }break;
			// 5 = simple line
		case DT_Line:  sdkDrawLine(TFPaintP->LINE_ELMT->LINE_FROM, TFPaintP->LINE_ELMT->LINE_TO, TFPaintP->LINE_ELMT->LINE_COLOR, TFPaintP->LINE_ELMT->LINE_SIZE);  break;
			//6 = string value
		case DT_Text_Value:  STRVAL = TFPaintP->VALUE_ELMT;
			sdkFillText(STRVAL->RECT, AnsiCharPtr(STRVAL->VALUE_STRING.c_str()), STRVAL->VALUE_COLOR, VALUE_SIZE, VALUE_IS_BOLD, FALSE, STRVAL->H_Align, STRVAL->V_Align, FALSE); break;
			// array of lines grid type
		case DT_Lines_Array: { for (int i = 0; i < *TFPaintP->GRID_SUBDIVS; i++) { sdkDrawLine(TFPaintP->GRID_GRID_FROM[i], TFPaintP->GRID_GRID_TO[i], *TFPaintP->COLOR_Grid_OUT, *TFPaintP->LINE_SIZE*0.5f); } } break;
			// simple dot using start point from line pointer
		case DT_Square_Dot: sdkDrawPoint(TFPaintP->LINE_ELMT->LINE_FROM, TFPaintP->LINE_ELMT->LINE_COLOR, TFPaintP->LINE_ELMT->LINE_SIZE, FALSE);  break;
			// simple dot using start point from line pointer
		case DT_Round_Dot: sdkDrawPoint(TFPaintP->LINE_ELMT->LINE_FROM, TFPaintP->LINE_ELMT->LINE_COLOR, TFPaintP->LINE_ELMT->LINE_SIZE, TRUE);  break;

			///////////// Beyond this point is encoder. to do: need more serializing/optimizing

		case DT_Ring: 	DRAW_QUAD_RING(TFItem.IEncoder.QUAD_OFF_MAINS, TFItem.IEncoder.QUAD_OFF_CTRLS, TFItem.IEncoder.QUAD_NBP_B);  //first draw baground ring
			if (TFItem.IEncoder.SHOW.FILLED_RING) { sdkDrawPathFill(TFItem.IEncoder.COLOR.Ring_Off.OUT); }
			if (TFItem.IEncoder.SHOW.RING_BORDER) { sdkDrawPathDraw(TFItem.IEncoder.COLOR.Ring_Off_Border.OUT, TFItem.IEncoder.RING_BORDER_SIZE); }
			DRAW_QUAD_RING(TFItem.IEncoder.QUAD_ON_MAINS, TFItem.IEncoder.QUAD_ON_CTRLS, TFItem.IEncoder.QUAD_NBP_B);                              //draw 'on' ring
			if (TFItem.IEncoder.SHOW.FILLED_RING) { sdkDrawPathFill(TFItem.IEncoder.COLOR.Ring_On.OUT); }
			if (TFItem.IEncoder.SHOW.RING_BORDER) { sdkDrawPathDraw(TFItem.IEncoder.COLOR.Ring_On_Border.OUT, TFItem.IEncoder.RING_BORDER_SIZE); }
			break;
			/////////////////////////
		case DT_Enc_Leds:   //encoder leds
			if (TFItem.IEncoder.SHOW.LEDS_RING)
			{
				for (int j = 0; j < TFItem.IEncoder.RING_LEDS; j++)
				{
					if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 0) //ring
					{
						if (TFItem.IEncoder.LEDS_TYPE == 0) //dots
						{
							if (TFItem.IEncoder.FX_SHOW_SHADOWS)
							{
								sdkDrawPoint(sdkPointF(TFItem.IEncoder.DOTS[j].x + TFItem.IEncoder.SHADOW_X*0.125f, TFItem.IEncoder.DOTS[j].y + TFItem.IEncoder.SHADOW_Y*0.125f), TFItem.IEncoder.SHADOW_COLOR_LO, TFItem.IEncoder.DOT_SIZE_USED, TRUE);//sdw
							}
							sdkDrawPoint(TFItem.IEncoder.DOTS[j], TFItem.IEncoder.DOTS_COLOR[j], TFItem.IEncoder.DOT_SIZE_USED, TRUE);
						}
						else if (TFItem.IEncoder.LEDS_TYPE == 1) //small lines each 2 = segment
						{
							if ((j % 2 == 0) && (j + 1)<TFItem.IEncoder.RING_LEDS) { sdkDrawLine(TFItem.IEncoder.DOTS[j], TFItem.IEncoder.DOTS[j + 1], TFItem.IEncoder.DOTS_COLOR[j], TFItem.IEncoder.DOT_SIZE_USED); }
						}
						else if (TFItem.IEncoder.LEDS_TYPE == 2) //small lines to center
						{
							sdkDrawLine(TFItem.IEncoder.DOTS[j], TFItem.IEncoder.CENTER, TFItem.IEncoder.DOTS_COLOR[j], TFItem.IEncoder.DOT_SIZE_USED*0.5f);
						}
						else if (TFItem.IEncoder.LEDS_TYPE == 3) //pseudo 3D
						{
							sdkDrawPathStart();
							sdkDrawPathMoveTo(TFItem.IEncoder.DOTS[j]);
							sdkDrawPathLineTo(sdkPointF(TFItem.IEncoder.DOTS[j].x*0.5f, TFItem.IEncoder.DOTS[j].y*0.5f));
							sdkDrawPathDraw(TFItem.IEncoder.DOTS_COLOR[j], TFItem.IEncoder.DOT_SIZE_USED);
							sdkDrawPathClose();
							sdkDrawPoint(TFItem.IEncoder.DOTS[j], TFItem.IEncoder.DOTS_COLOR[j], TFItem.IEncoder.DOT_SIZE_USED, TRUE);
						}

					}
					else if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 1 || TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 2) // horizontal or vert bars
					{
						DRAW_ROUND_PATH(TFItem.IEncoder.LEDS_RECTS[j], FALSE, 0, 1, 0);
						sdkDrawPathFill(TFItem.IEncoder.DOTS_COLOR[j]);
					}
				}//loop

			}// end leds//----------------------------------
			break;

		case DT_Enc_Cursor:
			//if (TFItem.IEncoder.CENTER_LAYER_POSITION == 0) goto GTcenter;
			//GTcursor:
			if (TFItem.IEncoder.SHOW.CURSOR_ && TFItem.IEncoder.ENCODER_LAYOUT_TYPE < 3)
			{

				TPointF FROM, TO;
				if (TFItem.IEncoder.CURSOR_FLIP) { FROM = TFItem.IEncoder.LINE_TO; TO = TFItem.IEncoder.LINE_FROM; }
				else { FROM = TFItem.IEncoder.LINE_FROM; TO = TFItem.IEncoder.LINE_TO; }


				if (TFItem.IEncoder.CURSOR_TYPE > 1 && TFItem.IEncoder.FX_SHOW_SHADOWS) //if shp  prepare shadow
				{
					TPointF SHAPE_SHADOW[18]; memcpy(SHAPE_SHADOW, TFItem.IEncoder.SHAPE, (TFItem.IEncoder.SHAPE_NB_POINTS + 1)*sizeof(TPointF));
					OFFSET_TPOINTF(SHAPE_SHADOW, TFItem.IEncoder.SHAPE_NB_POINTS + 1, TFItem.IEncoder.SHADOW_X, TFItem.IEncoder.SHADOW_Y);
					sdkFillPolyLine(SHAPE_SHADOW, TFItem.IEncoder.SHAPE_NB_POINTS + 1, TFItem.IEncoder.SHADOW_COLOR);
				}

				switch (TFItem.IEncoder.CURSOR_TYPE)
				{
				case 0: if (TFItem.IEncoder.FX_SHOW_SHADOWS) { sdkDrawPoint(OFFSET_POINT(FROM, TFItem.IEncoder.SHADOW_X*0.5f, TFItem.IEncoder.SHADOW_Y*0.5f), TFItem.IEncoder.SHADOW_COLOR, TFItem.IEncoder.CURSOR_SIZE_USED, TRUE); }
						sdkDrawPoint(FROM, TFItem.IEncoder.COLOR.Cursor.OUT, TFItem.IEncoder.CURSOR_SIZE_USED, TRUE); break; //dot
				case 1: if (TFItem.IEncoder.FX_SHOW_SHADOWS) { sdkDrawLine(OFFSET_POINT(FROM, TFItem.IEncoder.SHADOW_X*0.5f, TFItem.IEncoder.SHADOW_Y*0.5f), OFFSET_POINT(TO, TFItem.IEncoder.SHADOW_X*0.5f, TFItem.IEncoder.SHADOW_Y*0.5f), TFItem.IEncoder.SHADOW_COLOR, TFItem.IEncoder.CURSOR_SIZE_USED); }
						sdkDrawLine(FROM, TO, TFItem.IEncoder.COLOR.Cursor.OUT, TFItem.IEncoder.CURSOR_SIZE_USED); break; //line
				case 2: sdkDrawPolyLine(TFItem.IEncoder.SHAPE, TFItem.IEncoder.SHAPE_NB_POINTS + 2, TFItem.IEncoder.COLOR.Cursor.OUT, TFItem.IEncoder.CURSOR_SIZE_USED / 2.0f); break;
				case 3: sdkFillPolyLine(TFItem.IEncoder.SHAPE, TFItem.IEncoder.SHAPE_NB_POINTS + 2, TFItem.IEncoder.COLOR.Cursor.OUT);
					sdkDrawPolyLine(TFItem.IEncoder.SHAPE, TFItem.IEncoder.SHAPE_NB_POINTS + 2, TFItem.IEncoder.COLOR.Cursor.OUT, TFItem.IEncoder.CURSOR_SIZE_USED / 2.0f);
					break;
					//break; //shape
				default:break;
				}
				if (TFItem.IEncoder.FX_SHOW_REFLEXIONS && TFItem.IEncoder.CURSOR_TYPE > 1) //shape cursor reflexion
				{
					sdkDrawPathStart();
					sdkDrawPathMoveTo(TFItem.IEncoder.SHAPE[2]);
					for (int i = 3; i < TFItem.IEncoder.SHAPE_NB_POINTS + 1; i++)
					{
						sdkDrawPathLineTo(sdkPointF(TFItem.IEncoder.SHAPE[i].x, TFItem.IEncoder.SHAPE[i].y));
					}
					sdkDrawPathClose();
					sdkDrawPathFill(HSLA(ITEM_HUE, 0.25f, 0.9f, TFItem.IEncoder.SHAPE[0].x *0.25f));
					sdkFillPolyLine(TFItem.IEncoder.SHAPE, int(float(TFItem.IEncoder.SHAPE_NB_POINTS) / 2.0f) + 1, HSLA(ITEM_HUE, 0.25f, 0.9f, (1 - TFItem.IEncoder.SHAPE[0].x) *0.25f));

					TColorUsine COLOR_A, COLOR_A2, COLOR_B;
					float IN_LUM = 0;
					IN_LUM = RE_RANGE_NORM_IN(TFItem.IEncoder.encoder_val, 0.2f, 1.2f);
					COLOR_A = HSLA(ITEM_HUE, 0.5f, 1 - IN_LUM, 0.5f);
					COLOR_A2 = HSLA(ITEM_HUE, 0.5f, 1 - IN_LUM, IN_LUM);
					COLOR_B = HSLA(ITEM_HUE, 0.5f, IN_LUM, 0.5f);

					sdkDrawLine(TFItem.IEncoder.SHAPE[0], TFItem.IEncoder.SHAPE[1], COLOR_A, 4.0f);
					sdkDrawLine(TFItem.IEncoder.SHAPE[1], TFItem.IEncoder.SHAPE[2], COLOR_A, 4.0f);
					sdkDrawLine(TFItem.IEncoder.SHAPE[0], TFItem.IEncoder.SHAPE[2], COLOR_A2, 2.0f);
					sdkDrawLine(TFItem.IEncoder.SHAPE[2], TFItem.IEncoder.SHAPE[3], COLOR_B, 4.0f);
					sdkDrawLine(TFItem.IEncoder.SHAPE[3], TFItem.IEncoder.SHAPE[0], COLOR_B, 4.0f);
				}//refl			
			}//end cursor
			break;

		case DT_Enc_Center:
			if (TFItem.IEncoder.SHOW.CENTER && TFItem.IEncoder.ENCODER_LAYOUT_TYPE<3) //only rings & bars centers
			{
				if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 0) //ring round centers
				{
					if (TFItem.IEncoder.CENTER_DRAW_TYPE == 0) //simple
					{
						if (TFItem.IEncoder.FX_SHOW_SHADOWS)
						{
							float size_max = fmaxf(TFItem.IEncoder.CENTER_A_SIZE, TFItem.IEncoder.CENTER_A_SIZE);
							sdkDrawPoint(sdkPointF(TFItem.IEncoder.CENTER.x + TFItem.IEncoder.SHADOW_X*0.5f, TFItem.IEncoder.CENTER.y + TFItem.IEncoder.SHADOW_Y*0.5f), TFItem.IEncoder.SHADOW_COLOR, size_max, TRUE); //shdow
						}

						DRAW_CIRCLE(TFItem.IEncoder.CENTER, TFItem.IEncoder.CENTER_A_SIZE);/////////////////////////////////////////////circle
						sdkDrawPathFill(TFItem.IEncoder.COLOR.Center_Cap.OUT);
						sdkDrawPathDraw(TFItem.IEncoder.COLOR.Center_Cap_Border.OUT, TFItem.IEncoder.CENTER_BORDER_SIZE);

						if (TFItem.IEncoder.FX_SHOW_REFLEXIONS)  // high proccess to optimise precompute somewhere
						{
							float Width = TFItem.IEncoder.RING_RADIUS *0.65f;//0.1 + 0.04 ;
							float t = 0;
							float t_norm = 0;
							float t_B = 0;
							int NBP = FX_GRAD_MAX;
							int NBP_half = (int)(FX_GRAD_MAX / 2);
							TPointF SH_Point[FX_GRAD_MAX];
							TPointF LI_Point[FX_GRAD_MAX];
							float Radius = TFItem.IEncoder.CENTER_A_RADIUS*TFItem.IEncoder.RING_RADIUS*0.5f - Width*0.5f;
							float Offset = 0.08f;
							float Length = 0.35f - 0.02f;

							for (int i = 0; i < NBP; i++)
							{
								t_norm = float(i) / (float)(NBP - 1);
								t = t_norm*Length;
								t += Offset;
								t = pow(t, 0.9f);
								t_B = t + 0.5f;

								SH_Point[i].x = cos(t*M2PI)*Radius*TFParams->CANVAS_RATIO + 0.5f + TFItem.IEncoder.RING_H_OFFSET;
								SH_Point[i].y = sin(t*M2PI)*Radius + 0.5f + TFItem.IEncoder.RING_V_OFFSET;
								LI_Point[i].x = cos(t_B*M2PI)*Radius*TFParams->CANVAS_RATIO + 0.5f + TFItem.IEncoder.RING_H_OFFSET;
								LI_Point[i].y = sin(t_B*M2PI)*Radius + 0.5f + TFItem.IEncoder.RING_V_OFFSET;
							}
							float W_mult = 0.50;// TFItem.IEncoder.DEBUG_TEST;

							for (int i = 0; i < ((float)NBP / 2.0); i++)
							{
								t_norm = float(i) / (float)(NBP - 1);
								sdkDrawLine(LI_Point[i], LI_Point[NBP - 1 - i], HSLA(0, 0, 0.7f, t_norm*TFItem.IEncoder.REFLEXIONS_OPACITY*0.7f), Width*TFParams->CANVAS_S*W_mult);
								sdkDrawLine(SH_Point[i], SH_Point[NBP - 1 - i], HSLA(0, 0, 0.7f, t_norm*TFItem.IEncoder.REFLEXIONS_OPACITY*0.4f), Width*TFParams->CANVAS_S*W_mult);
								//sdkDrawLine(SH_Point[i], SH_Point[NBP - 1 - i], HSLA(0, 0, 0.1, t_norm*TFItem.IEncoder.REFLEXIONS_OPACITY*0.4), Width*TFParams->CANVAS_S*W_mult);
							}

							for (int i = 0; i < NBP; i++)  //tek A individual mixed with tek B
							{
								t_norm = float(i) / (float)(NBP - 1);
								//t = (1- abs(t_norm-0.5))/4.0 +0.75;
								t = (1 - abs(t_norm - 0.5f)) / 3.5f + 0.714f;
								t = t*t;
								sdkDrawPoint(SH_Point[i], HSLA(ITEM_HUE, 0.1f, 0.1f, TFItem.IEncoder.REFLEXIONS_OPACITY*t*0.35f*0.5f), (Width *W_mult)*TFParams->CANVAS_S*t, TRUE);//sh
								sdkDrawPoint(LI_Point[i], HSLA(ITEM_HUE, 0.1f, 0.6f, TFItem.IEncoder.REFLEXIONS_OPACITY*t*0.30f*0.5f), (Width *W_mult)*TFParams->CANVAS_S*t, TRUE);//sh
							}
							sdkDrawPolyLine(SH_Point, NBP, HSLA(ITEM_HUE, 0.1f, 0.1f, TFItem.IEncoder.REFLEXIONS_OPACITY*0.4f), (Width*W_mult)*TFParams->CANVAS_S);//sh     ///tek B
							sdkDrawPolyLine(LI_Point, NBP, HSLA(ITEM_HUE, 0.1f, 0.6f, TFItem.IEncoder.REFLEXIONS_OPACITY*0.4f*0.5f), (Width *W_mult)*TFParams->CANVAS_S); //li  //tek B

							sdkDrawLine(LI_Point[NBP_half - 2], LI_Point[NBP - NBP_half + 1], HSLA(ITEM_HUE, 0.1f, 0.6f, TFItem.IEncoder.REFLEXIONS_OPACITY*0.50f), (Width *W_mult*0.5f)*TFParams->CANVAS_S); //hotspot facultative -0.19
							sdkDrawPoint(LI_Point[NBP - NBP_half], HSLA(ITEM_HUE, 0.1f, 0.7f, TFItem.IEncoder.REFLEXIONS_OPACITY*0.55f), (Width*W_mult*0.5f)*TFParams->CANVAS_S, TRUE); //hotspot facultative
																																													//sdkDrawLine(SH_Point[0], SH_Point[NBP - 1], HSLA(ITEM_HUE, 0.1, 0.1, TFItem.IEncoder.REFLEXIONS_OPACITY), Width*TFParams->CANVAS_S*0.5);
						}//relf



					}
					else // turning Ngon
					{
						//float center_border_size = abs(TFItem.IEncoder.CENTER_A_RADIUS - TFItem.IEncoder.CENTER_B_RADIUS)*0.5*TFItem.IEncoder.RING_RADIUS*TFParams->CANVAS_S;
						sdkFillPolyLine(TFItem.IEncoder.CENTER_POINTS, TFItem.IEncoder.CENTER_NBP + 1, TFItem.IEncoder.COLOR.Center_Cap.OUT);
						sdkFillPolyLine(TFItem.IEncoder.CENTER_POINTS, TFItem.IEncoder.CENTER_NBP + 1, TFItem.IEncoder.COLOR.Center_Cap.OUT);
						sdkDrawPolyLine(TFItem.IEncoder.CENTER_POINTS, TFItem.IEncoder.CENTER_NBP + 1, TFItem.IEncoder.COLOR.Center_Cap.OUT, TFItem.IEncoder.CENTER_BORDER_SIZE);
						for (int i = 0; i < TFItem.IEncoder.CENTER_NBP; i++) { sdkDrawPoint(TFItem.IEncoder.CENTER_POINTS[i], TFItem.IEncoder.COLOR.Center_Cap_Border.OUT, TFItem.IEncoder.CENTER_BORDER_SIZE*1.5f, TRUE); }
					}

				}
				else if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 1 || TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 2) //bars center as cap
				{
					DRAW_ROUND_PATH(TFItem.IEncoder.Center_Led, TFItem.IEncoder.BAR_IS_ROUND, TFItem.IEncoder.BAR_ROUND_SIZE, 1, 0);
					sdkDrawPathFill(TFItem.IEncoder.COLOR.Center_Cap.OUT);
					sdkDrawPathDraw(TFItem.IEncoder.COLOR.Center_Cap_Border.OUT, TFItem.IEncoder.CENTER_BORDER_SIZE);
				}

			}//end center
			break;

		case DT_Enc_Panels:
			////////////////panels
			if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 3 || TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 4) //color panel of fader
			{
				if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 3) //simple panel
				{
					DRAW_ROUND_PATH(TFItem.IEncoder.Encoder_Rect, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); sdkDrawPathFill(TFItem.IEncoder.PANEL_COLOR);
				}

				else if (TFItem.IEncoder.ENCODER_LAYOUT_TYPE == 4) //fader panel  //differents order relative mode
				{
					if (TFItem.IEncoder.ENCODER_LEDS_MODE > 1) {
						DRAW_ROUND_PATH(TFItem.IEncoder.Cusor_Fader, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); sdkDrawPathFill(TFItem.IEncoder.FADER_COLOR); sdkDrawPathDraw(TFItem.IEncoder.FADER_COLOR, 1.0);
						DRAW_ROUND_PATH(TFItem.IEncoder.Encoder_Rect, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); sdkDrawPathFill(TFItem.IEncoder.PANEL_COLOR);
					}
					else {
						DRAW_ROUND_PATH(TFItem.IEncoder.Encoder_Rect, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); sdkDrawPathFill(TFItem.IEncoder.PANEL_COLOR);
						DRAW_ROUND_PATH(TFItem.IEncoder.Cusor_Fader, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); sdkDrawPathFill(TFItem.IEncoder.FADER_COLOR); sdkDrawPathDraw(TFItem.IEncoder.FADER_COLOR, 1.0);
					}

					if (TFItem.IEncoder.ADD_PANEL_CURSOR_) { sdkDrawLine(TFItem.IEncoder.CURSOR_FROM, TFItem.IEncoder.CURSOR_TO, TFItem.IEncoder.COLOR.Cursor.OUT, TFItem.IEncoder.PANEL_CURSOR_WIDTH); }
				}//fader panel
				DRAW_ROUND_PATH(TFItem.IEncoder.Encoder_Rect, TFItem.IEncoder.PANEL_IS_ROUND, TFItem.IEncoder.PANEL_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
				sdkDrawPathDraw(TFItem.IEncoder.PANEL_COLOR, TFItem.IEncoder.PANEL_BORDER_SIZE);
			}//panels types
			break;
			//////////////////////////////////////////

		case DT_None: break;
			///////////////////////////////
		default:break; //if return is 0 DT_None do paint nothing
		}
	}//paint layers loop
	 ////////////////////////////////////////////////////////////////				

}
void Touch_Fader::onPaint()
{
	if (MODE < MListBox) { PAINT_ITEM(); } //generic 'auto paint' already started to build for all modes exept listbox, = onpaint will ask what is needed to be paint to concened item file,
											  //trying to move most of the paint code in each item local file rather than here 
											  // & trying to find 'generic' recurent needs procedures like painting a rounded square path to serialize and reduce code redundancy between items
									///////////////////////////////////////////////////////////////// 
	if (SHOW_NAME && MODE < MListBox)  //name display
	{
		if (MODE == MFader && TFItem.IFader.NAME_IN_CAP) //fader name in cap
		{
			sdkFillText(TFItem.IFader.ELMT.CAP.RECT, AnsiCharPtr(item_name.c_str()), NAME_COLOR, NAME_SIZE, NAME_IS_BOLD, FALSE, taCenter, taTrailling);
		}
		else { sdkFillText(NAME_RECT, AnsiCharPtr(item_name.c_str()), NAME_COLOR, NAME_SIZE, NAME_IS_BOLD, FALSE, taCenter, taLeading); }
	}///name
	 /////////////////////////////////////////////////////////////////////////////////
	 // list Box to do : optimize
	 //
	else if (MODE == MListBox) //LB
	{
		float selected_item_y_pos = 0;

		TFItem.IListBox.UPD_LAYOUT();  // asks for dynamic upd

		int USED_DISPLAY_NB = TFItem.IListBox.USED_DISPLAY_NB;
		float Y_POS = 0;
		float lum_mod = 0;
		float offset_Y = 0;


		TColorUsine LIST_ITEM_COLOR, LIST_ITEM_BORDER_COLOR;
		int item = 0;
		//int lum_it = 0;

		for (int j = 0; j < USED_DISPLAY_NB + 1; j++) //NB_LIST_ITEMS /////////////////////// ITEMS
		{
			item = (int)fmin(j + TFItem.IListBox.LIST_SCROLL_VAL, TFItem.IListBox.NB_LIST_ITEMS - 1);

			if (TFItem.IListBox.SHOW.LIST_ITEMS_NUMBERS) // if show item nuber type >0
			{
				if (TFItem.IListBox.SHOW.ITEMS_NUMBERS_TYPE == 2) // original id pick
				{
					TFItem.IListBox.item_string = sdkIntToString(LIST_ITEMS.at(item).id);
				}
				else  // if 1 norml 'visual' number
				{
					TFItem.IListBox.item_string = sdkIntToString(item);
				}

			}
			else {
				TFItem.IListBox.item_string = "";  // if 0 none then no string for number
			}

			Y_POS = (float)(item) / (float)(USED_DISPLAY_NB);
			Y_POS = (Y_POS * TFItem.IListBox.LIST_CONTAINER_H) + TFItem.IListBox.LIST_HEADER_H;
			offset_Y = TFItem.IListBox.LIST_SCROLL_VAL01 * TFItem.IListBox.SCROLL_RATIO;
			Y_POS -= offset_Y;
			TFItem.IListBox.LIST_ITEM_RECT.top = Y_POS;
			TFItem.IListBox.LIST_ITEM_RECT.bottom = Y_POS + TFItem.IListBox.LIST_ITEM_H;

			if (TFItem.IListBox.SHOW.LIST_ITEMS_NAMES && item < TFItem.IListBox.NB_LIST_ITEMS)
			{
				if (TFItem.IListBox.SHOW.LIST_ITEMS_NUMBERS) { TFItem.IListBox.item_string.append(" - "); }
				//TFItem.IListBox.item_string.append(COMMA_LIST[item]);
				TFItem.IListBox.item_string.append(LIST_ITEMS.at(item).name);
				if (TFItem.IListBox.SHOW.ITEMS_ASSIGNED_VALUE != 0)
				{
					TFItem.IListBox.item_string.append(" = "); /////////////////////////////////////////checking value
					TFItem.IListBox.item_string.append(VAL_TO_STRING(LIST_ITEMS.at(item).assigned_value, TFItem.IListBox.SHOW.ITEMS_ASSIGNED_VALUE - 1));///////////////////////////////////////////////////////////////
				}
			}

			if (item == TFItem.IListBox.LIST_ITEM_SEL)
			{
				LIST_ITEM_BORDER_COLOR = TFItem.IListBox.COLOR.Item_Border_On.OUT;
				LIST_ITEM_COLOR = TFItem.IListBox.COLOR.Item_On.OUT;

				TFItem.IListBox.sel_item_string = TFItem.IListBox.item_string;
				selected_item_y_pos = Y_POS;

				TFItem.IListBox.LIST_ACTIVE_ITEM_RECT = TFItem.IListBox.LIST_ITEM_RECT; ////////////////////
			}
			else { LIST_ITEM_COLOR = TFItem.IListBox.COLOR.Item_Off.OUT;  LIST_ITEM_BORDER_COLOR = TFItem.IListBox.COLOR.Item_Border_Off.OUT; }
			lum_mod = RANDOM_PATERN[item] * TFItem.IListBox.LIST_LUM_MOD; ///   ????????????????
			DRAW_ROUND_PATH(TFItem.IListBox.LIST_ITEM_RECT, TFItem.IListBox.LIST_ITEM_IS_ROUND, TFItem.IListBox.LIST_ITEM_ROUND_SIZE, TFParams->CANVAS_RATIO, 0); //rnd/*LIST_ITEM_H
			sdkDrawPathFill(HI_COLOR(LIST_ITEM_COLOR, lum_mod));

			if (TFItem.IListBox.SHOW.LIST_ITEM_BORDER) { sdkDrawPathDraw(LIST_ITEM_BORDER_COLOR, TFItem.IListBox.LIST_ITEM_BORDER_SIZE); }

			sdkFillText(TFItem.IListBox.LIST_ITEM_RECT, AnsiCharPtr(TFItem.IListBox.item_string.c_str()), TFItem.IListBox.COLOR.Item_Name.OUT, TFItem.IListBox.LIST_ITEM_NAME_SIZE, FALSE, FALSE, taCenter, taCenter, FALSE); //names

		}//loop////////////////////////////////////////////// ITEMS


		 //sdkTraceInt(item);
		 ///////////////////////////////////////////////////////// XFADER on item
		if (TFItem.IListBox.SHOW.LIST_ITEM_XFADER  && (TFItem.IListBox.INSIDE.LIST_ACTIVE_ITEM || TFItem.IListBox.INSIDE.LIST_HEADER)) //&& TFItem.IListBox.LISTBOX_SETTING_ITEM
		{
			float XFADER_margin = 0.15f;

			if (!TFItem.IListBox.LIST_SCROLL_ACTIVE && TFParams->MSDWN)// && TFItem.IListBox.X_MODE)
			{
				TRectF ITEM_FADER;
				ITEM_FADER.top = selected_item_y_pos + (XFADER_margin*TFItem.IListBox.LIST_ITEM_H);
				ITEM_FADER.bottom = ITEM_FADER.top + TFItem.IListBox.LIST_ITEM_H*(1 - (2 * XFADER_margin));
				ITEM_FADER.left = TFItem.IListBox.LIST_ITEM_RECT.left;
				ITEM_FADER.right = ITEM_FADER.left + fmaxf(0.01f, (TFItem.IListBox.LIST_ITEM_RECT.right - TFItem.IListBox.LIST_ITEM_RECT.left)*TFItem.IListBox.LIST_ITEM_XFADER_VAL);

				DRAW_ROUND_PATH(ITEM_FADER, FALSE, 0, TFParams->CANVAS_RATIO, 0);
				sdkDrawPathFill(TFItem.IListBox.COLOR.Xfader.OUT);
				sdkDrawPathDraw(TFItem.IListBox.COLOR.Xfader_Border.OUT, 2);
				int ranged_val = (int)(TFItem.IListBox.LIST_ITEM_XFADER_VAL * 127.0f);
				string LIST_ITEM_XFADER_VAL_STRING = sdkIntToString(ranged_val);
				LIST_ITEM_XFADER_VAL_STRING.append(" \b");
			}

		}//end x fader////////////////////////////////////////////////////////////////////
		 ///KH////////////////////////
		if (TFItem.IListBox.SHOW.LIST_KH) { sdkFillRect(TFItem.IListBox.LIST_KH_UP, TFItem.IListBox.COLOR.KH.OUT, 0.0, TFItem.IListBox.COLOR.KH.OUT, 0.0); sdkFillRect(TFItem.IListBox.LIST_KH_DWN, TFItem.IListBox.COLOR.KH.OUT, 0.0, TFItem.IListBox.COLOR.KH.OUT, 0.0);
		}
		///////////////////////////
		if (TFItem.IListBox.SHOW.LIST_HEADER)
		{
			DRAW_ROUND_PATH(TFItem.IListBox.LIST_HEADER, TFItem.IListBox.LIST_HEADER_IS_ROUND, TFItem.IListBox.LIST_HEADER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
			sdkDrawPathFill(TFItem.IListBox.COLOR.Header.OUT);
			if (TFItem.IListBox.SHOW.LIST_HEADER_BORDER) { sdkDrawPathDraw(TFItem.IListBox.COLOR.Header_Border.OUT, TFItem.IListBox.LIST_HEADER_BORDER_SIZE); }
			sdkFillText(TFItem.IListBox.LIST_HEADER, AnsiCharPtr(TFItem.IListBox.sel_item_string.c_str()), TFItem.IListBox.COLOR.Item_Name.OUT, TFItem.IListBox.LIST_HEADER_NAME_SIZE, FALSE, FALSE, taCenter, taCenter, FALSE);
		}
		//bg
		if (TFItem.IListBox.SHOW.LIST_BORDER)
		{
			DRAW_ROUND_PATH(TFItem.IListBox.LIST_CONTAINER, TFItem.IListBox.LIST_BORDER_IS_ROUND, TFItem.IListBox.LIST_BG_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
			sdkDrawPathDraw(TFItem.IListBox.COLOR.Border.OUT, TFItem.IListBox.LIST_BORDER_SIZE);
		}
		//scrollers
		if (TFItem.IListBox.SHOW.LIST_SCROLLER && !TFItem.IListBox.LISTBOX_INERTIA_MODE)
		{
			if (TFItem.IListBox.SHOW.LIST_SCROLLER_CONTAINER)
			{
				DRAW_ROUND_PATH(TFItem.IListBox.LIST_SCROLLER_CONTAINER, TFItem.IListBox.LIST_SCROLLER_IS_ROUND, TFItem.IListBox.LIST_SCROLLER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
				sdkDrawPathFill(TFItem.IListBox.COLOR.Scroller_Container.OUT);
				if (TFItem.IListBox.SHOW.LIST_SCROLLER_BORDER) { sdkDrawPathDraw(TFItem.IListBox.COLOR.Scroller_Container_Border.OUT, TFItem.IListBox.LIST_SCROLLER_BORDER_SIZE); }

				if (TFItem.IListBox.LIST_USE_PAGES)
				{
					float Y_page = 0;
					for (int i = 0; i < TFItem.IListBox.LIST_NB_OF_PAGES; i++)
					{
						Y_page = ((i * TFItem.IListBox.LIST_PAGE_W)*TFItem.IListBox.LIST_CONTAINER_H) + TFItem.IListBox.LIST_HEADER_H;
						sdkDrawLine(sdkPointF(TFItem.IListBox.LIST_SCROLLER_CONTAINER.left, Y_page), sdkPointF(TFItem.IListBox.LIST_SCROLLER_CONTAINER.right, Y_page), HSLA(0, 0, 0.1f, 0.7f), 2);
					}
				}
				//end page
			}//conta

			// scroller shadow

			DRAW_ROUND_PATH(OFFSET_RECT(TFItem.IListBox.LIST_SCROLLER, 0.03f, 0.03f), TFItem.IListBox.LIST_SCROLLER_IS_ROUND, TFItem.IListBox.LIST_SCROLLER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
			sdkDrawPathFill(TFItem.IListBox.COLOR.Scroller_Shadow.OUT);

			DRAW_ROUND_PATH(TFItem.IListBox.LIST_SCROLLER, TFItem.IListBox.LIST_SCROLLER_IS_ROUND, TFItem.IListBox.LIST_SCROLLER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
			sdkDrawPathFill(TFItem.IListBox.COLOR.Scroller.OUT);

			if (TFItem.IListBox.SHOW.LIST_SCROLLER_BORDER) { sdkDrawPathDraw(TFItem.IListBox.COLOR.Scroller_Border.OUT, TFItem.IListBox.LIST_SCROLLER_BORDER_SIZE); }
			if (TFItem.IListBox.SHOW.LIST_SCROLLER_VALUE) { sdkFillText(TFItem.IListBox.LIST_SCROLLER, AnsiCharPtr(sdkIntToString((int)TFItem.IListBox.LIST_SCROLL_VAL).c_str()), HSLA(0, 0, 0.1f, 0.7f), 0.04f, FALSE, FALSE, taCenter, taLeading, FALSE); }
			//zoom
			if (TFItem.IListBox.SHOW.LIST_ZOOM)
			{
				sdkDrawLine(sdkPointF(1, TFItem.IListBox.zoom_up), sdkPointF(1, TFItem.IListBox.zoom_dwn), TFItem.IListBox.COLOR.Zoom_Line.OUT, TFItem.IListBox.LIST_ZOOM_LINE_WIDTH);
			}

			if (TFItem.IListBox.LIST_USE_PAGES)
			{
				sdkFillText(TFItem.IListBox.LIST_SCROLLER, AnsiCharPtr(TFItem.IListBox.LIST_PAGE_STRING.c_str()), HSLA(0, 0, 0.05f, 0.5f), 0.03f, TRUE, TRUE, taTrailling, taTrailling, FALSE);
			}
		}//end scroller////////////////////////////////////////////////////////

		 ///////////////////////////////////////////////////////// XFADER on header
		if (TFItem.IListBox.SHOW.LIST_ITEM_XFADER  && (TFItem.IListBox.INSIDE.LIST_ACTIVE_ITEM || TFItem.IListBox.INSIDE.LIST_HEADER))  //&& TFItem.IListBox.LISTBOX_SETTING_ITEM
		{
			float XFADER_margin = 0.15f;

			if (!TFItem.IListBox.LIST_SCROLL_ACTIVE && TFParams->MSDWN)// && TFItem.IListBox.X_MODE)
			{
				TRectF ITEM_FADER = { 0,0,0,0 };
				string LIST_ITEM_XFADER_VAL_STRING = sdkIntToString((int)(TFItem.IListBox.LIST_ITEM_XFADER_VAL * 127.0f));
				LIST_ITEM_XFADER_VAL_STRING.append(" \b");

				if (TFItem.IListBox.SHOW.LIST_HEADER)
				{
					if (TFItem.IListBox.SHOW.LIST_ITEM_XFADER_VALUE)
					{
						sdkFillText(TFItem.IListBox.LIST_HEADER, AnsiCharPtr(LIST_ITEM_XFADER_VAL_STRING.c_str()), TFItem.IListBox.COLOR.Xfader_Value.OUT, 0.03f, FALSE, FALSE, taTrailling, taCenter, FALSE);
					}
					//sdkFillText(ITEM_FADER, AnsiCharPtr(LIST_ITEM_XFADER_VAL_STRING.c_str()), HSLA(0, 0, 0.05, 0.5), 0.03, TRUE, TRUE, taLeading, taLeading, FALSE);
					ITEM_FADER.top = TFItem.IListBox.LIST_HEADER.top + (XFADER_margin*TFItem.IListBox.LIST_HEADER_H);
					ITEM_FADER.bottom = ITEM_FADER.top + TFItem.IListBox.LIST_HEADER_H*(1 - (3 * XFADER_margin));
					ITEM_FADER.left = TFItem.IListBox.LIST_HEADER.left;
					ITEM_FADER.right = ITEM_FADER.left + fmaxf(0.01f, (TFItem.IListBox.LIST_HEADER_W)*TFItem.IListBox.LIST_ITEM_XFADER_VAL);
					DRAW_ROUND_PATH(ITEM_FADER, FALSE, 0, TFParams->CANVAS_RATIO, 0);
					sdkDrawPathFill(TFItem.IListBox.COLOR.Xfader.OUT);
					sdkDrawPathDraw(TFItem.IListBox.COLOR.Xfader_Border.OUT, 2);
				}
			}

		}//end x fader////////////////////////////////////////////////////////////////////

		 // higlight cliced hitem//////////////////////////////
		if (TFItem.IListBox.SHOW.LIST_PULSE && TFParams->PULSE) ///
		{

				//DRAW_ROUND_PATH(SCALE_RECT(TFItem.IListBox.LIST_HEADER,TFParams->PULSE_RAMP, TFParams->PULSE_RAMP), TFItem.IListBox.LIST_HEADER_IS_ROUND, TFItem.IListBox.LIST_HEADER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
				DRAW_ROUND_PATH(TFItem.IListBox.LIST_HEADER, TFItem.IListBox.LIST_HEADER_IS_ROUND, TFItem.IListBox.LIST_HEADER_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
				sdkDrawPathFill(HSLA(0, 0, 1, (1-TFParams->PULSE_RAMP)*TFItem.IListBox.LIST_PULSE_AMOUNT));

				DRAW_ROUND_PATH(TFItem.IListBox.LIST_ACTIVE_ITEM_RECT, TFItem.IListBox.LIST_ITEM_IS_ROUND, TFItem.IListBox.LIST_ITEM_ROUND_SIZE, TFParams->CANVAS_RATIO, 0);
				sdkDrawPathFill(HSLA(0, 0, 1, (1 - TFParams->PULSE_RAMP)*TFItem.IListBox.LIST_PULSE_AMOUNT));

		} //// clic pulse



	}//end LB//////////////////////////////////////////////////////////////////////////////////////////////////


	 /*
	 //////////// MATRIX ///////////////////////////////////////////////
	 else if (MODE == 7)
	 {
	 // MX_CELL_STRING = "";

	 if (TFItem.IMatrix.MX.SHOW_NAME)
	 {
	 TFItem.IMatrix.NB_LIST_ITEMS = size_t _countof(COMMA_LIST); // to opte
	 }

	 TRectF MX_CELL;
	 float MX_CELL_W = 0;
	 float MX_CELL_H = 0;

	 MX_CELL_W = (1.0 / (float)TFItem.IMatrix.MX.nb_cols) - (TFItem.IMatrix.MX.gap*2.0);
	 MX_CELL_H = (1.0 / (float)TFItem.IMatrix.MX.nb_rows) - (TFItem.IMatrix.MX.gap*2.0);

	 float X_CELL = 0;
	 float Y_CELL = 0;

	 int cell = 0;
	 float cell_hilight = 0;
	 int cell_hue_offset = 0;
	 float cell_sat_offset = 0;
	 float cell_Z_val = 0;
	 float cell_X_val = 0;
	 float cell_Y_val = 0;
	 TColorUsine Border_color;

	 float HUE_SHIFT = 0;

	 float cell_lum = 0;

	 for (int i = 0; i < TFItem.IMatrix.MX.nb_cols; i++)
	 {
	 X_CELL = ((float)i / (float)TFItem.IMatrix.MX.nb_cols) + TFItem.IMatrix.MX.gap;

	 for (int j = 0; j < TFItem.IMatrix.MX.nb_rows; j++)
	 {

	 cell = i + (j*TFItem.IMatrix.MX.nb_cols);

	 cell_Z_val = TFItem.IMatrix.Z_VALUES[cell];// *TFItem.IMatrix.SELECTED_HILIGHT;
	 cell_X_val = TFItem.IMatrix.X_VALUES[cell];
	 cell_Y_val = TFItem.IMatrix.Y_VALUES[cell];

	 Y_CELL = ((float)j / (float)TFItem.IMatrix.MX.nb_rows) + TFItem.IMatrix.MX.gap;


	 HUE_SHIFT = ((float)cell / (float)(TFItem.IMatrix.MX.nb_cols*TFItem.IMatrix.MX.nb_rows)) * TFItem.IMatrix.CELLS_HUE_OFFSET;

	 MX_CELL.left = X_CELL;
	 MX_CELL.right = MX_CELL.left + MX_CELL_W;
	 MX_CELL.top = Y_CELL;
	 MX_CELL.bottom = MX_CELL.top + MX_CELL_H;

	 if (cell == TFItem.IMatrix.MX.cell){ cell_hilight = (TFParams->MSDWN + 1)*TFItem.IMatrix.SELECTED_HILIGHT; cell_hue_offset = (TFParams->MSDWN + 1)*TFItem.IMatrix.SELECTED_HUE_OFFSET; cell_sat_offset = (TFParams->MSDWN + 1)*TFItem.IMatrix.SELECTED_SAT_OFFSET; }
	 else{ cell_hilight = 0; cell_hue_offset = 0; cell_sat_offset = 0; }

	 DRAW_ROUND_PATH(MX_CELL, TFItem.IMatrix.ROUND_CELL, TFItem.IMatrix.ROUND_SIZE, TFParams->CANVAS_RATIO, CAP_ROUND_SMOOTHNESS);

	 if (TFItem.IMatrix.MODE == 0 || TFItem.IMatrix.MODE == 1) //mmt or toggle
	 {
	 cell_lum = TFItem.IMatrix.CELL_OFF_LUMINOSITY + (cell_Z_val*TFItem.IMatrix.SELECTED_HILIGHT);
	 }
	 else if (TFItem.IMatrix.MODE == 2) //fade
	 {
	 cell_lum = TFItem.IMatrix.CELL_OFF_LUMINOSITY + cell_Z_val;
	 }
	 else if (TFItem.IMatrix.MODE >= 3) //sliders
	 {
	 cell_lum = TFItem.IMatrix.CELL_OFF_LUMINOSITY+TFItem.IMatrix.SELECTED_HILIGHT;// +cell_Y_val;
	 }


	 sdkDrawPathFill(HSLA(ITEM_HUE +HUE_SHIFT+ cell_hue_offset, TFItem.IMatrix.CELL_SATURATION + cell_sat_offset, cell_lum, TFItem.IMatrix.CELL_OPACITY));

	 if (TFItem.IMatrix.SHOW_CELL_BORDER)
	 {
	 if (TFItem.IMatrix.MODE < 3)
	 {
	 Border_color = HSLA(ITEM_HUE + HUE_SHIFT + cell_hue_offset, TFItem.IMatrix.CELL_BORDER_SATURATION + cell_sat_offset, TFItem.IMatrix.CELL_BORDER_LUMINOSITY + cell_hilight + cell_Z_val, TFItem.IMatrix.CELL_BORDER_OPACITY);
	 }
	 else if (TFItem.IMatrix.MODE >= 3)
	 //fade Butt
	 {
	 if (TFItem.IMatrix.Z_VALUES[cell] == 1){ Border_color = TFItem.IMatrix.ON_COLOR; }
	 else//{Border_color = TFItem.IMatrix.OFF_COLOR;}}
	 {
	 Border_color = HSLA(ITEM_HUE + HUE_SHIFT + cell_hue_offset, TFItem.IMatrix.CELL_BORDER_SATURATION + cell_sat_offset, TFItem.IMatrix.CELL_BORDER_LUMINOSITY + cell_hilight + cell_Z_val, TFItem.IMatrix.CELL_BORDER_OPACITY);
	 }
	 }

	 sdkDrawPathDraw(Border_color, TFItem.IMatrix.BORDER_SIZE);
	 }//border

	 if (TFItem.IMatrix.SHOW_FADERS && TFItem.IMatrix.MODE>2)
	 {
	 TRectF MX_FADER;
	 if (TFItem.IMatrix.SHOW_Y_FADER)
	 {
	 MX_FADER.left = MX_CELL.left ;
	 MX_FADER.right = MX_CELL.right ;
	 MX_FADER.top = MX_CELL.top ;
	 MX_FADER.bottom = MX_CELL.bottom - (cell_Y_val*MX_CELL_H);
	 DRAW_ROUND_PATH(MX_FADER, TFItem.IMatrix.ROUND_CELL, TFItem.IMatrix.ROUND_SIZE, TFParams->CANVAS_RATIO, CAP_ROUND_SMOOTHNESS);
	 sdkDrawPathFill(HSLA(ITEM_HUE + HUE_SHIFT + cell_hue_offset, TFItem.IMatrix.FADERS_SATURATION, TFItem.IMatrix.FADERS_LUMINOSITY, TFItem.IMatrix.FADERS_OPACITY));
	 sdkDrawLine(sdkPointF(MX_CELL.left + 0.01, MX_FADER.bottom), sdkPointF(MX_CELL.right - 0.01, MX_FADER.bottom), HSLA(ITEM_HUE + HUE_SHIFT, 0.25, 0.5, TFItem.IMatrix.FADERS_LINE_OPACITY), TFItem.IMatrix.FADERS_LINE_SIZE);
	 }

	 if (TFItem.IMatrix.SHOW_X_FADER)
	 {
	 MX_FADER.left = MX_CELL.left + (cell_X_val*MX_CELL_W);
	 MX_FADER.right = MX_CELL.right ;
	 MX_FADER.top = MX_CELL.top ;
	 MX_FADER.bottom = MX_CELL.bottom ;
	 DRAW_ROUND_PATH(MX_FADER, TFItem.IMatrix.ROUND_CELL, TFItem.IMatrix.ROUND_SIZE, TFParams->CANVAS_RATIO, CAP_ROUND_SMOOTHNESS);
	 sdkDrawPathFill(HSLA(ITEM_HUE + HUE_SHIFT+cell_hue_offset, TFItem.IMatrix.FADERS_SATURATION, TFItem.IMatrix.FADERS_LUMINOSITY, TFItem.IMatrix.FADERS_OPACITY));
	 sdkDrawLine(sdkPointF(MX_FADER.left, MX_FADER.top), sdkPointF(MX_FADER.left, MX_FADER.bottom), HSLA(ITEM_HUE + HUE_SHIFT, 0.25, 0.5, TFItem.IMatrix.FADERS_LINE_OPACITY), TFItem.IMatrix.FADERS_LINE_SIZE);
	 }

	 }


	 if (TFItem.IMatrix.MX.SHOW_INDEX)
	 {
	 sdkFillText(MX_CELL, AnsiCharPtr(sdkIntToString(cell).c_str()), NAME_COLOR, NAME_SIZE*TFItem.IMatrix.NAME_SIZE_MULTIPLIER, FALSE, FALSE, taCenter, taLeading, FALSE);
	 }

	 if (TFItem.IMatrix.MX.SHOW_NAME)
	 {
	 if (cell < TFItem.IMatrix.NB_LIST_ITEMS){ TFItem.IMatrix.MX_CELL_STRING = COMMA_LIST[cell]; }
	 else{ TFItem.IMatrix.MX_CELL_STRING = ""; }
	 sdkFillText(MX_CELL, AnsiCharPtr(TFItem.IMatrix.MX_CELL_STRING.c_str()), NAME_COLOR, NAME_SIZE*TFItem.IMatrix.NAME_SIZE_MULTIPLIER, FALSE, FALSE, taCenter, taCenter, FALSE);
	 }

	 if (TFItem.IMatrix.MX.SHOW_X_VAL)
	 {
	 int X_VAL = (TFItem.IMatrix.X_VALUES[cell] * (TFParams->OUT_MAX - TFParams->OUT_MIN)) + TFParams->OUT_MIN;
	 sdkFillText(MX_CELL, AnsiCharPtr(sdkIntToString(X_VAL).c_str()), NAME_COLOR, NAME_SIZE*TFItem.IMatrix.VALUE_SIZE_MULTIPLIER, FALSE, FALSE, taLeading, taTrailling, FALSE);
	 }
	 if (TFItem.IMatrix.MX.SHOW_Y_VAL)
	 {
	 int Y_VAL = (TFItem.IMatrix.Y_VALUES[cell] * (TFParams->OUT_MAX - TFParams->OUT_MIN)) + TFParams->OUT_MIN;
	 sdkFillText(MX_CELL, AnsiCharPtr(sdkIntToString(Y_VAL).c_str()), NAME_COLOR, NAME_SIZE*TFItem.IMatrix.VALUE_SIZE_MULTIPLIER, FALSE, FALSE, taCenter, taTrailling, FALSE);
	 }
	 if (TFItem.IMatrix.MX.SHOW_Z_VAL)
	 {
	 int Z_VAL = (TFItem.IMatrix.Z_VALUES[cell] * (TFParams->OUT_MAX - TFParams->OUT_MIN)) + TFParams->OUT_MIN;
	 sdkFillText(MX_CELL, AnsiCharPtr(sdkIntToString(Z_VAL).c_str()), NAME_COLOR, NAME_SIZE*TFItem.IMatrix.VALUE_SIZE_MULTIPLIER, FALSE, FALSE, taTrailling, taTrailling, FALSE);
	 }



	 }//j loop
	 }// i loop


	 }//mx
	 */

	 /// end matrix
	 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	 // values & caption disp////////////////////////////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////


}//////////end paint
/////////////////////////////////////////end TF


