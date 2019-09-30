//-----------------------------------------------------------------------------
//@file  
//	PluginLister.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the PluginLister class.
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
#include "PluginWrapperExe.h"
#include <iomanip>
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------


PluginWrapperExe::PluginWrapperExe()
	: pluginInstance(nullptr)
	, lastEditorPosX(50)
	, lastEditorPosY(50)
	, currentSampleRate(44100)
	, currentUsineBlockSize(128)
	, isBypass(false)
	, isInstrument(false)
	, isPlugMissing(false)
	, isProducingMidi(false)
	, timeLastParameterChanged(0.0)
	, lastParamChangeIndex(-2)
	, processCounter(0)
{
	//namesProgList.add("prog1");
	//namesProgList.add("prog2");
	//namesProgList.add("prog3");
	//namesProgList.add("prog4");
	//namesProgList.add("prog5");
	//namesProgList.add("prog6");
	//namesProgList.add("prog7");
	//namesProgList.add("prog8");

	// for test purpose
	listProgNames.add("1");
	listProgNames.add("2");

	LookAndFeel::setDefaultLookAndFeel(usineLook = new UsineLook());

	// for logging purpose
	listPacketCmd.add("");
	listPacketCmd.add("CMD_BLOC_CHANGED");
	listPacketCmd.add("CMD_SAMPLE_RATE_CHANGED");
	listPacketCmd.add("CMD_PARAM_VALUE");
	listPacketCmd.add("CMD_CHUNK");
	listPacketCmd.add("CMD_SHOW_PLUGIN");
	listPacketCmd.add("CMD_HIDE_PLUGIN");
	listPacketCmd.add("CMD_TOGGLE_SHOW_HIDE_PLUGIN");
	listPacketCmd.add("CMD_PROGRAM_NUM");
	listPacketCmd.add("CMD_CHUNK_REQUEST");
	listPacketCmd.add("CMD_KILL");
	listPacketCmd.add("CMD_PING");
	listPacketCmd.add("CMD_PLUGIN_NAME");
	listPacketCmd.add("CMD_NUM_CHANELS_IN");
	listPacketCmd.add("CMD_NUM_CHANELS_OUT");
	listPacketCmd.add("CMD_NUM_PARAMS");
	listPacketCmd.add("CMD_PARAM_NAME");
	listPacketCmd.add("CMD_SET_PARAM_VISIBLE");
	listPacketCmd.add("NOT_USED");
	listPacketCmd.add("NOT_USED");
	listPacketCmd.add("CMD_SET_ALL_PARAM_VISIBLE");
	listPacketCmd.add("CMD_PROGRAM_LIST");
	listPacketCmd.add("CMD_KEYPRESSED");
	listPacketCmd.add("CMD_READY");
	listPacketCmd.add("CMD_PONG");
	listPacketCmd.add("CMD_PROCESS");
	listPacketCmd.add("CMD_PARAM_DISPLAY_VALUE");


	pluginFormatManager = new AudioPluginFormatManager();
	pluginFormatManager->addDefaultFormats();

}



void PluginWrapperExe::initialise(const String& commandLine)
{
	// parse the commandLine
	//commandLine.
	StringArray commands;
	commands.addTokens(commandLine, true);

	if (commands.size() != 5)
	{
		// problem with commands

	}

	//for (int i = 0; i < commands.size(); i++)
	//{
	//	String cmd = commands[i];
	//}

	pathPluginFile = commands[0];

	portCommandUsine = commands[1].getIntValue();
	portAudioUsine = commands[2].getIntValue();

	portCommandWrapper = commands[3].getIntValue();
	portAudioWrapper = commands[4].getIntValue();

	socketCommand = new JuceUdpServer(this, &PluginWrapperExe::parseCommands, portCommandWrapper, portCommandUsine);
	socketAudio = new JuceUdpServer(this, &PluginWrapperExe::parseProcess, portAudioWrapper, portAudioUsine);

//	int toto = sizeof(TCommandPacket);
//	setVisible();

	Logger::writeToLog("pathPluginFile=" + pathPluginFile);
	Logger::writeToLog("portCommandWrapper=" + String(portCommandWrapper));
	Logger::writeToLog("portAudioWrapper=" + String(portAudioWrapper));
	Logger::writeToLog("portCommandUsine=" + String(portCommandUsine));
	Logger::writeToLog("portAudioUsine=" + String(portAudioUsine));


	//testFakePlugin();

	createPlugin(pathPluginFile.unquoted());

	updateModuleParamsName();
	preparePlugin();
	initBuffers();
	setVisible();
	sendPlugInfosToUsine();
}


