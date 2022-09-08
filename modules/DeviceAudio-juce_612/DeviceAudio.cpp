//-----------------------------------------------------------------------------
//@file  
//	DeviceAudio.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DeviceAudio class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "DeviceAudio.h"

//-------------------------------------------------------------------------
static void  StringArrayToCommaString (String& result, StringArray list/*, bool noneatfirst = true*/)
{

    result = list.joinIntoString("\",\"").quoted();
};


//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DeviceAudio();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete ((DeviceAudio*)pModule);
}


//-----------------------------------------------------------------------------
// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "audio";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "audio device user module";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = MODULE_USER_VERSION;

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructor/destructor
//------------------------------------------------------------------------
// constructor
DeviceAudio::DeviceAudio()
	: Thread("NoAudioThread")
	, initDevices(false)
	, setupLoaded(false)
	, lboxLastSampleRateIndex(0)
	, lboxLastBufferSizeIndex(-1)
	, deviceBufferSize(0)
	, indexSelectedDeviceDriver(1)
	, maxAudioInputs(0)
	, maxAudioOutputs(0)
	, numOfInputs(2)
	, numOfOutputs(2)
	, oldIndexSelectedDeviceDriver(-2)

{

	indexSelectedDeviceDriver = 1;
	deviceManager.addAudioCallback(this);

	// TODO : implement juce callback when a device change (plugged/unplugged)     
};

// destructor 
DeviceAudio::~DeviceAudio()
{
	try
	{
		deviceManager.removeAudioCallback(this);
		wait(200);
		deviceManager.closeAudioDevice();
		

	}
	catch (...)
	{
	}


	Logger::setCurrentLogger(nullptr);

	//stop the NoAudioThread in case we was in NO AUDIO mode
	if (isThreadRunning())
		stopThread(1000);
};


//-----------------------------------------------------------------------------
// module description
void DeviceAudio::onGetModuleInfo(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	if (pMasterInfo != nullptr)
	{
		maxAudioInputs = sdkGetUsineMaxAudioInputs();
		maxAudioOutputs = sdkGetUsineMaxAudioOutputs();
		pModuleInfo->BackColor = sdkGetUsineColor(clAudioModuleColor);
	}
	else
	{
		maxAudioInputs = 192;
		maxAudioOutputs = 192;
	}

	pModuleInfo->Name = MODULE_NAME;
	pModuleInfo->Description = MODULE_DESC;
	pModuleInfo->ModuleType = mtDeviceAudio;
	pModuleInfo->NumberOfParams = PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs + 1;
	pModuleInfo->DontProcess = TRUE;

	pModuleInfo->QueryString = "";
	pModuleInfo->QueryListValues = "";
	pModuleInfo->QueryDefaultIdx = 1;
	pModuleInfo->Version = MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;

	//Logger::setCurrentLogger(this);
}

//-----------------------------------------------------------------------------
// initialisation
void DeviceAudio::onInitModule(MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	indexSelectedDeviceDriver = listDeviceDriversNames.indexOf(DRIVER_NAME_WINDOWS_AUDIO);
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	indexSelectedDeviceDriver = listDeviceDriversNames.indexOf(DRIVER_NAME_CORE_AUDIO);
#endif			

	sdkSetEvtData(m_lboxDeviceDrivers, (TPrecision)indexSelectedDeviceDriver);

}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------
// Parameters description
void DeviceAudio::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
	// m_ledDeviceAutoSetup
	if (ParamIndex == 0)
	{
		strSepDevices = std::string(sdkGetTrans("devices"));

		pParamInfo->ParamType = ptLeftLed;
		pParamInfo->Caption = "auto setup";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->Translate = TRUE;
		pParamInfo->DefaultValue = 0;
		pParamInfo->IsInvisible = TRUE;
		pParamInfo->DontSave = TRUE;
		//		}

	}
	// m_lboxDeviceDrivers
	else if (ParamIndex == 1)
	{
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "drivers";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->Translate = TRUE;
		pParamInfo->ListBoxStrings = getQuotedDeviceDriversNames();
		pParamInfo->DefaultValue = (TPrecision)indexSelectedDeviceDriver;
		pParamInfo->MinValue = -1;
		pParamInfo->MaxValue = 100;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepDevices.c_str();
	}
	// m_lboxDeviceIns
	if (ParamIndex == 2)
	{
		// we need saved name because we change the caption for ASIO as it use the same device 
		// for in and out and we use only the in to show the devices names in this case
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "in";
		pParamInfo->SavedName = "in";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ListBoxStrings = getQuotedDeviceInsNames();
		pParamInfo->DefaultValue = (TPrecision)selectFirstDeviceIn("airplay");
		pParamInfo->MinValue = -1;
		pParamInfo->MaxValue = 100;
		pParamInfo->IsSeparator = TRUE;
	}
	// m_lboxDeviceOuts
	else if (ParamIndex == 3)
	{
		//"devices"
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "out";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ListBoxStrings = getQuotedDeviceOutsNames();
		pParamInfo->DefaultValue = (TPrecision)selectFirstDeviceOut("airplay");
		pParamInfo->MinValue = -1;
		pParamInfo->MaxValue = 100;
	}
	// m_dtfNbAudioIn
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 6))
	{
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "nb in";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 16;
		pParamInfo->DefaultValue = 0;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.0f";
		pParamInfo->ReadOnly = TRUE;
		pParamInfo->DontSave = TRUE;
		pParamInfo->Translate = TRUE;
		pParamInfo->IsSeparator = TRUE;
	}
	// m_dtfNbAudioOut
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 5))
	{
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "nb out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 16;
		pParamInfo->DefaultValue = 0;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.0f";
		pParamInfo->ReadOnly = TRUE;
		pParamInfo->DontSave = TRUE;
		pParamInfo->Translate = TRUE;
	}
	// m_lboxSampleRate
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 4))
	{
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "sample rate";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ListBoxStrings = getAudioSampleRatesList();
		pParamInfo->DefaultValue = (float)lboxLastSampleRateIndex;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 1000000;
		pParamInfo->DontSave = TRUE;
		pParamInfo->Translate = TRUE;
	}
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	//m_dtfBufferSize
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 3))
	{
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "buffer size";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 4096;
		pParamInfo->DefaultValue = 0;
		pParamInfo->Symbol = "";
		pParamInfo->Format = "%.0f";
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->DontSave = FALSE;
		pParamInfo->Translate = TRUE;
	}
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	//m_lboxBufferSize
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 3))
	{
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "buffer size";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ListBoxStrings = getBufferSizesList();
		pParamInfo->DefaultValue = 16.0f;
		pParamInfo->MinValue = 1;
		pParamInfo->MaxValue = 48000;
		pParamInfo->DontSave = TRUE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->Translate = TRUE;
	}
