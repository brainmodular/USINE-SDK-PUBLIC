//-----------------------------------------------------------------------------
//@file  
//	Wiimote.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the Wiimote class.
//
//  A template include to start from for new user module.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "Wiimote.h"

#ifdef min
    #undef min
#endif

#ifdef max
    #undef max
#endif
//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    //initialiseJuce_GUI ();
	pModule = new Wiimote();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((Wiimote*)pModule);	
	
    //shutdownJuce_GUI (); 
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Wiimote";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Wiimote";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0b";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
Wiimote::Wiimote()
    : wm (nullptr)
	, numWiiIndex (-1)
    , updateTimestamp (0)
	, isWiiConnected (false)
    , isWiiIrActivated (false)
    , isWiiNunchukActivated (false)
    , isWiiMotionplusActivated (false)
	, connectLedState (WIIMOTE_LED_NONE)
	, connectRumbleState (0)
    , connectMotionplusState (false)
	, connectInfraredState (false)
	, wiiAccX (0.0f)
    , wiiAccY (0.0f)
    , wiiAccZ (0.0f)
    , wiiAccOldX (0.0f)
    , wiiAccOldY (0.0f)
    , wiiAccOldZ (0.0f)
    , wiiSpeedX (0.0f)
    , wiiSpeedY (0.0f)
    , wiiSpeedZ (0.0f)
    , wiiSpeedOldX (0.0f)
    , wiiSpeedOldY (0.0f)
    , wiiSpeedOldZ (0.0f)
    , wiiDistanceX (0.0f)
    , wiiDistanceY (0.0f)
    , wiiDistanceZ (0.0f)
    , wiiDistanceOldX (0.0f)
    , wiiDistanceOldY (0.0f)
	, wiiDistanceOldZ (0.0f)
	, wiiNcAccX (0.0f)
	, wiiNcAccY (0.0f)
	, wiiNcAccZ (0.0f)
{
}

// destructor
Wiimote::~Wiimote()
{
	//String traceString;
	//traceString << "deconnect from wiimote id:" << numWiiIndex;
	//sdkTraceLogChar (traceString.toUTF8().getAddress());
	
    try 
	{
        if (wiimoteManagerPtr != nullptr)
        
        {
			if (numWiiIndex != -1)
			{
				wiimoteManagerPtr->wantAccelFeedback(numWiiIndex, false);
				wiimoteManagerPtr->wantInfraredFeedback(numWiiIndex, false);
				wiimoteManagerPtr->wantMotionplusFeedback(numWiiIndex, false);
				// 
				wiimoteManagerPtr->disconnectWiimote(numWiiIndex, this);
			}
		    // we have to call it manually because we just unplugged the wiimote event callback
		
		    onWiiDisconnect ();
        
            wiimoteManagerPtr = nullptr;
	        WiimoteManager::unregister ();
        }
	}
	catch (...) 
	{
		
	}
}


void Wiimote::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = PARAMS_TOTAL_NUM;

	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;

    if (pMasterInfo != nullptr)
    	wiimoteManagerPtr = WiimoteManager::getInstance ();
}

//-----------------------------------------------------------------------------
// query system and init
void Wiimote::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
}

