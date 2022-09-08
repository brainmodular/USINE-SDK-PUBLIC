//-----------------------------------------------------------------------------
//@file  
//	DeviceMidi.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DeviceMidi class.
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
#include "DeviceMidi.h"

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
 //   initialiseJuce_GUI ();
	pModule = new DeviceMidi();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((DeviceMidi*)pModule);
//    shutdownJuce_GUI ();
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "midi";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "midi device user module";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = MODULE_USER_VERSION;

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructor/destructor
//-------------------------------------------------------------------------
// constructor
DeviceMidi::DeviceMidi () 
	: midiOutCode(128, 60, 0)
	, autoSetup(false)
	, maxMidiInputs (0)
	, maxMidiOutputs (0)
{
	// collect input devices names
	collectMidiInputsDeviceNames();

	// collect output devices names
	collectMidiOutputsDeviceNames();
}

// destructor
DeviceMidi::~DeviceMidi ()
{	
	listMidiInputDevicesNames.clear();
	listMidiOutputDevicesNames.clear();
	
	//
	for (int i = 0; i < maxMidiOutputs; ++i)
	{
		if (arrayMidiOutputDevices[i] != nullptr)
		{
			arrayMidiOutputDevices[i]->stopBackgroundThread();
		}
	}
	arrayMidiOutputDevices.clear();
}

//-----------------------------------------------------------------------------
// module description
void DeviceMidi::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	if (pMasterInfo != nullptr) 
	{
		maxMidiInputs			= sdkGetUsineMaxMidiDevices();
		maxMidiOutputs			= sdkGetUsineMaxMidiDevices();
		pModuleInfo->BackColor  = sdkGetUsineColor(clMIDIModuleColor);
	}
	else
	{
		maxMidiInputs	= 16;
		maxMidiOutputs  = 16;
    }	
    
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtDeviceMidi;
	pModuleInfo->NumberOfParams     = PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS + maxMidiOutputs; // + maxMidiOutputs;
	pModuleInfo->DontProcess		= TRUE;
	
	pModuleInfo->QueryString		= "";
	pModuleInfo->QueryListValues	= "";
	pModuleInfo->QueryDefaultIdx	= 1;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = FALSE;

}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void DeviceMidi::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	initDeviceMidiModule();
	rescanMidi();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------
