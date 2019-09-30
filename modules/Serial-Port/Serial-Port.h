//-----------------------------------------------------------------------------
//@file  
//	Serial-Port.h
//
//@author
//	Peter Heﬂ alias "hesspet"
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the SerialPort class.
//
//  Handles serial device communication in Usine
//
//HISTORIC 
//	2013/09/15
//    rewrite for Hollyhock 1.02.012 (Martin FLEURENT)
//	2012/11/28
//    first release for Usine 5.8 (Peter Heﬂ)
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
#ifndef __SERIAL_PORT_H__
#define __SERIAL_PORT_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------


#include "../../sdk/UserDefinitions.h"
#include "SerialManager.h"
#include "CircularBuffer.h"

#include <iostream>
#include <cmath>
#include <limits>
#include <queue>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>


//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define CIRCULAR_BUFFER_SIZE    256

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

typedef  circular_buffer <int> CircularBuffer;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class SerialPort : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	SerialPort();

    // destructor
	virtual ~SerialPort();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	//  init
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();

	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:
    
    //void setupAndOpenPort();
    void createSerialManager();
    void destroySerialManager();

    // the next methods
    // do_read()
    // read_complete()
    // WriteRxDataToPin
    // should not hosted in this class. But I run into problems to create a good solution for a callback
    // with boost. Due to my limited knowledge, I can't handle this in SerialBasics.h
    // It works here KISS and goodbye...

    //void do_read() ;

    //void read_complete (const boost::system::error_code& error, size_t bytes_transferred);

    // implements SerialBasicsCallbackHelper
    void writeRxDataToBuffer (int bytes_transferred);
  
    
	//------------------------------------------------------------------------
    // trace helpers
	void trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg);
	void trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, int value);
	void trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, std::string value);
	void trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, AnsiCharPtr value);
	void trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, int v1, int v2, int v3);
    void traceError (AnsiCharPtr fnname, AnsiCharPtr traceMsg, std::string errorMessage);
	void traceError (AnsiCharPtr fnname, AnsiCharPtr traceMsg, AnsiCharPtr errorMessage); 
	//------------------------------------------------------------------------
    // trace level helpers
	inline bool isVerbose() 
    {
		return (settingTraceLevel >= TL_VERBOSE);
	}

	inline bool isInfo() 
    {
		return (settingTraceLevel >= TL_INFO);
	}
    
	//------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr txtPortname;
	UsineEventPtr cboxBaudrate;
	UsineEventPtr fdrRxBufferSize;
	UsineEventPtr dtfRxBufferDelimiter;
    
	UsineEventPtr swtchConnect;
	UsineEventPtr ledIsConnected;

	UsineEventPtr txtTxDataString;
	UsineEventPtr arrTxDataArray;
	UsineEventPtr midifTxDataMidi;

	UsineEventPtr btnClearRxBuffer;
	UsineEventPtr arrRxDataArray;
	UsineEventPtr ledRxBufferOverflow;
  
    typedef enum ParamsIndex
    {
          piPort
        , piBaudrate
        , piRxBufferSize
        , piRxBufferDelim
        , piConnect
        , piIsConnected
        , piTxDataString
        , piTxDataArray
        , piTxDataMidi
        , piClearRxBuffer
        , piRxData
        , piRxBufferOverflow
        , piNumOfParams
    };

    //-------------------------------------------------------------------------
	std::string serialPortName;

    bool                    serialIsConnected;
	int                     serialBaudrate;
	unsigned int            serialRxBufferSize;
	int                     serialRxBufferDelimiter;
	SerialParity::type		serialParity;
	int                     serialCharSize;
	SerialStopBits::type	serialStopBits;
	
	int settingParity;
	int settingCharSize;
	int settingStopBits;
    int settingTraceLevel;
    
	// instance of the serial adapter
	SerialManager* serialManagerPtr;
    
    //std::string serialRxBuffer;
    //std::string serialRxBufferParse;
    CircularBuffer serialRxCircularBuffer;
    unsigned int   serialRxAvailableSize;
    int            serialRxBufferOverflow;
    int             nextDelimiterIndex;

	boost::mutex  serialRxBufferMutex;

    // used for trace messages
    std::string stringTrace;

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:  
    int getBaudRateFromIndex (int index);
    SerialParity::type getParityIndex (int index);
    SerialStopBits::type getStopBitsIndex (int index);
    
    void findNextRxDelimiter();

    void trace (std::string traceMsg);

}; // class SerialPort

#endif //__SERIAL_PORT_H__