//-----------------------------------------------------------------------------
// parameters and process
void Wiimote::onGetParamInfo (int paramIndex, TParamInfo* pParamInfo)
{	
	// btnPollWiiEvent
	if (paramIndex == 0)
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "wii";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsInvisible		= TRUE;
	}
	// fdrActiveWiiNum
	else if (paramIndex == 1)
	{
		pParamInfo->ParamType		    = ptDataFader;
		pParamInfo->Caption			    = "wiimote num";
		pParamInfo->IsInput			    = TRUE;
		pParamInfo->IsOutput		    = FALSE;
		pParamInfo->MinValue		    = 1;
		pParamInfo->MaxValue		    = 4;
		//pParamInfo->DefaultValue	    = 1;
		pParamInfo->Symbol			    = "";
		pParamInfo->Format			    = "%.0f";
		pParamInfo->DontSave			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
	}
	// swtchWiiVibration
	else if (paramIndex == 2)
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= "vibration";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		= TRUE;
		//pParamInfo->SeparatorCaption = "devices";
	}	
	// swtchWiiLeds
	else if ((paramIndex >= PARAMS_BEFORE_WII_LEDS) && (paramIndex < (PARAMS_BEFORE_WII_LEDS + PARAMS_WII_LEDS_NUM)))
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= getWiiLedsCaptions (paramIndex - PARAMS_BEFORE_WII_LEDS);
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
	}
	// fdrWiiAcc
	else if ((paramIndex >= PARAMS_WII_BEFORE_ACC) && (paramIndex < (PARAMS_WII_BEFORE_ACC + PARAMS_WII_ACC_NUM)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= getWiiAccCaptions (paramIndex - PARAMS_WII_BEFORE_ACC);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -200;
		pParamInfo->MaxValue		= 200;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;

        if (paramIndex == PARAMS_WII_BEFORE_ACC)
		    pParamInfo->IsSeparator		 = TRUE;
	}
	// fdrWiiPos
	else if ((paramIndex >= PARAMS_WII_BEFORE_POS) && (paramIndex < (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= getWiiPosCaptions (paramIndex - PARAMS_WII_BEFORE_POS);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->DefaultValue	= 0.5;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.3f";
        pParamInfo->DontSave        = TRUE;

        if (paramIndex == PARAMS_WII_BEFORE_POS)
		    pParamInfo->IsSeparator		 = TRUE;
	}
	// btnResetWiiPos
	if ((paramIndex >= PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM) && (paramIndex < (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM + 1)))
	{
		pParamInfo->ParamType		= ptButton;
		pParamInfo->Caption			= "reset pos";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
	}
	// fdrWiiPitch
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 1)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "wii pitch";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -180;
		pParamInfo->MaxValue		= 180;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
        pParamInfo->IsSeparator		= TRUE;
	}
	// fdrWiiRoll
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT + 1) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 2)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "wii roll";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -180;
		pParamInfo->MaxValue		= 180;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
	}
	// fdrWiiYaw
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT + 2) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 3)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "wii yaw";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -180;
		pParamInfo->MaxValue		= 180;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonA
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 1)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn A";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
        pParamInfo->IsSeparator		= TRUE;
	}
	// rledWiiButtonB
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 1) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 2)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn B";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonOne
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 2) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 3)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn 1";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonTwo
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 3) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 4)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn 2";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonPlus
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 4) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 5)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn +";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonMinus
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 5) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 6)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn -";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonHome
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 6) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 7)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn Home";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonUp
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 7) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 8)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn Up";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonDown
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 8) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 9)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn Down";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonLeft
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 9) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 10)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn Left";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiButtonRight
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 10) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 11)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "wii btn Right";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// swtchWantMotionpluss
	if ((paramIndex >= PARAMS_WII_BEFORE_WANT_MPLUS) && (paramIndex < (PARAMS_WII_BEFORE_WANT_MPLUS + 1)))
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= "motion plus";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		 = TRUE;
	}
	// fdrWiiMplusAcc
	else if ((paramIndex >= PARAMS_WII_BEFORE_MPLUS_ACC_NUM) && (paramIndex < (PARAMS_WII_BEFORE_MPLUS_ACC_NUM + PARAMS_WII_MPLUS_ACC_NUM)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= getWiiMplusAccCaptions (paramIndex - PARAMS_WII_BEFORE_MPLUS_ACC_NUM);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -600;
		pParamInfo->MaxValue		= 600;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
	}
	// rledWiiNunchukButtonC
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK + 1)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "nc btn C";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
		pParamInfo->IsSeparator		 = TRUE;
	}
	// rledWiiNunchukButtonZ
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK + 1) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK + PARAMS_WII_NUNCHUK_BUTTONS_NUM)))
	{
		pParamInfo->ParamType		= ptRightLed;
		pParamInfo->Caption			= "nc btn Z";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
        pParamInfo->DontSave        = TRUE;
	}
	// fdrWiiNunchukOrient
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= getWiiNunchukOrientCaptions (paramIndex - PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM);
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= -200;
		pParamInfo->MaxValue		= 200;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
	}
	// fdrWiiNunchukJsAngle
	else if ((paramIndex >= (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM)) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + 1)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "nc js ang";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 360;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.0f";
        pParamInfo->DontSave        = TRUE;
	}
	// fdrWiiNunchukJsMag
	else if ((paramIndex >= (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + 1)) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_JS_NUM)))
	{
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "nc js mag";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.3f";
        pParamInfo->DontSave        = TRUE;
	}
	// swtchWantIr
	else if ((paramIndex >= PARAMS_WII_BEFORE_WANT_IR) && (paramIndex < PARAMS_WII_BEFORE_IR_ZONE_NUM))
	{
		pParamInfo->ParamType		= ptSwitch;
		pParamInfo->Caption			= "infra red";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		 = TRUE;
	}
	// fdrWiiIrX, fdrWiiIrY, fdrWiiIrSize for all zones
	else if ((paramIndex >= (PARAMS_WII_BEFORE_IR_ZONE_NUM)) && (paramIndex < (PARAMS_WII_BEFORE_IR_ZONE_NUM + (PARAMS_WII_IR_ZONE_NUM * PARAMS_WII_IR_ZONE_INFOS_NUM))))
	{
		int irBase = paramIndex - PARAMS_WII_BEFORE_IR_ZONE_NUM;
		int zoneOffset;
		int infosOffset = irBase % PARAMS_WII_IR_ZONE_INFOS_NUM;
		
		if (irBase < PARAMS_WII_IR_ZONE_INFOS_NUM)
			zoneOffset = 0;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 2))
			zoneOffset = 1;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 3))
			zoneOffset = 2;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 4))
			zoneOffset = 3;
		
		AnsiCharPtr caption = getWiiIrCaptions (zoneOffset, infosOffset);

		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= caption;
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 1;
		pParamInfo->DefaultValue	= 0;
		pParamInfo->Format			= "%.2f";
		pParamInfo->Symbol			= "";
        pParamInfo->DontSave        = TRUE;
		if (paramIndex - PARAMS_WII_BEFORE_IR_ZONE_NUM == 0)
			pParamInfo->IsSeparator		 = TRUE;
			
	}
  	// default case
	else{}
}