// Parameters description
void DeviceMidi::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
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
		if (sdkGetUsineVersionType() == uvStandAloneReaderProtected)
		{
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepDevices.c_str();
			pParamInfo->DefaultValue = 1;
			pParamInfo->IsInvisible = FALSE;
		}
		else
		{
			pParamInfo->DefaultValue = 0;
			pParamInfo->IsInvisible = TRUE;
		}

	}
	// m_btnRescanAllMidiDevices
	if (ParamIndex == 1)
	{
        strSepDevices = std::string(sdkGetTrans("devices"));

		pParamInfo->ParamType		 = ptButton;
		pParamInfo->Caption			 = "rescan devices";
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		pParamInfo->ReadOnly		 = FALSE;
		pParamInfo->Translate		 = TRUE;
		if (sdkGetUsineVersionType() != uvStandAloneReaderProtected)
		{
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepDevices.c_str();
		}
	}
	// m_btnResetAllMidiDevices
	else if (ParamIndex == 2)
	{
		pParamInfo->ParamType		 = ptButton;
		pParamInfo->Caption			 = "reset devices";
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		pParamInfo->ReadOnly		 = FALSE;
		pParamInfo->Translate		 = TRUE;
		pParamInfo->IsSeparator		 = TRUE;
	}
	// m_ledMidiDeviceInputNames
	else if ((ParamIndex >= PARAMS_BEFORE_MIDI_INS) && (ParamIndex < (PARAMS_BEFORE_MIDI_INS + maxMidiInputs)))
	{
		pParamInfo->ParamType		 = ptLeftLed;
		pParamInfo->Caption			 = getMidiInputCaption (ParamIndex - PARAMS_BEFORE_MIDI_INS);
		pParamInfo->SavedName		 = getMidiInputsSavedName (ParamIndex - PARAMS_BEFORE_MIDI_INS);
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;		
		if (ParamIndex == PARAMS_BEFORE_MIDI_INS)
		{
            strSepInputs = std::string(sdkGetTrans("inputs"));

			pParamInfo->IsSeparator		 = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepInputs.c_str();
		}
	}
	// m_ledMidiDeviceOutputNames
	else if (ParamIndex >= (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) && (ParamIndex < (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS + maxMidiOutputs)))
	{
		pParamInfo->ParamType		 = ptLeftLed;
		pParamInfo->Caption			 = getMidiOutputCaption (ParamIndex - (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS));
		pParamInfo->SavedName		 = getMidiOutputsSavedName (ParamIndex - (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS));
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		if (ParamIndex == (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS))
		{
            strSepOutputs = std::string(sdkGetTrans("outputs"));

			pParamInfo->IsSeparator		 = TRUE;
			pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepOutputs.c_str();
		}
	}
	// default case
	else{}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void DeviceMidi::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
	// m_ledDeviceAutoSetup
	if (ParamIndex == 0)
	{
		m_ledDeviceAutoSetup = pEvent;
	}
	// m_btnRescanAllMidiDevices
	if (ParamIndex == 1)
	{
		m_btnRescanAllMidiDevices = pEvent;
	}
	// m_btnResetAllMidiDevices
	else if (ParamIndex == 2)
	{
		m_btnResetAllMidiDevices = pEvent;
	}
	// m_lboxMidiDeviceInputNames
	else if ((ParamIndex >= PARAMS_BEFORE_MIDI_INS) && (ParamIndex < (PARAMS_BEFORE_MIDI_INS + maxMidiInputs)))
	{
		m_ledMidiDeviceInputNames.set(ParamIndex - PARAMS_BEFORE_MIDI_INS, pEvent);
	}
	// m_lboxMidiDeviceOutputNames
	else if (ParamIndex >= (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) && (ParamIndex < (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS + maxMidiOutputs)))
	{
		m_ledMidiDeviceOutputNames.set(ParamIndex - (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS), pEvent);
	}
	// default case
	else{}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DeviceMidi::onCallBack (UsineMessage* Message) 
{ 
	int ParamIndex = (int)Message->wParam;
	// m_ledDeviceAutoSetup
	if ((ParamIndex == 0) && (Message->lParam == MSG_CHANGE))
	{
		if (sdkGetEvtData(m_ledDeviceAutoSetup) == 1)
		{
			autoSetup = true;
			rescanMidi();

			// for inputs, all existing device actives except MIDI LAN
			for (int i = 1; i < maxMidiInputs; i++)
			{
				sdkSetParamCaption(PARAMS_BEFORE_MIDI_INS + i, getMidiInputCaption(i));

				// adjust visibility of the control panel button
				if (i < listMidiInputDevicesNames.size())
				{
					sdkSetEvtData(m_ledMidiDeviceInputNames[i], 1.0f);
				}
				else
				{
					sdkSetEvtData(m_ledMidiDeviceInputNames[i], 0.0f);
				}
			}
			// for Outputs, all existing device inactives
			for (int i = 0; i < maxMidiOutputs; i++)
			{
				// adjust visibility of the control panel button
				sdkSetEvtData(m_ledMidiDeviceOutputNames[i], 0.0f);
			}
		}
		else
		{
			autoSetup = false;
		}

	}
	// m_btnRescanAllMidiDevices
	else if ((ParamIndex == 1) && (Message->lParam == MSG_CHANGE))
	{
		rescanMidi();
		resetMidi();
	}
	// m_btnResetAllMidiDevices
	else if ((ParamIndex == 2) && (Message->lParam == MSG_CHANGE))
	{
		resetMidi();
	}
	// m_lboxMidiDeviceInputNames
	else if ((ParamIndex >= PARAMS_BEFORE_MIDI_INS) && (ParamIndex < maxMidiInputs) && (Message->lParam == MSG_CHANGE))
	{
		setInputMidiDevice (ParamIndex - PARAMS_BEFORE_MIDI_INS);
	}
	// m_lboxMidiDeviceOutputNames
	else if (ParamIndex >= (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) && (ParamIndex < (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS + maxMidiOutputs)) && (Message->lParam == MSG_CHANGE))
	{
		int numOutput = ParamIndex - (PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS);
		setOutputMidiDevice (numOutput);
	}
	// default case
	else{}
}

