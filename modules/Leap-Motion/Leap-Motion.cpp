//-----------------------------------------------------------------------------
//@file  
//	Leap-Motion.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the LeapMotion class.
//
//  Handle LeapMotion device communication in Usine
//
//HISTORIC 
//	2013/09/15
//    first release for Hollyhock 1.02.013 
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
#include "Leap-Motion.h"
#include <string>
#include <cmath>
#include <limits>
#include <algorithm>
#include <Leap.h>


//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new LeapMotion();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((LeapMotion*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "LeapMotion";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "LeapMotion";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.1";

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
LeapMotion::LeapMotion()
    : settingTraceLevel (tlInfo)
    , leapConnected (FALSE)
    , handsEnabled (false)
    , palmsEnabled (false)
    , fingersEnabled (false)
    , gesturesEnabled (false)
    , handsIdSlotsList (NUM_OF_HANDS_MAX + 1, ARR_INDEX_NULL)
    , fingersIdSlotsList (NUM_OF_FINGERS_MAX + 1, ARR_INDEX_NULL)
    , isPatchRunning (false)
    , numOfHands (0)
    , numOfTotalFingers (0)
    , handId (0)
    , handIsValid (0)
    , fingerId (0)
    , fingerIsTool (0)
    , fingerIsValid (0)
    , zoomPositionsLevel (0.5)
    , zoomValuesList (2)
    , settingGestureSwipeMinLength (GEST_DEF_SWIPE_MIN_LENGTH)
    , settingGestureSwipeMinVelocity (GEST_DEF_SWIPE_MIN_VEL)
    , settingGestureCircleMinRadius (GEST_DEF_CIRCLE_MIN_RADIUS)
    , settingGestureCircleMinArc (GEST_DEF_CIRCLE_MIN_ARC)
    , settingGestureKeyTapeMinDownVelocity (GEST_DEF_KEY_TAPE_MIN_DOWN_VEL)
    , settingGestureKeyTapeHistorySeconds (GEST_DEF_KEY_TAPE_HIST_SECONDS)
    , settingGestureKeyTapeMinDistance (GEST_DEF_KEY_TAPE_MIN_DISTANCE)
    , settingGestureScreenTapeMinForwardVelocity (GEST_DEF_SCREEN_TAPE_MIN_DOWN_VEL)
    , settingGestureScreenTapeHistorySeconds (GEST_DEF_SCREEN_TAPE_HIST_SECONDS)
    , settingGestureScreenTapeMinDistance (GEST_DEF_SCREEN_TAPE_MIN_DISTANCE)
    , settingViewAngle (0.0f)
    , paintCounter (0)
    , isCollectingDatas (false)
    , isLeftHandValid (FALSE)
    , isRightHandValid (FALSE)
    , isDrawTools (false)
{
    zoomValuesList[0] = 1.0f;
    zoomValuesList[1] = 0.5f;
}

// destructor
LeapMotion::~LeapMotion()
{
    try
	{
        leapController.removeListener (*this);
	}
	catch (...) // std::exception& e
	{
	}
}

//-------------------------------------------------------------------------
// public methodes inherited from UserModule
//------------------------------------------------------------------------
void LeapMotion::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->DefaultWidth		= MODULE_DIM_DEFAULT;
	pModuleInfo->DefaultHeight		= MODULE_DIM_DEFAULT;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = piNumOfParams;
	pModuleInfo->DontProcess		= TRUE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init
void LeapMotion::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
    settingDrawThickness   = 2.0f;
    settingLeftHandColor = sdkGetUsineColor (cl20);
    settingRightHandColor = sdkGetUsineColor (cl22);
    
    sdkSetEvtNbLines (arrLeftPalm3DPoint, ARR_3D_DATA_NB_LINES);
    sdkSetEvtSize (arrLeftPalm3DPoint, ARR_3D_DATA_NB_LINES);
    sdkSetEvtNbLines (arrLeftHand3DPoints, ARR_3D_DATA_NB_LINES);
    sdkSetEvtSize (arrLeftHand3DPoints, ARR_3D_DATA_NB_LINES * ARR_3D_DATA_POINTS);
    sdkSetEvtSize (arrLeftHandTipsPosX, 5);
    sdkSetEvtSize (arrLeftHandTipsPosY, 5);
    sdkSetEvtSize (arrLeftHandTipsPosZ, 5);
    sdkSetEvtSize (arrLeftHandTipsDirX, 5);
    sdkSetEvtSize (arrLeftHandTipsDirY, 5);
    sdkSetEvtSize (arrLeftHandTipsDirZ, 5);
    sdkSetEvtSize (arrLeftHandTipsValid, 5);
    
    sdkSetEvtNbLines (arrRightPalm3DPoint, ARR_3D_DATA_NB_LINES);
    sdkSetEvtSize (arrRightPalm3DPoint, ARR_3D_DATA_NB_LINES);
    sdkSetEvtNbLines (arrRightHand3DPoints, ARR_3D_DATA_NB_LINES);
    sdkSetEvtSize (arrRightHand3DPoints, ARR_3D_DATA_NB_LINES * ARR_3D_DATA_POINTS);
    sdkSetEvtSize (arrRightHandTipsPosX, 5);
    sdkSetEvtSize (arrRightHandTipsPosY, 5);
    sdkSetEvtSize (arrRightHandTipsPosZ, 5);
    sdkSetEvtSize (arrRightHandTipsDirX, 5);
    sdkSetEvtSize (arrRightHandTipsDirY, 5);
    sdkSetEvtSize (arrRightHandTipsDirZ, 5);
    sdkSetEvtSize (arrRightHandTipsValid, 5);

    sdkSetEvtNbLines (arrTools3DPoints, ARR_3D_DATA_NB_LINES);
}

