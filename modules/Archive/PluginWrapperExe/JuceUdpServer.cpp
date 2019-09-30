//-----------------------------------------------------------------------------
//@file  
//	JuceUdpServer.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//  Class that manage sending receiving packets through UPD ports.
//
//HISTORIC 
//	2019/09/22
//    first release for Hollyhock CPP SDK 6.04.006 
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

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "JuceUdpServer.h"
#include <functional>
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

JuceUdpServer::JuceUdpServer(PluginWrapperExe* owner, ParseCommandsProc parseCommands, int port, int portClient)
	: Thread("UDP_Listener Thread")
	, udpPort(port)
	, udpPortClient(portClient)
	, commandsOwner(owner)
	, parseCommandsProc(parseCommands)
	, bufferOutWritePos(0)
{
//	socket = new DatagramSocket();
	startThread(10);
}

JuceUdpServer::~JuceUdpServer()
{
	signalThreadShouldExit();

	// allow the thread 2 seconds to stop cleanly - should be plenty of time.
	stopThread(2000);
}


//-----------------------------------------------------------------------------
//
void JuceUdpServer::startCommandBloc()
{
	bufferOutWritePos = 0;
	bufferOut.reset();
}

//
void JuceUdpServer::addCommandBloc()
{
	try
	{
		int newSize = bufferOutWritePos + sizeof(commandBlock);
		bufferOut.append(&commandBlock, sizeof(commandBlock));
		bufferOutWritePos = newSize;
	}
	catch (...)
	{}
}

//
void JuceUdpServer::flushCommandBloc()
{
	if (bufferOutWritePos > 0)
	{
		socket.bindToPort(udpPortClient);
		int result = socket.waitUntilReady(false, 500);
		socket.write("127.0.0.1", udpPortClient, bufferOut.getData(), bufferOutWritePos);
	}
	bufferOutWritePos = 0;
}

//
void JuceUdpServer::sendCommandSimple(int cmdId)
{
	try
	{
		startCommandBloc();
		commandBlock.cmd = cmdId;
		addCommandBloc();
		flushCommandBloc();
	}
	catch (...)
	{
	}
}

void JuceUdpServer::addBuffer(void* buffer, int size)
{
	try
	{
		if (size > 0)
		{
			int newSize = bufferOutWritePos + size;
			bufferOut.ensureSize(newSize);
			bufferOut.copyFrom(buffer, bufferOutWritePos, size);
			bufferOutWritePos = newSize;
		}
	}
	catch (...)
	{
	}
}

/*
//-----------------------------------------------------------------------------
//
void JuceUdpServer::createServer(ParseCommandsProc parseCommandsProc, int port, int portClient)
{
	parseCommandsProc = parseCommandsProc;
	udpPort = port;
	udpPortClient = portClient;
}
*/

//-----------------------------------------------------------------------------
//
void JuceUdpServer::handleMessage(const Message& message)
{
	Message* inMsg = const_cast<Message*>(&message);
	JuceUdpServerMessage* myMessage = dynamic_cast<JuceUdpServerMessage*>(inMsg);

	auto CommandsProc = std::mem_fn(parseCommandsProc);
	CommandsProc(commandsOwner, myMessage->pointerParameter, 0);
	/*
	Message* inMsg = const_cast<Message*>(&message);
	JuceUdpServerMessage* myMessage = dynamic_cast<JuceUdpServerMessage*>(inMsg);
	if (myMessage)
	{
		if (myMessage->intParameter1 == 1) // 1 means I've received stuff.
		{
			int length = myMessage->intParameter2;

			ScopedPointer <MemoryBlock> data(static_cast <MemoryBlock*> (myMessage->pointerParameter));

			// Here you can do whatever you see fit to the packet received.
			//		 ProcessPacket(...);

		}
	}
	*/
}

void JuceUdpServer::run()
{
	messageData = new MemoryBlock(65535, true);
	//char tmp[65535];
	socket.bindToPort(udpPort);

	int readyForReading;
	int bytesRead;

	// threadShouldExit() returns true when the stopThread() method has been
	// called, so we should check it often, and exit as soon as it gets flagged.
	while (!threadShouldExit())
	{

		// need a timeout to let the while loop turn and check "threadShouldExit()"
		readyForReading = socket.waitUntilReady(true, 1000);

		if (readyForReading > 0)
		{
			//bytesRead = socket.read(tmp, 65535, false);
			bytesRead = socket.read(messageData->getData(), 65535, false);


			if (bytesRead > 0)
			{
				//messageData->setSize(bytesRead);
				postMessage(new JuceUdpServerMessage(1, bytesRead, new MemoryBlock(messageData->getData(), bytesRead)));
				
				//auto CommandsProc = std::mem_fn(parseCommandsProc);
				//CommandsProc(commandsOwner, messageData, 0);
			}
		}
	}
}