//-----------------------------------------------------------------------------
//
void PluginWrapperExe::parseProcess(MemoryBlock* dataBlock, int dataIndex)
{
	//logCommands(dataBlock, dataIndex);
	//

	if (dataBlock->getSize() <= dataIndex)
	{
		// error
		return;
	}

	TCommandPacket* packet = (TCommandPacket*)(((char*)dataBlock->getData()) + dataIndex);


	if (packet->cmd == PW_CMD_PROCESS)
	{
		process(packet, dataBlock, dataIndex + sizeof(TCommandPacket));
	}
	else
	{
		// trace('error bad command in parseProcess');
	}

	dataIndex += sizeof(TCommandPacket);
	parseProcess(dataBlock, dataIndex);

	/*
	     if length(Adata)<=AreadIdx
     then begin
//        trace('error bad length in parseProcess');
        exit;
     end;

     P := PTCommandPacket(@AData[AreadIdx]);
     case P.cmd of
       PW_CMD_PROCESS: begin
                       Process(P,@AData[AreadIdx+sizeof(TCommandPacket)],AreadIdx);
                      // exit;
                       end;
        else trace('error bad command in parseProcess');
     end;
     inc(AreadIdx,sizeof(TCommandPacket));
     ParseProcess(AData,AreadIdx);
	 */
}

//
void PluginWrapperExe::parseCommands(MemoryBlock* dataBlock, int dataIndex)
{
	logCommands(dataBlock, dataIndex);
	
	if (dataBlock->getSize() <= dataIndex)
	{
		return;
	}

	TCommandPacket* packet = (TCommandPacket*)(((char*)dataBlock->getData()) + dataIndex);

	//
	if (packet->cmd == PW_CMD_BLOC_CHANGED)
	{
		currentUsineBlockSize = packet->intValue1;
	}
	else if (packet->cmd == PW_CMD_SAMPLE_RATE_CHANGED)
	{
		currentSampleRate = packet->intValue1;
	}
	else if (packet->cmd == PW_CMD_CHUNK)
	{
		// pluginInstance->setStateInformation(currentPlugStateInfo.getData(), sizePlugChunk);

		socketCommand->sendCommandSimple(PW_CMD_READY);
	}
	else if (packet->cmd == PW_CMD_CHUNK_REQUEST)
	{
		sendChunck();
	}
	else if (packet->cmd == PW_CMD_PROGRAM_NUM)
	{
		// trace('program num='+inttostr(P.intvalue1));
	}
	else if (packet->cmd == PW_CMD_PING)
	{
		socketCommand->sendCommandSimple(PW_CMD_PONG);
	}
	else if (packet->cmd == PW_CMD_SHOW_PLUGIN)
	{
		setVisible(1);
	}
	else if (packet->cmd == PW_CMD_HIDE_PLUGIN)
	{
		setVisible(0);
	}
	else if (packet->cmd == PW_CMD_TOGGLE_SHOW_HIDE_PLUGIN)
	{
		if (pluginWindow->isVisible())
			setVisible(0);
		else
			setVisible(1);
	}
	else if (packet->cmd == PW_CMD_KILL)
	{
		systemRequestedQuit();
	}
	else if (packet->cmd == PW_CMD_PARAM_VALUE)
	{
		setParamDisplayValue(packet->intValue1, packet->floatValue);
	}

	dataIndex += sizeof(TCommandPacket);
	parseCommands(dataBlock, dataIndex);
}

