//-----------------------------------------------------------------------------
//@file  
//	DeviceMidi.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the DeviceMidi class.
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

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define PARAMS_BEFORE_MIDI_INS 3 
#define PARAMS_BEFORE_MIDI_OUTS 0 

// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_DM_01[] = { 'D', 'M', '0', '1' };
static char const CHUNCK_DM_02[] = { 'D', 'M', '0', '2' };

static const String MIDI_LAN	= "MIDI lan & bridge";

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// the audio device module class
class DeviceMidi : public UserModuleBase, MidiInputCallback
{
    //-------------------------------------------------------------------------
	// module constructor/destructor
	//-------------------------------------------------------------------------
public:
    // constructor
	DeviceMidi();

    // destructor
	virtual ~DeviceMidi();


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


	//-----------------------------------------------------------------------------
	// midi out callbacks
	void onMidiSendOut (int DeviceID, UsineMidiCode Code);
    void onMidiSendOutArray (int DeviceID, UsineMidiCode** arrayCode, int arraySize);
	void onMidiSysexSendOut (int DeviceID, char** Sysex, int sysexSize);

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
	UsineEventPtr m_ledDeviceAutoSetup;

	UsineEventPtr m_btnRescanAllMidiDevices;
	UsineEventPtr m_btnResetAllMidiDevices;

	Array <UsineEventPtr> m_ledMidiDeviceInputNames;
	Array <UsineEventPtr> m_ledMidiDeviceOutputNames;

	//-------------------------------------------------------------------------
	AudioDeviceManager deviceManager;

	bool autoSetup;

    int maxMidiInputs;
    int maxMidiOutputs;

	// midi inputs and outputs
    StringArray listMidiInputDevicesNames;
	String qsMidiInputsDevicesNames;
	UsineMidiCode midiCallbackDataBuffer;

    StringArray listMidiOutputDevicesNames;
	String qsMidiOutputsDevicesNames;

	// inputs outputs midi captions
	OwnedArray <MidiInput>	arrayMidiInputDevices;
	Array <const char*>	caMidiInputsCaptions;
	Array <String>		captionMidiInputsSavedName;
	Array <const char*>	caMidiInputsSavedName;

	OwnedArray <MidiOutput>	arrayMidiOutputDevices;
	Array <const char*>	caMidiOutputsCaptions;
	Array <String>		captionMidiOutputsSavedName;
	Array <const char*>	caMidiOutputsSavedName;

	MidiMessage midiOutCode;
    
    // for onMidiSendOutArray
    MidiMessage midiOutArrayCode;
	MidiBuffer midiBufferSendOutArray;
	
	String  captionErrorInvalidMidiOut;
	AnsiCharPtr   caErrorInvalidMidiOut;
	
	// for the chunk
	int numOfChunkMidiInputs;
	int sizeChunkMidiInputChoiceNames; //size in byte
	String qsChunkMidiInputChoiceNames;
	const char* caChunkMidiInputChoiceNames;
	StringArray listMidiInputWantedNames;

	int numOfChunkMidiOutputs;
	int sizeChunkMidiOutputChoiceNames; //size in byte
	String qsChunkMidiOutputChoiceNames;
	const char* caChunkMidiOutputChoiceNames;
	StringArray listMidiOutputWantedNames;
    
    // strings for setup properties tab separators 
    std::string strSepDevices;
    std::string strSepInputs;
    std::string strSepOutputs;

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:
	//-------------------------------------------------------------------------
	// input message
	void handleIncomingMidiMessage (juce::MidiInput *source, const MidiMessage &message);
    
	// input sysex
	void handlePartialSysexMessage (juce::MidiInput *source, const uint8 *messageData, int numBytesSoFar, double timestamp);
    
	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	//
	void initDeviceMidiModule();

	char* getMidiInputList ();

	//
	char* getMidiOutputList ();
    //
	char* getMidiInputCaption (int index);
	
	char* getMidiInputsSavedName (int index);

    //
	char* getMidiOutputCaption (int index);

	char* getMidiOutputsSavedName (int index);
	//
	void rescanMidi();

	//
	void resetMidi();

    //
	void setInputMidiDevice (int numInput);
		
	//
	void setOutputMidiDevice (int numOutput);

    //
    juce::MidiMessage constructJuceMidiMessage (UsineMidiCode Code);


	//-------------------------------------------------------------------------
	// settings
	void tryRestoreWantedSettings(bool recursive);

	void popupRestoreWantedSettings();

	void collectMidiInputsDeviceNames();
	
	void collectMidiOutputsDeviceNames();

	JUCE_LEAK_DETECTOR (DeviceMidi);

}; // class DeviceMidi



#endif //__DEVICE_MIDI_H__