//-----------------------------------------------------------------------------
// midi out callbacks
//----------------------------------------------------------------------------
// called by Usine to send out midi
void DeviceMidi::onMidiSendOut (int DeviceID, UsineMidiCode Code)
{
	try
	{
		if (DeviceID >= 0 && DeviceID < maxMidiOutputs)
		{
			MidiOutput* midiOutput = arrayMidiOutputDevices[DeviceID];
			if (midiOutput != nullptr )
			{		
				//midiOutCode.setChannel (Code.Channel);		
                midiOutCode = constructJuceMidiMessage (Code);
				midiOutput->sendMessageNow (midiOutCode);
			}
			else
			{
				//sdkTraceWarningPChar(caErrorInvalidMidiOut);
			}
		}		
		else
		{
			sdkTraceWarningChar(caErrorInvalidMidiOut);
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in onMidiSendOut");
	}
}

void DeviceMidi::onMidiSendOutArray (int DeviceID, UsineMidiCode** arrayCode, int arraySize)
{
	try
	{
		if (DeviceID >= 0 && DeviceID < maxMidiOutputs)
		{
			MidiOutput* midiOutput = arrayMidiOutputDevices[DeviceID];
			if (midiOutput != nullptr )
			{	
                midiBufferSendOutArray.clear();

                for (int i = 0; i < arraySize; i++)
                {
                    midiOutArrayCode = constructJuceMidiMessage ((*arrayCode)[i]);
                    midiBufferSendOutArray.addEvent (midiOutArrayCode, 0);
                }
                midiOutput->sendBlockOfMessages (midiBufferSendOutArray, Time::getMillisecondCounter(), 32);
			}
			else
			{
				//sdkTraceWarningPChar(caErrorInvalidMidiOut);
			}
		}		
		else
		{
			sdkTraceWarningChar(caErrorInvalidMidiOut);
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in onMidiSendOutArray");
	}
}


// called by Usine to send out midi sysex
void DeviceMidi::onMidiSysexSendOut (int DeviceID, char** Sysex, int sysexSize)
{
    // REINJECTER LE SYSEX DANS LE MIDI OUT DE LA DEVICE concerné
    // voir timestamp
    	try
	{
		if (DeviceID >= 0 && DeviceID < maxMidiOutputs)
		{
			MidiOutput* midiOutput = arrayMidiOutputDevices[DeviceID];
			if (midiOutput != nullptr )
			{	
                MidiBuffer bufferOutput;

				MidiMessage tmpMessage = MidiMessage::createSysExMessage ((uint8*)Sysex[0], sysexSize);
                bufferOutput.addEvent (tmpMessage, 0);

                midiOutput->sendBlockOfMessages (bufferOutput, Time::getMillisecondCounter(), 32);
			}
			else
			{
				//sdkTraceWarningPChar(caErrorInvalidMidiOut);
			}
		}		
		else
		{
			sdkTraceWarningChar(caErrorInvalidMidiOut);
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in onMidiSysexSendOut");
	}
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------
// GetChunkLen 
int DeviceMidi::onGetChunkLen (LongBool Preset)
{
	int ChunkLen = 0;

	try
	{
		int choice = 0;

		// collect inputs choice by name into a quotted string
		StringArray listMidiInputChoiceNames;

		for (int i = 0; i < maxMidiInputs; i++)
		{
			choice = (int)sdkGetEvtData (m_ledMidiDeviceInputNames[i]);
			if (i < listMidiInputDevicesNames.size() && choice == 1)
			{
				listMidiInputChoiceNames.set(i, listMidiInputDevicesNames[i]);
			}
			else
			{
				listMidiInputChoiceNames.set(i, String::empty);
			}
		}

		qsChunkMidiInputChoiceNames = listMidiInputChoiceNames.joinIntoString("\",\"").quoted();
		sizeChunkMidiInputChoiceNames = (int)qsChunkMidiInputChoiceNames.toUTF8().sizeInBytes();
		caChunkMidiInputChoiceNames = qsChunkMidiInputChoiceNames.toUTF8();
		numOfChunkMidiInputs = maxMidiInputs;

		// collect outputs choice by name into a quotted string
		StringArray listMidiOutputChoiceNames;

		for (int i = 0; i < maxMidiOutputs; i++)
		{
			choice = (int)sdkGetEvtData (m_ledMidiDeviceOutputNames[i]);
			if (i < listMidiOutputDevicesNames.size() && choice == 1)
			{
				listMidiOutputChoiceNames.set(i, listMidiOutputDevicesNames[i]);
			}
			else
			{
				listMidiOutputChoiceNames.set(i, String::empty);
			}
		}

		qsChunkMidiOutputChoiceNames = listMidiOutputChoiceNames.joinIntoString("\",\"").quoted();
		sizeChunkMidiOutputChoiceNames = (int)qsChunkMidiOutputChoiceNames.toUTF8().sizeInBytes();
		caChunkMidiOutputChoiceNames = qsChunkMidiOutputChoiceNames.toUTF8();
		numOfChunkMidiOutputs = maxMidiOutputs;

		// compute size of the cells value array
		ChunkLen = ( sizeof(char) * 4		// the version header
			+ sizeof(int)						// num of midi in
			+ sizeof(int)						// sizeof qsChunkMidiInputChoiceNames in byte
			+ sizeChunkMidiInputChoiceNames		// imputs names content
			+ sizeof(int)						// num of midi out
			+ sizeof(int)						// sizeof qsChunkMidiOutputChoiceNames in byte
			+ sizeChunkMidiOutputChoiceNames	// outputs name content
			);
	}
	catch (...)
	{
		sdkTraceErrorChar(" in getChunkLen");
	}

	return ChunkLen;
}

//-----------------------------------------------------------------------------
// GetChunk : store infos for patch/wkp/preset into chnuk
void DeviceMidi::onGetChunk (void* chunk, LongBool Preset)
{
	try
	{
		int charInt = sizeof(int);

		int charHeader = 4;
	
		// -----------------------------------------------------------------------
		// HEADER section
		// copy the header code into the chunk
		memcpy( chunk
			, (void*)CHUNCK_DM_02
			, charHeader 
			);
		// -----------------------------------------------------------------------
		// num of inputs section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader)
			, (void*)&numOfChunkMidiInputs
			, charInt 
			);
		// -----------------------------------------------------------------------
		// inputs names content size section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader + charInt)
			, (void*)&sizeChunkMidiInputChoiceNames
			, charInt 
			);
		// -----------------------------------------------------------------------
		// inputs names content section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader + charInt + charInt)
			, (void*)caChunkMidiInputChoiceNames
			, sizeChunkMidiInputChoiceNames 
			);
		// -----------------------------------------------------------------------
		// num of outputs section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader + charInt + charInt + sizeChunkMidiInputChoiceNames)
			, (void*)&numOfChunkMidiOutputs
			, charInt 
			);
		// -----------------------------------------------------------------------
		// outputs names content size section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader + charInt + charInt + sizeChunkMidiInputChoiceNames + charInt)
			, (void*)&sizeChunkMidiOutputChoiceNames
			, charInt 
			);
		// -----------------------------------------------------------------------
		// outputs names content section
		// copy the num of cols into the chunk
		memcpy( (void*)((char*)chunk + charHeader + charInt + charInt + sizeChunkMidiInputChoiceNames + charInt + charInt)
			, (void*)caChunkMidiOutputChoiceNames
			, sizeChunkMidiOutputChoiceNames 
			);
	}
	catch (...)
	{
		sdkTraceErrorChar(" in getChunk");
	}

}