//
void PluginWrapperExe::process(TCommandPacket* commandBlock, MemoryBlock* dataBlock, int dataIndex)
{

	try
	{   // zeroing the midi event size in case we have no more activity
		//sdkSetEvtSize(m_midiOut, 0);

		/*
		if ((*m_masterInfo->NeedAllNoteOff) != FALSE
			//&& isInstrument
			&& pluginInstance->acceptsMidi()
			)
		{
			usineEventsToMidiBuffer(m_masterInfo->AllNotOffEvent);
		}
		*/

		int commandsSize;
		// populate buffers from audio and midi ins currentUsineBlockSize
		//bufferAudioInOut->setSize(bufferAudioInOut->getNumChannels(), commandBlock->sizes[0]);
		bufferAudioInOut->setSize(bufferAudioInOut->getNumChannels(), currentUsineBlockSize);
		for (int i = 0; i < numOfAudioIn; ++i)
		{
			commandsSize = commandBlock->sizes[i];
			if (commandsSize > 0)
			{
				//bufferAudioInOut->copyFrom(i, 0, ((TPrecision*)dataBlock->getData()) + dataIndex, commandsSize);
				dataBlock->copyTo(bufferAudioInOut->getWritePointer(i), dataIndex, commandsSize* sizeof(TPrecision));
				dataIndex += (commandsSize* sizeof(TPrecision));
									
			}
		}


		int tmp = sizeof(TCommandPacket);

		if (commandBlock->nbMidi > 0)
		{
			for (int i = 0; i < commandBlock->nbMidi; ++i)
			{
				dataBlock->copyTo((void*)&usineMidiOutTmp, dataIndex, sizeof(UsineMidiCode));
				dataIndex += sizeof(UsineMidiCode);

				if (usineMidiOutTmp.Msg != 0)
				{
					MidiMessage juceMidiTmp(usineMidiOutTmp.Msg, usineMidiOutTmp.Data1, usineMidiOutTmp.Data2, Time::getMillisecondCounterHiRes() * 0.001);
					juceMidiTmp.setChannel(usineMidiOutTmp.Channel);
					collectorMidiMessages->addMessageToQueue(juceMidiTmp);
				}
			}
		}
		/*
		UsineMidiCode usineMidiTmp;
		//MidiMessage juceMidiTmp;
		for (int i = 0; i < sdkGetEvtSize(eventSource); ++i)
		{
			usineMidiTmp = sdkGetEvtArrayMidi(eventSource, i);
			if (usineMidiTmp.Msg != 0)
			{
				//VstTimeInfo* tmp = sdkGetVstTimeInfo ();
				//MidiMessage juceMidiTmp ( usineMidiTmp.Msg, usineMidiTmp.Data1, usineMidiTmp.Data2, sdkGetVstTimeInfo ()->samplePos);
				//if (usineMidiTmp.Data2 == 0)
				//    usineMidiTmp.Data2 = 100;
				MidiMessage juceMidiTmp(usineMidiTmp.Msg, usineMidiTmp.Data1, usineMidiTmp.Data2, Time::getMillisecondCounterHiRes() * 0.001);
				juceMidiTmp.setChannel(usineMidiTmp.Channel);
				collectorMidiMessages->addMessageToQueue(juceMidiTmp);
			}
		}
		*/
		processCounter++;
		if (commandBlock->processCounter != processCounter)
		{
			Logger::writeToLog("process, lost : " + String(commandBlock->processCounter - processCounter));
			processCounter = commandBlock->processCounter;
		}


		if (!isBypass)
		{
			collectorMidiMessages->removeNextBlockOfMessages(*bufferMidiMessages, currentUsineBlockSize);
			// process buffers by the plug
			plugAudioPlayHead->setTimeInfo(commandBlock->timeInfo);
			pluginInstance->processBlock(*bufferAudioInOut, *bufferMidiMessages);

			// update audio  outs
			//*/
			if (numOfAudioOut > 0)
			{
				socketAudio->startCommandBloc();
				socketAudio->commandBlock.cmd = PW_CMD_PROCESS;

				for (int i = 0; i < MAX_WAVE_CHANNELS; i++)
					socketAudio->commandBlock.sizes[i] = 0;

				for (int i = 0; i < numOfAudioOut; ++i)
				{
					socketAudio->commandBlock.sizes[i] = bufferAudioInOut->getNumSamples();
				}
				socketAudio->commandBlock.nbMidi = 0;

				socketAudio->addCommandBloc();
				// buffers for audio
				for (int i = 0; i < numOfAudioOut; ++i)
				{
					socketAudio->addBuffer((void*)bufferAudioInOut->getReadPointer(i), sizeof(TPrecision)*bufferAudioInOut->getNumSamples());
				}
				// buffer for midi
				//ProcessSocket.AddBuffer(MidiIn.len*SizeOf(TCodeMIDI), PByte(MidiIn.Pvalue));
				socketAudio->flushCommandBloc();
			}
			/*
			// update midi outs
			if (isProducingMidi)
			{
				int samplePos;
				int i = 0;
				itorMidiOutMessages->setNextSamplePosition(0);
				while (itorMidiOutMessages->getNextEvent(juceMidiOutTmp, samplePos))
				{
					if (juceMidiOutTmp.getRawDataSize() <= 3)
					{
						int rawDataSize = juceMidiOutTmp.getRawDataSize();
						const uint8* rawData = juceMidiOutTmp.getRawData();

						usineMidiOutTmp.Channel = (unsigned char)juceMidiOutTmp.getChannel();
						usineMidiOutTmp.Data2 = 0;
						usineMidiOutTmp.Data1 = 0;

						switch (rawDataSize)
						{
						case 3:
							usineMidiOutTmp.Data2 = rawData[2];
						case 2:
							usineMidiOutTmp.Data1 = rawData[1];
						case 1:
							usineMidiOutTmp.Msg = rawData[0];
							break;
						default:
							// should never arrive
							break;
						}
					}
					sdkSetEvtArrayMidi(m_midiOut, i, usineMidiOutTmp);
					i++;
				}
				if (i > 0)
				{
					sdkRepaintParam(offsetMidiOut);
					bufferMidiMessages->clear();
				}
			}
			*/
		}
		else
		{
			// passthru if corresponding input else set size to 0
			//for (int i = 0; i < numOfAudioOut; ++i)
			//{
			//	if (m_audioIns[i] != nullptr)
			//		sdkCopyEvt(m_audioIns[i], m_audioOuts[i]);
			//	else
			//		sdkSetEvtSize(m_audioOuts[i], 0);
			//}

			socketAudio->startCommandBloc();
			socketAudio->commandBlock.cmd = PW_CMD_PROCESS;

			for (int i = 0; i < MAX_WAVE_CHANNELS; i++)
				socketAudio->commandBlock.sizes[i] = 0;

			for (int i = 0; i < numOfAudioOut; ++i)
			{
				socketAudio->commandBlock.sizes[i] = bufferAudioInOut->getNumSamples();
			}
			socketAudio->commandBlock.nbMidi = 0;

			socketAudio->addCommandBloc();
			// buffers for audio
			for (int i = 0; i < numOfAudioOut; ++i)
			{
				socketAudio->addBuffer(bufferAudioInOut->getWritePointer(i), sizeof(TPrecision)*currentUsineBlockSize);
			}
			// buffer for midi
			//ProcessSocket.AddBuffer(MidiIn.len*SizeOf(TCodeMIDI), PByte(MidiIn.Pvalue));
			socketAudio->flushCommandBloc();

		}

	}
	catch (...)
	{
		//sdkTraceErrorChar(" in Process");
		Logger::writeToLog("error in Process");
	}
}