//-----------------------------------------------------------------------------
// parameters and process
void LeapMotion::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
    using namespace Leap;

	switch (ParamIndex) 
    {
		// swtchOn
		case piOn:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "on";
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->DefaultValue	= 1;
			break;
        // ledIsConnected
		case piConnected:
		    pParamInfo->ParamType		= ptTriggerLed;
		    pParamInfo->Caption			= "connected";
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
		    pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // cboxPosZoomLevel
		case piPosZoomLevel:
		    pParamInfo->ParamType		= ptListBox;
		    pParamInfo->Caption			= "zoom";
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
			pParamInfo->ListBoxStrings	= "\"one hand\",\"two hands\"";
		    pParamInfo->DefaultValue	= 1;
			break;
        // dtfFrameId
		case piFrameId:
			pParamInfo->ParamType		= ptDataField;
			pParamInfo->Caption			= "frame id";
			pParamInfo->DefaultValue	= 0;
			pParamInfo->Format			= "%g";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsSeparator     = TRUE;
			break;
        // dtfNumOfHands
		case piNumOfHands:
			pParamInfo->ParamType		= ptDataField;
			pParamInfo->Caption			= "num hands";
			pParamInfo->DefaultValue	= 0;
			pParamInfo->Format			= "%g";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        //swtchEnableHands
		case piEnableHands:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "enable hands";
		    pParamInfo->DefaultValue	= 0.0f;
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->CallBackType	= ctImmediate;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsId
		case piHandsId:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "hands id";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsNumFingers
		case piHandsNumFingers:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "hands num fingers";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsPosX
		case piHandsPosX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "spheres pos x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsPosY
		case piHandsPosY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "spheres pos y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsPosZ
		case piHandsPosZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "spheres pos z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsRadius
		case piHandsRadius:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "spheres radius";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrHandsValid
		case piHandsValid:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "hands valid";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        //swtchEnablePalms
		case piEnablePalms:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "enable palms";
		    pParamInfo->DefaultValue	= 0.0f;
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->CallBackType	= ctImmediate;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsHandsId
		case piPalmsHandsId:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "hands id";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsPosX
		case piPalmsPosX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms pos x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsPosY
		case piPalmsPosY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms pos y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsPosZ
		case piPalmsPosZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms pos z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsDirX
		case piPalmsDirX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms dir x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsDirY
		case piPalmsDirY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms dir y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsDirZ
		case piPalmsDirZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms dir z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsVelX
		case piPalmsVelX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms vel x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsVelY
		case piPalmsVelY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms vel y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsVelZ
		case piPalmsVelZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms vel z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsNorX
		case piPalmsNorX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms nor x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsNorY
		case piPalmsNorY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms nor y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsNorZ
		case piPalmsNorZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms nor z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrPalmsValid
		case piPalmsValid:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "palms valid";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // dtfNumOfFingers
		case piNumOfFingers:
			pParamInfo->ParamType		= ptDataField;
			pParamInfo->Caption			= "num fingers";
			pParamInfo->DefaultValue	= 0.0;
			pParamInfo->Format			= "%g";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->IsSeparator     = TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        //swtchEnableFingers
		case piEnableFingers:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "enable fingers";
		    pParamInfo->DefaultValue	= 0.0f;
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->CallBackType	= ctImmediate;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersId
		case piFingersId:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers id";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersHandsId
		case piFingersHandsId:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers hand id";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersPosX
		case piFingersPosX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers pos x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersPosY
		case piFingersPosY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers pos y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersPosZ
		case piFingersPosZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers pos z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersDirX
		case piFingersDirX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers dir x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersDirY
		case piFingersDirY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers dir y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersDirZ
		case piFingersDirZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers dir z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersVelX
		case piFingersVelX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers vel x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersVelY
		case piFingersVelY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers vel y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersVelZ
		case piFingersVelZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers vel z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersLength
		case piFingersLength:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers length";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersIsTool
		case piFingersIsTool:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers is tool";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        // arrFingersValid
		case piFingersValid:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "fingers valid";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsVisibleByDefault = FALSE;
			break;
        //swtchEnableGestures
		case piEnableGestures:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "enable gestures";
		    pParamInfo->DefaultValue	= 1.0f;
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
            pParamInfo->IsSeparator     = TRUE;
		    pParamInfo->CallBackType	= ctImmediate;
			break;
        // arrGesture
		case piGesture:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "gesture";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break; 
        // arrLeftPalm3DPoint
		case piLeftPalm3DPoint:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left palm 3d point";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsSeparator     = TRUE;
			break;
        // arrLeftHand3DPoints
		case piLeftHand3DPoints:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand 3d points";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        //ledLeftHandValid
		case piLeftHandValid:
		    pParamInfo->ParamType		= ptRightLed;
		    pParamInfo->Caption			= "left hand presence";
		    pParamInfo->IsInput		    = FALSE;
            pParamInfo->IsOutput        = TRUE;
		    pParamInfo->CallBackType	= ctImmediate;
			break;
        // fdrLeftHandConfidence
        case piLeftHandConfidence:
		    pParamInfo->ParamType       = ptDataFader;
		    pParamInfo->Caption         = "left hand confidence";
		    pParamInfo->IsInput         = FALSE;
		    pParamInfo->IsOutput        = TRUE;
		    pParamInfo->MinValue        = 0;
		    pParamInfo->MaxValue        = 1;
		    pParamInfo->DefaultValue    = 0;
		    pParamInfo->Symbol          = "";
		    pParamInfo->Format          = "%.3f";
		    pParamInfo->ReadOnly		= TRUE; // can't change the field
		    break;
        // arrLeftHandTipsPosX
		case piLeftHandTipsPosX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips pos x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsPosY
		case piLeftHandTipsPosY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips pos y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsPosZ
		case piLeftHandTipsPosZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips pos z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsDirX
		case piLeftHandTipsDirX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips dir x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsDirY
		case piLeftHandTipsDirY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips dir y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsDirZ
		case piLeftHandTipsDirZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips dir z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrLeftHandTipsValid
		case piLeftHandTipsValid:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "left hand tips presence";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightPalm3DPoint
		case piRightPalm3DPoint:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right palm 3d point";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsSeparator     = TRUE;
			break;
        // arrRightHand3DPoints
		case piRightHand3DPoints:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand 3d points";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        //ledRightHandValid
		case piRightHandValid:
		    pParamInfo->ParamType		= ptRightLed;
		    pParamInfo->Caption			= "right hand presence";
		    pParamInfo->IsInput		    = FALSE;
            pParamInfo->IsOutput        = TRUE;
		    pParamInfo->CallBackType	= ctImmediate;
			break;
        // fdrRightHandConfidence
        case piRightHandConfidence:
		    pParamInfo->ParamType       = ptDataFader;
		    pParamInfo->Caption         = "right hand confidence";
		    pParamInfo->IsInput         = FALSE;
		    pParamInfo->IsOutput        = TRUE;
		    pParamInfo->MinValue        = 0;
		    pParamInfo->MaxValue        = 1;
		    pParamInfo->DefaultValue    = 0;
		    pParamInfo->Symbol          = "";
		    pParamInfo->Format          = "%.3f";
		    pParamInfo->ReadOnly		= TRUE; // can't change the field
		    break;
        // arrRightHandTipsPosX
		case piRightHandTipsPosX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips pos x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsPosY
		case piRightHandTipsPosY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips pos y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsPosZ
		case piRightHandTipsPosZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips pos z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsDirX
		case piRightHandTipsDirX:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips dir x";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->MinValue		= -1.0f;
			pParamInfo->MaxValue		= 1.0f;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsDirY
		case piRightHandTipsDirY:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips dir y";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->MinValue		= -1.0f;
			pParamInfo->MaxValue		= 1.0f;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsDirZ
		case piRightHandTipsDirZ:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips dir z";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->MinValue		= -1.0f;
			pParamInfo->MaxValue		= 1.0f;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrRightHandTipsValid
		case piRightHandTipsValid:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "right hand tips presence";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
			break;
        // arrTools3DPoints
		case piTools3DPoints:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "tools 3d points";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= TRUE;
            pParamInfo->DontSave        = TRUE;
            pParamInfo->IsSeparator     = TRUE;
			break;

	        // default case
        default:
	        break;
	}
}
void LeapMotion::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
	switch (ParamIndex) 
    {
        // swtchOn
		case piOn:
			swtchOn = pEvent;
			break;
        // ledIsConnected
		case piConnected:
			ledIsConnected = pEvent;
			break;
        // cboxPosZoomLevel
		case piPosZoomLevel:
            cboxPosZoomLevel = pEvent;
			break;
        // dtfFrameId
		case piFrameId:
            dtfFrameId = pEvent;
			break;
        // dtfNumOfHands
		case piNumOfHands:
            dtfNumOfHands = pEvent;
			break;
        //swtchEnableHands
		case piEnableHands:
            swtchEnableHands = pEvent;
			break;
        // arrHandsId
		case piHandsId:
            arrHandsId = pEvent;
			break;
        // arrHandsNumFingers
		case piHandsNumFingers:
            arrHandsNumFingers = pEvent;
			break;
        // arrHandsPosX
		case piHandsPosX:
            arrHandsPosX = pEvent;
			break;
        // arrHandsPosY
		case piHandsPosY:
            arrHandsPosY = pEvent;
			break;
        // arrHandsPosZ
		case piHandsPosZ:
            arrHandsPosZ = pEvent;
			break;
        // arrHandsRadius
		case piHandsRadius:
            arrHandsRadius = pEvent;
			break;
        // arrHandsValid
		case piHandsValid:
            arrHandsValid = pEvent;
			break;
        //swtchEnablePalms
		case piEnablePalms:
            swtchEnablePalms = pEvent;
			break;
        // arrPalmsHandsId
		case piPalmsHandsId:
            arrPalmsHandsId = pEvent;
			break;
        // arrPalmsPosX
		case piPalmsPosX:
            arrPalmsPosX = pEvent;
			break;
        // arrPalmsPosY
		case piPalmsPosY:
            arrPalmsPosY = pEvent;
			break;
        // arrPalmsPosZ
		case piPalmsPosZ:
            arrPalmsPosZ = pEvent;
			break;
        // arrPalmsDirX
		case piPalmsDirX:
            arrPalmsDirX = pEvent;
			break;
        // arrPalmsDirY
		case piPalmsDirY:
            arrPalmsDirY = pEvent;
			break;
        // arrPalmsDirZ
		case piPalmsDirZ:
            arrPalmsDirZ = pEvent;
			break;
        // arrPalmsVelX
		case piPalmsVelX:
            arrPalmsVelX = pEvent;
			break;
        // arrPalmsVelY
		case piPalmsVelY:
            arrPalmsVelY = pEvent;
			break;
        // arrPalmsVelZ
		case piPalmsVelZ:
            arrPalmsVelZ = pEvent;
			break;
        // arrPalmsNorX
		case piPalmsNorX:
            arrPalmsNorX = pEvent;
			break;
        // arrPalmsNorY
		case piPalmsNorY:
            arrPalmsNorY = pEvent;
			break;
        // arrPalmsNorZ
		case piPalmsNorZ:
            arrPalmsNorZ = pEvent;
			break;
        // arrPalmsValid
		case piPalmsValid:
            arrPalmsValid = pEvent;
			break;
        // dtfNumOfFingers
		case piNumOfFingers:
            dtfNumOfFingers = pEvent;
			break;
        //swtchEnableFingers
		case piEnableFingers:
            swtchEnableFingers = pEvent;
			break;
        // arrFingersId
		case piFingersId:
            arrFingersId = pEvent;
			break;
        // arrFingersHandsId
		case piFingersHandsId:
            arrFingersHandsId = pEvent;
			break;
        // arrFingersPosX
		case piFingersPosX:
            arrFingersPosX = pEvent;
			break;
        // arrFingersPosY
		case piFingersPosY:
            arrFingersPosY = pEvent;
			break;
        // arrFingersPosZ
		case piFingersPosZ:
            arrFingersPosZ = pEvent;
			break;
        // arrFingersDirX
		case piFingersDirX:
            arrFingersDirX = pEvent;
			break;
        // arrFingersDirY
		case piFingersDirY:
            arrFingersDirY = pEvent;
			break;
        // arrFingersDirZ
		case piFingersDirZ:
            arrFingersDirZ = pEvent;
			break;
        // arrFingersVelX
		case piFingersVelX:
            arrFingersVelX = pEvent;
			break;
        // arrFingersVelY
		case piFingersVelY:
            arrFingersVelY = pEvent;
			break;
        // arrFingersVelZ
		case piFingersVelZ:
            arrFingersVelZ = pEvent;
			break;
        // arrFingersLength
		case piFingersLength:
            arrFingersLength = pEvent;
			break;
        // arrFingersIsTool
		case piFingersIsTool:
            arrFingersIsTool = pEvent;
			break;
        // arrFingersValid
		case piFingersValid:
            arrFingersValid = pEvent;
			break;
        //swtchEnableGestures
		case piEnableGestures:
            swtchEnableGestures = pEvent;
			break;
        // arrGesture
		case piGesture:
            arrGesture = pEvent;
			break;  
        // arrLeftPalm3DPoint
		case piLeftPalm3DPoint:
			arrLeftPalm3DPoint = pEvent;
			break;
        // arrLeftHand3DPoints
		case piLeftHand3DPoints:
			arrLeftHand3DPoints = pEvent;
			break;
        //ledLeftHandValid
		case piLeftHandValid:
			ledLeftHandValid = pEvent;
			break;
        // fdrLeftHandConfidence
        case piLeftHandConfidence:
			fdrLeftHandConfidence = pEvent;
		    break;
        // arrLeftHandTipsPosX
		case piLeftHandTipsPosX:
			arrLeftHandTipsPosX = pEvent;
			break;
        // arrLeftHandTipsPosY
		case piLeftHandTipsPosY:
			arrLeftHandTipsPosY = pEvent;
			break;
        // arrLeftHandTipsPosZ
		case piLeftHandTipsPosZ:
			arrLeftHandTipsPosZ = pEvent;
			break;
        // arrLeftHandTipsDirX
		case piLeftHandTipsDirX:
			arrLeftHandTipsDirX = pEvent;
			break;
        // arrLeftHandTipsDirY
		case piLeftHandTipsDirY:
			arrLeftHandTipsDirY = pEvent;
			break;
        // arrLeftHandTipsDirZ
		case piLeftHandTipsDirZ:
			arrLeftHandTipsDirZ = pEvent;
			break;
        // arrLeftHandTipsValid
		case piLeftHandTipsValid:
			arrLeftHandTipsValid = pEvent;
			break;
        // arrRightPalm3DPoint
		case piRightPalm3DPoint:
			arrRightPalm3DPoint = pEvent;
			break;
        // arrRightHand3DPoints
		case piRightHand3DPoints:
			arrRightHand3DPoints = pEvent;
			break;
        //ledRightHandValid
		case piRightHandValid:
			ledRightHandValid = pEvent;
			break;
        // fdrRightHandConfidence
        case piRightHandConfidence:
			fdrRightHandConfidence = pEvent;
		    break;
        // arrRightHandTipsPosX
		case piRightHandTipsPosX:
			arrRightHandTipsPosX = pEvent;
			break;
        // arrRightHandTipsPosY
		case piRightHandTipsPosY:
			arrRightHandTipsPosY = pEvent;
			break;
        // arrRightHandTipsPosZ
		case piRightHandTipsPosZ:
			arrRightHandTipsPosZ = pEvent;
			break;
        // arrRightHandTipsDirX
		case piRightHandTipsDirX:
			arrRightHandTipsDirX = pEvent;
			break;
        // arrRightHandTipsDirY
		case piRightHandTipsDirY:
			arrRightHandTipsDirY = pEvent;
			break;
        // arrRightHandTipsDirZ
		case piRightHandTipsDirZ:
			arrRightHandTipsDirZ = pEvent;
			break;
        // arrRightHandTipsValid
		case piRightHandTipsValid:
			arrRightHandTipsValid = pEvent;
			break;
        // arrTools3DPoints
		case piTools3DPoints:
			arrTools3DPoints = pEvent;
			break;
        // default case
        default:
	        break;
	}
}
void LeapMotion::onCallBack (UsineMessage *Message) 
{	   
	using namespace Leap;

    if ( Message->lParam == MSG_CHANGE ) 
	{
	    switch (Message->wParam) // ParamIndex
        {
            // swtchOn
		    case piOn:
			    {
                    //leapConnected = (int) sdkGetEvtData (swtchOn);
                    //checkLeapListener();
                    leapConnected = (int) sdkGetEvtData (swtchOn);
                    if (leapConnected == TRUE)
                    {
                        leapController.addListener (*this);
                        sdkSetEvtData (ledIsConnected, 1.0f);
                    }
                    else
                    {
                        leapController.removeListener (*this);
                        sdkSetEvtData (ledIsConnected, 0.0f);
                    }
                        
                }
			    break;
            // cboxPosZoomLevel
		    case piPosZoomLevel:
                zoomPositionsLevel = zoomValuesList[(int)sdkGetEvtData (cboxPosZoomLevel)];
			    break;
            //swtchEnableHands
		    case piEnableHands:
                {
			        int result = sdkGetEvtData (swtchEnableHands);    
                    if (result == 1)
                        handsEnabled = true;
                    else
                        handsEnabled = false;

                    if (handsEnabled)
                    {
                        sdkSetEvtSize (arrHandsId,          NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsNumFingers,  NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsPosX,        NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsPosY,        NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsPosZ,        NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsRadius,      NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrHandsValid,       NUM_OF_HANDS_MAX);
                    }
                    else
                    {
                        sdkSetEvtSize (arrHandsId,          0);
                        sdkSetEvtSize (arrHandsNumFingers,  0);
                        sdkSetEvtSize (arrHandsPosX,        0);
                        sdkSetEvtSize (arrHandsPosY,        0);
                        sdkSetEvtSize (arrHandsPosZ,        0);
                        sdkSetEvtSize (arrHandsRadius,      0);
                        sdkSetEvtSize (arrHandsValid,       0);
                        // free all slots
                        handsIdSlotsList.assign (handsIdSlotsList.size(), ARR_INDEX_NULL);
                    }
                }
			    break;
            //swtchEnablePalms
		    case piEnablePalms:
                {
			        int result = sdkGetEvtData (swtchEnablePalms);    
                    if (result == 1)
                        palmsEnabled = true;
                    else
                        palmsEnabled = false;

                    if (palmsEnabled)
                    {
                        sdkSetEvtSize (arrPalmsHandsId, NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsPosX,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsPosY,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsPosZ,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsDirX,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsDirY,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsDirZ,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsVelX,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsVelY,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsVelZ,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsNorX,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsNorY,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsNorZ,    NUM_OF_HANDS_MAX);
                        sdkSetEvtSize (arrPalmsValid,   NUM_OF_HANDS_MAX);            
                    }
                    else
                    {
                        sdkSetEvtSize (arrPalmsHandsId, 0);
                        sdkSetEvtSize (arrPalmsPosX,    0);
                        sdkSetEvtSize (arrPalmsPosY,    0);
                        sdkSetEvtSize (arrPalmsPosZ,    0);
                        sdkSetEvtSize (arrPalmsDirX,    0);
                        sdkSetEvtSize (arrPalmsDirY,    0);
                        sdkSetEvtSize (arrPalmsDirZ,    0);
                        sdkSetEvtSize (arrPalmsVelX,    0);
                        sdkSetEvtSize (arrPalmsVelY,    0);
                        sdkSetEvtSize (arrPalmsVelZ,    0);
                        sdkSetEvtSize (arrPalmsNorX,    0);
                        sdkSetEvtSize (arrPalmsNorY,    0);
                        sdkSetEvtSize (arrPalmsNorZ,    0);
                        sdkSetEvtSize (arrPalmsValid,   0);
                        // free all slots
                        handsIdSlotsList.assign (handsIdSlotsList.size(), ARR_INDEX_NULL);
                    }

                }
			    break;
            //swtchEnableFingers
		    case piEnableFingers:
                {
			        int result = sdkGetEvtData (swtchEnableFingers);    
                    if (result == 1)
                        fingersEnabled = true;
                    else
                        fingersEnabled = false;
  
                    if (fingersEnabled)
                    {
                        sdkSetEvtSize (arrFingersId,         NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersHandsId,    NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersPosX,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersPosY,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersPosZ,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersDirX,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersDirY,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersDirZ,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersVelX,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersVelY,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersVelZ,       NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersLength,     NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersIsTool,     NUM_OF_FINGERS_MAX);
                        sdkSetEvtSize (arrFingersValid,      NUM_OF_FINGERS_MAX);
                    }
                    else
                    {
                        sdkSetEvtSize (arrFingersId,         0);
                        sdkSetEvtSize (arrFingersHandsId,    0);
                        sdkSetEvtSize (arrFingersPosX,       0);
                        sdkSetEvtSize (arrFingersPosY,       0);
                        sdkSetEvtSize (arrFingersPosZ,       0);
                        sdkSetEvtSize (arrFingersDirX,       0);
                        sdkSetEvtSize (arrFingersDirY,       0);
                        sdkSetEvtSize (arrFingersDirZ,       0);
                        sdkSetEvtSize (arrFingersVelX,       0);
                        sdkSetEvtSize (arrFingersVelY,       0);
                        sdkSetEvtSize (arrFingersVelZ,       0);
                        sdkSetEvtSize (arrFingersLength,     0);
                        sdkSetEvtSize (arrFingersIsTool,     0);
                        sdkSetEvtSize (arrFingersValid,      0);
                        // free all slots
                        fingersIdSlotsList.assign (fingersIdSlotsList.size(), ARR_INDEX_NULL);
                    }
                }
			    break;
            //swtchEnableGestures
		    case piEnableGestures:
                {
			        int result = sdkGetEvtData (swtchEnableGestures);    
                    if (result == 1)
                        gesturesEnabled = true;
                    else
                        gesturesEnabled = false;
                    if (gesturesEnabled)
                    {
                        leapController.enableGesture (Gesture::TYPE_SWIPE, true);
                        leapController.enableGesture (Gesture::TYPE_CIRCLE, true);
                        leapController.enableGesture (Gesture::TYPE_KEY_TAP, true);
                        leapController.enableGesture (Gesture::TYPE_SCREEN_TAP, true);
                        sdkSetEvtSize (arrGesture, 0);
                    }
                    else
                    {
                        leapController.enableGesture (Gesture::TYPE_SWIPE, false);
                        leapController.enableGesture (Gesture::TYPE_CIRCLE, false);
                        leapController.enableGesture (Gesture::TYPE_KEY_TAP, false);
                        leapController.enableGesture (Gesture::TYPE_SCREEN_TAP, false);
                        sdkSetEvtSize (arrGesture, 0);
                    }
                }
			    break;
            // default case
            default:
	            break;
	    }
    } 
}