#endif

	//m_btnShowDriverPanel			 
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 2))
	{
		pParamInfo->ParamType = ptButton;
		pParamInfo->Caption = "show driver";
		pParamInfo->IsInput = TRUE;
		pParamInfo->DontSave = TRUE;
		pParamInfo->Translate = TRUE;
	}
	//m_lboxUsineNotUsed not used anymore			 
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 1))
	{

		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "not used";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->IsInvisible = FALSE;
		pParamInfo->MinValue = -1;
		pParamInfo->MaxValue = 1;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->DontSave = TRUE;
		pParamInfo->Translate = TRUE;
	}
	// m_ledAudioDeviceInputNames
	else if
		(ParamIndex >= PARAMS_BEFORE_INS_OUTS
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
			)
	{
		pParamInfo->ParamType = ptLeftLed;
		pParamInfo->Caption = "-";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->SavedName = getAudioInputSavedName(ParamIndex - PARAMS_BEFORE_INS_OUTS);
		pParamInfo->IsInvisible = TRUE;
		pParamInfo->DontSave = TRUE;
		if (ParamIndex == PARAMS_BEFORE_INS_OUTS)
		{
			strSepInputs = std::string(sdkGetTrans("inputs"));

			pParamInfo->DefaultValue = 1;  // default ON
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepInputs.c_str();
		}
		else if (ParamIndex == PARAMS_BEFORE_INS_OUTS + 1)
		{
			pParamInfo->DefaultValue = 1;  // default ON
		}
		else
		{
			pParamInfo->DefaultValue = 0;  // default OFF
		}
	}
	// m_ledAudioDeviceOutputNames
	else if
		(ParamIndex >= (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs)
			)
	{

		pParamInfo->ParamType = ptLeftLed;
		pParamInfo->Caption = "-";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->SavedName = getAudioOutputSavedName(ParamIndex - (PARAMS_BEFORE_INS_OUTS + maxAudioInputs));
		pParamInfo->IsInvisible = TRUE;
		pParamInfo->DontSave = TRUE;
		if (ParamIndex == PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
		{
			strSepOutputs = std::string(sdkGetTrans("outputs"));

			pParamInfo->DefaultValue = 1;  // default ON
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepOutputs.c_str();
		}
		else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + 1))
		{
			pParamInfo->DefaultValue = 1;  // default ON
		}
		else
		{
			pParamInfo->DefaultValue = 0;  // default OFF
		}
	}
	// m_ledSetupLoaded
	else if
		(ParamIndex >= (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs)
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs + 1)
			)
	{
		pParamInfo->ParamType = ptDataFader;
		pParamInfo->Caption = "setup loaded";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsInvisible = TRUE;
		pParamInfo->MinValue = -1;
		pParamInfo->MaxValue = 1.0;
		pParamInfo->DefaultValue = 1;
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->Translate = FALSE;
        pParamInfo->DontSave = FALSE;
		pParamInfo->CallBackId = 0xF135B;

	}
	// default case
	else {}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void DeviceAudio::onSetEventAddress(int ParamIndex, UsineEventPtr pEvent)
{

	// m_ledDeviceAutoSetup
	if (ParamIndex == 0)
	{
		m_ledDeviceAutoSetup = pEvent;
	}
	// m_lboxDeviceDrivers
	else if (ParamIndex == 1)
	{
		m_lboxDeviceDrivers = pEvent;
	}
	// m_lboxDeviceIns
	if (ParamIndex == 2)
	{
		m_lboxDeviceIns = pEvent;
	}
	// m_lboxDeviceOuts
	else if (ParamIndex == 3)
	{
		m_lboxDeviceOuts = pEvent;
	}
	// m_dtfNbAudioIn
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 6))
	{
		m_dtfNbAudioIn = pEvent;
	}
	// m_dtfNbAudioOut
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 5))
	{
		m_dtfNbAudioOut = pEvent;
	}
	// m_lboxSampleRate
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 4))
	{
		m_lboxSampleRate = pEvent;
	}
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	//m_dtfBufferSize
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 3))
	{
		m_dtfBufferSize = pEvent;
	}
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	//m_lboxBufferSize
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 3))
	{
		m_lboxBufferSize = pEvent;
	}