void Wiimote::onSetEventAddress (int paramIndex, UsineEventPtr pEvent)
{
	// btnPollWiiEvent
	if (paramIndex == 0)
	{
		btnPollWiiEvent = pEvent;
	}
	// m_faderActiveWiiNum
	else if (paramIndex == 1)
	{
		fdrActiveWiiNum = pEvent;
	}
	// switchWiiVibration
	else if (paramIndex == 2)
	{
		swtchWiiVibration = pEvent;
	}	
	// switchWiiLeds
	else if ((paramIndex >= PARAMS_BEFORE_WII_LEDS) && (paramIndex < (PARAMS_BEFORE_WII_LEDS + PARAMS_WII_LEDS_NUM)))
	{
		swtchWiiLeds[paramIndex - PARAMS_BEFORE_WII_LEDS] = pEvent;
	}
	// fdrWiiAcc
	else if ((paramIndex >= PARAMS_WII_BEFORE_ACC) && (paramIndex < (PARAMS_WII_BEFORE_ACC + PARAMS_WII_ACC_NUM)))
	{
		fdrWiiAcc[paramIndex - PARAMS_WII_BEFORE_ACC] = pEvent;
	}
	// fdrWiiPos
	else if ((paramIndex >= PARAMS_WII_BEFORE_POS) && (paramIndex < (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM)))
	{
		fdrWiiPos[paramIndex - PARAMS_WII_BEFORE_POS] = pEvent;
	}
	// btnResetWiiPos
	if ((paramIndex >= PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM) && (paramIndex < (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM + 1)))
	{
		btnResetWiiPos = pEvent;
	}
	// fdrWiiPitch
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 1)))
	{
		fdrWiiPitch = pEvent;
	}
	// fdrWiiRoll
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT + 1) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 2)))
	{
		fdrWiiRoll = pEvent;
	}
	// fdrWiiYaw
	else if ((paramIndex >= PARAMS_WII_BEFORE_ROT + 2) && (paramIndex < (PARAMS_WII_BEFORE_ROT + 3)))
	{
		fdrWiiYaw = pEvent;
	}
	// rledWiiButtonA
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 1)))
	{
		rledWiiButtonA = pEvent;
	}
	// rledWiiButtonB
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 1) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 2)))
	{
		rledWiiButtonB = pEvent;
	}
	// rledWiiButtonOne
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 2) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 3)))
	{
		rledWiiButtonOne = pEvent;
	}
	// rledWiiButtonTwo
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 3) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 4)))
	{
		rledWiiButtonTwo = pEvent;
	}
	// rledWiiButtonPlus
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 4) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 5)))
	{
		rledWiiButtonPlus = pEvent;
	}
	// rledWiiButtonMinus
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 5) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 6)))
	{
		rledWiiButtonMinus = pEvent;
	}
	// rledWiiButtonHome
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 6) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 7)))
	{
		rledWiiButtonHome = pEvent;
	}
	// rledWiiButtonUp
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 7) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 8)))
	{
		rledWiiButtonUp = pEvent;
	}
	// rledWiiButtonDown
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 8) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 9)))
	{
		rledWiiButtonDown = pEvent;
	}
	// rledWiiButtonLeft
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 9) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 10)))
	{
		rledWiiButtonLeft = pEvent;
	}
	// rledWiiButtonRight
	else if ((paramIndex >= PARAMS_WII_BEFORE_BUTTONS + 10) && (paramIndex < (PARAMS_WII_BEFORE_BUTTONS + 11)))
	{
		rledWiiButtonRight = pEvent;
	}
	// swtchWantMotionpluss
	if ((paramIndex >= PARAMS_WII_BEFORE_WANT_MPLUS) && (paramIndex < (PARAMS_WII_BEFORE_WANT_MPLUS + 1)))
	{
		swtchWantMotionplus = pEvent;
	}
	// fdrWiiMplusAcc
	else if ((paramIndex >= PARAMS_WII_BEFORE_MPLUS_ACC_NUM) && (paramIndex < (PARAMS_WII_BEFORE_MPLUS_ACC_NUM + PARAMS_WII_MPLUS_ACC_NUM)))
	{
		fdrWiiMplusAcc[paramIndex - PARAMS_WII_BEFORE_MPLUS_ACC_NUM] = pEvent;
	}
	// rledWiiNunchukButtonC
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK + 1)))
	{
		rledWiiNunchukButtonC = pEvent;
	}
	// rledWiiNunchukButtonZ
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK + 1) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK + PARAMS_WII_NUNCHUK_BUTTONS_NUM)))
	{
		rledWiiNunchukButtonZ = pEvent;
	}
	// fdrWiiNunchukOrient
	else if ((paramIndex >= PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM)))
	{
		fdrWiiNunchukOrient[paramIndex - PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM] = pEvent;
	}
	// fdrWiiNunchukJsAngle
	else if ((paramIndex >= (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM)) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + 1)))
	{
		fdrWiiNunchukJsAngle = pEvent;
	}
	// fdrWiiNunchukJsMag
	else if ((paramIndex >= (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + 1)) && (paramIndex < (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_JS_NUM)))
	{
		fdrWiiNunchukJsMag = pEvent;
	}
	// swtchWantIr
	else if ((paramIndex >= PARAMS_WII_BEFORE_WANT_IR) && (paramIndex < PARAMS_WII_BEFORE_IR_ZONE_NUM))
	{
		swtchWantInfrared = pEvent;
	}
	// fdrWiiIrX, fdrWiiIrY, fdrWiiIrSize for all zones
	else if ((paramIndex >= (PARAMS_WII_BEFORE_IR_ZONE_NUM)) && (paramIndex < (PARAMS_WII_BEFORE_IR_ZONE_NUM + (PARAMS_WII_IR_ZONE_NUM * PARAMS_WII_IR_ZONE_INFOS_NUM))))
	{
		int irBase = paramIndex - PARAMS_WII_BEFORE_IR_ZONE_NUM;
		int zoneOffset;
		int infosOffset = irBase % PARAMS_WII_IR_ZONE_INFOS_NUM;
		
		if (irBase < PARAMS_WII_IR_ZONE_INFOS_NUM)
			zoneOffset = 0;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 2))
			zoneOffset = 1;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 3))
			zoneOffset = 2;
		else if (irBase < (PARAMS_WII_IR_ZONE_INFOS_NUM * 4))
			zoneOffset = 3;
		
		
		
		switch (infosOffset)
		{
			case 0:
				fdrWiiIrX[zoneOffset] = pEvent;
				break;
			case 1:
				fdrWiiIrY[zoneOffset] = pEvent;
				break;
			case 2:
				fdrWiiIrSize[zoneOffset] = pEvent;
				break;
			default:
				break;
		}
	}
    
	// default case
	else{}
}