//-----------------------------------------------------------------------------
// SetChunk restore info for patch/wkp/preset from chunk
void DeviceMidi::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset)
{
	try
	{
		int charInt = sizeof(int);

		int charHeader = 4;
		//int charMatrix = charFloat * MATRIX_SIZE_MAX;

		char headerChunk[4];

		// extract the header code from the chunk
		memcpy( (void*)headerChunk
			, (void*)((char*)chunk)
			, charHeader 
			);

		//chunk was saved with version 1.0
		if (std::string(CHUNCK_DM_01, 4) == std::string(headerChunk, 4))
		{


		}
		//chunk was saved with version 2.0
		else if ( String(CHUNCK_DM_02, 4) == String(headerChunk, 4) )
		{		
			// -----------------------------------------------------------------------
			// num of inputs section
			// extract the values array size from the chunk
			int numOfMidiIns;
			memcpy( (void*)&numOfMidiIns
				, (void*)((char*)chunk + charHeader)
				, charInt 
				);

			if (numOfMidiIns == maxMidiInputs)
				numOfChunkMidiInputs = numOfMidiIns;
			//else
				// error
		
			// -----------------------------------------------------------------------
			// inputs names content size section
			// extract the values array size from the chunk
			int sizeOfMidiInsNames;
			memcpy( (void*)&sizeOfMidiInsNames
				, (void*)((char*)chunk + charHeader + charInt)
				, charInt 
				);

			// -----------------------------------------------------------------------
			// inputs names content section
			String qsMidiInsNames = String::createStringFromData((AnsiCharPtr)chunk + charHeader + charInt + charInt, sizeOfMidiInsNames);

			// for inputs
			listMidiInputWantedNames.clear();
			listMidiInputWantedNames.addTokens(qsMidiInsNames, ",", String::empty);

			if (listMidiInputWantedNames.size() != maxMidiInputs)
				sdkTraceErrorChar("Midi ins corrupted, check setup.");

			// -----------------------------------------------------------------------
			// num of outputs section
			int numOfMidiOuts;
			memcpy( (void*)&numOfMidiOuts
				, (void*)((char*)chunk + charHeader + charInt + charInt + sizeOfMidiInsNames)
				, charInt 
				);	
			
			if (numOfMidiOuts == maxMidiOutputs)
				numOfChunkMidiOutputs = numOfMidiOuts;
			//else
				// error

			// -----------------------------------------------------------------------
			// outputs names content size section
			int sizeOfMidiOutsNames;
			memcpy( (void*)&sizeOfMidiOutsNames
				, (void*)((char*)chunk + charHeader + charInt + charInt + sizeOfMidiInsNames + charInt)
				, charInt 
				);

			// -----------------------------------------------------------------------
			// outputs names content section
			String qsMidiOutsNames = String::createStringFromData((AnsiCharPtr)chunk + charHeader + charInt + charInt + sizeOfMidiInsNames + charInt + charInt, sizeOfMidiOutsNames);
			
			// for outputs
			listMidiOutputWantedNames.clear();
			listMidiOutputWantedNames.addTokens(qsMidiOutsNames, ",", String::empty);
			
			if (listMidiOutputWantedNames.size() != maxMidiOutputs)
				sdkTraceErrorChar("Midi outs corrupted, check setup");

			tryRestoreWantedSettings(true);

		}
	}
	catch (...)
	{
		sdkTraceErrorChar(" in setChunk");
	}
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------

//
void DeviceMidi::initDeviceMidiModule()
{
	caMidiInputsCaptions.resize (maxMidiInputs);
	captionMidiInputsSavedName.resize (maxMidiInputs);
	caMidiInputsSavedName.resize (maxMidiInputs);
	
	caMidiOutputsCaptions.resize (maxMidiOutputs);
	captionMidiOutputsSavedName.resize (maxMidiOutputs);
	caMidiOutputsSavedName.resize (maxMidiOutputs);
	

	for (int i = 0; i < maxMidiOutputs; ++i)
	{
		arrayMidiOutputDevices.set(i, nullptr);
	}

	captionErrorInvalidMidiOut = "Invalid midi out device.";
	caErrorInvalidMidiOut = captionErrorInvalidMidiOut.toUTF8().getAddress();
}

char* DeviceMidi::getMidiInputList ()
{
	qsMidiInputsDevicesNames = listMidiInputDevicesNames.joinIntoString("\",\"").quoted();
	const char* caMidiInputDevices = qsMidiInputsDevicesNames.toUTF8 ();
	return const_cast <char*> (caMidiInputDevices);
}

//
char* DeviceMidi::getMidiInputCaption (int index)
{
	String name("-");

	if (index < listMidiInputDevicesNames.size())
    {
            name = listMidiInputDevicesNames[index];
    }
	caMidiInputsCaptions.set(index, listMidiInputDevicesNames[index].toUTF8());
	return const_cast <char*> (caMidiInputsCaptions[index]);
}
	
char* DeviceMidi::getMidiInputsSavedName (int index)
{
	captionMidiInputsSavedName.set(index, "In " + String(index + 1));
	caMidiInputsSavedName.set(index, captionMidiInputsSavedName[index].toUTF8 ());
	return const_cast <char*> (caMidiInputsSavedName[index]);
}

//
char* DeviceMidi::getMidiOutputList()
{
	qsMidiOutputsDevicesNames = listMidiOutputDevicesNames.joinIntoString("\",\"").quoted();
	const char* caMidiOutputDevices = qsMidiOutputsDevicesNames.toUTF8();
	return const_cast <char*> (caMidiOutputDevices);
}

//
char* DeviceMidi::getMidiOutputCaption (int index)
{
	String name("-");

	if (index < listMidiOutputDevicesNames.size())
    {
            name = listMidiOutputDevicesNames[index];
    }
	caMidiOutputsCaptions.set(index, listMidiOutputDevicesNames[index].toUTF8());
	return const_cast <char*> (caMidiOutputsCaptions[index]);
}

char* DeviceMidi::getMidiOutputsSavedName (int index)
{
	captionMidiOutputsSavedName.set(index, "Out " + String(index + 1));
	caMidiOutputsSavedName.set(index, captionMidiOutputsSavedName[index].toUTF8 ());
	return const_cast <char*> (caMidiOutputsSavedName[index]);
}

//
void DeviceMidi::rescanMidi()
{
    
	sdkTraceLogChar("begin rescan midi devices");

	// store actual devices names and state
	// listMidiInputDevicesNames
	// int state = (int)sdkGetEvtData(m_ledMidiDeviceInputNames[numInput]);

	PropertySet inputsNameAndState;

	// store actual devices names and state
	for (int i = 0; i < maxMidiInputs; i++)
	{
		if (listMidiInputDevicesNames[i].isNotEmpty())
			inputsNameAndState.setValue(getMidiInputCaption(i), (int)sdkGetEvtData(m_ledMidiDeviceInputNames[i]));
	}

	// collect input devices names
	collectMidiInputsDeviceNames();
 
	for (int i = 0; i < maxMidiInputs; i++)
	{
        sdkSetParamCaption(PARAMS_BEFORE_MIDI_INS + i, getMidiInputCaption (i));

		// adjust visibility of the control panel button
		if (i < listMidiInputDevicesNames.size())
		{
            sdkSetParamVisible (PARAMS_BEFORE_MIDI_INS + i, TRUE);
            sdkRepaintParam (PARAMS_BEFORE_MIDI_INS + i);       

			// update the state (in case of order changed)
			if (inputsNameAndState.getIntValue(listMidiInputDevicesNames[i], -1) >= 0)
				sdkSetEvtData(m_ledMidiDeviceInputNames[i], inputsNameAndState.getIntValue(listMidiInputDevicesNames[i]));
        }
        else 
        {
            sdkSetParamVisible (PARAMS_BEFORE_MIDI_INS + i, FALSE);
			sdkRepaintParam(PARAMS_BEFORE_MIDI_INS + i);
			
			// update the state to off
			//sdkSetEvtData(m_ledMidiDeviceInputNames[i], 0);
        }
	}

	PropertySet outputsNameAndState;

	// store actual devices names and state
	for (int i = 0; i < maxMidiInputs; i++)
	{
		if (listMidiOutputDevicesNames[i].isNotEmpty())
			outputsNameAndState.setValue(getMidiOutputCaption(i), (int)sdkGetEvtData(m_ledMidiDeviceOutputNames[i]));
	}

	// collect output devices names
	collectMidiOutputsDeviceNames();

	for (int i = 0; i < maxMidiOutputs; i++)
	{
        sdkSetParamCaption((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) + i, getMidiOutputCaption (i));
        sdkSetParamCaption((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS + maxMidiOutputs) + i, getMidiOutputCaption (i));

		// adjust visibility of the control panel button
		if (i < listMidiOutputDevicesNames.size())
		{
			// outputs
            sdkSetParamVisible ((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) + i, TRUE);
            sdkRepaintParam ((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) + i);  

			// update the state (in case of order changed)
			if (outputsNameAndState.getIntValue(listMidiOutputDevicesNames[i], -1) >= 0)
				sdkSetEvtData(m_ledMidiDeviceOutputNames[i], outputsNameAndState.getIntValue(listMidiOutputDevicesNames[i]));
        }
        else 
        {
			// outputs
            sdkSetParamVisible ((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) + i, FALSE);
			sdkRepaintParam((PARAMS_BEFORE_MIDI_INS + maxMidiInputs + PARAMS_BEFORE_MIDI_OUTS) + i);
			
			// update the state to off
			//sdkSetEvtData(m_ledMidiDeviceOutputNames[i], 0);
        }
	}
	sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_MIDI_DRIVER_CHANGED, 0, 0);
	sdkTraceLogChar("finish rescan midi devices");

}

