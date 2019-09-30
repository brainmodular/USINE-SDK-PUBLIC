//-----------------------------------------------------------------------------
//@file  
//	Wiimote.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the Wiimote class.
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
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
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
#ifndef __WIIMOTE_H__
#define __WIIMOTE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <limits>
#include <wiiuse.h>
#include "JuceHeader.h"
#include "WiimoteManager.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define PARAMS_BEFORE_WII_LEDS	                3
#define PARAMS_WII_LEDS_NUM		                4
#define PARAMS_WII_BEFORE_ACC	                (PARAMS_BEFORE_WII_LEDS + PARAMS_WII_LEDS_NUM)
#define PARAMS_WII_ACC_NUM		                3
#define PARAMS_WII_BEFORE_POS	                (PARAMS_WII_BEFORE_ACC + PARAMS_WII_ACC_NUM)
#define PARAMS_WII_POS_NUM		                3
#define PARAMS_WII_POS_RES		                1
#define PARAMS_WII_BEFORE_ROT	                (PARAMS_WII_BEFORE_POS + PARAMS_WII_POS_NUM + PARAMS_WII_POS_RES)
#define PARAMS_WII_ROT_NUM		                3
#define PARAMS_WII_BEFORE_BUTTONS	            (PARAMS_WII_BEFORE_ROT + PARAMS_WII_ROT_NUM)
#define PARAMS_WII_BUTTONS		                11
#define PARAMS_WII_BEFORE_WANT_MPLUS            (PARAMS_WII_BEFORE_BUTTONS + PARAMS_WII_BUTTONS)
#define PARAMS_WII_WANT_MPLUS	                1
#define PARAMS_WII_BEFORE_MPLUS_ACC_NUM	        (PARAMS_WII_BEFORE_WANT_MPLUS + PARAMS_WII_WANT_MPLUS)
#define PARAMS_WII_MPLUS_ACC_NUM                3
#define PARAMS_WII_BEFORE_NUNCHUK               (PARAMS_WII_BEFORE_MPLUS_ACC_NUM + PARAMS_WII_MPLUS_ACC_NUM)
#define PARAMS_WII_NUNCHUK_BUTTONS_NUM	        2
#define PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM    (PARAMS_WII_BEFORE_NUNCHUK + PARAMS_WII_NUNCHUK_BUTTONS_NUM)
#define PARAMS_WII_NUNCHUK_ORIENT_NUM		    3
#define PARAMS_WII_NUNCHUK_JS_NUM		        2
#define PARAMS_WII_BEFORE_WANT_IR               (PARAMS_WII_BEFORE_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_ORIENT_NUM + PARAMS_WII_NUNCHUK_JS_NUM)
#define PARAMS_WII_WANT_IR						1
#define PARAMS_WII_BEFORE_IR_ZONE_NUM			(PARAMS_WII_BEFORE_WANT_IR + PARAMS_WII_WANT_IR)
#define PARAMS_WII_IR_ZONE_NUM					4
#define PARAMS_WII_IR_ZONE_INFOS_NUM			3
#define PARAMS_TOTAL_NUM		                (PARAMS_WII_BEFORE_IR_ZONE_NUM + (PARAMS_WII_IR_ZONE_NUM * PARAMS_WII_IR_ZONE_INFOS_NUM))

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class Wiimote : public UserModuleBase, public ActionListener
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	Wiimote();

    // destructor
	virtual ~Wiimote();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// init
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:
	
	//-------------------------------------------------------------------------
	// from ActionListener
	virtual void actionListenerCallback(const String& message);

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events	
	UsineEventPtr btnPollWiiEvent;
	UsineEventPtr fdrActiveWiiNum;
	
	UsineEventPtr swtchWiiVibration;
	UsineEventPtr swtchWiiLeds[PARAMS_WII_LEDS_NUM];
	
	// acceleration
	UsineEventPtr fdrWiiAcc[PARAMS_WII_ACC_NUM];

	// position
	UsineEventPtr fdrWiiPos[PARAMS_WII_POS_NUM];
	UsineEventPtr btnResetWiiPos;

	// orientation
	UsineEventPtr fdrWiiPitch;
	UsineEventPtr fdrWiiRoll;
	UsineEventPtr fdrWiiYaw;
	
	// buttons
	UsineEventPtr rledWiiButtonA;
	UsineEventPtr rledWiiButtonB;
	UsineEventPtr rledWiiButtonOne;
	UsineEventPtr rledWiiButtonTwo;
	UsineEventPtr rledWiiButtonPlus;
	UsineEventPtr rledWiiButtonMinus;
	UsineEventPtr rledWiiButtonHome;
	UsineEventPtr rledWiiButtonUp;
	UsineEventPtr rledWiiButtonDown;
	UsineEventPtr rledWiiButtonLeft;
	UsineEventPtr rledWiiButtonRight;
    
    //motion plus
	UsineEventPtr swtchWantMotionplus;
	UsineEventPtr fdrWiiMplusAcc[PARAMS_WII_MPLUS_ACC_NUM];

    // nunchuk
	UsineEventPtr rledWiiNunchukButtonC;
	UsineEventPtr rledWiiNunchukButtonZ;
    
	UsineEventPtr fdrWiiNunchukOrient[PARAMS_WII_NUNCHUK_ORIENT_NUM];

	UsineEventPtr fdrWiiNunchukJsAngle;
	UsineEventPtr fdrWiiNunchukJsMag;
	
	// ir
	UsineEventPtr swtchWantInfrared;
	UsineEventPtr fdrWiiIrX[PARAMS_WII_IR_ZONE_NUM];
	UsineEventPtr fdrWiiIrY[PARAMS_WII_IR_ZONE_NUM];
	UsineEventPtr fdrWiiIrSize[PARAMS_WII_IR_ZONE_NUM];
	
	//
	struct wiimote_t* wm;
	WiimoteManagerPtr wiimoteManagerPtr;
    juce::int64 updateTimestamp;

    //
    static const int WII_GRAVITY         = 25;
    static const int WII_GRAVITY_NUNCHUK = 50;
    static const int WII_ACC_X           = 128;
    static const int WII_ACC_Y           = 125;
    static const int WII_ACC_Z           = 153; //128 + WII_GRAVITY
    static const int WII_NC_ACC_X        = 123;
    static const int WII_NC_ACC_Y        = 122;
    static const int WII_NC_ACC_Z        = 179; // 129 + WII_GRAVITY_NUNCHUK
    
    static const float WII_POS_RELEASE;
    
    float wiiAccX;
    float wiiAccY;
    float wiiAccZ;
    float wiiAccOldX;
    float wiiAccOldY;
    float wiiAccOldZ;
    float wiiSpeedX;
    float wiiSpeedY;
    float wiiSpeedZ;
    float wiiSpeedOldX;
    float wiiSpeedOldY;
    float wiiSpeedOldZ;
    float wiiDistanceX;
    float wiiDistanceY;
    float wiiDistanceZ;
    float wiiDistanceOldX;
    float wiiDistanceOldY;
    float wiiDistanceOldZ;
	
    float wiiNcAccX;
    float wiiNcAccY;
    float wiiNcAccZ;
	
    String tmpWiiIrCaption;

	int numWiiIndex;
	bool isWiiConnected;
	
	bool isWiiIrActivated;
	bool isWiiNunchukActivated;
	bool isWiiMotionplusActivated;
    
	int connectLedState;
	int connectRumbleState;

    bool connectMotionplusState;
    bool connectInfraredState;

    //
	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:  
    // Fast inverse square-root
    // See: http://en.wikipedia.org/wiki/Fast_inverse_square_root
    float invSqrt(float x) {
	    float halfx = 0.5f * x;
	    float y = x;
	    long i = *(long*)&y;
	    i = 0x5f3759df - (i>>1);
	    y = *(float*)&i;
	    y = y * (1.5f - (halfx * y * y));
	    return y;
    };

    static float deg2rad(float degrees)
    {
        return (float)(PI / 180.0) * degrees;
    }
    static float rad2deg(float rad)
    {
        return (float)(180.0 * rad) / PI;
    }

    void resetWiiPos ();
    
    //
	char* getWiiLedsCaptions (int index);
	char* getWiiAccCaptions (int index);
	char* getWiiPosCaptions (int index);
    
	char* getWiiMplusAccCaptions (int index);
	char* getWiiMplusOrientCaptions (int index);
    char* getWiiNunchukOrientCaptions (int index);
    const char* getWiiIrCaptions (int indexZone, int indexParam);

	void toggleWiiVibration();

	void switchWiiLed (int index);
	void switchWiiMotionplus (int index);
	void switchWiiInfrared  (int index);
    
	//-------------------------------------------------------------------------
	// wiimote callbacks 
    void onWiiConnect ();
    void onWiiDisconnect ();
    void onWiiUpdateEvent ();
    void onWiiIrActivated ();
    void onWiiIrDeactivated ();
    void onWiiNunchukActivated ();
    void onWiiNunchukDeactivated ();
    void onWiiMotionplusActivated ();
    void onWiiMotionplusDeactivated ();

}; // class Wiimote


const float Wiimote::WII_POS_RELEASE     = 0.999f;

#endif //__WIIMOTE_H__