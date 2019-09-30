//-----------------------------------------------------------------------------
//@file  
//	Leap-Motion.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the LeapMotion class.
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

// include once, no more
#ifndef __LEAP_MOTION_H__
#define __LEAP_MOTION_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"

#include <iostream>
#include <cmath>
#include <limits>
#include <queue>
#include <string>

#include <Leap.h>


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
// module constant
const int MODULE_DIM_DEFAULT = 200; 

static const int NUM_OF_HANDS_MAX       = 2;
static const int NUM_OF_FINGERS_MAX     = 10;
static const int ARR_INDEX_NULL         = -1;
static const float ARR_CONTENT_NULL     = 0;

static const float ARR_3D_DATA_NB_LINES  = 3;
static const float ARR_3D_DATA_POINTS    = 21;

// gestures identifiants
static const int GEST_ID_SWIPE_LEFT             = 200;
static const int GEST_ID_SWIPE_RIGHT            = 201;
static const int GEST_ID_SWIPE_UP               = 202;
static const int GEST_ID_SWIPE_DOWN             = 203;
static const int GEST_ID_SWIPE_FORWARD          = 204;
static const int GEST_ID_SWIPE_BACKWARD         = 205;
static const int GEST_ID_CIRCLE_CLOCK           = 206;
static const int GEST_ID_CIRCLE_COUNTER_CLOCK   = 207;
static const int GEST_ID_KEY_TAPE               = 208;
static const int GEST_ID_SCREEN_TAPE            = 209;

// gestures setting def values
static const std::string GEST_ID_SWIPE_MIN_LENGTH           = "Gesture.Swipe.MinLength";
static const std::string GEST_ID_SWIPE_MIN_VEL              = "Gesture.Swipe.MinVelocity";
static const std::string GEST_ID_CIRCLE_MIN_RADIUS          = "Gesture.Circle.MinRadius";
static const std::string GEST_ID_CIRCLE_MIN_ARC             = "Gesture.Circle.MinArc";
static const std::string GEST_ID_KEY_TAPE_MIN_DOWN_VEL      = "Gesture.KeyTap.MinDownVelocity";
static const std::string GEST_ID_KEY_TAPE_HIST_SECONDS      = "Gesture.KeyTap.HistorySeconds";
static const std::string GEST_ID_KEY_TAPE_MIN_DISTANCE      = "Gesture.KeyTap.MinDistance";
static const std::string GEST_ID_SCREEN_TAPE_MIN_DOWN_VEL   = "Gesture.ScreenTap.MinForwardVelocity";
static const std::string GEST_ID_SCREEN_TAPE_HIST_SECONDS   = "Gesture.ScreenTap.HistorySeconds";
static const std::string GEST_ID_SCREEN_TAPE_MIN_DISTANCE   = "Gesture.ScreenTap.MinDistance";

static const float GEST_DEF_SWIPE_MIN_LENGTH           = 150;      // mm
static const float GEST_DEF_SWIPE_MIN_VEL              = 1000;     // mm/s
static const float GEST_DEF_CIRCLE_MIN_RADIUS          = 5;        // mm
static const float GEST_DEF_CIRCLE_MIN_ARC             = 1.5 * PI; // radians  
static const float GEST_DEF_KEY_TAPE_MIN_DOWN_VEL      = 50;       // mm/s
static const float GEST_DEF_KEY_TAPE_HIST_SECONDS      = 0.1;      // s
static const float GEST_DEF_KEY_TAPE_MIN_DISTANCE      = 5.0;      // mm
static const float GEST_DEF_SCREEN_TAPE_MIN_DOWN_VEL   = 50;       // mm/s
static const float GEST_DEF_SCREEN_TAPE_HIST_SECONDS   = 0.1;      // s
static const float GEST_DEF_SCREEN_TAPE_MIN_DISTANCE   = 3.0;      // mm