//
void PluginWrapperExe::testFakePlugin()
{
	pluginName = CharPointer_UTF8("PluginWrapperExe é test");
	numOfAudioIn    = 4;
	numOfAudioOut   = 4;
	numOfParamsInOut = 16;

	// audio buffer
	bufferAudioInOut = new AudioSampleBuffer(std::max(1, std::max(numOfAudioIn, numOfAudioOut)), currentUsineBlockSize);

	// midi buffer
	bufferMidiMessages = new MidiBuffer();
	itorMidiOutMessages = new MidiBuffer::Iterator(*bufferMidiMessages);
	collectorMidiMessages = new MidiMessageCollector();
	collectorMidiMessages->reset(currentSampleRate);

	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_CHANELS_IN;
	socketCommand->commandBlock.intValue1 = numOfAudioIn;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_CHANELS_OUT;
	socketCommand->commandBlock.intValue1 = numOfAudioOut;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_PARAMS;
	socketCommand->commandBlock.intValue1 = numOfParamsInOut;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_PLUGIN_NAME;
	pluginName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
	socketCommand->addCommandBloc();
	socketCommand->flushCommandBloc();

	String paramName;
	for (int i = 0; i < numOfParamsInOut; i++)
	{
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_NAME;
		socketCommand->commandBlock.intValue1 = i;
		paramName = String("p") + String(i + 1);
		paramName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_VALUE;
		socketCommand->commandBlock.intValue1 = i;
		socketCommand->commandBlock.floatValue = 0;
		PARAM_VALUE_FORMAT.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->flushCommandBloc();
	}

	
	String progsList = listProgNames.joinIntoString(",");

	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_PROGRAM_LIST;
	socketCommand->commandBlock.intValue1 = progsList.getNumBytesAsUTF8();
	socketCommand->addCommandBloc();
	socketCommand->addBuffer((void*)progsList.toRawUTF8(), progsList.getNumBytesAsUTF8());
	socketCommand->flushCommandBloc();
	

	for (int i = 0; i < numOfParamsInOut; i++)
	{
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_NAME;
		socketCommand->commandBlock.intValue1 = i;
		paramName = String("p") + String(i + 1);
		paramName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_VALUE;
		socketCommand->commandBlock.intValue1 = i;
		socketCommand->commandBlock.floatValue = 0;
		PARAM_VALUE_FORMAT.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->flushCommandBloc();
	}

	socketCommand->sendCommandSimple(PW_CMD_READY);
}