//
void DeviceMidi::resetMidi()
{
	// start from 1 because 0 is our virtual midi lan device
	for (int i = 1; i < listMidiInputDevicesNames.size(); i++)
	{
		if ((arrayMidiInputDevices[i - 1] != nullptr))
		{
			arrayMidiInputDevices[i - 1]->stop();
			arrayMidiInputDevices[i - 1]->start();
		}
	}
	sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_MIDI_DRIVER_CHANGED, 0, 0);
	sdkTraceChar("reset midi devices");
}

//
void DeviceMidi::setInputMidiDevice(int numInput)
{
	int state = (int)sdkGetEvtData(m_ledMidiDeviceInputNames[numInput]);

	if (numInput > 0 && (numInput <= arrayMidiInputDevices.size()))
	{
		if (arrayMidiInputDevices[numInput] != nullptr)
		{
			if (state)
			{
				arrayMidiInputDevices[numInput]->start();
			}
			else
			{
				arrayMidiInputDevices[numInput]->stop();

			}
		}
	}
	sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_MIDI_DRIVER_CHANGED, 0, 0);
}
		
//
void DeviceMidi::setOutputMidiDevice (int numOutput)
{
	try
	{
		int state = (int)sdkGetEvtData (m_ledMidiDeviceOutputNames[numOutput]);
		String midiOutputDeviceName = listMidiOutputDevicesNames[numOutput];
		
		StringArray midiOutsDevices = MidiOutput::getDevices();

		// remember the numoutput = 0 is our virtual midi lan
		if ((midiOutputDeviceName != String::empty) && numOutput > 0)
		{
			if (state && (arrayMidiOutputDevices[numOutput] == nullptr))
			{
                // use of numOutput - 1 because midilan isn't part of juce, but usine internal
				MidiOutput* outputDevice = MidiOutput::openDevice (numOutput - 1);
				arrayMidiOutputDevices.set(numOutput, outputDevice);
				outputDevice->startBackgroundThread ();
			}
			else if (!state && (arrayMidiOutputDevices[numOutput] != nullptr))
			{
				arrayMidiOutputDevices[numOutput]->stopBackgroundThread();
				arrayMidiOutputDevices.set(numOutput, nullptr);
			}
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in setOutputMidiDevice.");
	}
	sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_MIDI_DRIVER_CHANGED);
}


