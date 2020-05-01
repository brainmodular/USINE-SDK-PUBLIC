//-----------------------------------------------------------------------------
//@file  
//	DeviceAudio.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the DeviceAudio class.
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
#ifndef __DEVICE_AUDIO_H__
#define __DEVICE_AUDIO_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <JuceHeader.h>
//#include "JuceUtils.h"


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

/*
// MAC (core audio) let choice for device input and device output
// while PC (ASIO) have only one type in/out
*/
/*
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
#define PARAMS_BEFORE_INS_OUTS 10
//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
//----------------------------------------------------------------------------
#define PARAMS_BEFORE_INS_OUTS 10
//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------
*/

// #define JUCE_ASIO_DEBUGGING 1

#define PARAMS_BEFORE_INS_OUTS			10 
#define DRIVER_NAME_NO_AUDIO			"NO AUDIO"
#define DRIVER_NAME_ASIO				"ASIO"
#define DRIVER_NAME_WINDOWS_AUDIO		"Windows Audio"
#define DRIVER_NAME_WINDOWS_AUDIO_EXCL	"Windows Audio (Exclusive Mode)"
#define DRIVER_NAME_DIRECT_SOUND		"DirectSound"
#define DRIVER_NAME_CORE_AUDIO			"CoreAudio"

// chunk header code to validate the module version how saved chunk of wkp/pat/presets
//static char const CHUNCK_DA01[] = { 'D', 'A', '1', '4' };
//static char const CHUNCK_DA02[] = { 'D', 'A', '2', '0' };
//static char const CHUNCK_DA21[] = { 'D', 'A', '2', '1' };
//static char const CHUNCK_DA22[] = { 'D', 'A', '2', '2' };
static char const CHUNCK_DA23[] = { 'D', 'A', '2', '3' };


static const NativeInt MSG_CHANGE_DRIVERS_REQUEST = 0xFAB100;

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// the audio device module class
class DeviceAudio : public UserModuleBase, Thread, AudioIODeviceCallback
{
	//-------------------------------------------------------------------------
	// public members
	//-------------------------------------------------------------------------
public:

	//-------------------------------------------------------------------------
	// module constructor/destructor
	//-------------------------------------------------------------------------
public:
	// constructor
	DeviceAudio();