//-----------------------------------------------------------------------------
// chunk system
int  LeapMotion::onGetChunkLen (LongBool Preset) {return 0;}
void LeapMotion::onGetChunk (void* chunk, LongBool Preset) {}
void LeapMotion::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}

//-----------------------------------------------------------------------------
// layout
void LeapMotion::onCreateSettings() 
{
    sdkAddSettingLineCaption (PROPERTIES_TAB_NAME, "gestures");
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureSwipeMinLength), "swipe min length", 5, 500, scLog, "mm", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_SWIPE_MIN_LENGTH);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureSwipeMinVelocity), "swipe min velocity", 10, 10000, scLog, "mm/s", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_SWIPE_MIN_VEL);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureCircleMinRadius), "circle min radius", 5, 500, scLog, "mm", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_CIRCLE_MIN_RADIUS );
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureCircleMinArc), "circle min arc", 0.1 * PI, 10 * PI, scLog, "rad", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_CIRCLE_MIN_ARC);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureKeyTapeMinDownVelocity), "key tap min down velocity", 10, 10000, scLog, "mm/s", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_KEY_TAPE_MIN_DOWN_VEL);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureKeyTapeHistorySeconds), "key tap history seconds", 0.1f, 10.0f, scLog, "s", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_KEY_TAPE_HIST_SECONDS);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureKeyTapeMinDistance), "key tap min distance", 5, 500, scLog, "mm", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_KEY_TAPE_MIN_DISTANCE);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureScreenTapeMinForwardVelocity), "screen tap min down velocity", 10, 10000, scLog, "mm/s", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_SCREEN_TAPE_MIN_DOWN_VEL);
	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureScreenTapeHistorySeconds), "screen tap history seconds", 0.1f, 10.0f, scLog, "s", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_SCREEN_TAPE_HIST_SECONDS);
 	sdkAddSettingLineSingle (PROPERTIES_TAB_NAME, &(settingGestureScreenTapeMinDistance), "screen tap min distance", 5, 500, scLog, "mm", DEFAULT_FORMAT_FLOAT_2, GEST_DEF_SCREEN_TAPE_MIN_DISTANCE);

    sdkAddSettingLineCaption (PROPERTIES_TAB_NAME, "trace");
	sdkAddSettingLineCombobox (PROPERTIES_TAB_NAME, &(settingTraceLevel), "trace level", "\"none\",\"info\",\"debug\",\"verbose\"");
    
    sdkAddSettingLineCaption (DESIGN_TAB_NAME, "skeleton");
	sdkAddSettingLineSingle (DESIGN_TAB_NAME, &(settingDrawThickness), "bone width", 1.0f, 50.0f, scLog, "pix", DEFAULT_FORMAT_FLOAT_2, 2.0f);
	sdkAddSettingLineColor (DESIGN_TAB_NAME, &settingLeftHandColor, "left hand color");
	sdkAddSettingLineColor (DESIGN_TAB_NAME, &settingRightHandColor, "right hand color");
	sdkAddSettingLineSingle (DESIGN_TAB_NAME, &(settingViewAngle), "view angle", -0.5f, 0.5f, scLinear, "", DEFAULT_FORMAT_FLOAT_2, 0.0f);

}