//-------------------------------------------------------------------------
// settings
void DeviceMidi::tryRestoreWantedSettings(bool recursive)
{
	// try to retrieve user choice from existing midi devices
	String choice;
		
	for (int i = 0; i < maxMidiInputs; i++)
	{
		sdkSetEvtData(m_ledMidiDeviceInputNames[i], 0.0);
		setInputMidiDevice(i);
	}

	for (int i = 0; i < maxMidiOutputs; i++)
	{
			sdkSetEvtData(m_ledMidiDeviceOutputNames[i], 0.0);
	}

	// for inputs
	for (int i = 0; i < maxMidiInputs; i++)
	{
		choice = listMidiInputWantedNames[i].unquoted();
		if (listMidiInputDevicesNames.contains(choice))
		{
			int newIndex = listMidiInputDevicesNames.indexOf(choice);
			sdkSetEvtData(m_ledMidiDeviceInputNames[newIndex], 1.0);
			setInputMidiDevice(newIndex);
		}
		//else
		//{
		//	if (choice != String::empty)
		//	{
		//		//String trace = "'" + choice + "' not found, turn in on and try rescan";
		//		//const char* mess = trace.toUTF8();
		//		//sdkTraceWarningPChar(const_cast <char*> (mess));
		//		missing = true;
		//	}
		//}
	}

	// for outputs
	for (int i = 0; i < maxMidiOutputs; i++)
	{
		choice = listMidiOutputWantedNames[i].unquoted();
		if (listMidiOutputDevicesNames.contains(choice))
		{
			int newIndex = listMidiOutputDevicesNames.indexOf(choice);
			sdkSetEvtData(m_ledMidiDeviceOutputNames[newIndex], 1.0);
			setOutputMidiDevice(newIndex);
		}
		//else
		//{
		//	if (choice != String::empty)
		//	{
		//		//String trace = "'" + choice + "' device not found, turn in on and try rescan";
		//		//const char* mess = trace.toUTF8();
		//		//sdkTraceWarningPChar(const_cast <char*> (mess));
		//		missing = true;
		//	}
		//}
	}
	//if (missing && recursive)
	//	popupRestoreWantedSettings();

	sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_MIDI_DRIVER_CHANGED, 0, 0);
}