#endif

	//m_btnShowDriverPanel
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 2))
	{
		m_btnShowDriverPanel = pEvent;
	}
	//m_lboxUsineNotUsed
	else if (ParamIndex == (PARAMS_BEFORE_INS_OUTS - 1))
	{
		m_lboxUsineNotUsed = pEvent;
	}
	// m_ledAudioDeviceInputNames
	else if
		(ParamIndex >= PARAMS_BEFORE_INS_OUTS
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
			)
	{
		m_ledAudioDeviceInputNames.set(ParamIndex - PARAMS_BEFORE_INS_OUTS, pEvent);
	}
	// m_ledAudioDeviceOutputNames
	else if
		(ParamIndex >= (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs)
			)
	{
		m_ledAudioDeviceOutputNames.set(ParamIndex - (PARAMS_BEFORE_INS_OUTS + maxAudioInputs), pEvent);
	}
	// m_ledSetupLoaded
	else if
		(ParamIndex >= (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs)
			&& ParamIndex < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs + 1)
			)
	{
		m_ledSetupLoaded = pEvent;
	}
	// default case
	else {}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DeviceAudio::onCallBack(UsineMessage* Message)
{

	if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
	{

		if ((Message->wParam == MSG_CHANGE_DRIVERS_REQUEST) && (Message->lParam == MSG_CHANGE))
		{
			traceLog("CALLBACK MSG_CHANGE_DRIVERS_REQUEST received");

			AudioDeviceManager::AudioDeviceSetup audioSetup = getAudioSetupFromUser();
			checkAudioDrivers();
			doChangeDriverSettings(audioSetup);
		}
		// m_ledDeviceAutoSetup
		if ((Message->wParam == 0) && (Message->lParam == MSG_CHANGE))
		{

			//			autoSetup = false;

		}
		// m_lboxDeviceDrivers
		else if ((Message->wParam == 1) && (Message->lParam == MSG_CHANGE))
		{
			if (setupLoaded)
			{
				traceLog("CALLBACK m_lboxDeviceDrivers CHANGED received");
				indexSelectedDeviceDriver = (int)sdkGetEvtData(m_lboxDeviceDrivers);

				updateDevicesDriversCaptions();
				applyDevice(indexSelectedDeviceDriver);
			}

			updateParamsVisibility();

		}
		else if ((Message->wParam < (PARAMS_BEFORE_INS_OUTS - 4)) && (Message->lParam == MSG_CHANGE))
		{
			if (setupLoaded)
			{
				applyDevice(indexSelectedDeviceDriver);
				notifyChangeDriverSettings();
			}
		}

		// m_lboxSampleRate
		else if ((Message->wParam == (PARAMS_BEFORE_INS_OUTS - 4))
			&& (Message->lParam == MSG_CHANGE)
			)
		{
			if (setupLoaded == true)
			{
				traceLog("CALLBACK m_lboxSampleRate CHANGED received");
				changeSampleRate();
			}
		}
		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------
		// m_lboxBufferSize
		else if ((Message->wParam == (PARAMS_BEFORE_INS_OUTS - 3))
			&& (Message->lParam == MSG_CHANGE)
			)
		{
			if (setupLoaded == true)
			{
				traceLog("CALLBACK BufferSize CHANGED received");
				changeBufferSize();
			}
		}
		//-----------------------------------------------------------------------------
		//-----------------------------------------------------------------------------

		//m_btnShowDriverPanel
		else if ((Message->wParam == (PARAMS_BEFORE_INS_OUTS - 2))
			&& (Message->lParam == MSG_CHANGE)
			)
		{
			traceLog("CALLBACK m_btnShowDriverPanel CHANGED received");
			showDriverPanel();
		}
		// m_lboxUsineNotUsed
		else if ((Message->wParam == (PARAMS_BEFORE_INS_OUTS - 1))
			&& (Message->lParam == MSG_CHANGE)
			)
		{

		}
		// m_ledAudioDeviceInputNames
		else if
			(Message->wParam >= PARAMS_BEFORE_INS_OUTS
				&& Message->wParam < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
				&& (Message->lParam == MSG_CHANGE)
				)
		{
			traceLog("CALLBACK m_ledAudioDeviceInputNames CHANGED received");
			switchAudioInput((int)Message->wParam - PARAMS_BEFORE_INS_OUTS);
		}
		// m_ledAudioDeviceOutputNames
		else if
			(Message->wParam >= (PARAMS_BEFORE_INS_OUTS + maxAudioInputs)
				&& Message->wParam < (PARAMS_BEFORE_INS_OUTS + maxAudioInputs + maxAudioOutputs)
				&& (Message->lParam == MSG_CHANGE)
				)
		{
			traceLog("CALLBACK m_ledAudioDeviceOutputNames CHANGED received");
			switchAudioOutput((int)Message->wParam - (PARAMS_BEFORE_INS_OUTS + maxAudioInputs));
		}
		// m_ledSetupLoaded
		else if
			(Message->wParam == 0xF135B)
		{
            if (setupLoaded==false)
            {
                setupLoaded = true;
			    traceLog("CALLBACK m_ledSetupLoaded CHANGED received");
			    int driverID = static_cast<int>(sdkGetEvtData(m_lboxDeviceDrivers));
			    applyDevice(driverID);
			    updateParamsVisibility();			
            }
		}
		else
		{
			//unhandled callbacks
		}
	}
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// compute the lentgh of the chunk
int DeviceAudio::onGetChunkLen(LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;
	int charInt = sizeof(int);
	//int charFloat = sizeof(float);

	int ChunkLen = 0;


	ChunkLen = (charHeader					// the version header
		+ charInt					// driver choice
		+ charInt					// input device choice
		+ charInt 					// output device coice (not used when driver = asio)
		+ charInt					// sample rate choice index
		+ charInt					// buffer size choice index (only used on mac)
		+ charInt					// not used
		+ charInt					// num of max inputs channels
		+ charInt * maxAudioInputs	// inputs channels
		+ charInt					// num of max outputs channels
		+ charInt * maxAudioOutputs	// outputs channels
		);

	return ChunkLen;
}

//-----------------------------------------------------------------------------
// store module datas in the chunk
void DeviceAudio::onGetChunk(void* chunk, LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;
	int charInt = sizeof(int);

	int chunkOffset = 0;


	// -----------------------------------------------------------------------
	// HEADER section
	// copy the header code into the chunk
	memcpy(chunk
		, (void*)CHUNCK_DA23
		, charHeader
	);
	chunkOffset += charHeader;

	// -----------------------------------------------------------------------
	// audio driver choice section
	int driverChoice = 0;

	driverChoice = static_cast<int>(sdkGetEvtData(m_lboxDeviceDrivers));

	// copy input device choice index into the chunk
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&driverChoice
		, charInt
	);
	chunkOffset += charInt;

	// -----------------------------------------------------------------------
	// audio device choice section
	int inputChoice = 0;
	int outputChoice = 0;

	inputChoice = static_cast<int>(sdkGetEvtData(m_lboxDeviceIns));
	outputChoice = static_cast<int>(sdkGetEvtData(m_lboxDeviceOuts));

	// copy input device choice index into the chunk
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&inputChoice
		, charInt
	);
	chunkOffset += charInt;

	// copy the output device choice index into the chunk
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&outputChoice
		, charInt
	);
	chunkOffset += charInt;

	// -----------------------------------------------------------------------		
	// copy the sample rate choice index into the chunk
	int rateChoice = static_cast<int>(sdkGetEvtData(m_lboxSampleRate));
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&rateChoice
		, charInt
	);
	chunkOffset += charInt;

	// copy the buffer size choice index into the chunk
	int bufferChoice = 0;
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	bufferChoice = static_cast<int>(sdkGetEvtData(m_dtfBufferSize));
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	bufferChoice = static_cast<int>(sdkGetEvtData(m_lboxBufferSize));
#endif
	//----------------------------------------------------------------------------
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&bufferChoice
		, charInt
	);

	chunkOffset += charInt;

	// not used field
	int notused;
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&notused
		, charInt
	);

	chunkOffset += charInt;

	//-----------------------------------------------------------------------------
	// inputs/outputs channels activation section
	// copy the num of max inputs channels into the chunk
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&maxAudioInputs
		, charInt
	);
	chunkOffset += charInt;

	// copy each inputs channels state into the chunk
	int inputState = 0;
	for (int i = 0; i < maxAudioInputs; i++)
	{
		inputState = static_cast<int>(sdkGetEvtData(m_ledAudioDeviceInputNames[i]));

		memcpy((void*)((char*)chunk + chunkOffset)
			, (void*)&inputState
			, charInt
		);

		chunkOffset += charInt;
	}

	// copy the num of max outputs channels into the chunk
	memcpy((void*)((char*)chunk + chunkOffset)
		, (void*)&maxAudioOutputs
		, charInt
	);

	chunkOffset += charInt;

	// copy each outputs channels state into the chunk
	int outputState = 0;
	for (int i = 0; i < maxAudioOutputs; i++)
	{
		outputState = static_cast<int>(sdkGetEvtData(m_ledAudioDeviceOutputNames[i]));

		memcpy((void*)((char*)chunk + chunkOffset)
			, (void*)&outputState
			, charInt
		);

		chunkOffset += charInt;
	}
}