void LeapMotion::onSettingsHasChanged() 
{
    Leap::Config leapConfig = leapController.config();

    if (   leapConfig.setFloat (GEST_ID_SWIPE_MIN_LENGTH, settingGestureSwipeMinLength) 
        || leapConfig.setFloat (GEST_ID_SWIPE_MIN_VEL, settingGestureSwipeMinVelocity)
        || leapConfig.setFloat (GEST_ID_CIRCLE_MIN_RADIUS, settingGestureCircleMinRadius)
        || leapConfig.setFloat (GEST_ID_CIRCLE_MIN_ARC, settingGestureCircleMinArc)
        || leapConfig.setFloat (GEST_ID_KEY_TAPE_MIN_DOWN_VEL,settingGestureKeyTapeMinDownVelocity)
        || leapConfig.setFloat (GEST_ID_KEY_TAPE_HIST_SECONDS, settingGestureKeyTapeHistorySeconds)
        || leapConfig.setFloat (GEST_ID_KEY_TAPE_MIN_DISTANCE, settingGestureKeyTapeMinDistance)
        || leapConfig.setFloat (GEST_ID_SCREEN_TAPE_MIN_DOWN_VEL, settingGestureScreenTapeMinForwardVelocity)
        || leapConfig.setFloat (GEST_ID_SCREEN_TAPE_HIST_SECONDS, settingGestureScreenTapeHistorySeconds)
        || leapConfig.setFloat (GEST_ID_SCREEN_TAPE_MIN_DISTANCE, settingGestureScreenTapeMinDistance)
        )
        leapConfig.save();

}