void DeviceMidi::popupRestoreWantedSettings()
{
	String popupMessage = sdkGetTrans("Some MIDI device are missing:\nconnect them and press Ok\npress Cancel to abort");
	const char* mess = popupMessage.toUTF8();
	int result = sdkDialogConfirmationOKCancel(const_cast <char*> (mess));

	if (result == idOk) // 1 = ok, 2 = cancel
	{
		rescanMidi();
		tryRestoreWantedSettings(false);
	}
}

MidiMessage DeviceMidi::constructJuceMidiMessage (UsineMidiCode Code)
{
    MidiMessage juceMidiMessage;

    switch (Code.Msg)
    {
        // three bytes messages
    case MIDI_NOTEOFF:
    case MIDI_NOTEON:
    case MIDI_KEYAFTERTOUCH:
    case MIDI_CONTROLCHANGE:
    case MIDI_PITCHBEND:
    case MIDI_SONGPOSPTR:
        {
            juceMidiMessage = MidiMessage (Code.Msg, Code.Data1, Code.Data2);
            juceMidiMessage.setChannel(Code.Channel);
        }
        break;
        // two bytes messages
    case MIDI_PROGRAMCHANGE:
    case MIDI_CHANAFTERTOUCH:
    case MIDI_MTCQUARTERFRAME:
    case MIDI_SONGSELECT:
        {
            juceMidiMessage = MidiMessage (Code.Msg, Code.Data1);
            juceMidiMessage.setChannel(Code.Channel);
        }
        break;
        //so it's a one byte message
    case MIDI_BEGINSYSEX:
    default:
        {
            juceMidiMessage = MidiMessage (Code.Msg);
            juceMidiMessage.setChannel(Code.Channel);
        }
        break;
    }
    return juceMidiMessage;
}