//-----------------------------------------------------------------------------
// restore module datas from the chunk
void DeviceAudio::onSetChunk(const void* chunk, int sizeInBytes, LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;
	int charInt = sizeof(int);

	char headerChunk[4];

	int chunkOffset = 0;

	// -----------------------------------------------------------------------
	// HEADER section
	// extract the header code from the chunk
	memcpy((void*)headerChunk
		, (void*)((char*)chunk)
		, charHeader
	);
	chunkOffset += charHeader;

	//chunk was saved with version 1.0
	//	if ( std::string(CHUNCK_DA01, 4) == std::string(headerChunk, 4) )
	//	{
	//	}
	//chunk was saved with version 2.0
	//	else if (std::string(CHUNCK_DA02, 4) == std::string(headerChunk, 4))
	//	{
	//	}
	//	//chunk was saved with version 2.3
	if (std::string(CHUNCK_DA23, 4) == std::string(headerChunk, 4))
	{
		// -----------------------------------------------------------------------
		// audio driver choice section
		// extract driver choice index from the chunk
		int driverChoice = 0;
		memcpy((void*)&driverChoice
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// -----------------------------------------------------------------------
		// audio device choice section
		// extract input device choice index from the chunk
		int inputChoice = 0;
		memcpy((void*)&inputChoice
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// extract output device choice index from the chunk
		int outputChoice = 0;
		memcpy((void*)&outputChoice
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// -----------------------------------------------------------------------		
		// extract the sample rate choice index from the chunk
		int rateChoice = 0;
		memcpy((void*)&rateChoice
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// extract the buffer size choice index from the chunk
		int bufferChoice = 0;
		memcpy((void*)&bufferChoice
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// not used field
		int notused = 0;
		memcpy((void*)&notused
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		//-----------------------------------------------------------------------------
		// inputs/outputs channels activation section
		// extract the num of max inputs channels into the chunk
		int numAudioInputs = 0;
		memcpy((void*)&numAudioInputs
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// extract each inputs channels state from the chunk
		int inputState = 0;
		for (int i = 0; i < numAudioInputs; i++)
		{
			memcpy((void*)&inputState
				, (void*)((char*)chunk + chunkOffset)
				, charInt
			);

			sdkSetEvtData(m_ledAudioDeviceInputNames[i], (TPrecision)inputState);
			switchAudioInput(i, false);

			chunkOffset += charInt;
		}

		// extract the num of max outputs channels from the chunk
		int numAudioOutputs = 0;
		memcpy((void*)&numAudioOutputs
			, (void*)((char*)chunk + chunkOffset)
			, charInt
		);
		chunkOffset += charInt;

		// extract each outputs channels state from the chunk
		int outputState = 0;
		for (int i = 0; i < numAudioOutputs; i++)
		{
			memcpy((void*)&outputState
				, (void*)((char*)chunk + chunkOffset)
				, charInt
			);

			sdkSetEvtData(m_ledAudioDeviceOutputNames[i], (TPrecision)outputState);
			switchAudioOutput(i, false);

			chunkOffset += charInt;
		}

		//-----------------------------------------------------------------------------
		// verification, apply params and create device

		sdkSetEvtData(m_lboxSampleRate, (TPrecision)rateChoice);

#if (defined (USINE_WIN32) || defined (USINE_WIN64))
		sdkSetEvtData(m_dtfBufferSize, (TPrecision)bufferChoice);
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
		sdkSetEvtData(m_lboxBufferSize, bufferChoice);
#endif

		sdkSetEvtData(m_lboxDeviceDrivers, (TPrecision)driverChoice);
		sdkSetEvtData(m_lboxDeviceIns, (TPrecision)inputChoice);
		sdkSetEvtData(m_lboxDeviceOuts, (TPrecision)outputChoice);

	}
	else
	{
		// error with version header
	}
	int driverID = static_cast<int>(sdkGetEvtData(m_lboxDeviceDrivers));
	applyDevice(driverID);
}

//-------------------------------------------------------------------------
// private methodes implementation
//-----------------------------------------------------------------------


AnsiCharPtr DeviceAudio::getQuotedDeviceDriversNames()
{
	if (initDevices == false)
		checkAudioDrivers();

	StringArrayToCommaString(qsDeviceDriversNames, listDeviceDriversNames);

	return (AnsiCharPtr)qsDeviceDriversNames.toUTF8().getAddress();
}

AnsiCharPtr DeviceAudio::getQuotedDeviceInsNames()
{
	if (initDevices == false)
		checkAudioDrivers();

	StringArrayToCommaString(qsDeviceInsNames, listDeviceInsNames);

	return (AnsiCharPtr)qsDeviceInsNames.toUTF8().getAddress();
}

AnsiCharPtr DeviceAudio::getQuotedDeviceOutsNames()
{
	if (initDevices == false)
		checkAudioDrivers();

	StringArrayToCommaString(qsDeviceOutsNames, listDeviceOutsNames);

	return (AnsiCharPtr)qsDeviceOutsNames.toUTF8().getAddress();
}



int DeviceAudio::selectFirstDeviceIn(String deviceToExclude)
{
	int result = 0;
	for (int i = 0; i < listDeviceInsNames.size(); ++i)
	{
		if (!listDeviceInsNames[i].containsIgnoreCase(deviceToExclude))
		{
			result = i;
			return i;
		}
	}
	return result;
}

int DeviceAudio::selectFirstDeviceOut(String deviceToExclude)
{
	int result = 0;

	for (int i = 0; i < listDeviceOutsNames.size(); ++i)
	{
		if (!listDeviceOutsNames[i].containsIgnoreCase(deviceToExclude))
		{
			result = i;
			return i;
		}
	}
	return result;
}

//-------------------------------------------------------------------------
//
char* DeviceAudio::getAudioInputCaption(int index)
{
	String name("-");

	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{
		StringArray names = deviceManager.getCurrentAudioDevice()->getInputChannelNames();
		if (index < names.size())
			name = names[index];
	}
	captionEnableInputs.set(index, name);
	caAudioInputsCaptions.set(index, captionEnableInputs[index].toUTF8());
	return const_cast <char*> (caAudioInputsCaptions[index]);
}

//-------------------------------------------------------------------------
//
char* DeviceAudio::getAudioInputSavedName(int index)
{
	captionAudioInputsSavedName.set(index, "In " + String(index + 1));

	caAudioInputsSavedName.set(index, captionAudioInputsSavedName[index].toUTF8());
	return const_cast <char*> (caAudioInputsSavedName[index]);
}

//-------------------------------------------------------------------------
//
char* DeviceAudio::getAudioOutputCaption(int index)
{
	String name("-");

	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{
		StringArray names = deviceManager.getCurrentAudioDevice()->getOutputChannelNames();
		if (index < names.size())
			name = names[index];
	}
	captionEnableOutputs.set(index, name);
	caAudioOutputsCaptions.set(index, captionEnableOutputs[index].toUTF8());
	return const_cast <char*> (caAudioOutputsCaptions[index]);
}

//-------------------------------------------------------------------------
//
char* DeviceAudio::getAudioOutputSavedName(int index)
{
	captionAudioOutputsSavedName.set(index, "Out " + String(index + 1));
	caAudioOutputsSavedName.set(index, captionAudioOutputsSavedName[index].toUTF8());
	return const_cast <char*> (caAudioOutputsSavedName[index]);
}

//-------------------------------------------------------------------------
char* DeviceAudio::getAudioSampleRatesList()
{
	traceLog("--getAudioSampleRatesList--");
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	listSampleRates.clear();
	listSampleRates.add(String(44100));
	listSampleRates.add(String(48000));
	listSampleRates.add(String(88200));
	listSampleRates.add(String(96000));
	listSampleRates.add(String(176400));
	listSampleRates.add(String(192000));
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	AudioIODevice* currentAudioDevice = deviceManager.getCurrentAudioDevice();
	listSampleRates.clear();
	if (currentAudioDevice != nullptr)
	{
		traceLog("  device:", currentAudioDevice->getName());
		Array <double> tmp = currentAudioDevice->getAvailableSampleRates();

		int numSampleRates = tmp.size();

		for (int i = 0; i < numSampleRates; i++)
			listSampleRates.add(String(tmp.getUnchecked(i)));

		listSampleRates.minimiseStorageOverheads();
		traceLog("  rates", listSampleRates.joinIntoString(","));
	}
	else
	{
		traceLog("  error - audio device not instanciated");
	}
#endif

	StringArrayToCommaString(qsSampleRates, listSampleRates);
	caSamplingRates = qsSampleRates.toUTF8();
	return const_cast <char*> (caSamplingRates);
}

//-------------------------------------------------------------------------
//
char* DeviceAudio::getBufferSizesList()
{
	AudioIODevice * currentAudioDevice = deviceManager.getCurrentAudioDevice();
	listBufferSizes.clear();
	if (currentAudioDevice != nullptr)
	{
		Array <int> tmp = currentAudioDevice->getAvailableBufferSizes();
		int numBufferSizes = tmp.size();

		for (int i = 0; i < numBufferSizes; i++)
			listBufferSizes.add(String(tmp.getUnchecked(i)));

		listBufferSizes.minimiseStorageOverheads();
	}

	StringArrayToCommaString(qsBufferSizes, listBufferSizes);
	caBufferSizes = qsBufferSizes.toUTF8();
	return const_cast <char*> (caBufferSizes);
}


// 
int DeviceAudio::getNumOfActiveInputs()
{
	int result = 0;
	for (int i = 0; i < numOfInputs; i++)
	{
		if (sdkGetEvtData(m_ledAudioDeviceInputNames[i]) == TRUE)
			result++;
	}
	return result;
}


// senso
BigInteger DeviceAudio::getStateInputs()
{
	BigInteger stateIns;
	stateIns.clear();
	for (int i = 0; i < numOfInputs; i++)
	{
		if (sdkGetEvtData(m_ledAudioDeviceInputNames[i]) == TRUE)
			stateIns.setBit(i, true);
	}
	return stateIns;
}

//-------------------------------------------------------------------------
//
void DeviceAudio::switchAudioInput(int numInput, bool updateDriver)
{

	currentAudioDeviceSetup.inputChannels = getStateInputs();

	if (updateDriver == true
		&& deviceManager.getCurrentAudioDevice() != nullptr
		)
		notifyChangeDriverSettings();
}

// 
int DeviceAudio::getNumOfActiveOutputs()
{
	int result = 0;
	for (int i = 0; i < numOfOutputs; i++)
	{
		if (sdkGetEvtData(m_ledAudioDeviceOutputNames[i]) == TRUE)
			result++;
	}
	return result;
}

// 
BigInteger DeviceAudio::getStateOutputs()
{

	BigInteger stateOuts;
	stateOuts.clear();
	for (int i = 0; i < numOfOutputs; i++)
	{
		if (sdkGetEvtData(m_ledAudioDeviceOutputNames[i]) == TRUE)
			stateOuts.setBit(i, true);
	}
	return stateOuts;
}

//
void DeviceAudio::switchAudioOutput(int numOutput, bool updateDriver)
{
	currentAudioDeviceSetup.outputChannels = getStateOutputs();

	if (updateDriver == true
		&& deviceManager.getCurrentAudioDevice() != nullptr
		)
		notifyChangeDriverSettings();
};

//
void DeviceAudio::showDriverPanel()
{
	if ((deviceManager.getCurrentAudioDevice() != nullptr) && (deviceManager.getCurrentAudioDevice()->hasControlPanel()))
		deviceManager.getCurrentAudioDevice()->showControlPanel();
}

//-------------------------------------------------------------------------
// apply devices
// onchange device listbox
// et qd on crée le module
//
void DeviceAudio::applyDevice(int deriverID)
{
//	return;
	traceLog("--applyDevice--");

	traceLog("  driver index: ", deriverID);
	traceLog("  indexSelectedDeviceDriver: ", indexSelectedDeviceDriver);

	if (indexSelectedDeviceDriver < 0) // pass ici la première fois
	{
		// faire un truc pour choisir la device par defaut

	}
	else if (indexSelectedDeviceDriver == 0)
	{
		closeDevice();
		// create thread
		traceLog("  NO AUDIO selected");
		// use a thread to call sdkUsineAudioDeviceIOCallback with empty buffers but actual buffersize value
		
		updateParamsVisibility();
		startThread(10);
	}
	else if (setupLoaded)
	{
		closeDevice();
		stopThread(100);

		// if we change device type, do some default value stuff
		if (deviceManager.getCurrentAudioDeviceType().compare(listDeviceDriversNames[indexSelectedDeviceDriver]) != 0)
		{
			deviceManager.setCurrentAudioDeviceType(listDeviceDriversNames[indexSelectedDeviceDriver], true);
		}

		checkAudioDrivers();

		AudioDeviceManager::AudioDeviceSetup audioSetup = getAudioSetupFromUser();

		doChangeDriverSettings(audioSetup);
		notifyChangeDriverSettings();
		updateParameters();
		deviceManager.removeAudioCallback(this);
		deviceManager.addAudioCallback(this);
		notifyChangeSampleRate();
	}
}

//-------------------------------------------------------------------------
void DeviceAudio::checkAudioDrivers()
{
	traceLog("--checkAudioDrivers--");

	// construct devices names list
	StringArray tmpIns;
	StringArray tmpOuts;

	const OwnedArray<AudioIODeviceType>& types = deviceManager.getAvailableDeviceTypes();
	traceLog("  deviceManager.getAvailableDeviceTypes");

	String tmpDriver = listDeviceDriversNames[indexSelectedDeviceDriver];
	String tmpDevice = getDeviceNamesInputs();

	String result = deviceManager.initialise(128, 128, 0, false, tmpDevice);

	listDeviceDriversNames.clear();
	listDeviceDriversNames.insert(0, DRIVER_NAME_NO_AUDIO);

	listDeviceInsNames.clear();
	listDeviceOutsNames.clear();

	traceLog("  num devices type: ", types.size());

	for (int i = 0; i < types.size(); ++i)
	{
		traceLog("  found devices type: ", types[i]->getTypeName());
		listDeviceDriversNames.add(types[i]->getTypeName());

		if (i == (indexSelectedDeviceDriver - 1))
		{
			// for ins
			tmpIns = types[i]->getDeviceNames(true);
			listDeviceInsNames.addArray(tmpIns);

			// for outs
			tmpOuts = types[i]->getDeviceNames(false);
			listDeviceOutsNames.addArray(tmpOuts);
		}
	}

	initDevices = true;
	oldIndexSelectedDeviceDriver = indexSelectedDeviceDriver;
}

//-------------------------------------------------------------------------
//
//
void DeviceAudio::updateParameters()
{
	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{
		AudioDeviceManager::AudioDeviceSetup audioSetup;
		deviceManager.getAudioDeviceSetup(audioSetup);

		// update num ins outs       
		numOfInputs = deviceManager.getCurrentAudioDevice()->getInputChannelNames().size();
		numOfOutputs = deviceManager.getCurrentAudioDevice()->getOutputChannelNames().size();
		sdkSetEvtData(m_dtfNbAudioIn, (float)numOfInputs);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 6);
		sdkSetEvtData(m_dtfNbAudioOut, (float)numOfOutputs);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 5);

		// update samplerate
		selectSampleRate(deviceManager.getCurrentAudioDevice()->getCurrentSampleRate());

		// buffersize
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
		deviceBufferSize = deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples();
		sdkSetEvtData(m_dtfBufferSize, (float)deviceBufferSize);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 3);
#endif

	}
	else
	{
		clearParameters();
	}
}


//-------------------------------------------------------------------------
// clear all setup panels parameters
//
void DeviceAudio::clearParameters()
{
	//m_dtfNbAudioIn;
	sdkSetEvtData(m_dtfNbAudioIn, 0);
	sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 6);

	//m_dtfNbAudioOut;
	sdkSetEvtData(m_dtfNbAudioOut, 0);
	sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 5);

	//m_dtfBufferSize
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	sdkSetEvtData(m_dtfBufferSize, 1024);
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
	int index = listBufferSizes.indexOf("256");
	sdkSetEvtData(m_lboxBufferSize, index);
#endif

	// clear audio ins
	//m_ledAudioDeviceInputNames[];	
	for (int i = 0; i < maxAudioInputs; i++)
	{
		sdkSetParamCaption(PARAMS_BEFORE_INS_OUTS + i, "");
		sdkSetParamVisible(PARAMS_BEFORE_INS_OUTS + i, FALSE);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS + i);
	}

	// clear audio outs
	//m_ledAudioDeviceOutputNames[];   
	for (int i = 0; i < maxAudioOutputs; i++)
	{
		sdkSetParamCaption(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i, "");
		sdkSetParamVisible(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i, FALSE);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i);
	}

}