void PluginWrapperExe::setVisible(LongBool isVisible)
{
	try
	{
		const MessageManagerLock mmLock;
		//if (pluginInstance->hasEditor())
		{
			// make visible
			if (isVisible != FALSE)
			{
				if (pluginWindow != nullptr)
				{
					pluginWindow->setVisible(true);
				}
				else
				{
					pluginWindow = PluginWindow::getWindowFor(this, pluginInstance);
				}
				//sdkSetEvtData(m_visibleEditor, 1.0);
				pluginWindow->toFront(true);
				//sdkRepaintParam(offsetVisibleIn);
			}
			// make invisible
			else
			{
				//sdkNotifyUsine(NOTIFY_TARGET_USINE, NOTIFY_MSG_FOCUS_LOST);
				if (pluginWindow != nullptr)
				{
					pluginWindow->setVisible(false);

					//sdkSetEvtData(m_visibleEditor, FALSE);
					//sdkRepaintParam(offsetVisibleIn);
				}
			}
		}
	}
	catch (...)
	{
	}
}


//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//
void PluginWrapperExe::preparePlugin()
{
	if (pluginInstance != nullptr)
	{
		// store the state of the plugin
		juce::MemoryBlock plugState;
		pluginInstance->getStateInformation(plugState);

		plugAudioPlayHead = new UsineAudioPlayHead();
		pluginInstance->setPlayHead(plugAudioPlayHead);
		pluginInstance->prepareToPlay(currentSampleRate, currentUsineBlockSize);

		pluginInstance->setStateInformation(plugState.getData(), plugState.getSize());
	}

}

//
void PluginWrapperExe::initBuffers()
{
	try
	{
		if (pluginInstance != nullptr)
		{
			pluginInstance->suspendProcessing(true);

			// audio buffer
			bufferAudioInOut = new AudioSampleBuffer(std::max(1, std::max(numOfAudioIn, numOfAudioOut)), currentUsineBlockSize);
			
			/*
			if (numOfAudioOut > 0)
			{
				for (int i = 0; i < numOfAudioOut; ++i)
					sdkSetEvtSize(m_audioOuts[i], currentUsineBlockSize);

				sdkSetEvtSize(m_arrParamsInOut, numOfParamsInOut);
			}
			*/

			// midi buffer
			bufferMidiMessages = new MidiBuffer();
			itorMidiOutMessages = new MidiBuffer::Iterator(*bufferMidiMessages);
			collectorMidiMessages = new MidiMessageCollector();
			collectorMidiMessages->reset(currentSampleRate);
			// to force updating all modules params
			//pluginInstance->setCurrentProgram (1);

			pluginInstance->addListener(this);
			pluginInstance->suspendProcessing(false);
		}
		else if (lastError != String::empty)
		{
			//pluginName
			//sdkTraceErrorChar((AnsiCharPtr)lastError.toUTF8().getAddress());
			Logger::writeToLog("initBuffers error=" + lastError);

		}
	}
	catch (...)
	{
		//sdkTraceErrorChar("in initBuffers");
		Logger::writeToLog("error in initBuffers");
	}
}