typedef    std::vector <float>          FloatList;
typedef    std::vector <int>            IntList;
typedef    std::vector<int>::iterator   IntListItor;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class LeapMotion 
    : public UserModuleBase
    , private Leap::Listener
{

    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	LeapMotion();

    // destructor
	virtual ~LeapMotion();


	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	//  init
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
    void onProcess (){};

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
	void onPaint ();

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:
    
	//------------------------------------------------------------------------
    // trace helpers
	void trace (AnsiCharPtr traceMsg);
	void trace (AnsiCharPtr traceMsg, int value);
	void trace (AnsiCharPtr traceMsg, std::string value);
	void trace (AnsiCharPtr traceMsg, AnsiCharPtr value);
	void trace (AnsiCharPtr traceMsg, int v1, int v2, int v3);
    void traceError (AnsiCharPtr traceMsg, std::string errorMessage);
	void traceError (AnsiCharPtr traceMsg, AnsiCharPtr errorMessage); 
    
	//------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr swtchOn;
	UsineEventPtr ledIsConnected;
    UsineEventPtr cboxPosZoomLevel;
	UsineEventPtr dtfFrameId;
    
    
	UsineEventPtr dtfNumOfHands;
    UsineEventPtr swtchEnableHands;
	UsineEventPtr arrHandsId;
	UsineEventPtr arrHandsNumFingers;
	UsineEventPtr arrHandsPosX;
	UsineEventPtr arrHandsPosY;
	UsineEventPtr arrHandsPosZ;
	UsineEventPtr arrHandsRadius;
	UsineEventPtr arrHandsValid;

    UsineEventPtr swtchEnablePalms;
	UsineEventPtr arrPalmsHandsId;
    UsineEventPtr arrPalmsPosX;
    UsineEventPtr arrPalmsPosY;
    UsineEventPtr arrPalmsPosZ;
    UsineEventPtr arrPalmsDirX;
    UsineEventPtr arrPalmsDirY;
    UsineEventPtr arrPalmsDirZ;
    UsineEventPtr arrPalmsVelX;
    UsineEventPtr arrPalmsVelY;
    UsineEventPtr arrPalmsVelZ;
    UsineEventPtr arrPalmsNorX;
    UsineEventPtr arrPalmsNorY;
    UsineEventPtr arrPalmsNorZ;
    UsineEventPtr arrPalmsValid;
    
    UsineEventPtr dtfNumOfFingers;
    UsineEventPtr swtchEnableFingers;
    UsineEventPtr arrFingersId;
    UsineEventPtr arrFingersHandsId;
    UsineEventPtr arrFingersPosX;
    UsineEventPtr arrFingersPosY;
    UsineEventPtr arrFingersPosZ;
    UsineEventPtr arrFingersDirX;
    UsineEventPtr arrFingersDirY;
    UsineEventPtr arrFingersDirZ;
    UsineEventPtr arrFingersVelX;
    UsineEventPtr arrFingersVelY;
    UsineEventPtr arrFingersVelZ;
    UsineEventPtr arrFingersLength;
    UsineEventPtr arrFingersIsTool;
    UsineEventPtr arrFingersValid;
    
    UsineEventPtr swtchEnableGestures;
    UsineEventPtr arrGesture;
	
    UsineEventPtr arrLeftPalm3DPoint;
    UsineEventPtr arrLeftHand3DPoints;
    UsineEventPtr ledLeftHandValid;
    UsineEventPtr fdrLeftHandConfidence;
    UsineEventPtr arrLeftHandTipsPosX;
    UsineEventPtr arrLeftHandTipsPosY;
    UsineEventPtr arrLeftHandTipsPosZ;
    UsineEventPtr arrLeftHandTipsDirX;
    UsineEventPtr arrLeftHandTipsDirY;
    UsineEventPtr arrLeftHandTipsDirZ;
    UsineEventPtr arrLeftHandTipsValid;

    UsineEventPtr arrRightPalm3DPoint;
    UsineEventPtr arrRightHand3DPoints;
    UsineEventPtr ledRightHandValid;
    UsineEventPtr fdrRightHandConfidence;
    UsineEventPtr arrRightHandTipsPosX;
    UsineEventPtr arrRightHandTipsPosY;
    UsineEventPtr arrRightHandTipsPosZ;
    UsineEventPtr arrRightHandTipsDirX;
    UsineEventPtr arrRightHandTipsDirY;
    UsineEventPtr arrRightHandTipsDirZ;
    UsineEventPtr arrRightHandTipsValid;

    UsineEventPtr arrTools3DPoints;

    typedef enum ParamsIndex
    {
          piOn
        , piConnected
        , piPosZoomLevel
        , piFrameId
        , piNumOfHands
        , piEnableHands
        , piHandsId
        , piHandsNumFingers
        , piHandsPosX
        , piHandsPosY
        , piHandsPosZ
        , piHandsRadius
        , piHandsValid
        , piEnablePalms
        , piPalmsHandsId
        , piPalmsPosX
        , piPalmsPosY
        , piPalmsPosZ
        , piPalmsDirX
        , piPalmsDirY
        , piPalmsDirZ
        , piPalmsVelX
        , piPalmsVelY
        , piPalmsVelZ
        , piPalmsNorX
        , piPalmsNorY
        , piPalmsNorZ
        , piPalmsValid
        , piNumOfFingers
        , piEnableFingers
        , piFingersId
        , piFingersHandsId
        , piFingersPosX
        , piFingersPosY
        , piFingersPosZ
        , piFingersDirX
        , piFingersDirY
        , piFingersDirZ
        , piFingersVelX
        , piFingersVelY
        , piFingersVelZ
        , piFingersLength
        , piFingersIsTool
        , piFingersValid
        , piEnableGestures
        , piGesture
        , piLeftPalm3DPoint
        , piLeftHand3DPoints
        , piLeftHandValid
        , piLeftHandConfidence
        , piLeftHandTipsPosX
        , piLeftHandTipsPosY
        , piLeftHandTipsPosZ
        , piLeftHandTipsDirX
        , piLeftHandTipsDirY
        , piLeftHandTipsDirZ
        , piLeftHandTipsValid
        , piRightPalm3DPoint
        , piRightHand3DPoints
        , piRightHandValid
        , piRightHandConfidence
        , piRightHandTipsPosX
        , piRightHandTipsPosY
        , piRightHandTipsPosZ
        , piRightHandTipsDirX
        , piRightHandTipsDirY
        , piRightHandTipsDirZ
        , piRightHandTipsValid
        , piTools3DPoints
        , piNumOfParams
    }ParamsIndex;
    
    //------------------------------------------------------------------------
    // id persistence stuff
    IntList     handsIdSlotsList;
    IntListItor handIdItor;
    IntList     fingersIdSlotsList;
    IntListItor fingerIdItor;

    //------------------------------------------------------------------------
    // selection stuff
    bool handsEnabled;
    bool palmsEnabled;
    bool fingersEnabled;
    bool gesturesEnabled;

    //------------------------------------------------------------------------
    // trace stuff
    int settingTraceLevel;
    
    typedef enum TraceLevel
    {
          tlNone     // 0 = no tracing
        , tlInfo     // 1 = connection and parameter changes
        , tlDebug    // 2 = 1 + data transfer
        , tlVerbose  // 3 = 2 + imoprtant function enter/exit
    }TraceLevel;

    //-------------------------------------------------------------------------
    // leap motion stuff
    Leap::Controller leapController;
    Leap::Frame lastFrame;
    LongBool leapConnected;

    bool isPatchRunning;

    Leap::InteractionBox leapInteractionBox;
    FloatList zoomValuesList;
    TPrecision zoomPositionsLevel;
    int numOfHands;
    int numOfTotalFingers;
            
    Leap::Hand      hand;
    int             handId;
    Leap::Vector    handBallCenter;
    int             handIsValid;
        
    Leap::Vector    palmPos;
    Leap::Vector    palmDir;
    Leap::Vector    palmVel;
    Leap::Vector    palmNor;

    Leap::Finger    finger;
    int             fingerId;
    Leap::Vector    fingerPos;
    Leap::Vector    fingerDir;
    Leap::Vector    fingerVel;
    float           fingerIsTool;
    int             fingerIsValid;

    // gestures settings stuff
    float settingGestureSwipeMinLength;
    float settingGestureSwipeMinVelocity;
    float settingGestureCircleMinRadius;
    float settingGestureCircleMinArc;
    float settingGestureKeyTapeMinDownVelocity;
    float settingGestureKeyTapeHistorySeconds;
    float settingGestureKeyTapeMinDistance;
    float settingGestureScreenTapeMinForwardVelocity;
    float settingGestureScreenTapeHistorySeconds;
    float settingGestureScreenTapeMinDistance;


    // drawing stuff
	float		settingDrawThickness;
    float       settingViewAngle;
	TColorUsine settingLeftHandColor;
	TColorUsine settingRightHandColor;

	//-------------------------------------------------------------------------
	// protected methodes
	//-------------------------------------------------------------------------
protected:  
    Leap::Vector getZoomedPos (Leap::Vector vector);
    TPointF transform3DPointToDraw (T3DPointF point);
    void updateLeapMotionDatas (Leap::Frame);
    void updateHand3DPoints (const Leap::Hand& hand);
    void drawAxis ();
    void drawSkeletonHand (const UsineEventPtr arrPalm3DPoint, const UsineEventPtr arrHand3DPoints, TColorUsine color);
    void drawTools ();

    int paintCounter;
    bool isCollectingDatas;
    LongBool isLeftHandValid;
    LongBool isRightHandValid;
    bool isDrawTools;
	//------------------------------------------------------------------------
    // trace level helpers
	inline bool isTraceLevelNone    () { return (settingTraceLevel == tlNone);      }
	inline bool isTraceLevelInfo    () { return (settingTraceLevel >= tlInfo);      }
	inline bool isTraceLevelDebug   () { return (settingTraceLevel >= tlDebug);     }
	inline bool isTraceLevelVerbose () { return (settingTraceLevel >= tlVerbose);   }

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:  
	//-----------------------------------------------------------------------
    // from Leap::Listener
    virtual void onInit (const Leap::Controller&);
    virtual void onConnect (const Leap::Controller&);
    virtual void onFrame (const Leap::Controller& controller);
    virtual void onDisconnect (const Leap::Controller&);
    virtual void onExit (const Leap::Controller&);

}; // class LeapMotion

#endif //__LEAP_MOTION_H__