//-----------------------------------------------------------------------------
// paint the module panel
void LeapMotion::onPaint ()
{
    if (leapConnected == TRUE && sdkPatchIsRunning() != FALSE)
    {
        if (isCollectingDatas)
        {
            paintCounter++;
            sdkRepaintPanel ();
            
            if (isTraceLevelDebug())
                trace ("no paint because inside leap frame", paintCounter);
        }
        else
        {
            if (isLeftHandValid == TRUE)
                drawSkeletonHand(arrLeftPalm3DPoint, arrLeftHand3DPoints, settingLeftHandColor);
            if (isRightHandValid == TRUE)
                drawSkeletonHand(arrRightPalm3DPoint, arrRightHand3DPoints, settingRightHandColor);

            if (isDrawTools)
                drawTools ();
        }
    }
}

//-------------------------------------------------------------------------
// protected methodes
//------------------------------------------------------------------------
Leap::Vector LeapMotion::getZoomedPos (Leap::Vector vector)
{
    using namespace Leap;

    Vector result;

    result.x = ((vector.x - 0.5f) * zoomPositionsLevel) + 0.5f;
    result.y = ((vector.y - 0.5f) * zoomPositionsLevel) + 0.5f;
    result.z = ((vector.z - 0.5f) * zoomPositionsLevel) + 0.5f;

    return result;

}