void Wiimote::onCallBack (UsineMessage *Message) 
{
	int paramIndex = (int)Message->wParam;
 
	// btnPollWiiEvent
	if (paramIndex == 0)
	{
	}
	// fdrActiveWiiNum
	else if (paramIndex == 1)
	{
		if (numWiiIndex >= 0)
			wiimoteManagerPtr->disconnectWiimote(numWiiIndex, this);
		
		numWiiIndex = (int)sdkGetEvtData(fdrActiveWiiNum) - 1;
		wiimoteManagerPtr->connectWiimote(numWiiIndex, this);
		
		String traceString;
		traceString << "connected to wiimote id:" << numWiiIndex;
		sdkTraceLogChar (traceString.toUTF8().getAddress());	
	}
	// switchWiiVibration
	else if (paramIndex == 2)
	{
		toggleWiiVibration();
	}	
	// switchWiiLeds
	else if ((paramIndex >= PARAMS_BEFORE_WII_LEDS) && (paramIndex < (PARAMS_BEFORE_WII_LEDS + PARAMS_WII_LEDS_NUM)))
	{
		switchWiiLed(paramIndex - PARAMS_BEFORE_WII_LEDS);
	}
	// btnResetWiiPos
	if ((paramIndex >= PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM) && (paramIndex < (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM + 1)))
	{
		resetWiiPos ();
	}
	// swtchWantMotionpluss
	if ((paramIndex >= PARAMS_WII_BEFORE_WANT_MPLUS) && (paramIndex < (PARAMS_WII_BEFORE_WANT_MPLUS + 1)))
	{
        switchWiiMotionplus (paramIndex - PARAMS_BEFORE_WII_LEDS);
	}
	// swtchWantIr
	else if ((paramIndex >= PARAMS_WII_BEFORE_WANT_IR) && (paramIndex < PARAMS_WII_BEFORE_IR_ZONE_NUM))
	{
		switchWiiInfrared (paramIndex - PARAMS_WII_BEFORE_WANT_IR);
	}
	// default case
	else{}
}