//-------------------------------------------------------------------------
// collecter choix user des params
AudioDeviceManager::AudioDeviceSetup DeviceAudio::getAudioSetupFromUser()
{
	AudioDeviceManager::AudioDeviceSetup result;
	result.bufferSize = getBufferSize();
	result.inputChannels = getStateInputs();
	result.outputChannels = getStateOutputs();
	result.inputDeviceName = getDeviceNamesInputs();

	if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_ASIO) == 0)
		result.outputDeviceName = result.inputDeviceName;
	else
		result.outputDeviceName = getDeviceNamesOutputs();

	result.sampleRate = getSampleRate();
	result.useDefaultInputChannels = false;
	result.useDefaultOutputChannels = false;
	return result;
}

//-------------------------------------------------------------------------
//
int DeviceAudio::getBufferSize()
{
	int result = 0;

	if (deviceManager.getCurrentAudioDevice() == nullptr)
	{
		result = 512;
	}
	else
	{
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
		result = static_cast<int>(sdkGetEvtData (m_dtfBufferSize));
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
		int newBufferSizeIndex = (int)sdkGetEvtData(m_lboxBufferSize);
		result = listBufferSizes[newBufferSizeIndex].getIntValue();
#endif

	}

	return result;
}

//-------------------------------------------------------------------------
//
double DeviceAudio::getSampleRate()
{
	double result = 0;

	if (deviceManager.getCurrentAudioDevice() == nullptr)
	{
		result = 44100;
	}
	else
	{
		result = listSampleRates[(int)sdkGetEvtData(m_lboxSampleRate)].getDoubleValue();
	}
	return result;

}

