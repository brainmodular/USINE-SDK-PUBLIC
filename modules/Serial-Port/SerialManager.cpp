//-----------------------------------------------------------------------------
//
//@file  
//	SerialManager.h
//
//@date 
//	2012/11/30
//
//@author
//	Peter Heß alias "hesspet"
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the SerialManager class.
//
//  manager to communicate asynchronously with a serial port
//
//HISTORIC 
//	2013/09/15
//    rewrite for Hollyhock 1.02.012 (Martin FLEURENT)
//	2012/11/28
//    first release for Usine 5.8 (Peter Heß)
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

// suppress winsock allready inlcuded message (hpe)

#include "SerialManager.h"
#include "Serial-Port.h"

//-------------------------------------------------------------------------
// public constructors/destructors
//-------------------------------------------------------------------------

// constructor
SerialManager::SerialManager(const std::string& serialPortName, UserModuleBase* pTUserModule) 
	: asioIoService()									// build instance of IO_SERVICE
	, asioSerialPort (asioIoService, serialPortName)    // initialize the COM Port
	, asioIoServiceWorker(asioIoService)                // bind IO_SERIVCE to a worker thread pattern, so the run() method is repeated endless 

{
	// must be set before any trace output! No tracing before this point!
	userModulePtr = pTUserModule; 
	
	// start the endless running worker thread to process the async data.
	//threadWorker = new boost::thread (boost::bind(&boost::asio::io_service::run, &asioIoService));
	boost::thread t (boost::bind(&boost::asio::io_service::run, &asioIoService));
	threadWorker.swap (t);
    //setErrorStatus(false);

    do_read();
	//trace("SerialManager","constructed");
}

// destructor
SerialManager::~SerialManager()
{
	boost::mutex::scoped_lock lock(managerMutex);
	destroyServiceAndPort();
	
}

void SerialManager::do_write(std::string strDataOut)
{ 
	try 
	{
		boost::mutex::scoped_lock lock(managerMutex);

		trace("do_write()", (AnsiCharPtr) strDataOut.c_str());

		// append to buffer, two situations
		// there is a thread which processes the queue, so no new thread for processing must be spawned
		// otherwise we must create a new process to send the buffer
		appendDataToTxStreamBuffer( strDataOut );

		// now let's see if there is a process running,
		// if not, we must start a new async transfer process.

		// callback to handle write call from outside this class
		write_start();
	}
	catch (std::exception& e) 
	{
		traceError("do_write(const unsigned char * dataArray, int arrayLen)", (AnsiCharPtr)e.what() );
	}
} 

void SerialManager::do_write(const unsigned char * dataArray, int arrayLen)
{ 
	try 
	{
		boost::mutex::scoped_lock lock(managerMutex);

		// append to buffer, two situations
		// there is a thread which processes the queue, so no new thread for processing must be spawned
		// otherwise we must create a new process to send the buffer
		appendDataToTxStreamBuffer( dataArray, arrayLen );

		// now let's see if there is a process running,
		// if not, we must start a new async transfer process.

		// callback to handle write call from outside this class
		write_start();
	}
	catch (std::exception& e) 
	{
		traceError("do_write(const unsigned char * dataArray, int arrayLen)", (AnsiCharPtr)e.what() );
	}
} 
    
void SerialManager::read_complete(const boost::system::error_code& error, size_t bytes_transferred)
{ 
	// the asynchronous read operation has now completed or failed and returned an error
	if (! error)
    {
        {
		    boost::mutex::scoped_lock lock(managerMutex);
            memcpy (RxDatas, m_RxBuffer, bytes_transferred);
        }
        // read completed, so ask module to process the data
	    //userModulePtr->sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, MSG_SERIAL_READ_COMPLETE, (NativeInt)bytes_transferred, 0);
        ((SerialPort*)userModulePtr)->writeRxDataToBuffer (bytes_transferred);
        // start waiting for another asynchronous read again
        do_read(); 
	}
    else
	{
        traceError ("read_complete()", "error", error.message ());
	}
} 

void SerialManager::do_close(const boost::system::error_code& error)
{ 
	// 
	trace( "SerialManager::do_close", "enter");
	// something has gone wrong, so close the socket & make this object inactive
	if (error == boost::asio::error::operation_aborted) // if this call is the result of a timer cancel()
	{
		trace("do_close", "ERROR error.message()", (AnsiCharPtr)error.message().c_str());
		return; // ignore it because the connection cancelled the timer
	}
	if (error)
	{
		trace("do_close", "ERROR error.message()", (AnsiCharPtr)error.message().c_str());
	}

} 
void SerialManager::setBaudrate (int baudrate)
{
	trace("setBaudrate", "baudrate", baudrate);

	asioSerialPort.set_option( boost::asio::serial_port_base::baud_rate( baudrate ) );
}

void SerialManager::setParity   (SerialParity::type serialParity)
{
	trace("setParity", "parity", (int)serialParity);

	asioSerialPort.set_option (SerialParity (serialParity));	// default none
}

void SerialManager::setCharSize (int charSize)
{
	trace("setCharSize", "charSize", charSize);
	asioSerialPort.set_option (boost::asio::serial_port_base::character_size (charSize));
}

void SerialManager::setStopBits (SerialStopBits::type serialStopBits)
{
	trace("setStopBits", "stopBits", (int)serialStopBits);

	asioSerialPort.set_option (SerialStopBits (serialStopBits));	// default one
}
	
void SerialManager::destroyServiceAndPort() 
{
    try
    {
		if (! asioIoService.stopped())
        {
			asioIoService.stop();
		}

		if( asioSerialPort.is_open())
        {
			asioSerialPort.cancel();			
			asioSerialPort.close();
            threadWorker.join();
		}
    }
    catch (boost::system::system_error const& e)
    {
        traceError ("SerialManager::destroyServiceAndPort ()", (AnsiCharPtr)e.what());
    }
}