void LeapMotion::updateLeapMotionDatas (Leap::Frame frame)
{
    using namespace Leap;
    
    leapInteractionBox =  frame.interactionBox();

    numOfHands          = frame.hands().count();
    numOfTotalFingers   = frame.fingers().count();

    sdkSetEvtData (dtfFrameId, frame.id());
    sdkRepaintParam (piFrameId);
    sdkSetEvtData (dtfNumOfHands, (float)numOfHands);
    sdkRepaintParam (piNumOfHands);
    sdkSetEvtData (dtfNumOfFingers, (float)numOfTotalFingers);
    sdkRepaintParam (piNumOfFingers);

    if (handsEnabled || palmsEnabled)
    { 
        // check slots for dead hands
        for (int i = 0; i < NUM_OF_HANDS_MAX; ++i)
        {
            // get the hand id for the slot
            handId = handsIdSlotsList[i];
            hand = frame.hand(handId);
            
            // if hand not valid banymore, free the slot
            if ((handId == ARR_INDEX_NULL) || (! hand.isValid()))
                handsIdSlotsList[i] = ARR_INDEX_NULL;
        }

        // for all active hands, check if it already have a slot
        for (int i = 0; i < frame.hands().count(); ++i)
        {
            // get the hand id
            handId = frame.hands()[i].id();

            // find the hand id in hands slot array
            handIdItor = std::find (handsIdSlotsList.begin(), handsIdSlotsList.end(), handId);
            
            // if not found, it's a new one, so add it
            if (handIdItor == handsIdSlotsList.end())
            {
                // find first empty slot
                handIdItor = std::find (handsIdSlotsList.begin(), handsIdSlotsList.end(), ARR_INDEX_NULL);
                // add the hand id
                if (handIdItor < handsIdSlotsList.end())
                    (*handIdItor) = handId;
            }
        }
    }

    if (handsEnabled)
    {
        // for all hands slots
        for (int i = 0; i < NUM_OF_HANDS_MAX; ++i)
        {
            // get the hand id for the slot
            handId = handsIdSlotsList[i];
            hand = frame.hand(handId);

            // if slot not empty and hand id still exist
            if ((handId != ARR_INDEX_NULL) && (hand.isValid()))
            {
                //hand = frame.hand(handArrayIndex);
                handBallCenter = getZoomedPos (leapInteractionBox.normalizePoint (hand.sphereCenter(), false));
                hand.isValid() ? handIsValid = 1 : handIsValid = 0;

                sdkSetEvtArrayData (arrHandsId,         i, handId);
                sdkSetEvtArrayData (arrHandsNumFingers, i, hand.fingers().count());
                sdkSetEvtArrayData (arrHandsPosX,       i, std::min (1.0f, std::max (0.0f, handBallCenter.x)));
                sdkSetEvtArrayData (arrHandsPosY,       i, std::min (1.0f, std::max (0.0f, handBallCenter.y)));
                sdkSetEvtArrayData (arrHandsPosZ,       i, std::min (1.0f, std::max (0.0f, 1.0f - handBallCenter.z)));
                sdkSetEvtArrayData (arrHandsRadius,     i, hand.sphereRadius() / 125.0 - 0.2);
                sdkSetEvtArrayData (arrHandsValid,      i, handIsValid);
            }
            // hand not exist any more, free the slot and reset arrays values
            else
            {
                //handsIdSlotsList[i] = ARR_INDEX_NULL;
                sdkSetEvtArrayData (arrHandsId,         i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsNumFingers, i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsPosX,       i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsPosY,       i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsPosZ,       i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsRadius,     i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrHandsValid,      i, ARR_CONTENT_NULL);
            }
        }
    }

    if (palmsEnabled)
    {
        for (int i = 0; i < NUM_OF_HANDS_MAX; ++i)
        {
            // get the hand id for the slot
            handId = handsIdSlotsList[i];
            hand = frame.hand(handId);

            // if slot not empty and hand id still exist
            if ((handId != ARR_INDEX_NULL) && (hand.isValid()))
            {
                palmPos = getZoomedPos (leapInteractionBox.normalizePoint (hand.palmPosition(), false));
                palmDir = hand.direction();  
                palmVel = leapInteractionBox.normalizePoint (hand.palmVelocity(), true); 
                palmNor = hand.palmNormal();   
                hand.isValid() ? handIsValid = 1 : handIsValid = 0;  

                sdkSetEvtArrayData (arrPalmsHandsId, i, hand.id());
                sdkSetEvtArrayData (arrPalmsPosX,    i, std::min (1.0f, std::max (0.0f, palmPos.x)));
                sdkSetEvtArrayData (arrPalmsPosY,    i, std::min (1.0f, std::max (0.0f, palmPos.y)));
                sdkSetEvtArrayData (arrPalmsPosZ,    i, std::min (1.0f, std::max (0.0f, 1.0f - palmPos.z)));
                sdkSetEvtArrayData (arrPalmsDirX,    i, palmDir.x);
                sdkSetEvtArrayData (arrPalmsDirY,    i, palmDir.y);
                sdkSetEvtArrayData (arrPalmsDirZ,    i, palmDir.z);
                sdkSetEvtArrayData (arrPalmsVelX,    i, palmVel.x);
                sdkSetEvtArrayData (arrPalmsVelY,    i, palmVel.y);
                sdkSetEvtArrayData (arrPalmsVelZ,    i, palmVel.z);
                sdkSetEvtArrayData (arrPalmsNorX,    i, palmNor.x);
                sdkSetEvtArrayData (arrPalmsNorY,    i, palmNor.y);
                sdkSetEvtArrayData (arrPalmsNorZ,    i, palmNor.z);
                sdkSetEvtArrayData (arrPalmsValid,   i, handIsValid);
            }
            // hand not exist any more, free the slot and reset arrays values
            else
            {
                handsIdSlotsList[i] = ARR_INDEX_NULL;
                sdkSetEvtArrayData (arrPalmsHandsId, i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsPosX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsPosY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsPosZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsDirX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsDirY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsDirZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsVelX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsVelY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsVelZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsNorX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsNorY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsNorZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrPalmsValid,   i, ARR_CONTENT_NULL);
            }
        }
    }
        
    if (fingersEnabled)
    {   
        // check slots for dead hands
        for (int i = 0; i < NUM_OF_FINGERS_MAX; ++i)
        {
            // get the hand id for the slot
            fingerId = fingersIdSlotsList[i];
            finger = frame.finger (fingerId);

            // if finger not valid banymore, free the slot
            if ((fingerId == ARR_INDEX_NULL) || (! finger.isValid()))
            {
                fingersIdSlotsList[i] = ARR_INDEX_NULL;
            }
        }

        // for all active fingers, check if it already have a slot
        for (int i = 0; i < frame.fingers().count(); ++i)
        {
            // get the hand id
            finger   = frame.fingers()[i];
            fingerId = finger.id();

            // find the finger id in hands slot array
            fingerIdItor = std::find (fingersIdSlotsList.begin(), fingersIdSlotsList.end(), fingerId);
            
            // if not found, it's a new one, so add it
            if (fingerIdItor == fingersIdSlotsList.end())
            {
                // find first empty slot
                fingerIdItor = std::find (fingersIdSlotsList.begin(), fingersIdSlotsList.end(), ARR_INDEX_NULL);
                // add the finger id

                if (fingerIdItor != fingersIdSlotsList.end())
                    (*fingerIdItor) = fingerId;
            }
        }

        for (int i = 0; i < NUM_OF_FINGERS_MAX; ++i)
        {            
            // get the hand id for the slot
            fingerId = fingersIdSlotsList[i];
            finger = frame.finger (fingerId);

            // if slot not empty and hand id still exist
            if ((fingerId != ARR_INDEX_NULL) && (finger.isValid()))
            {
                fingerPos = getZoomedPos (leapInteractionBox.normalizePoint (finger.tipPosition(), false)); 
                fingerDir = finger.direction(); 
                fingerVel = finger.tipVelocity(); 
                finger.isTool() ? fingerIsTool = 1.0f : fingerIsTool = 0.0f;
                finger.isValid() ? fingerIsValid = 1 : fingerIsValid = 0; 

                sdkSetEvtArrayData (arrFingersId,      i, finger.id());
                sdkSetEvtArrayData (arrFingersHandsId, i, finger.hand().id());
                sdkSetEvtArrayData (arrFingersPosX,    i, std::min (1.0f, std::max (0.0f, fingerPos.x)));
                sdkSetEvtArrayData (arrFingersPosY,    i, std::min (1.0f, std::max (0.0f, fingerPos.y)));
                sdkSetEvtArrayData (arrFingersPosZ,    i, std::min (1.0f, std::max (0.0f, 1.0f - fingerPos.z)));
                sdkSetEvtArrayData (arrFingersDirX,    i, fingerDir.x);
                sdkSetEvtArrayData (arrFingersDirY,    i, fingerDir.y);
                sdkSetEvtArrayData (arrFingersDirZ,    i, fingerDir.z);
                sdkSetEvtArrayData (arrFingersVelX,    i, fingerVel.x);
                sdkSetEvtArrayData (arrFingersVelY,    i, fingerVel.y);
                sdkSetEvtArrayData (arrFingersVelZ,    i, fingerVel.z);
                sdkSetEvtArrayData (arrFingersLength,  i, finger.length()  / 90.0);
                sdkSetEvtArrayData (arrFingersIsTool,  i, fingerIsTool);
                sdkSetEvtArrayData (arrFingersValid,   i, fingerIsValid);
            }
            // hand not exist any more, free the slot and reset arrays values
            else
            {
                fingersIdSlotsList[i] = ARR_INDEX_NULL;
                sdkSetEvtArrayData (arrFingersId,      i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersHandsId, i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersPosX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersPosY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersPosZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersDirX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersDirY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersDirZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersVelX,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersVelY,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersVelZ,    i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersLength,  i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersIsTool,  i, ARR_CONTENT_NULL);
                sdkSetEvtArrayData (arrFingersValid,   i, ARR_CONTENT_NULL);
            }
        }
    }
 
    if (gesturesEnabled)
    {
        int numGestures = frame.gestures().count();
        if (numGestures > 0)
        {
            Gesture gesture = frame.gestures()[0];
            switch (gesture.type())
            {
            case Gesture::TYPE_SWIPE:
                {
					SwipeGesture gestSwipe = SwipeGesture (gesture);
                    if (gesture.state() == Gesture::STATE_STOP)
                    {
						Leap::Vector startPos (getZoomedPos (leapInteractionBox.normalizePoint (gestSwipe.startPosition(), false)));
                        Leap::Vector stopPos (getZoomedPos (leapInteractionBox.normalizePoint (gestSwipe.position(), false)));
                        float duration = gestSwipe.duration() / 1000.0;
                        
                        Leap::Vector gestureDir (gestSwipe.direction());
                        int gestureId;

                        if (std::abs (gestureDir.x) > std::abs (gestureDir.y) && std::abs (gestureDir.x) > std::abs (gestureDir.z))
                        {
                            if (gestureDir.x < 0)
                                gestureId = GEST_ID_SWIPE_LEFT;
                            else
                                gestureId = GEST_ID_SWIPE_RIGHT;
                        }                        
                        else if (std::abs (gestureDir.y) > std::abs (gestureDir.x) && std::abs (gestureDir.y) > std::abs (gestureDir.z))
                        {
                            if (gestureDir.y > 0)
                                gestureId = GEST_ID_SWIPE_UP;
                            else
                                gestureId = GEST_ID_SWIPE_DOWN;
                        }
                        else if (std::abs (gestureDir.z) > std::abs (gestureDir.x) && std::abs (gestureDir.z) > std::abs (gestureDir.y))
                        {
                            if (gestureDir.z < 0)
                                gestureId = GEST_ID_SWIPE_FORWARD;
                            else
                                gestureId = GEST_ID_SWIPE_BACKWARD;
                        }
                        else
                        {
                            gestureId = GEST_ID_SWIPE_BACKWARD;
                            
                        }
						
                        sdkSetEvtSize (arrGesture, 12);
                        sdkSetEvtArrayData (arrGesture, 0, gestureId);
                        sdkSetEvtArrayData (arrGesture, 1, std::min (1.0f, std::max (0.0f, startPos.x)));
                        sdkSetEvtArrayData (arrGesture, 2, std::min (1.0f, std::max (0.0f, startPos.y)));
                        sdkSetEvtArrayData (arrGesture, 2, std::min (1.0f, std::max (0.0f, startPos.z)));
                        sdkSetEvtArrayData (arrGesture, 4, std::min (1.0f, std::max (0.0f, stopPos.x)));
                        sdkSetEvtArrayData (arrGesture, 5, std::min (1.0f, std::max (0.0f, stopPos.y)));
                        sdkSetEvtArrayData (arrGesture, 6, std::min (1.0f, std::max (0.0f, stopPos.z)));
                        sdkSetEvtArrayData (arrGesture, 7, duration);
                        sdkSetEvtArrayData (arrGesture, 8, gestSwipe.pointable().id());
                        sdkSetEvtArrayData (arrGesture, 9, gestureDir.x);
                        sdkSetEvtArrayData (arrGesture, 10, gestureDir.y);
                        sdkSetEvtArrayData (arrGesture, 11, gestureDir.z);
                    }
                }
                break;
            case Leap::Gesture::TYPE_CIRCLE:
                {
                    if (gesture.state() == Gesture::STATE_UPDATE)
                    {
						CircleGesture gestCircle = CircleGesture (gesture);
						
                        Vector centerPos (getZoomedPos (leapInteractionBox.normalizePoint (gestCircle.center(), false)));
                        float radius = gestCircle.radius();
                        float progress = gestCircle.progress();
                        float duration = gestCircle.duration() / 1000.0;
                        Vector normal = gestCircle.normal();
                        
                        int gestireId; 
                        if (((CircleGesture)gesture).pointable().direction().angleTo(normal) <= Leap::PI/2.0)
                            gestireId = GEST_ID_CIRCLE_CLOCK;
                        else
                            gestireId = GEST_ID_CIRCLE_COUNTER_CLOCK;
						
                        sdkSetEvtSize (arrGesture, 11);
                        sdkSetEvtArrayData (arrGesture, 0, gestireId);
                        sdkSetEvtArrayData (arrGesture, 1, std::min (1.0f, std::max (0.0f, centerPos.x)));
                        sdkSetEvtArrayData (arrGesture, 2, std::min (1.0f, std::max (0.0f, centerPos.y)));
                        sdkSetEvtArrayData (arrGesture, 3, std::min (1.0f, std::max (0.0f, centerPos.z)));
                        sdkSetEvtArrayData (arrGesture, 4, radius);
                        sdkSetEvtArrayData (arrGesture, 5, progress);
                        sdkSetEvtArrayData (arrGesture, 6, duration);
                        sdkSetEvtArrayData (arrGesture, 7, gestCircle.pointable().id());
                        sdkSetEvtArrayData (arrGesture, 8, normal.x);
                        sdkSetEvtArrayData (arrGesture, 9, normal.y);
                        sdkSetEvtArrayData (arrGesture, 10, normal.z);
                    }
                }
                break;
            case Leap::Gesture::TYPE_KEY_TAP:
                {
                    if (gesture.state() == Gesture::STATE_STOP)
                    {
						KeyTapGesture gestKeyTap = KeyTapGesture (gesture);
						
                        Vector stopPos (getZoomedPos (leapInteractionBox.normalizePoint (gestKeyTap.position(), false)));
                        float duration = gestKeyTap.duration() / 1000.0;
						
                        sdkSetEvtSize (arrGesture, 6);
                        sdkSetEvtArrayData (arrGesture, 0, GEST_ID_KEY_TAPE);
                        sdkSetEvtArrayData (arrGesture, 1, std::min (1.0f, std::max (0.0f, stopPos.x)));
                        sdkSetEvtArrayData (arrGesture, 2, std::min (1.0f, std::max (0.0f, stopPos.y)));
                        sdkSetEvtArrayData (arrGesture, 3, std::min (1.0f, std::max (0.0f, stopPos.z)));
                        sdkSetEvtArrayData (arrGesture, 4, duration);
                        sdkSetEvtArrayData (arrGesture, 5, gestKeyTap.pointable().id());
                    }
                }
                break;
            case Leap::Gesture::TYPE_SCREEN_TAP:
                {
                    if (gesture.state() == Gesture::STATE_STOP)
                    {
						ScreenTapGesture gestScreenTap = ScreenTapGesture (gesture);
						
                        Vector stopPos (getZoomedPos (leapInteractionBox.normalizePoint (gestScreenTap.position(), false)));
                        float duration = gestScreenTap.duration() / 1000.0;
						
                        sdkSetEvtSize (arrGesture, 6);
                        sdkSetEvtArrayData (arrGesture, 0, GEST_ID_SCREEN_TAPE);
                        sdkSetEvtArrayData (arrGesture, 1, std::min (1.0f, std::max (0.0f, stopPos.x)));
                        sdkSetEvtArrayData (arrGesture, 2, std::min (1.0f, std::max (0.0f, stopPos.y)));
                        sdkSetEvtArrayData (arrGesture, 3, std::min (1.0f, std::max (0.0f, stopPos.z)));
                        sdkSetEvtArrayData (arrGesture, 4, duration);
                        sdkSetEvtArrayData (arrGesture, 5, gestScreenTap.pointable().id());
                    }
                }
                break;
            default:
                break;

            }
                    
        }
        else
        {
            sdkSetEvtSize (arrGesture, 0);
        }
    }
    
    isLeftHandValid = FALSE;
    isRightHandValid = FALSE;

    isCollectingDatas = true;

    // populate 3d points arrays
    for (int i = 0; i < frame.hands().count(); ++i)
    {
        updateHand3DPoints (frame.hands()[i]);
    }
    
    // hand validity are collected during the updateHand3DPoints ()
    sdkSetEvtData (ledLeftHandValid, isLeftHandValid);
    sdkSetEvtArrayData (arrLeftHandTipsValid, 0, isLeftHandValid);
    sdkSetEvtArrayData (arrLeftHandTipsValid, 1, isLeftHandValid);
    sdkSetEvtArrayData (arrLeftHandTipsValid, 2, isLeftHandValid);
    sdkSetEvtArrayData (arrLeftHandTipsValid, 3, isLeftHandValid);
    sdkSetEvtArrayData (arrLeftHandTipsValid, 4, isLeftHandValid);
    sdkSetEvtData (ledRightHandValid, isRightHandValid);
    sdkSetEvtArrayData (arrRightHandTipsValid, 0, isRightHandValid);
    sdkSetEvtArrayData (arrRightHandTipsValid, 1, isRightHandValid);
    sdkSetEvtArrayData (arrRightHandTipsValid, 2, isRightHandValid);
    sdkSetEvtArrayData (arrRightHandTipsValid, 3, isRightHandValid);
    sdkSetEvtArrayData (arrRightHandTipsValid, 4, isRightHandValid);

    ToolList allTools = frame.tools();
    Tool tool;
    sdkSetEvtSize (arrTools3DPoints, allTools.count() * 3 *  2);
    TPrecision* hand3DPointList = sdkGetEvtDataAddr (arrTools3DPoints);
    Vector pos;

    allTools.count() > 0 ? isDrawTools = true : isDrawTools = false;

    for (int index = 0; index < allTools.count(); index++) 
    {
        tool = allTools[index];
        pos = tool.tipPosition();
        hand3DPointList[(index*6) + 0] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).x;
        hand3DPointList[(index*6) + 1] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).y;
        hand3DPointList[(index*6) + 2] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).z;
        
        pos = tool.tipPosition() - (tool.direction() * tool.length ());
        hand3DPointList[(index*6) + 3] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).x;
        hand3DPointList[(index*6) + 4] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).y;
        hand3DPointList[(index*6) + 5] = getZoomedPos (leapInteractionBox.normalizePoint (pos, false)).z;
    }
    
    isCollectingDatas = false;

    sdkRepaintPanel ();

    if (isTraceLevelVerbose())
    {
        trace ("new leap frame, id=", frame.id());
        trace ("hands, num=", frame.hands().count());
        trace ("fingers, num=", frame.fingers().count());
        trace ("tools, num=", frame.tools().count());
    }
}