//-------------------------------------------------------------------------
// input message
void DeviceMidi::handleIncomingMidiMessage (MidiInput *source, const MidiMessage &message)
{
	int deviceIndex = arrayMidiInputDevices.indexOf(source);

	//  sysex message
	if (message.isSysEx ())
	{		
		int sysexSize = message.getSysExDataSize ();
		const uint8* sysexData = message.getSysExData ();

		sdkUsineMidiSysexDeviceCallback (deviceIndex, 0.0, (char**)&sysexData, sysexSize);
	}
	// classique message
	else if ( message.getRawDataSize () <= 3)
	{

		int rawDataSize = message.getRawDataSize ();	
		const uint8* rawData = message.getRawData ();
        uint8 rawDataZero = rawData[0];

		midiCallbackDataBuffer.Channel = 0;
		midiCallbackDataBuffer.Data2 = 0;
		midiCallbackDataBuffer.Data1 = 0;
			
		// separate channel and message informations
		if ((rawData[0] >= 0x80) && (rawData[0] < 0xF0))
		{
			midiCallbackDataBuffer.Channel = (rawData[0] & 0x0F) + 1;
			rawDataZero = rawData[0] & 0xF0;
		}

		switch (rawDataSize)
		{
		case 3:
			midiCallbackDataBuffer.Data2 = rawData[2];
		case 2:
			midiCallbackDataBuffer.Data1 = rawData[1];
		case 1:
			midiCallbackDataBuffer.Msg = rawDataZero;
			break;
		default:
			// should never happen
			break;
		}
		sdkUsineMidiDeviceCallback (deviceIndex, message.getTimeStamp (), midiCallbackDataBuffer);
	}
	else
	{
		// unknown message;
	}
}

// input sysex
void DeviceMidi::handlePartialSysexMessage (MidiInput *source, const uint8 *messageData, const int numBytesSoFar, const double timestamp)	
{
    // complete sysex message are processed in the handleIncomingMidiMessage callback

	// int deviceIndex = listMidiInputDevicesNames.indexOf (source->getName());
    // sdkUsineMidiSysexDeviceCallback (deviceIndex, 0.0, (char**)&sysexData, sysexSize);
}


void DeviceMidi::collectMidiInputsDeviceNames()
{
	// closing and deleting existing devices
	for (int i = 0; i < arrayMidiInputDevices.size(); i++)
	{
		if (arrayMidiInputDevices[i] != nullptr)
		{
			arrayMidiInputDevices[i]->stop();
			arrayMidiInputDevices.remove(i, true);
		}
	}

	// collect input devices names
	listMidiInputDevicesNames = MidiInput::getDevices();
	// insert the virtual usine midi lan at the beginning
	listMidiInputDevicesNames.insert(0, MIDI_LAN);

	// for each device open the device and store the pointer (store a nullptr in case of MIDI LAN)
	for (int i = 0; i < listMidiInputDevicesNames.size(); i++)
	{
		arrayMidiInputDevices.set(i, MidiInput::openDevice(i - 1, this));
	}
    // in case two midi devices with the same name
    listMidiInputDevicesNames.appendNumbersToDuplicates (false, true,  CharPointer_UTF8 ("("),  CharPointer_UTF8 (")"));
}
	
void DeviceMidi::collectMidiOutputsDeviceNames()
{
	// collect output devices names
	listMidiOutputDevicesNames = MidiOutput::getDevices ();
	// insert the virtual usine midi lan at the beginning
	listMidiOutputDevicesNames.insert  (0, MIDI_LAN);

    // in case two midi devices with the same name
    listMidiOutputDevicesNames.appendNumbersToDuplicates (false, true,  CharPointer_UTF8 ("("),  CharPointer_UTF8 (")"));
}