//-------------------------------------------------------------------------
//
String DeviceAudio::getDeviceNamesInputs()
{
	int selIndex = 0;

	if (setupLoaded)
		selIndex = static_cast<int>(sdkGetEvtData(m_lboxDeviceIns));

	String result;
	result = listDeviceInsNames[selIndex];

	return result;
}

//-------------------------------------------------------------------------
//
String DeviceAudio::getDeviceNamesOutputs()
{
	int  selIndex = static_cast<int>(sdkGetEvtData(m_lboxDeviceOuts));

	String result;
	result = listDeviceOutsNames[selIndex];

	return result;
}

//-------------------------------------------------------------------------
//
void DeviceAudio::traceAudioSetup(AudioDeviceManager::AudioDeviceSetup audioSetup)
{
	traceLog("--traceAudioSetup--");
	traceLog("  in device:", audioSetup.inputDeviceName);
	traceLog("  channels:", audioSetup.inputChannels.toString(2, 16));
	traceLog("  out device:", audioSetup.outputDeviceName);
	traceLog("  channels", audioSetup.outputChannels.toString(2, 16));
	traceLog("  buffer size:", audioSetup.bufferSize);
	traceLog("  samplerate:", (int)audioSetup.sampleRate);
}

//-------------------------------------------------------------------------
//
String DeviceAudio::doChangeDriverSettings(AudioDeviceManager::AudioDeviceSetup audioSetup)
{
	traceLog("--changeDriverSettings--");
	String result;

	traceAudioSetup(audioSetup);
    if (audioSetup.inputChannels == 0)
    {
        audioSetup.inputChannels = 1;
    }
    if (audioSetup.outputChannels == 0)
    {
        audioSetup.outputChannels = 1;
    }
    
	result = deviceManager.setAudioDeviceSetup(audioSetup, true);

	if (result.isNotEmpty())
	{
		result = result.replace("Error:", "");

		if (result != lastErrorMessage)
			sdkTraceErrorChar((AnsiCharPtr)result.toUTF8().getAddress());

		lastErrorMessage = result;
	}
	// tell to usine settings have changed
	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{
		sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_AUDIO_DRIVER_CHANGED, 0, 0);
		updateInsOutsCaptions();
	}
	else
	{
		traceLog("  null audio device");
	}

	return result;
}