//populate the params name list
void PluginWrapperExe::populateProgramsNameList()
{
	listProgNames.clear();
	for (int i = 0; i < pluginInstance->getNumPrograms(); ++i)
	{
		String name(pluginInstance->getProgramName(i));
		listProgNames.add(name);
	}
	offsetProgIn = offsetMidiOut + numOfMidiOut;
	StringArrayToCommaString(qsProgInName, listProgNames, false);


	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_PROGRAM_LIST;
	socketCommand->commandBlock.intValue1 = qsProgInName.getNumBytesAsUTF8();
	socketCommand->addCommandBloc();
	socketCommand->addBuffer((void*)qsProgInName.toRawUTF8(), qsProgInName.getNumBytesAsUTF8());
	socketCommand->flushCommandBloc();
}
//populate the params name list
void PluginWrapperExe::populateParamsNameList()
{
	if (pluginInstance != nullptr)
	{
		listParamInOutNames.clear();
		listParamInOutIndex.clear();
		for (int i = 0; i < numOfParamsInOut; ++i)
		{
			String name(pluginInstance->getParameterName(i));

			//if (name == String::empty || name == " ")
			//	name = String("p-") + String(i);
			String nameReaktorNotUsed("P");
			nameReaktorNotUsed << String(i + 1).paddedLeft('0', 4);

			if (name.isNotEmpty()
				&& (name != nameReaktorNotUsed)
				&& !name.containsAnyOf("{}#")
				)
			{
				listParamInOutNames.add(name);
				listParamInOutIndex.add(i);
			}
		}
		//sdkSetEvtSize(m_arrParamsInOut, listParamInOutNames.size());
	}
}

void PluginWrapperExe::updateModuleParamsName()
{
	if (pluginInstance != nullptr)
	{
		populateParamsNameList();
		for (int i = 0; i < listParamInOutNames.size(); ++i)
		{
			//construct the name
			//caStringToUTF8Buffer = listParamInOutNames[i].toUTF8();
			//sdkSetParamCaption(offsetParamsInOut + listParamInOutIndex[i], const_cast <char*> (caStringToUTF8Buffer));
		}
		StringArrayToCommaString(qsParamNamesOut, listParamInOutNames, false);
		//caStringToUTF8Buffer = qsParamNamesOut.toUTF8();
		//sdkSetEvtPChar(m_txtParamsNamesOut, const_cast <char*> (caStringToUTF8Buffer));
	}
};

void PluginWrapperExe::setParamDisplayValue(int index, TPrecision newValue)
{
	/*
	sdkSetEvtData(m_fdrParamsInsOuts[index], newValue);

	paramTmpValueText = pluginInstance->getParameterText(index);
	if (paramTmpValueText.isNotEmpty())
	{
		const char* caValueText = paramTmpValueText.toUTF8();
		sdkSetParamValueText(offsetParamsInOut + index, const_cast <char*> (caValueText));
		sdkRepaintParam(offsetParamsInOut + index);
	}
	else
	{
		sdkSetParamValueText(offsetParamsInOut + index, nullptr);
	}
	sdkSetEvtArrayData(m_arrParamsInOut, index, newValue);
	*/

	try
	{
		paramTmpValueText = pluginInstance->getParameterText(index);
		//if (paramTmpValueText.isNotEmpty())
		//{
		//	const char* caValueText = paramTmpValueText.toUTF8();
		//	sdkSetParamValueText(offsetParamsInOut + index, const_cast <char*> (caValueText));
		//	sdkRepaintParam(offsetParamsInOut + index);
		//}

		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_DISPLAY_VALUE;
		socketCommand->commandBlock.intValue1 = index;
		socketCommand->commandBlock.floatValue = 0;
		paramTmpValueText.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->flushCommandBloc();
	}
	catch (...)
	{
	}
}
void PluginWrapperExe::updateParams()
{
	//sdkSetEvtData(m_lboxProgramIn, (float)pluginInstance->getCurrentProgram());
	// program changed so update all params
	for (int i = 0; i < numOfParamsInOut; ++i)
	{
		float value = pluginInstance->getParameter(i);
		setParamDisplayValue(i, value);
	}
}