void Wiimote::onProcess () 
{
    if (wiimoteManagerPtr->receiveTimestamp >= updateTimestamp + 10)
    {
        onWiiUpdateEvent ();
        updateTimestamp = wiimoteManagerPtr->receiveTimestamp;
    }
}

void Wiimote::resetWiiPos()
{
    wiiAccX         = 0.0f;
    wiiAccY         = 0.0f;
    wiiAccZ         = 0.0f;
    wiiAccOldX      = 0.0f;
    wiiAccOldY      = 0.0f;
    wiiAccOldZ      = 0.0f;
    wiiSpeedX       = 0.0f;
    wiiSpeedY       = 0.0f;
    wiiSpeedZ       = 0.0f;
    wiiSpeedOldX    = 0.0f;
    wiiSpeedOldY    = 0.0f;
    wiiSpeedOldZ    = 0.0f;
    wiiDistanceX    = 0.0f;
    wiiDistanceY    = 0.0f;
    wiiDistanceZ    = 0.0f;
    wiiDistanceOldX = 0.0f;
    wiiDistanceOldY = 0.0f;
    wiiDistanceOldZ = 0.0f;
    wiiNcAccX       = 0.0f;
    wiiNcAccY       = 0.0f;
    wiiNcAccZ       = 0.0f;
}

//
char* Wiimote::getWiiLedsCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "led 1";
        break;
    case 1:
        return "led 2";
        break;
    case 2:
        return "led 3";
        break;
    case 3:
        return "led 4";
        break;
    default:
        return "";
    }
}

//
char* Wiimote::getWiiAccCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "wii acc X";
        break;
    case 1:
        return "wii acc Y";
        break;
    case 2:
        return "wii acc Z";
        break;
    default:
        return "";
    }
}

//
char* Wiimote::getWiiPosCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "wii pos X";
        break;
    case 1:
        return "wii pos Y";
        break;
    case 2:
        return "wii pos Z";
        break;
    default:
        return "";
    }
}

char* Wiimote::getWiiMplusAccCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "mplus acc X";
        break;
    case 1:
        return "mplus acc Y";
        break;
    case 2:
        return "mplus acc Z";
        break;
    default:
        return "";
    }
}

char* Wiimote::getWiiMplusOrientCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "mplus pitch";
        break;
    case 1:
        return "mplus roll";
        break;
    case 2:
        return "mplus yaw";
        break;
    default:
        return "";
    }
}


char* Wiimote::getWiiNunchukOrientCaptions (int index)
{
    switch (index)
    {
    case 0:
        return "nc pitch";
        break;
    case 1:
        return "nc roll";
        break;
    case 2:
        return "nc yaw";
        break;
    default:
        return "";
    }
}


const char* Wiimote::getWiiIrCaptions (int indexZone, int indexParam)
{
	int currentZone = indexZone + 1;
	
    tmpWiiIrCaption = CharPointer_UTF8 ("ir[");
	tmpWiiIrCaption << currentZone << "] ";
    switch (indexParam)
    {
		case 0:
			tmpWiiIrCaption << "X";
			break;
		case 1:
			tmpWiiIrCaption << "Y";
			break;
		case 2:
			tmpWiiIrCaption << "Size";
			break;
		default:
			return "";
    }
	return (AnsiCharPtr)tmpWiiIrCaption.toUTF8().getAddress();
}

void Wiimote::toggleWiiVibration()
{
	int wantEnabled;
	float switchState = sdkGetEvtData (swtchWiiVibration);
	switchState == 1 ? wantEnabled = 1 : wantEnabled = 0;
	
	if (isWiiConnected)
	{
		wiimoteManagerPtr->setWiiRumble(numWiiIndex, switchState);
	}
	else 
	{
		connectRumbleState = wantEnabled;
	} 
}