//-------------------------------------------------------------------------
void DeviceAudio::updateDevicesDriversCaptions()
{
	traceLog("--updateDevicesDriversCaptions--");

	checkAudioDrivers();

	sdkSetListBoxCommatext(1, getQuotedDeviceDriversNames());
	sdkSetListBoxCommatext(2, getQuotedDeviceInsNames());
	sdkSetListBoxCommatext(3, getQuotedDeviceOutsNames());

	updateParamsVisibility();
}

//-------------------------------------------------------------------------
void DeviceAudio::updateInsOutsCaptions()
{
	// update num ins outs       
	numOfInputs = deviceManager.getCurrentAudioDevice()->getInputChannelNames().size();
	numOfOutputs = deviceManager.getCurrentAudioDevice()->getOutputChannelNames().size();

	// update audio ins led caption
	LongBool visible;
	for (int i = 0; i < maxAudioInputs; i++)
	{
		sdkSetParamCaption(PARAMS_BEFORE_INS_OUTS + i, getAudioInputCaption(i));

		if (i < numOfInputs)
			visible = TRUE;
		else
			visible = FALSE;

		sdkSetParamVisible(PARAMS_BEFORE_INS_OUTS + i, visible);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS + i);
	}

	// update audio outs led caption     
	for (int i = 0; i < maxAudioOutputs; i++)
	{
		sdkSetParamCaption(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i, getAudioOutputCaption(i));

		if (i < numOfOutputs)
			visible = TRUE;
		else
			visible = FALSE;

		sdkSetParamVisible(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i, visible);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS + maxAudioInputs + i);
	}
}

//-------------------------------------------------------------------------
void DeviceAudio::updateParamsVisibility()
{
	//traceLog("updateParamsVisibility: true= ", true);
	traceLog("--updateParamsVisibility--");
	traceLog("  indexSelectedDeviceDriver ", indexSelectedDeviceDriver);
	if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_NO_AUDIO) == 0)
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "in");
		sdkSetParamVisible(2, FALSE);
		sdkSetParamVisible(3, FALSE);
		sdkSetParamVisible(4, FALSE);
		sdkSetParamVisible(5, FALSE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, FALSE);
		sdkSetParamVisible(8, FALSE);
	}
	else if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_WINDOWS_AUDIO) == 0)
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "in");
		sdkSetParamVisible(2, TRUE);
		sdkSetParamVisible(3, TRUE);
		sdkSetParamVisible(4, TRUE);
		sdkSetParamVisible(5, TRUE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, TRUE);
		sdkSetParamVisible(8, FALSE);
	}
	else if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_WINDOWS_AUDIO_EXCL) == 0)
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "in");
		sdkSetParamVisible(2, TRUE);
		sdkSetParamVisible(3, TRUE);
		sdkSetParamVisible(4, TRUE);
		sdkSetParamVisible(5, TRUE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, TRUE);
		sdkSetParamVisible(8, FALSE);
	}
	else if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_DIRECT_SOUND) == 0)
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "in");
		sdkSetParamVisible(2, TRUE);
		sdkSetParamVisible(3, TRUE);
		sdkSetParamVisible(4, TRUE);
		sdkSetParamVisible(5, TRUE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, TRUE);
		sdkSetParamVisible(8, FALSE);
	}
	else if (listDeviceDriversNames[indexSelectedDeviceDriver].compare(DRIVER_NAME_ASIO) == 0)
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "device");
		sdkSetParamVisible(2, TRUE);
		sdkSetParamVisible(3, FALSE);
		sdkSetParamVisible(4, TRUE);
		sdkSetParamVisible(5, TRUE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, TRUE);
		sdkSetParamVisible(8, TRUE);
	}
	else
	{
		sdkSetParamVisible(1, TRUE);
		sdkSetParamCaption(2, "in");
		sdkSetParamVisible(2, TRUE);
		sdkSetParamVisible(3, TRUE);
		sdkSetParamVisible(4, TRUE);
		sdkSetParamVisible(5, TRUE);
		sdkSetParamVisible(6, TRUE);
		sdkSetParamVisible(7, TRUE);
	}

}

