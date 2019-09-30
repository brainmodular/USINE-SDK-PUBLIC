//-----------------------------------------------------------------------------
//@file  
//	Wiimote.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the WiimoteMManager class.
//
//  module to act as audio and midi inputs/outputs for the devices categorie.
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

// include once, no more
#ifndef __WIIMOTE_MANAGER_H__
#define __WIIMOTE_MANAGER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <limits>
#include <wiiuse.h>
#include <wiiuse_internal.h>
#include "JuceHeader.h"


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define WII_MAX_WIIMOTES        4
#define WII_POLL_PERIODE_MS     1

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------
class WiimoteManager;

typedef ReferenceCountedObjectPtr <WiimoteManager> WiimoteManagerPtr;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// the audio device module class
class WiimoteManager : public Thread, public ReferenceCountedObject
{    
	//-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	WiimoteManager();
	
    // destructor
	~WiimoteManager();
	
	//-------------------------------------------------------------------------
	// public members
	//-------------------------------------------------------------------------
public:
	
	// events for action broadcaster
	static const String WII_EVT_CONNECT;
	static const String WII_EVT_DISCONNECT;
	static const String WII_EVT_UPDATE;
	static const String WII_EVT_NUNCHUK_ACTIVATED;
	static const String WII_EVT_NUNCHUK_DEACTIVATED;
	static const String WII_EVT_MOTION_PLUS_ACTIVATED;
	static const String WII_EVT_MOTION_PLUS_DEACTIVATED;
	
    juce::int64 receiveTimestamp;

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:
	//
	static WiimoteManager* getInstance ()
	{
		if (managerInstance == nullptr)
		{
			initialiseJuce_GUI ();
			managerInstance = WiimoteManagerPtr(new WiimoteManager());
		}
		return managerInstance;
	};
	
	static void unregister ()
	{
		// 
	    if ( managerInstance->getReferenceCount() == 1 && managerInstance != nullptr)
		{
		    managerInstance = nullptr;
			shutdownJuce_GUI (); 
		}
	};

	void connectWiimote(int wiiIndex, ActionListener* listener);
	void disconnectWiimote(int wiiIndex, ActionListener* listener);
	
    void wantAccelFeedback (int wiiIndex, bool wanted);
    void wantInfraredFeedback (int wiiIndex, bool wanted);
    //void wantNunchukFeedback (int wiiIndex, bool wanted);
    void wantMotionplusFeedback (int wiiIndex, bool wanted);
	
	void setWiiLeds(int wiiIndex, int ledsState);
	void setWiiRumble(int wiiIndex, int rumbleState);
	
	wiimote* getWiimoteInfos (int wiiIndex);
	
	//-------------------------------------------------------------------------
	// from Thread
	// to pump  wiimote events
	virtual void run ();
	
	CriticalSection wiiCriticalSection;
	
	//-------------------------------------------------------------------------
	// protected methodes
	//-------------------------------------------------------------------------
protected:
	int pollWiiEvents ();
	
	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:	
	
	int numWiiFound;
	int indexWiiToConnect;
    wiimote** wiimotes;
	bool shutDown;
    
	int wiimoteConnected;

	static WiimoteManagerPtr managerInstance;
	
	ActionBroadcaster wiiBroadcaster[WII_MAX_WIIMOTES];

}; // class WiimoteManager


#endif //__WIIMOTE_MANAGER_H__