void Wiimote::switchWiiLed (int index)
{
	bool wantEnabled;
	float switchState = sdkGetEvtData (swtchWiiLeds[index]);
	switchState == 1 ? wantEnabled = true : wantEnabled = false;

    int ledsState = WIIMOTE_LED_NONE;
	
	if (wm != nullptr)
		ledsState = wm->leds;
    else
        ledsState = connectLedState;

	switch (index)
	{
	case 0:
		if (wantEnabled)
		{
			ledsState = ledsState | WIIMOTE_LED_1;
		}
		else
		{
			ledsState = ledsState &~ WIIMOTE_LED_1;
		}
		break;
	case 1:
		if (wantEnabled)
		{
			ledsState = ledsState | WIIMOTE_LED_2;
		}
		else
		{
			ledsState = ledsState &~ WIIMOTE_LED_2;
		}
		break;
	case 2:
		if (wantEnabled)
		{
			ledsState = ledsState | WIIMOTE_LED_3;
		}
		else
		{
			ledsState = ledsState &~ WIIMOTE_LED_3;
		}
		break;
	case 3:
		if (wantEnabled)
		{
			ledsState = ledsState | WIIMOTE_LED_4;
		}
		else
		{
			ledsState = ledsState &~ WIIMOTE_LED_4;
		}
		break;
	default:
		break;
	}
	
	if (isWiiConnected)
	{
		printf("BEGIN SET LED.\n");
		wiimoteManagerPtr->setWiiLeds(numWiiIndex, ledsState);
		printf("END SET LED.\n");
	}
	else 
	{
		connectLedState = ledsState;
	}  
}

void Wiimote::switchWiiMotionplus (int index)
{
	bool wantEnabled;
	float switchState = sdkGetEvtData (swtchWantMotionplus);
	switchState == 1 ? wantEnabled = true : wantEnabled = false;

    if (isWiiConnected)
	{
		wiimoteManagerPtr->wantMotionplusFeedback (numWiiIndex, wantEnabled);
	}
	else 
	{
		connectMotionplusState = wantEnabled;
	}
}
			
			
void Wiimote::switchWiiInfrared (int index)
{
	bool wantEnabled;
	float switchState = sdkGetEvtData (swtchWantInfrared);
	switchState == 1 ? wantEnabled = true : wantEnabled = false;
			
	if (isWiiConnected)
	{
		wiimoteManagerPtr->wantInfraredFeedback (numWiiIndex, wantEnabled);
	}
	else 
	{
		connectInfraredState = wantEnabled;
	}
}
			
//-----------------------------------------------------------------------
// wiimote callbacks 
void Wiimote::onWiiConnect ()
{
	isWiiConnected = true;
	
	wiimoteManagerPtr->setWiiLeds(numWiiIndex, connectLedState);
	wiimoteManagerPtr->setWiiRumble(numWiiIndex, connectRumbleState);
	wiimoteManagerPtr->wantMotionplusFeedback (numWiiIndex, connectMotionplusState);
	wiimoteManagerPtr->wantInfraredFeedback (numWiiIndex, connectInfraredState);
    	
    String logTrace (CharPointer_UTF8 ("connected to wiimote "));
	logTrace << numWiiIndex + 1;

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());	
}
void Wiimote::onWiiDisconnect ()
{
	String logTrace (CharPointer_UTF8 ("disconnected from wiimote "));
	logTrace << numWiiIndex + 1;

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());
}

