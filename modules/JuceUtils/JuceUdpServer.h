//-----------------------------------------------------------------------------
//@file  
//	JuceUdpServer.h
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
#ifndef __JUCEUDP_SERVER_H__
#define __JUCEUDP_SERVER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <JuceHeader.h>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------
class ServerListener
{
public:
    void parseCommands(MemoryBlock* dataBlock, int dataIndex, bool loopMod) {};
};

typedef void(ServerListener::*ParseCommandsProc) (MemoryBlock* data, int dataIndex, bool loopMod);

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
//
class JuceUdpServerMessage : public Message
{
public:
	JuceUdpServerMessage(int param1, int param2, MemoryBlock* dataParam) : intParameter1(param1), intParameter2(param2), pointerParameter(dataParam)
	{}

public:
	int intParameter1;
	int intParameter2;
	ScopedPointer <MemoryBlock> pointerParameter;
	//TCommandPacket content;
};





//
class JuceUdpServer : public Thread, private MessageListener
{
public:
	JuceUdpServer(ServerListener* owner, ParseCommandsProc parseCommands, int port, int portClient);

	~JuceUdpServer();

	//-----------------------------------------------------------------------------
	//
	TCommandPacket commandBlock;

	//-----------------------------------------------------------------------------
	//
	void startCommandBloc();
	void addCommandBloc();
	void flushCommandBloc();
	void sendCommandSimple(int cmdId);

	void addBuffer(void* buffer, int size);
	void addTextValue(String text);

	//-----------------------------------------------------------------------------
	//
	//void createServer(ParseCommandsProc parseCommands, int port, int portClient);

	//-----------------------------------------------------------------------------
	//
	void handleMessage(const Message& message) override;

	void run() override;

protected:

private:
    
	int udpPort;
	int udpPortClient;

	//std::bind(&Class::functionName, objPtr, _1);
	MemoryBlock bufferInTmp;
	MemoryBlock bufferOut;
	int bufferOutWritePos;

	ServerListener* commandsOwner;
	ParseCommandsProc parseCommandsProc;

	ScopedPointer <MemoryBlock>		messageData;
	DatagramSocket	socket;


	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceUdpServer);

}; // class JuceUdpServer

#endif //__JUCEUDP_SERVER_H__