//-----------------------------------------------------------------------------
//@file  
//	WiimoteManager.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the Wiimote class.
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//@IMPORTANT
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "WiimoteManager.h"

WiimoteManagerPtr WiimoteManager::managerInstance = nullptr;

const String WiimoteManager::WII_EVT_CONNECT				    = "WII_CONNECT";
const String WiimoteManager::WII_EVT_DISCONNECT				    = "WII_DISCONNECT";
const String WiimoteManager::WII_EVT_UPDATE					    = "WII_UPDATE";
const String WiimoteManager::WII_EVT_NUNCHUK_ACTIVATED		    = "WII_NUNCHUK_ACTIVATED";
const String WiimoteManager::WII_EVT_NUNCHUK_DEACTIVATED		= "WII_NUNCHUK_DEACTIVATED";
const String WiimoteManager::WII_EVT_MOTION_PLUS_ACTIVATED	    = "WII_MOTION_PLUS_ACTIVATED";
const String WiimoteManager::WII_EVT_MOTION_PLUS_DEACTIVATED	= "WII_MOTION_PLUS_DEACTIVATED";


//-------------------------------------------------------------------------
// manager constructor/destructor
//------------------------------------------------------------------------
// constructor
WiimoteManager::WiimoteManager () 
	: Thread ("WiimoteManager")
	, numWiiFound (0)
	, indexWiiToConnect (-1)
	, wiimoteConnected (-1)
    , shutDown (false)
    , receiveTimestamp (0)
{
    
	wiimotes =  wiiuse_init(WII_MAX_WIIMOTES);
	
	numWiiFound = wiiuse_find(wiimotes, WII_MAX_WIIMOTES, 2);
	
	wiiuse_set_timeout(wiimotes, 1, 5, 10);
    
}

// destructor
WiimoteManager::~WiimoteManager ()
{
	
    try 
	{
		
		
    
	for (int i = 0; i < WII_MAX_WIIMOTES; ++i) 
	{
        if (WIIMOTE_IS_CONNECTED(wiimotes[i]))
            wiiuse_disconnected (wiimotes[i]);
        
		wiiBroadcaster[i].removeAllActionListeners ();
    }
	
	const ScopedLock myLock (wiiCriticalSection);
    shutDown = true;
    const ScopedUnlock myUnlock (wiiCriticalSection);
    
    
    stopThread (2000);

	wiiuse_cleanup(wiimotes, WII_MAX_WIIMOTES);
		
	}
	catch (...) 
	{
		
	}
}

//
void WiimoteManager::connectWiimote(int wiiIndex, ActionListener* listener)
{
	if ( (wiiIndex >= 0) && (wiiIndex < WII_MAX_WIIMOTES) )
	{   	
		const ScopedLock myLock (wiiCriticalSection);
		wiiBroadcaster[wiiIndex].addActionListener (listener);
		indexWiiToConnect = wiiIndex;
		
		startThread (0);
		
    }
}

void WiimoteManager::wantAccelFeedback (int wiiIndex, bool wanted)
{
    
    //
	const ScopedLock myLock (wiiCriticalSection);
    if (wanted)
        wiiuse_motion_sensing (wiimotes[wiiIndex], 1);
    else
        wiiuse_motion_sensing (wiimotes[wiiIndex], 0);
}

void WiimoteManager::wantInfraredFeedback (int wiiIndex, bool wanted)
{
    // WIIMOTE_STATE_IR	
	const ScopedLock myLock (wiiCriticalSection);
    if (wanted)
        wiiuse_set_ir(wiimotes[wiiIndex], 1);
    else
        wiiuse_set_ir(wiimotes[wiiIndex], 0);
}

void WiimoteManager::wantMotionplusFeedback (int wiiIndex, bool wanted)
{
    while ((!WIIMOTE_IS_SET (wiimotes[wiiIndex], WIIMOTE_STATE_HANDSHAKE_COMPLETE) ) && (WIIMOTE_IS_CONNECTED(wiimotes[wiiIndex])))
        wiiuse_millisleep(500);
    
	const ScopedLock myLock (wiiCriticalSection);
    if (wanted)
    {
        if (WIIUSE_USING_EXP(wiimotes[wiiIndex]))
            wiiuse_set_motion_plus(wiimotes[wiiIndex], 2); // nunchuck pass-through
        else
            wiiuse_set_motion_plus(wiimotes[wiiIndex], 1);  // standalone
    }
    else
    {
        wiiuse_set_motion_plus(wiimotes[wiiIndex], 0);
    }
}

	
//
void WiimoteManager::disconnectWiimote(int wiiIndex, ActionListener* listener)
{
    wiiBroadcaster[wiiIndex].removeActionListener (listener);
    wiiuse_disconnected(wiimotes[wiiIndex]);
}

void WiimoteManager::setWiiLeds(int wiiIndex, int ledsState)
{
	const ScopedLock myLock (wiiCriticalSection);
	wiiuse_set_leds(wiimotes[wiiIndex], ledsState);
}

void WiimoteManager::setWiiRumble(int wiiIndex, int rumbleState)
{
	const ScopedLock myLock (wiiCriticalSection);
	wiiuse_rumble(wiimotes[wiiIndex], rumbleState);
}

wiimote* WiimoteManager::getWiimoteInfos (int wiiIndex)
{
	const ScopedLock myLock (wiiCriticalSection);
	if ( (wiiIndex >= 0) && (wiiIndex < WII_MAX_WIIMOTES) )
		return wiimotes[wiiIndex];
	else 
		return NULL;

}
int WiimoteManager::pollWiiEvents ()
{
	const ScopedLock myLock (wiiCriticalSection);
    if (shutDown)
    {
        //wiiuse_poll(wiimotes, WII_MAX_WIIMOTES);
        return 0;
    }
    else
        return wiiuse_poll(wiimotes, WII_MAX_WIIMOTES);
}