	// destructor
	virtual ~DeviceAudio();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);
	void onInitModule(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress(int ParamIndex, UsineEventPtr pEvent);
	void onCallBack(UsineMessage *Message);

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen(LongBool Preset);
	void onGetChunk(void* chunk, LongBool Preset);
	void onSetChunk(const void* chunk, int sizeInBytes, LongBool Preset);

	// from juce::Logger
	void logMessage(const String& message)
	{
		std::ostringstream os;
		os << "[" << m_moduleInfo->Name << "]" << " " << message;
		traceString = os.str();
		sdkTraceLogChar(traceString.toUTF8().getAddress());
	};

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events

	UsineEventPtr m_ledDeviceAutoSetup;
	UsineEventPtr m_lboxDeviceDrivers;
	UsineEventPtr m_lboxDeviceIns;
	UsineEventPtr m_lboxDeviceOuts;

	UsineEventPtr m_dtfNbAudioIn;
	UsineEventPtr m_dtfNbAudioOut;
	UsineEventPtr m_lboxSampleRate;

	//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	//-----------------------------------------------------------------------------
	UsineEventPtr m_dtfBufferSize;
	//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	//-----------------------------------------------------------------------------
	UsineEventPtr m_lboxBufferSize;
	//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
	//-----------------------------------------------------------------------------	

	UsineEventPtr m_lboxUsineNotUsed;
	UsineEventPtr m_btnShowDriverPanel;

	// ins and outs
	Array <UsineEventPtr> m_ledAudioDeviceInputNames;
	Array <UsineEventPtr> m_ledAudioDeviceOutputNames;

	// setup delimiter
	UsineEventPtr m_ledSetupLoaded;

	int lboxLastSampleRateIndex;
	int lboxLastBufferSizeIndex;
	//-------------------------------------------------------------------------
	juce::AudioDeviceManager deviceManager;

	//CriticalSection lock;

	// device drivers
	int indexSelectedDeviceDriver;
	int oldIndexSelectedDeviceDriver;
	StringArray listDeviceDriversNames;
	String qsDeviceDriversNames;

	bool initDevices;
	bool setupLoaded;
	//bool autoSetup;

	// device inputs
	StringArray listDeviceInsNames;
	String qsDeviceInsNames;
	String captionDeviceInsName;

	// device outputs
	StringArray listDeviceOutsNames;
	String qsDeviceOutsNames;

	juce::AudioDeviceManager::AudioDeviceSetup currentAudioDeviceSetup;

	// audio buffers
	//ScopedPointer <AudioSampleBufferFifo> audioInputsSampleFifoBuffer;
	//ScopedPointer <juce::AudioSampleBuffer> audioInputsFinalBuffer;

	//ScopedPointer <AudioSampleBufferFifo> audioOutputsSampleFifoBuffer;
	//ScopedPointer <juce::AudioSampleBuffer> audioOutputsFinalBuffer;

	// Samplerate
	StringArray listSampleRates;
	String qsSampleRates;
	const char* caSamplingRates;

	// Buffersize
	StringArray listBufferSizes;
	String qsBufferSizes;
	const char* caBufferSizes;
	int deviceBufferSize;

	// audio inouts and outputs
	String qsAudioInputNames;
	String qsAudioOutNames;

	int maxAudioInputs;
	int maxAudioOutputs;

	// inouts outputs switch captions
	Array <String>		captionEnableInputs;
	Array <const char*>	caAudioInputsCaptions;
	Array <String>		captionAudioInputsSavedName;
	Array <const char*>	caAudioInputsSavedName;

	Array <String>		captionEnableOutputs;
	Array <const char*>	caAudioOutputsCaptions;
	Array <String>		captionAudioOutputsSavedName;
	Array <const char*>	caAudioOutputsSavedName;

	// we must have at minimum one in and one out active
	int numOfInputs;
	int numOfOutputs;

	// used in aboutToStart when realocating buffers
	//ReadWriteLock buffersLock;

	String lastErrorMessage;

	// strings for setup properties tab separators 
	std::string strSepDevices;
	std::string strSepUsine;
	std::string strSepInputs;
	std::string strSepOutputs;

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	CriticalSection writerLock;
	//-------------------------------------------------------------------------
	// drivers and devices
	AnsiCharPtr getQuotedDeviceDriversNames();
	AnsiCharPtr getQuotedDeviceInsNames();
	AnsiCharPtr getQuotedDeviceOutsNames();

	int selectFirstDeviceIn(String deviceToExclude);
	int selectFirstDeviceOut(String deviceToExclude);

	//-------------------------------------------------------------------------
	// inputs and outputs
	char* getAudioInputCaption(int index);
	char* getAudioInputSavedName(int index);
	char* getAudioOutputCaption(int index);
	char* getAudioOutputSavedName(int index);

	//-------------------------------------------------------------------------
	char* getAudioSampleRatesList();

	//
	char* getBufferSizesList();


	//-------------------------------------------------------------------------
	// audio inputs 
	int getNumOfActiveInputs();
//	void activeAtLeastOneInput();
	BigInteger getStateInputs();
	void switchAudioInput(int numInput, bool updateDriver = true);

	//-------------------------------------------------------------------------
	// audio outputs 
	int getNumOfActiveOutputs();
	BigInteger getStateOutputs();
	void switchAudioOutput(int numOutput, bool updateDriver = true);

	//
	void showDriverPanel();

	// audio devices
	void applyDevice(int deriverID);
	void checkAudioDrivers();
	void updateParameters();
	void clearParameters();
	AudioDeviceManager::AudioDeviceSetup getAudioSetupFromUser();
	int getBufferSize();
	double getSampleRate();
	String getDeviceNamesInputs();
	String getDeviceNamesOutputs();
	void traceAudioSetup(AudioDeviceManager::AudioDeviceSetup audioSetup);
	String doChangeDriverSettings(AudioDeviceManager::AudioDeviceSetup audioSetup);

	void updateDevicesDriversCaptions();
	void updateInsOutsCaptions();
	void updateParamsVisibility();

	//
	void closeDevice();

	//
	bool selectSampleRate(double sampleRate);
	//
	void changeSampleRate();

	//-----------------------------------------------------------------------------
#if (defined (USINE_OSX32) || defined (USINE_OSX64))
	//-----------------------------------------------------------------------------   
	//
	bool selectBufferSize(double bufferSize);

	//-----------------------------------------------------------------------------
#endif
	//-----------------------------------------------------------------------------
	//
	void changeBufferSize();

	//
	void notifyChangeDriverSettings();

	//
	void notifyChangeSampleRate();


	//----------------------------------------------------------------------------
	// from Thread
	void run();

	//----------------------------------------------------------------------------
	// from ChangeBroadcaster, to be informed when something change at device level
	void changeListenerCallback(ChangeBroadcaster *source);

	//----------------------------------------------------------------------------
	// from juce::AudioIODeviceCallback
	void audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples);
	void audioDeviceAboutToStart(AudioIODevice *device);
	void audioDeviceStopped();
	void audioDeviceError(const String& errorMessage);

	//------------------------------------------------------------------------
	// trace helpers
	String traceString;
	void traceLog(AnsiCharPtr traceMsg);
	void traceLog(AnsiCharPtr traceMsg, int value);
	void traceLog(AnsiCharPtr traceMsg, String value);

//private:
	//JUCE_LEAK_DETECTOR(DeviceAudio);

}; // class DeviceAudio



#endif //__DEVICE_AUDIO_H__