void PluginWrapperExe::sendChunck()
{

	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_CHUNK;
	socketCommand->commandBlock.intValue1 = 0;
	socketCommand->commandBlock.intValue1 = 0; //longlool, preset or not
	//socketCommand->addCommandBloc();
	//socketCommand->addBuffer(currentPlugStateInfo.getData(), currentPlugStateInfo.getSize());
	socketCommand->flushCommandBloc();
	/*
	pluginInstance->getStateInformation(currentPlugStateInfo);

	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_CHUNK;
	socketCommand->commandBlock.intValue1 = currentPlugStateInfo.getSize();
	socketCommand->commandBlock.intValue1 = 0; //longlool, preset or not
	socketCommand->addCommandBloc();
	socketCommand->addBuffer(currentPlugStateInfo.getData(), currentPlugStateInfo.getSize());
	socketCommand->flushCommandBloc();
	*/
}

void PluginWrapperExe::createPlugin(String plugFilePath)
{
	File plug(plugFilePath);
	
	if ((plug.existsAsFile()) && (plug.getFileExtension().contains(PLUG_INS_EXT)))
	{
		StringArray plugInfos;
		plug.readLines(plugInfos);
		pluginDescription.fileOrIdentifier = plugInfos[1];

		pluginInstance = createPluginInstanceFromDesc(pluginDescription);
	}
}

AudioPluginInstance* PluginWrapperExe::createPluginInstanceFromDesc(PluginDescription& plugDesc)
{
	AudioPluginInstance* pluginInstance = pluginFormatManager->createPluginInstance(plugDesc, currentSampleRate, currentUsineBlockSize, lastError);
	if (pluginInstance != nullptr)
	{
		pluginName = pluginInstance->getName();
		numOfAudioIn = pluginInstance->getNumInputChannels();
		numOfAudioOut = pluginInstance->getNumOutputChannels();

		if (pluginInstance->producesMidi())
			isProducingMidi = true;

		//populateProgramsNameList();

		pluginInstance->fillInPluginDescription(plugDesc);
		isInstrument = plugDesc.isInstrument;
	}
	else
	{
		isInstrument = false;
	}


	return pluginInstance;
}


