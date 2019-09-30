//-----------------------------------------------------------------------------
//@file  
//	PluginWrapper.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the PluginWrapper class.
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
#ifndef __PLUGIN_WRAPPER_H__
#define __PLUGIN_WRAPPER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <algorithm>
#include "JuceHeader.h"
#include "JuceUtils.h"
#include "JucePluginWindow.h"
#include <iostream>
#include <cmath>
#include <limits>

//-----------------------------------------------------------------------------
/*
 watchdog for juce shutdown
 each plug instance share the same juce instance, so we have to check the 
 number of plug instance before closing juce
 */
static Array<void*> activePlugins;
//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define PLUG_INS_PATH String("Plug-Ins")
#define PLUG_INS_EXT String(".plugin")
#define KNOWN_PLUG_INS_FILE String("/KnownPlugins.init")

// chunk header code to validate the module version how saved chunk of wkp/pat/presets
static char const CHUNCK_PG01[] = { 'P', 'G', '0', '1' };

static int const PLUG_MAX_PARAMS = 4096;
static int const PLUG_MAX_PARAMS_VISIBLE = 256;
static const double PARAM_CHANGE_TIME_MS = 200.0;

static const NativeInt MSG_ASK_FOR_SHUTDOWN = 0xFAB101;


//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class PluginWrapper : public UserModuleBase, public AudioProcessorListener, public juce::KeyListener// , public ActionBroadcaster
{
	friend class PluginWindow;

    //-------------------------------------------------------------------------
	// module constructor/destructor
	//-------------------------------------------------------------------------
public:
    // constructor
	PluginWrapper(AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent);

    // destructor
	virtual~PluginWrapper();

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
	void onProcess ();

	//-----------------------------------------------------------------------------
	// midi out callbacks
	//void onMidiSendOut (int DeviceID, UsineMidiCode Code);
	//void onMidiSysexSendOut (int DeviceID, char* Sysex);

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	//void onCreateSettings();
	//void onSettingsHasChanged();
    //void onCreateCommands();
	//void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	//void onMouseMove(TShiftState Shift, float X, float Y);
	//void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	void onOpenEditor();
	void onBringToFront();
	void onCloseEditor();
	void updateName();
	void sendOnTop();
	void sendToBack();

	//-----------------------------------------------------------------------------
	// audio setup update
	void onBlocSizeChange (int BlocSize);
	void onSampleRateChange (double SampleRate);
	
	//-----------------------------------------------------------------------------
	// recording 
	// void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);

	//-------------------------------------------------------------------------
	// public methodes 
	//-------------------------------------------------------------------------
public:
    String getPluginName () {return pluginName;};
    String getPluginManufacturer ();
    String getPluginFormat ();
    void deleteEditor ();

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	int lastEditorPosX;
	int lastEditorPosY;

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr m_btnLoadBank;
	UsineEventPtr m_btnSaveBank;
	UsineEventPtr m_btnLoadPreset;
	UsineEventPtr m_btnSavePreset;
	UsineEventPtr m_btnPrevPreset;
	UsineEventPtr m_btnNextPreset;
	UsineEventPtr m_btnShowAllParams;
	UsineEventPtr m_btnHideAllParams;


	UsineEventPtr m_showEditor; 
	UsineEventPtr m_visibleEditor;
	UsineEventPtr m_bypassPlug; 

	Array <UsineEventPtr> m_audioIns; 
	Array <UsineEventPtr> m_audioOuts;
	
	UsineEventPtr m_midiIn; 
	UsineEventPtr m_midiOut;
	
	UsineEventPtr m_lboxProgramIn;
	UsineEventPtr m_dtfLatencyOut;
	
	UsineEventPtr m_arrParamsInOut; 
	UsineEventPtr m_txtParamsNamesOut;
	UsineEventPtr m_txtCurrProgName;

	Array <UsineEventPtr> m_fdrParamsInsOuts; 
		
	UsineEventPtr m_lledKeysToUsine;

	int currentSampleRate;
	int currentUsineBlockSize;

	ScopedPointer <UsineLook> usineLook;
	ScopedPointer <AudioPluginFormatManager> pluginFormatManager;
    ScopedPointer <AudioPluginInstance> pluginInstance;
    //AudioPluginInstance* pluginInstance;
	PluginDescription pluginDescription;
	String pluginName;
	const char* caStringToUTF8Buffer;

	// for graphic stuff
	PluginWindow* pluginWindow;

	// UI ins
	int offsetVisibleIn;
	int offsetBypassIn;
	int offsetLoadBank;
	int offsetSaveBank;
	int offsetLoadPreset;
	int offsetSavePreset;
	int offsetPrevPreset;
	int offsetNextPreset;
	int offsetShowAllParams;
	int offsetHideAllParams;

	// audio in out
	int numOfAudioIn;
	int offsetAudioIn;
	StringArray listAudioInNames;

	int numOfAudioOut;
	int offsetAudioOut;
	StringArray listAudioOutNames;
	// used by the processBlock function
    ScopedPointer <AudioSampleBuffer> bufferAudioInOut;
    //AudioSampleBuffer* bufferAudioInOut;

	// midi in out
	int numOfMidiIn;
	int offsetMidiIn;
	String midiInName;
	bool isProducingMidi;
	
	// midi out variable used in doProcess
	UsineMidiCode usineMidiOutTmp;	
	MidiMessage juceMidiOutTmp;
    ScopedPointer <MidiBuffer::Iterator> itorMidiOutMessages;
    //MidiBuffer::Iterator* itorMidiOutMessages;
	// used by the processBlock function
	ScopedPointer <MidiBuffer> bufferMidiMessages;
    ScopedPointer <MidiMessageCollector> collectorMidiMessages;
    //MidiBuffer* bufferMidiMessages;
    //MidiMessageCollector* collectorMidiMessages;

	int numOfMidiOut;
	int offsetMidiOut;
	String midiOutName;

	// programs
	int offsetProgIn;
	String progInName;
	StringArray listProgInNames;
	String qsProgInName;

	// latency
	int offsetLatencyOut;
	String latencyOutName;
	
	// params
	int offsetParamsArrayInOut;
	String paramArrayInOutName;
	String paramTmpValueText;

	// Curr Prog Name
	int offsetCurrProgName;
	String currProgNameName;
	String qsCurrProgName;

	int offsetParamsNamesOut;
	String paramNamesOutName;
	String qsParamNamesOut;
	Array <int> listVisiblePlugParamsIndex;

	int numOfParamsInOut;
	int offsetParamsInOut;

	StringArray listParamInOutNames;
	Array <int> listParamInOutIndex;
	
	// used by chunk functions for the current program data
	MemoryBlock currentPlugStateInfo;
	MemoryBlock totalChunk;

	bool isBypass;
	bool isInstrument;
    
    String lastError;
	
	bool isPlugMissing;
	String missingPlugDesc;
    String pluginsBasePath;
	File filePluginsDirectory;

	// synchro stuff
	ScopedPointer <UsineAudioPlayHead> plugAudioPlayHead;
	
    // for keyboard infos
    int kboardLastKeyPress;
    int kboardLastKeyModifiers;

    // used to detect when the user begin a gesture
    double timeLastParameterChanged;

    // used for workaround the 'change preset by using change param' 
    int lastParamChangeIndex;

	//-------------------------------------------------------------------------
	// protected methodes
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	void showModuleParam (int paramIndex);
	void hideModuleParam (int paramIndex);
	void showAllModuleParams ();
	void hideAllModuleParams ();
	void updateModuleParams ();

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:

	//
	void preparePlugin();
	//
	void initBuffers ();
	void setVisible(LongBool isVisible = TRUE);
    
	//populate the programs name list
    void populateProgramsNameList ();
	//populate the params name list
	void populateParamsNameList ();
	void updateModuleParamsName ();
    void setModuleParamValue (int index, TPrecision newValue);
	void createPlugin (String optionalString, LongBool Flag, String optionalContent);
	AudioPluginInstance* createPluginInstanceFromDesc (PluginDescription& plugDesc);
	PluginWindow* getPluginWindow();

	// convert usine midi event in juce message and add them to bufferMidiMessages
	void usineEventsToMidiBuffer(UsineEventPtr eventSource);
	//-------------------------------------------------------------------------
	// the plug isn't at the location saved in the patch/wrokspace
	// try to locate it in default location
	void tryToLocatePlug();

    PluginDescription* getDescriptionFromIdentifierString (const KnownPluginList& knownPluginList, const String& identifierString, const String& plugFormatName) const;
	//-------------------------------------------------------------------------
	// from AudioProcessorListener interface
	void audioProcessorParameterChanged (AudioProcessor *processor, int parameterIndex, float newValue);
	// from AudioProcessorListener interface
	void audioProcessorChanged (AudioProcessor *processor);
    
	// reimpl from KeyListener
    // Called to indicate that a key has been pressed.
    bool keyPressed (const KeyPress &key, Component *originatingComponent);
 
	// reimpl from KeyListener
    // Called when any key is pressed or released. 
    bool keyStateChanged (bool isKeyDown, Component *originatingComponent);
	
}; // class PluginWrapper

#endif //__PLUGIN_WRAPPER_H__