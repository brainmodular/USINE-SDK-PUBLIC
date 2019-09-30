//-----------------------------------------------------------------------------
//@file  
//	PluginWrapperExe.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the PluginLister class.
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
#ifndef __PLUGIN_WRAPPER_EXE_H__
#define __PLUGIN_WRAPPER_EXE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <JuceHeader.h>
#include "JuceUtils.h"
#include "JuceUdpServer.h"
#include "JucePluginWindow.h"

//-----------------------------------------------------------------------------
// watchdog for juce shutdown
//static Array<void*> activePlugins;

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define PLUG_INS_PATH String("/Plug-Ins")
#define PLUG_INS_EXT String(".plugin")
#define CORRPUT_PLUG_INS_FILE String("/CorruptedPlugins.init")
#define KNOWN_PLUG_INS_FILE String("/KnownPlugins.init")
#define ERROR_PLUG_INS_FILE String("/ErrorPlugins.init")
#define CRC_FILE String("CrcPlugins.init")

static String const PARAM_VALUE_FORMAT("0%");
static const double PARAM_CHANGE_TIME_MS = 200.0;


#define NUM_OF_USER_FOLDERS 4 
// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_PE01[] = { 'P', 'E', '0', '1' };

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
the initialisation and shutdown of the whole application.
*/
class PluginWrapperExe 
	: public JUCEApplication
	, public AudioProcessorListener
	, private MessageListener
{
public:
	//==============================================================================
	PluginWrapperExe();

	//==============================================================================
	//
	void initialise(const String& commandLine) override;


	void shutdown() override
	{
		// This method is where you should clear-up your app's resources..

	}

	//==============================================================================
	const String getApplicationName() override
	{
		return "PluginWrapperExe";
	}

	const String getApplicationVersion() override
	{
		// The ProjectInfo::versionString value is automatically updated by the Jucer, and
		// can be found in the JuceHeader.h file that it generates for our project.
		return ProjectInfo::versionString;
	}

	bool moreThanOneInstanceAllowed() override
	{
		return true;
	}

	void anotherInstanceStarted(const String& commandLine) override
	{
	}



public:
	//-----------------------------------------------------------------------------
	void parseProcess(MemoryBlock* dataBlock, int dataIndex);
	void parseCommands(MemoryBlock* dataBlock, int dataIndex);

	void process(TCommandPacket* commandBlock, MemoryBlock* dataBlock, int dataIndex);


	void testFakePlugin();

public:
	//-----------------------------------------------------------------------------
	// 
	String getPluginName() { return pluginName; };
	void setVisible(LongBool isVisible = TRUE);

	int lastEditorPosX;
	int lastEditorPosY;

private:

	//-----------------------------------------------------------------------------
	String pathPluginFile;

	// ports for udp servers
	int portCommandUsine;
	int portCommandWrapper;

	int portAudioUsine;
	int portAudioWrapper;

	// udp servers
	ScopedPointer <JuceUdpServer> socketAudio;
	ScopedPointer <JuceUdpServer> socketCommand;


	//-----------------------------------------------------------------------------
	// 
	ScopedPointer <UsineLook> usineLook;
	String pluginName;
	ScopedPointer <AudioPluginFormatManager> pluginFormatManager;
	ScopedPointer <AudioPluginInstance> pluginInstance;
	PluginDescription pluginDescription;
	ScopedPointer <PluginWindow> pluginWindow;


	int currentSampleRate;
	int currentUsineBlockSize;

	// audio in out
	int numOfAudioIn;
	//int offsetAudioIn;
	StringArray listAudioInNames;

	int numOfAudioOut;
	//int offsetAudioOut;
	StringArray listAudioOutNames;
	// used by the processBlock function
	ScopedPointer <AudioSampleBuffer> bufferAudioInOut;

	int processCounter;


	// midi in
	int numOfMidiIn;
	int offsetMidiIn;
	String midiInName;
	bool isProducingMidi;

	// midi out variable used in doProcess
	UsineMidiCode usineMidiOutTmp;
	MidiMessage juceMidiOutTmp;
	ScopedPointer <MidiBuffer::Iterator> itorMidiOutMessages;
	// used by the processBlock function
	ScopedPointer <MidiBuffer> bufferMidiMessages;
	ScopedPointer <MidiMessageCollector> collectorMidiMessages;

	int numOfMidiOut;
	int offsetMidiOut;
	String midiOutName;

	// programs
	int currentProg;
	int offsetProgIn;
	String progInName;
	StringArray listProgNames;
	String qsProgInName;

	// params
	int numOfParamsInOut;

	String paramTmpValueText;

	String qsParamNamesOut;

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

	// used to detect when the user begin a gesture
	double timeLastParameterChanged;

	// used for workaround the 'change preset by using change param' 
	int lastParamChangeIndex;

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	//
	void preparePlugin();
	//
	void initBuffers();

	//populate the programs name list
	void populateProgramsNameList();
	//populate the params name list
	void populateParamsNameList();
	void updateModuleParamsName();
	void setParamDisplayValue(int index, TPrecision newValue);
	void updateParams();
	void receiveChunck();
	void sendChunck();

	void createPlugin(String plugFilePath);
	//void createPlugin(String optionalString, LongBool Flag, String optionalContent);
	AudioPluginInstance* createPluginInstanceFromDesc(PluginDescription& plugDesc);
	//PluginWindow* getPluginWindow();

	void sendPlugInfosToUsine();
	// convert usine midi event in juce message and add them to bufferMidiMessages
	//void usineEventsToMidiBuffer(UsineEventPtr eventSource);
	//-------------------------------------------------------------------------
	// the plug isn't at the location saved in the patch/wrokspace
	// try to locate it in default location
	//void tryToLocatePlug();

	//PluginDescription* getDescriptionFromIdentifierString(const KnownPluginList& knownPluginList, const String& identifierString, const String& plugFormatName) const;
	//-----------------------------------------------------------------------------
	//
	void handleMessage(const Message& message) override;

	//-------------------------------------------------------------------------
	// from AudioProcessorListener interface
	void audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue);
	// from AudioProcessorListener interface
	void audioProcessorChanged(AudioProcessor *processor);

	//-----------------------------------------------------------------------------
	// 
	void logCommands(MemoryBlock* dataBlock, int dataIndex);
	StringArray listPacketCmd;
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginWrapperExe);

}; // class PluginWrapperExe

#endif //__PLUGIN_WRAPPER_EXE_H__