//-----------------------------------------------------------------------------
// module description
void WiimoteManager::run()
{
	
	wiimotes =  wiiuse_init(WII_MAX_WIIMOTES);
	
	numWiiFound = wiiuse_find(wiimotes, WII_MAX_WIIMOTES, 2);
	
	wiiuse_set_timeout(wiimotes, 1, 5, 10);
	

	while (! threadShouldExit () && !shutDown)
	{
		if (indexWiiToConnect >= 0 ) //&& (! WIIMOTE_IS_CONNECTED(wiimotes[indexWiiToConnect]))) 
		{
			wiimoteConnected = wiiuse_connect(&(wiimotes[indexWiiToConnect]), 1);
			if (wiimoteConnected > 0)
			{
				printf("Connected to %i wiimotes (of %i found).\n", wiimoteConnected, numWiiFound);
			
				//
				wiiuse_motion_sensing(wiimotes[indexWiiToConnect], 1);
				wiiuse_set_ir(wiimotes[indexWiiToConnect], 0);
				
				wiiuse_set_flags (wiimotes[indexWiiToConnect], WIIUSE_SMOOTHING, WIIUSE_CONTINUOUS); // WIIUSE_CONTINUOUS |
				wiiuse_set_accel_threshold (wiimotes[indexWiiToConnect], 1);
				wiiuse_set_orient_threshold (wiimotes[indexWiiToConnect], 1);
                wiiuse_set_ir_sensitivity (wiimotes[indexWiiToConnect], 1);
                
				wiiBroadcaster[indexWiiToConnect].sendActionMessage (WII_EVT_CONNECT);
			}
			indexWiiToConnect = -1;
		}
		  
		//wait(1);
		
		while (pollWiiEvents () && ! threadShouldExit ())
		{
			
			//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
			//-----------------------------------------------------------------------------
			wait(10);
			//-----------------------------------------------------------------------------
#endif
			//-----------------------------------------------------------------------------
		    
			//yield ();
			/*
			 *	This happens if something happened on any wiimote.
			 *	So go through each one and check if anything happened.
			 */
			
			for (int i = 0; i < WII_MAX_WIIMOTES; ++i)
			{
                
                const ScopedLock myLock (wiiCriticalSection);
				wiimote* wm = wiimotes[i];
                
				switch (wm->event)
				{
					case WIIUSE_NONE:
						break;
					case WIIUSE_EVENT:
						//printf("WIIUSE_EVENT\n");
						// a generic event occured
						//wiiBroadcaster[i].sendActionMessage (WII_EVT_UPDATE);
                        receiveTimestamp = Time::currentTimeMillis();
						break;
						
					case WIIUSE_STATUS:
						// a status event occured
						//					onWiiCtrlStatus(wiimotes[i]);
						break;
						
					case WIIUSE_DISCONNECT:
					case WIIUSE_UNEXPECTED_DISCONNECT:
						wiiBroadcaster[i].sendActionMessage (WII_EVT_DISCONNECT);
						break;
						
					case WIIUSE_READ_DATA:
						
						/*
						 *	Data we requested to read was returned.
						 *	Take a look at wiimotes[i]->read_req
						 *	for the data.
						 */
						break;
						
					case WIIUSE_NUNCHUK_INSERTED:
						wiiBroadcaster[i].sendActionMessage (WII_EVT_NUNCHUK_ACTIVATED);
						/*
						 *	a nunchuk was inserted
						 *	This is a good place to set any nunchuk specific
						 *	threshold values.  By default they are the same
						 *	as the wiimote.
						 */
						printf("Nunchuk inserted.\n");
						break;
						
					case WIIUSE_CLASSIC_CTRL_INSERTED:
						printf("Classic controller inserted.\n");
						break;
						
					case WIIUSE_GUITAR_HERO_3_CTRL_INSERTED:
						// some expansion was inserted
						//					onWiiCtrlStatus(wiimotes[i]);
						break;
						
					case WIIUSE_MOTION_PLUS_ACTIVATED:
						wiiBroadcaster[i].sendActionMessage (WII_EVT_MOTION_PLUS_ACTIVATED);
						printf("Motion Plus inserted.\n");
						break;
						
					case WIIUSE_WII_BOARD_CTRL_INSERTED:
						printf("Balance Board connected.\n");
						break;
						
					case WIIUSE_WII_BOARD_CTRL_REMOVED:
						printf("Balance Board disconnected.\n");
						break;
						
					case WIIUSE_NUNCHUK_REMOVED:
						wiiBroadcaster[i].sendActionMessage (WII_EVT_NUNCHUK_DEACTIVATED);
						printf("An expansion was removed.\n");
						break;
					case WIIUSE_CLASSIC_CTRL_REMOVED:
					case WIIUSE_GUITAR_HERO_3_CTRL_REMOVED:
						printf("An expansion was removed.\n");
						break;
					case WIIUSE_MOTION_PLUS_REMOVED:
						wiiBroadcaster[i].sendActionMessage (WII_EVT_MOTION_PLUS_DEACTIVATED);
						printf("An expansion was removed.\n");
						break;
						
					default:
						break;
				}
			}
            
		}
	}
	//wiiuse_cleanup(wiimotes, WII_MAX_WIIMOTES);
}