void Wiimote::onWiiUpdateEvent ()
{
    if (wm == nullptr)
        return;

    //rledWiiButtonA;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_A))     { sdkSetEvtData (rledWiiButtonA, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonA, 0.0f); } 
    
    //rledWiiButtonB;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_B))     { sdkSetEvtData (rledWiiButtonB, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonB, 0.0f); } 
    
	// rledWiiButtonOne;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_ONE))     { sdkSetEvtData (rledWiiButtonOne, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonOne, 0.0f); } 

	// rledWiiButtonTwo;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_TWO))     { sdkSetEvtData (rledWiiButtonTwo, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonTwo, 0.0f); } 

	// rledWiiButtonPlus;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_PLUS))     { sdkSetEvtData (rledWiiButtonPlus, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonPlus, 0.0f); } 

	// rledWiiButtonMinus;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_MINUS))     { sdkSetEvtData (rledWiiButtonMinus, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonMinus, 0.0f); } 

	// rledWiiButtonHome;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_HOME))     { sdkSetEvtData (rledWiiButtonHome, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonHome, 0.0f); } 

	// rledWiiButtonUp;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_UP))     { sdkSetEvtData (rledWiiButtonUp, 1.0f); } 
    else    { sdkSetEvtData (rledWiiButtonUp, 0.0f); } 

	// rledWiiButtonDown;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_DOWN))     { sdkSetEvtData (rledWiiButtonDown, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonDown, 0.0f); } 

	// rledWiiButtonLeft;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_LEFT))     { sdkSetEvtData (rledWiiButtonLeft, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonLeft, 0.0f); } 

	// rledWiiButtonRight;
    if (IS_PRESSED  (wm, WIIMOTE_BUTTON_RIGHT))     { sdkSetEvtData (rledWiiButtonRight, 1.0f); } 
    else     { sdkSetEvtData (rledWiiButtonRight, 0.0f); } 

    if (WIIUSE_USING_ACC(wm))
    {
		wiiAccX = wm->accel.x - WII_ACC_X;     
        wiiAccY = wm->accel.y - WII_ACC_Y;        
        wiiAccZ = wm->accel.z - WII_ACC_Z;

        // accelerations
        sdkSetEvtData (fdrWiiAcc[0], wiiAccX);
        sdkSetEvtData (fdrWiiAcc[1], wiiAccY);
        sdkSetEvtData (fdrWiiAcc[2], wiiAccZ);

        // orientations
        sdkSetEvtData (fdrWiiPitch, wm->orient.pitch);
        sdkSetEvtData (fdrWiiRoll, wm->orient.roll);
        sdkSetEvtData (fdrWiiYaw, wm->orient.yaw);

        
        // positions     
        wiiSpeedX       = (WII_POS_RELEASE * wiiSpeedX) + (wiiAccX + wiiAccOldX);   
        wiiDistanceX    =  wiiSpeedX - wiiSpeedOldX;
        wiiSpeedOldX    = wiiSpeedX;
        wiiDistanceOldX = wiiDistanceX;
        wiiAccOldX      = wiiAccX;
        sdkSetEvtData (fdrWiiPos[0], std::min (1.0f, std::max (0.0f, (50.0f + wiiDistanceX) / 100.0f)));
    
        wiiSpeedY       = (WII_POS_RELEASE * wiiSpeedY) + (wiiAccY + wiiAccOldY);   
        wiiDistanceY    =  wiiSpeedY - wiiSpeedOldY;
        wiiSpeedOldY    = wiiSpeedY;
        wiiDistanceOldY = wiiDistanceY;
        wiiAccOldY      = wiiAccY;
        sdkSetEvtData (fdrWiiPos[1], std::min (1.0f, std::max (0.0f, (50.0f + wiiDistanceY) / 100.0f)));
    
        wiiSpeedZ       = (WII_POS_RELEASE * wiiSpeedZ) + (wiiAccZ + wiiAccOldZ);   
        wiiDistanceZ    =  wiiSpeedZ - wiiSpeedOldZ;
        wiiSpeedOldZ    = wiiSpeedZ;
        wiiDistanceOldZ = wiiDistanceZ;
        wiiAccOldZ      = wiiAccZ;
        sdkSetEvtData (fdrWiiPos[2], std::min (1.0f, std::max (0.0f, (100.0f + wiiDistanceZ) / 100.0f)));
        
    }

    // ir dots
    if (WIIUSE_USING_IR(wm)) 
    {
        int i = 0;

        // go through each of the 4 possible IR sources
        for (; i < 4; ++i) 
		{
            if (wm->ir.dot[i].visible)
			{
				sdkSetEvtData (fdrWiiIrX[i], wm->ir.dot[i].x / (float)wm->ir.vres[0]);
				sdkSetEvtData (fdrWiiIrY[i], wm->ir.dot[i].y / (float)wm->ir.vres[1]);
				sdkSetEvtData (fdrWiiIrSize[i], ((unsigned int)wm->ir.dot[i].size) / 15.0);
			}
        };
    }

    // motion plus with or without nunchuk
    if (wm->exp.type == EXP_MOTION_PLUS ||  wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) 
    {
        // accelerations
        sdkSetEvtData (fdrWiiMplusAcc[0],  wm->exp.mp.angle_rate_gyro.pitch);
        sdkSetEvtData (fdrWiiMplusAcc[1],  wm->exp.mp.angle_rate_gyro.roll);
        sdkSetEvtData (fdrWiiMplusAcc[2],  wm->exp.mp.angle_rate_gyro.yaw);     
	}
    /* show events specific to supported expansions */
    if (wm->exp.type == EXP_NUNCHUK ||  wm->exp.type == EXP_MOTION_PLUS_NUNCHUK) 
    {
        /* nunchuk */
        struct nunchuk_t* nc = (nunchuk_t*)&wm->exp.nunchuk;

        // buttons
        if (IS_PRESSED(nc, NUNCHUK_BUTTON_C))		printf("Nunchuk: C pressed\n");
        if (IS_PRESSED(nc, NUNCHUK_BUTTON_Z))		printf("Nunchuk: Z pressed\n");
       
        if (IS_PRESSED  (nc, NUNCHUK_BUTTON_C))     { sdkSetEvtData (rledWiiNunchukButtonC, 1.0f); }
        if (! IS_HELD (nc, NUNCHUK_BUTTON_C))     { sdkSetEvtData (rledWiiNunchukButtonC, 0.0f); }
        
        if (IS_PRESSED  (nc, NUNCHUK_BUTTON_Z))     { sdkSetEvtData (rledWiiNunchukButtonZ, 1.0f); }
        if (! IS_HELD (nc, NUNCHUK_BUTTON_Z))     { sdkSetEvtData (rledWiiNunchukButtonZ, 0.0f); }
     
        // orientations
        sdkSetEvtData (fdrWiiNunchukOrient[0], nc->orient.pitch);
        sdkSetEvtData (fdrWiiNunchukOrient[1], nc->orient.roll);
        sdkSetEvtData (fdrWiiNunchukOrient[2], nc->orient.yaw);

        // joystick angle and magnitude
        sdkSetEvtData (fdrWiiNunchukJsAngle, nc->js.ang);
        sdkSetEvtData (fdrWiiNunchukJsMag, nc->js.mag);
    }     
}


