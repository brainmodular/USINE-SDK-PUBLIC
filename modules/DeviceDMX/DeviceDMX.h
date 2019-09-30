//-----------------------------------------------------------------------------
//@file  
//	DeviceDMX.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the DeviceDMX class.
//
//  module to act as dmx output for the devices categorie.
//
//HISTORIC 
//	2014/12/11
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
#ifndef __DEVICE_MIDI_H__
#define __DEVICE_MIDI_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <limits>
#include <JuceHeader.h>
#include <ftd2xx.h>
#include "EnttecProDriver.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define PARAMS_BEFORE_DMX_OUTS 2 

// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_DX_01[] = { 'D', 'X', '0', '1' };

static const int PRO_R_TIMEOUT = 120;
static const int PRO_W_TIMEOUT = 100;

static const String PRO_NAME_PREFIX = "Enttec USB Pro";
static const String OPEN_DMX_NAME_PREFIX = "Enttec Open DMX";
static const String PRO_MK2_NAME_PREFIX = "Enttec USB Pro MK2";
static const int DMX_UNIVERSE_SIZE = 512;
static const int DMX_START_CODE_SIZE = 1;
static const uint8 DMX_PACKET_START_CODE = 0;
static const int OPEN_DMX_TIMER_SLICE_TIME = 20;

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// the audio device module class
class DeviceDMX : public UserModuleBase, public HighResolutionTimer
{
    //-------------------------------------------------------------------------
	// module constructor/destructor
	//-------------------------------------------------------------------------
public:
    // constructor
	DeviceDMX();

    // destructor
	virtual ~DeviceDMX();


	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	//int  onGetNumberOfParams( int queryIndex);
	//void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	//void onProcess ();

	//-----------------------------------------------------------------------------
	// dmx out callbacks
	void onDMXSendOut (int deviceId, char* ByteArray, int len, int universeNum);

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr m_btnRescanAllDevices;
	UsineEventPtr m_btnResetAllDevices;

	Array <UsineEventPtr> m_ledDmxDeviceNames;

	//-------------------------------------------------------------------------
	AudioDeviceManager deviceManager;
	
    int maxDmxOutputs;

	// dmx outputs

    StringArray listDmxOutputDevicesNames;
	String qsDmxOutputsDevicesNames;

	// outputs dmx captions
	Array <String>		captionDmxOutputsCaptions;
	Array <const char*>	caDmxOutputsCaptions;
	Array <String>		captionDmxOutputsSavedName;
	Array <const char*>	caDmxOutputsSavedName;

    
    // for onDMXSendOut
    OwnedArray <MemoryBlock> dataBuffers;
	
	String          captionLogMessage;
	AnsiCharPtr    caLogMessage;

    // DMX devices stuff
    enum DeviceType
    {
        dtUnknown,
        dtEnttecOpenDMX,
        dtEnttecDMXProUSB,
        dtEnttecDMXProMK2
    };

    struct DeviceInfos
    {
        FT_HANDLE               handle;
        bool                    isActive;
        String                  vendor;
        String                  description;
        DeviceType              deviceType;
        String                  serial;
        String                  name;
        DMXUSBPROParamsType*    params;
        uint8                   latencyTimer;

    };

    OwnedArray <DeviceInfos> listDeviceInfos;

    FT_STATUS ftStatus;
    DWORD written;

	// for the chunk
	int numOfChunkDmxOutputs;
	int sizeChunkDmxOutputChoiceNames; //size in byte
	String qsChunkDmxOutputChoiceNames;
	const char* caChunkDmxOutputChoiceNames;
	StringArray listDmxOutputWantedNames;

        // strings for setup properties tab separators 
    std::string strSepDevices;
    std::string strSepOutputs;
    
	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	//
	void initDeviceOpenDMXModule();
    
	//
	void rescanDmxDevices();

    void openDmxDevices();

    void closeDmxDevice(int index);

    int identifyDmxDevice(int index);

    //void sendDMXPacket ();
	//
	void resetDmxDevices();

    //
	char* getDmxOutputCaption (int index);

	char* getDmxOutputsSavedName (int index);

	void setOutputDmxDeviceState (int index);

    void hiResTimerCallback();

	//
	CriticalSection bufferLock;
    
	//-------------------------------------------------------------------------
	// settings
	void tryRestoreWantedSettings(bool recursive);
	void popupRestoreWantedSettings();
    	
	//void collectDmxOutputsDeviceNames();

	JUCE_LEAK_DETECTOR (DeviceDMX);

}; // class DeviceDMX



//-------------------------------------------------------------------------
static void  StringArrayToCommaString (String& result, StringArray list, bool noneatfirst = true)
{
	StringArray tmpList(list);
	if (noneatfirst)
		tmpList.insert (0, "none");
		
	result = tmpList.joinIntoString("\",\"").quoted();
};

#endif //__DEVICE_MIDI_H__