void LeapMotion::updateHand3DPoints (const Leap::Hand& hand)
{
    using namespace Leap;

    TPrecision* hand3DPointList = nullptr;
    TPrecision* tipsPosXList = nullptr;
    TPrecision* tipsPosYList = nullptr;
    TPrecision* tipsPosZList = nullptr;
    TPrecision* tipsDirXList = nullptr;
    TPrecision* tipsDirYList = nullptr;
    TPrecision* tipsDirZList = nullptr;

    const Vector vPalm = hand.palmPosition();
    Vector bonePos = getZoomedPos (leapInteractionBox.normalizePoint (vPalm, false));
    Vector boneDir;

    if (hand.isLeft())
    {
        hand3DPointList = sdkGetEvtDataAddr (arrLeftHand3DPoints);
        tipsPosXList = sdkGetEvtDataAddr (arrLeftHandTipsPosX);
        tipsPosYList = sdkGetEvtDataAddr (arrLeftHandTipsPosY);
        tipsPosZList = sdkGetEvtDataAddr (arrLeftHandTipsPosZ);
        
        tipsDirXList = sdkGetEvtDataAddr (arrLeftHandTipsDirX);
        tipsDirYList = sdkGetEvtDataAddr (arrLeftHandTipsDirY);
        tipsDirZList = sdkGetEvtDataAddr (arrLeftHandTipsDirZ);

        sdkSetEvtData (fdrLeftHandConfidence, hand.confidence());
        sdkSetEvtData (ledLeftHandValid, 1.0f);
        isLeftHandValid = TRUE;

        // fill the palm pos 3d point
        sdkSetEvtArrayData (arrLeftPalm3DPoint, 0, bonePos.x);
        sdkSetEvtArrayData (arrLeftPalm3DPoint, 1, bonePos.y);
        sdkSetEvtArrayData (arrLeftPalm3DPoint, 2, 1.0f - bonePos.z);


    }
    else if (hand.isRight())
    {
        hand3DPointList = sdkGetEvtDataAddr (arrRightHand3DPoints);
        tipsPosXList = sdkGetEvtDataAddr (arrRightHandTipsPosX);
        tipsPosYList = sdkGetEvtDataAddr (arrRightHandTipsPosY);
        tipsPosZList = sdkGetEvtDataAddr (arrRightHandTipsPosZ);
        
        tipsDirXList = sdkGetEvtDataAddr (arrRightHandTipsDirX);
        tipsDirYList = sdkGetEvtDataAddr (arrRightHandTipsDirY);
        tipsDirZList = sdkGetEvtDataAddr (arrRightHandTipsDirZ);

        sdkSetEvtData (fdrRightHandConfidence, hand.confidence());
        sdkSetEvtData (ledRightHandValid, 1.0f);
        isRightHandValid = TRUE;

        // fill the palm pos 3d point
        sdkSetEvtArrayData (arrRightPalm3DPoint, 0, bonePos.x);
        sdkSetEvtArrayData (arrRightPalm3DPoint, 1, bonePos.y);
        sdkSetEvtArrayData (arrRightPalm3DPoint, 2, 1.0f - bonePos.z);
    }

    const Vector vPalmDir = hand.direction();
    const Vector vPalmNormal = hand.palmNormal();
    const Vector vPalmSide = vPalmDir.cross(vPalmNormal).normalized();

    const float fThumbDist = hand.fingers()[Finger::TYPE_THUMB].bone(Bone::TYPE_METACARPAL).prevJoint().distanceTo(hand.palmPosition());
    const Vector vWrist = vPalm - fThumbDist*(vPalmDir*0.90f + (hand.isLeft() ? -1.0f : 1.0f)*vPalmSide*0.38f);
    
    bonePos = getZoomedPos (leapInteractionBox.normalizePoint (vWrist, false));
    hand3DPointList[0] = bonePos.x;
    hand3DPointList[1] = bonePos.y;
    hand3DPointList[2] = 1.0f - bonePos.z;

    FingerList fingers = hand.fingers();

    float fRadius = 0.0f;
    Vector vCurBoxBase;
    Vector vLastBoxBase = vWrist;
    
    int counter = 0;

    for (int i = 0, ei = fingers.count(); i < ei; i++) 
	{
        const Finger& finger = fingers[i];
        fRadius = finger.width() * 0.5f;
      
        // draw individual fingers
        for (int j = Bone::TYPE_METACARPAL; j <= Bone::TYPE_DISTAL; j++) 
	    {
            const Bone& bone = finger.bone(static_cast<Bone::Type>(j));

            if (bone.isValid())
            {
                counter = ARR_3D_DATA_NB_LINES + (i*ARR_3D_DATA_NB_LINES*4) + (j*ARR_3D_DATA_NB_LINES);

                bonePos = getZoomedPos (leapInteractionBox.normalizePoint (bone.nextJoint(), false));

                hand3DPointList[counter]     = bonePos.x;
                hand3DPointList[counter + 1] = bonePos.y;
                hand3DPointList[counter + 2] = 1.0f - bonePos.z;

                if (bone.type() == Bone::TYPE_DISTAL)
                {
                    tipsPosXList[i] = bonePos.x;
                    tipsPosYList[i] = bonePos.y;
                    tipsPosZList[i] = 1.0f - bonePos.z;
                    
                    boneDir = bone.direction ();
                    tipsDirXList[i] = - boneDir.x;
                    tipsDirYList[i] = - boneDir.y;
                    tipsDirZList[i] = boneDir.z;
                }
            }
        }
    }
}