void Wiimote::onWiiIrActivated ()
{
    isWiiIrActivated  = true;
}

void Wiimote::onWiiIrDeactivated ()
{
    isWiiIrActivated  = false;
}

void Wiimote::onWiiNunchukActivated ()
{
    isWiiNunchukActivated  = true;
    String logTrace (CharPointer_UTF8 ("wiimote "));
	logTrace << numWiiIndex + 1 << " nunchuk activated";

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());
}

void Wiimote::onWiiNunchukDeactivated ()
{
    isWiiNunchukActivated  = false;

    String logTrace (CharPointer_UTF8 ("wiimote "));
	logTrace << numWiiIndex + 1 << " nunchuk deactivated";

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());
}

void Wiimote::onWiiMotionplusActivated ()
{
    isWiiMotionplusActivated  = true;
        
    String logTrace (CharPointer_UTF8 ("wiimote "));
	logTrace << numWiiIndex + 1 << " motion plus activated";

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());
}

void Wiimote::onWiiMotionplusDeactivated ()
{
    isWiiMotionplusActivated  = false;

    String logTrace (CharPointer_UTF8 ("wiimote "));
	logTrace << numWiiIndex + 1 << " motion plus deactivated";

	sdkTraceChar ((AnsiCharPtr)logTrace.toUTF8().getAddress());
}

void Wiimote::actionListenerCallback(const String& message)
{
/*	if (message == WiimoteManager::WII_EVT_UPDATE)
	{
		const ScopedLock myLock (wiimoteManagerPtr->wiiCriticalSection);
		wm = wiimoteManagerPtr->getWiimoteInfos (numWiiIndex);
		const ScopedUnlock myUnlock (wiimoteManagerPtr->wiiCriticalSection);
		
		onWiiUpdateEvent();
	}
	else */
    if (message == WiimoteManager::WII_EVT_CONNECT)
	{
		//const ScopedLock mylock (wiimoteManagerPtr->wiiCriticalSection);
		wm = wiimoteManagerPtr->getWiimoteInfos (numWiiIndex);
		//const ScopedUnlock myUnlock (wiimoteManagerPtr->wiiCriticalSection);
		
		onWiiConnect();
	}
	else if (message == WiimoteManager::WII_EVT_DISCONNECT)
	{
		onWiiDisconnect();
	}
	else if (message == WiimoteManager::WII_EVT_MOTION_PLUS_ACTIVATED)
	{
		//const ScopedLock myLock (wiimoteManagerPtr->wiiCriticalSection);
		wm = wiimoteManagerPtr->getWiimoteInfos (numWiiIndex);
		//const ScopedUnlock myUnlock (wiimoteManagerPtr->wiiCriticalSection);
		
		onWiiMotionplusActivated();
	}
	else if (message == WiimoteManager::WII_EVT_MOTION_PLUS_DEACTIVATED)
	{
		onWiiMotionplusDeactivated();
	}
	else if (message == WiimoteManager::WII_EVT_NUNCHUK_ACTIVATED)
	{
		//const ScopedLock myLock (wiimoteManagerPtr->wiiCriticalSection);
		wm = wiimoteManagerPtr->getWiimoteInfos (numWiiIndex);
		//const ScopedUnlock myUnlock (wiimoteManagerPtr->wiiCriticalSection);
		
		onWiiNunchukActivated();
	}
	else if (message == WiimoteManager::WII_EVT_NUNCHUK_ACTIVATED)
	{
		onWiiNunchukDeactivated();
	}
}