void PluginWrapperExe::sendPlugInfosToUsine()
{
	socketCommand->startCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_CHANELS_IN;
	socketCommand->commandBlock.intValue1 = numOfAudioIn;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_CHANELS_OUT;
	socketCommand->commandBlock.intValue1 = numOfAudioOut;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_NUM_PARAMS;
	socketCommand->commandBlock.intValue1 = numOfParamsInOut;
	socketCommand->addCommandBloc();
	socketCommand->commandBlock.cmd = PW_CMD_PLUGIN_NAME;
	pluginName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
	socketCommand->addCommandBloc();
	socketCommand->flushCommandBloc();

	String paramName;
	for (int i = 0; i < numOfParamsInOut; i++)
	{
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_NAME;
		socketCommand->commandBlock.intValue1 = i;
		paramName = String("p") + String(i + 1);
		paramName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_VALUE;
		socketCommand->commandBlock.intValue1 = i;
		socketCommand->commandBlock.floatValue = 0;
		PARAM_VALUE_FORMAT.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->flushCommandBloc();
	}


	//String progsList = listProgNames.joinIntoString(",");

	//socketCommand->startCommandBloc();
	//socketCommand->commandBlock.cmd = PW_CMD_PROGRAM_LIST;
	//socketCommand->commandBlock.intValue1 = progsList.getNumBytesAsUTF8();
	//socketCommand->addCommandBloc();
	//socketCommand->addBuffer((void*)progsList.toRawUTF8(), progsList.getNumBytesAsUTF8());
	//socketCommand->flushCommandBloc();
	populateProgramsNameList();

	for (int i = 0; i < numOfParamsInOut; i++)
	{
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_NAME;
		socketCommand->commandBlock.intValue1 = i;
		paramName = String("p") + String(i + 1);
		paramName.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = PW_CMD_PARAM_VALUE;
		socketCommand->commandBlock.intValue1 = i;
		socketCommand->commandBlock.floatValue = 0;
		PARAM_VALUE_FORMAT.copyToUTF8(socketCommand->commandBlock.textValue, TEXT_VALUE_SIZE);
		socketCommand->startCommandBloc();
		socketCommand->flushCommandBloc();
	}

	socketCommand->sendCommandSimple(PW_CMD_READY);
}
//-----------------------------------------------------------------------------
//
void PluginWrapperExe::handleMessage(const Message& message)
{
	//Message* inMsg = const_cast<Message*>(&message);
	//JuceUdpServerMessage* myMessage = dynamic_cast<JuceUdpServerMessage*>(inMsg);

	//auto CommandsProc = std::mem_fn(parseCommandsProc);
	//CommandsProc(commandsOwner, myMessage->pointerParameter, 0);

}

//-------------------------------------------------------------------------
// from AudioProcessorListener interface
void PluginWrapperExe::audioProcessorParameterChanged(AudioProcessor *processor, int parameterIndex, float newValue)
{
	double newTime = Time::currentTimeMillis();

	// make param visible if beginning a 'gesture' on a ui param of the plug
	if (newTime > timeLastParameterChanged + PARAM_CHANGE_TIME_MS)
	{
		//showModuleParam(parameterIndex);
		timeLastParameterChanged = newTime;
	}

	// some plugins (like Mcompressor) use change param when we change preset
	// workaround to detect if the plug is applying a preset
	// by checking if the two last params are changed successively
	if (parameterIndex == pluginInstance->getNumParameters() - 1
		&& lastParamChangeIndex == parameterIndex - 1
		)
	{
		//std::ostringstream os;
		//os << "delta time: " << (newTime - timeLastParameterChanged) << ", param index: " << parameterIndex;
		//stringTrace =  os.str();
		//sdkTraceLogChar ((AnsiCharPtr) stringTrace.c_str());

		timeLastParameterChanged = newTime;
		audioProcessorChanged(pluginInstance);
	}

	// update value
	setParamDisplayValue(parameterIndex, newValue);

	lastParamChangeIndex = parameterIndex;
}

// from AudioProcessorListener interface
void PluginWrapperExe::audioProcessorChanged(AudioProcessor *processor)
{
	//sdkTraceLogChar("audioProcessorChanged");
	populateProgramsNameList();
	const char* caProgInName = qsProgInName.toUTF8();
	//sdkSetListBoxCommatext(offsetProgIn, const_cast <char*> (caProgInName));
	//  TODO : reconstruir la list de programme et les noms des params
	if (processor->getCurrentProgram() != currentProg)
	{
		//sdkSetEvtData(m_lboxProgramIn, (float)processor->getCurrentProgram());
		currentProg = processor->getCurrentProgram();

		if (pluginWindow != nullptr)
		{
			String presetName = processor->getProgramName(processor->getCurrentProgram());
			pluginWindow->setName(processor->getName() + "-" + presetName);
		}
	}

	updateParams();
	updateModuleParamsName();
}


//-----------------------------------------------------------------------------
// 
void PluginWrapperExe::logCommands(MemoryBlock* dataBlock, int dataIndex)
{

	if (dataBlock->getSize() <= dataIndex)
	{
		return;
	}

	TCommandPacket* packet = (TCommandPacket*)(((char*)dataBlock->getData()) + dataIndex);

	Logger::writeToLog("Command.cmd=" + String(packet->cmd) + String(" (" + listPacketCmd[packet->cmd]) + String(")"));

	dataIndex += sizeof(TCommandPacket);
	logCommands(dataBlock, dataIndex);

}