//-------------------------------------------------------------------------
//
void DeviceAudio::changeListenerCallback(ChangeBroadcaster *source)
{
	// tell to usine settings have changed
	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{

		// device buffer size
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
		deviceBufferSize = deviceManager.getCurrentAudioDevice()->getCurrentBufferSizeSamples();
		currentAudioDeviceSetup.bufferSize = deviceBufferSize;
		sdkSetEvtData(m_dtfBufferSize, (float)deviceBufferSize);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 3);
#endif
	}
}

//
void DeviceAudio::closeDevice()
{

	String currentDevice("null");

	if (deviceManager.getCurrentAudioDevice() != nullptr)
	{
		currentDevice = deviceManager.getCurrentAudioDevice()->getName();

		traceLog("--close audio device--");
		traceLog("  current device: ", currentDevice);

		
	}
	deviceManager.closeAudioDevice();


	sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 6);
	sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 5);

	// invalidate device infos (see checkAudioDrivers())
	initDevices = false;
}

//
bool DeviceAudio::selectSampleRate(double sampleRate)
{
	int indexSampleRate = listSampleRates.indexOf(String(sampleRate));
	if (indexSampleRate > -1)
	{
		sdkSetEvtData(m_lboxSampleRate, (float)indexSampleRate);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 4);
		return true;
	}
	return false;
}

//
void DeviceAudio::changeSampleRate()
{
	// change
	traceLog("--changeSampleRate--");
	int newSampleRateIndex = (int)sdkGetEvtData(m_lboxSampleRate);
	newSampleRateIndex = std::min(newSampleRateIndex, listSampleRates.size() - 1);
	currentAudioDeviceSetup.sampleRate = listSampleRates[newSampleRateIndex].getIntValue();

	traceLog("  new index:", newSampleRateIndex);

	String log;
	log << (int)currentAudioDeviceSetup.sampleRate;
	traceLog("  new sample rate:", log);

	int driverID = static_cast<int>(sdkGetEvtData(m_lboxDeviceDrivers));
	applyDevice(driverID);
	
}

#if (defined (USINE_OSX32) || defined (USINE_OSX64))
//
bool DeviceAudio::selectBufferSize(double bufferSize)
{
	deviceBufferSize = (int)bufferSize;
	int indexBufferSize = listBufferSizes.indexOf(String(bufferSize));
	if (indexBufferSize > -1)
	{
		sdkSetEvtData(m_lboxBufferSize, (float)indexBufferSize);
		sdkRepaintParam(PARAMS_BEFORE_INS_OUTS - 3);

		lboxLastBufferSizeIndex = indexBufferSize;
		return true;
	}
	return false;
}
#endif

//------------------------------------------------------------------------------------
void DeviceAudio::changeBufferSize()
{

#if (defined (USINE_OSX32) || defined (USINE_OSX64))
	int newBufferSizeIndex = (int)sdkGetEvtData(m_lboxBufferSize);
	currentAudioDeviceSetup.bufferSize = listBufferSizes[newBufferSizeIndex].getIntValue();
#endif

#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	int newBufferSize = static_cast<int>(sdkGetEvtData(m_dtfBufferSize));
	currentAudioDeviceSetup.bufferSize = newBufferSize;
#endif

	int driverID = static_cast<int>(sdkGetEvtData(m_lboxDeviceDrivers));
	applyDevice(driverID);

}
//-----------------------------------------------------------------------------

//-------------------------------------------------------------------------
void DeviceAudio::notifyChangeDriverSettings()
{
	sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, MSG_CHANGE_DRIVERS_REQUEST, MSG_CHANGE);
}

//-------------------------------------------------------------------------
void DeviceAudio::notifyChangeSampleRate()
{
	if ((NativeInt)deviceManager.getCurrentAudioDevice() != NULL)
	{
		sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_SAMPLE_RATE_CHANGED, (NativeInt)deviceManager.getCurrentAudioDevice()->getCurrentSampleRate(), 0);
	}
}

//-------------------------------------------------------------------------
void DeviceAudio::audioDeviceIOCallback(const float **inputChannelData, int numInputChannels, float **outputChannelData, int numOutputChannels, int numSamples)
{
	sdkUsineAudioDeviceIOCallback(inputChannelData, numInputChannels, outputChannelData, numOutputChannels, numSamples);
}


//
void DeviceAudio::audioDeviceAboutToStart(AudioIODevice *device)
{
}

void DeviceAudio::audioDeviceStopped()
{
	traceLog("--audioDeviceStopped--");
}

void DeviceAudio::audioDeviceError(const String& errorMessage)
{
	sdkTraceLogChar((AnsiCharPtr)errorMessage.toUTF8().getAddress());
}

//------------------------------------------------------------------------
// NoAudioThread
void DeviceAudio::run()
{
	while (!threadShouldExit())
	{
		sdkUsineAudioDeviceIOCallback(nullptr, 0, nullptr, 0, sdkGetBlocSize());
		wait(std::max(1, (int)(sdkGetBlocSize() / 44.1)));
	}
}

//------------------------------------------------------------------------
// trace helpers
void DeviceAudio::traceLog(AnsiCharPtr traceMsg)
{
	std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg;
	stringTrace = os.str();
	sdkTraceLogChar((AnsiCharPtr)stringTrace.c_str());
}

void DeviceAudio::traceLog(AnsiCharPtr traceMsg, int value)
{

	std::ostringstream os;
	os << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << value;
	stringTrace = os.str();
	sdkTraceLogChar((AnsiCharPtr)stringTrace.c_str());
}

void DeviceAudio::traceLog(AnsiCharPtr traceMsg, String value)
{
	traceString = String();
	traceString << "[" << m_moduleInfo->Name << "]" << " " << traceMsg << " " << value;
	sdkTraceLogChar(traceString.toUTF8().getAddress());
}