void LeapMotion::drawAxis ()
{
    /*
    using namespace Leap;

    Vector axisOrigin (0.0f, 0.0f, 0.0f);
    Vector axisX (100.0f, 0.0f, 0.0f);
    Vector axisY (0.0f, 100.0f, 0.0f);
    Vector axisZ (0.0f, 0.0f, 100.0f);


    sdkDrawLine (transformLeapPointToDraw (axisOrigin), transformLeapPointToDraw (axisX), colorAxisX, 2);
    sdkDrawLine (transformLeapPointToDraw (axisOrigin), transformLeapPointToDraw (axisY), colorAxisY, 2);
    sdkDrawLine (transformLeapPointToDraw (axisOrigin), transformLeapPointToDraw (axisZ), colorAxisZ, 2);
    */
}

void LeapMotion::drawSkeletonHand (const UsineEventPtr arrPalm3DPoint, const UsineEventPtr arrHand3DPoints, TColorUsine color)
{
    int offset = 0;
    
    // begin draw path sequence
    sdkDrawPathStart ();

    for (int i = 0; i < 5; i++) 
	{
        offset = 1 + i*4;
        sdkDrawPathMoveTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, offset)));
        sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, offset + 1)));
        sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, offset + 2)));
        sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, offset + 3)));
    }

    //// draw the metacarpal box
    sdkDrawPathMoveTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 17)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 0)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 1)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 5)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 9)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 13)));
    sdkDrawPathLineTo (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, 17)));

     
    // end draw path sequence
    sdkDrawPathDraw (color, settingDrawThickness);

    // draw joints points
    sdkDrawPoint (transform3DPointToDraw (sdkGetEvt3DPoint (arrPalm3DPoint, 0)), color, settingDrawThickness * 2, true);
    for (int i = 0; i < ARR_3D_DATA_POINTS; i++)
    sdkDrawPoint (transform3DPointToDraw (sdkGetEvt3DPoint (arrHand3DPoints, i)), color, settingDrawThickness * 2, true);
}


void LeapMotion::drawTools ()
{
    TPointF pointDrawStart;
    TPointF pointDrawEnd;

    int numPoints = sdkGetEvtSize (arrTools3DPoints) / ARR_3D_DATA_NB_LINES;
    
    // begin draw path sequence
    sdkDrawPathStart ();

    for (int i = 0; i < numPoints; i+= 2 )
    {
        pointDrawStart = transform3DPointToDraw (sdkGetEvt3DPoint (arrTools3DPoints, i));
        pointDrawEnd = transform3DPointToDraw (sdkGetEvt3DPoint (arrTools3DPoints, i+1));

        sdkDrawPathMoveTo (pointDrawStart);
        sdkDrawPathLineTo (pointDrawEnd);
        
        sdkDrawPoint (pointDrawStart, settingLeftHandColor, settingDrawThickness * 2, true);
        sdkDrawPoint (pointDrawEnd, settingLeftHandColor, settingDrawThickness * 2, true);
    }
    // end draw path sequence
    sdkDrawPathDraw (settingLeftHandColor, settingDrawThickness);

}


TPointF LeapMotion::transform3DPointToDraw (T3DPointF point)
{
    TPointF result;
    float pointZ = (1.0f - point.z);

    //result.x = (point.x * (1.0f - viewTranslate)) - (point.z * viewTranslate);
    //result.y = 1.0f - ((point.y * cos(1.0f - abs(viewTranslate))) + (point.z * sin(abs(viewTranslate))));
    result.x = (point.x * (1.0f - settingViewAngle)) - (pointZ * settingViewAngle);
    result.y = ((1.0f - point.y) * 0.7f) + (pointZ * 0.3f);

    return result;

}
//------------------------------------------------------------------------
// trace helpers
void LeapMotion::trace (AnsiCharPtr traceMsg) 
{
	if (isTraceLevelNone ()) return;
    
    std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg;
    stringTrace = os.str();
	sdkTraceChar( (AnsiCharPtr) stringTrace.c_str());
}

void LeapMotion::trace (AnsiCharPtr traceMsg, int value ) 
{
	if (isTraceLevelNone ()) return;
    
    std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << value;
    stringTrace = os.str();
	sdkTraceChar( (AnsiCharPtr) stringTrace.c_str());
}
    
void LeapMotion::trace (AnsiCharPtr traceMsg, std::string value ) 
{
    trace (traceMsg, value.c_str() );
}

void LeapMotion::trace (AnsiCharPtr traceMsg, AnsiCharPtr value ) 
{
	if (isTraceLevelNone ()) return;
    
    std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << value;
    stringTrace = os.str();
	sdkTraceChar( (AnsiCharPtr) stringTrace.c_str());
}

void LeapMotion::trace (AnsiCharPtr traceMsg, int v1, int v2, int v3 ) 
{
	if (isTraceLevelNone ()) return;
    
    std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << v1 << ", " << v2 << ", " << v3;
    stringTrace = os.str();
	sdkTraceChar( (AnsiCharPtr) stringTrace.c_str());
}

void LeapMotion::traceError (AnsiCharPtr traceMsg, std::string errorMessage) 
{
    traceError (traceMsg, errorMessage.c_str());
} 

void LeapMotion::traceError (AnsiCharPtr traceMsg, AnsiCharPtr errorMessage) 
{
    if ( errorMessage != nullptr ) 
	{    
        std::ostringstream os;
	    os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << errorMessage;
        stringTrace = os.str();
	    sdkTraceErrorChar ( (AnsiCharPtr) stringTrace.c_str());
	} 
	else 
	{        
        std::ostringstream os;
	    os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << "UNKNOWN ERROR";
		sdkTraceErrorChar( (AnsiCharPtr) stringTrace.c_str() );
	}
} 

//-------------------------------------------------------------------------
// private methodes
//------------------------------------------------------------------------

//-----------------------------------------------------------------------
// from Leap::Listener
void LeapMotion::onInit(const Leap::Controller& controller) 
{  
    controller.setPolicyFlags(Leap::Controller::POLICY_BACKGROUND_FRAMES);
    if (isTraceLevelDebug ())
        trace ("init connection");
}

void LeapMotion::onConnect(const Leap::Controller&) 
{
    if (isTraceLevelInfo ())
        trace ("connected");
}

void LeapMotion::onFrame(const Leap::Controller& controller)
{
    if (leapConnected == TRUE && sdkPatchIsRunning() != FALSE)
        updateLeapMotionDatas (lastFrame = controller.frame());
}

void LeapMotion::onDisconnect(const Leap::Controller&) 
{
    if (isTraceLevelInfo ())
        trace ("disconnected");
}


void LeapMotion::onExit(const Leap::Controller&) 
{
    if (isTraceLevelInfo ())
        trace ("disconnected");
}
