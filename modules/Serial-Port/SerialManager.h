//-----------------------------------------------------------------------------
//
//@file  
//	SerialManager.h
//
//@date 
//	2012/11/30
//
//@author
//	Peter Heﬂ alias "hesspet"
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
//    first release for Usine 5.8 (Peter Heﬂ)
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
#ifndef __SERIAL_MANAGER_H__
#define __SERIAL_MANAGER_H__


#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/bind.hpp> 
#include <boost/thread.hpp>
#include <boost/format.hpp>

//#include <assert>
#include <iostream>
#include <deque> 

// #include <vector>
// #include <functional>

#include "../../sdk/UserDefinitions.h"  

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------


#define TL_NONE    0		// 0 = no tracing
#define TL_INFO    1		// 1 = parameter changes and initialisation
#define TL_DEBUG   2		// 2 = 1 + data transfer
#define TL_VERBOSE 3		// 3 = 2 + data transfer

#define TXBUFFER_SIZE 1024
#define RXBUFFER_SIZE 1028

#define MSG_SERIAL_READ_COMPLETE 200

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

typedef boost::asio::serial_port_base::stop_bits   SerialStopBits;
typedef boost::asio::serial_port_base::parity      SerialParity;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class SerialManager
{
	//-------------------------------------------------------------------------
	// public members
	//-------------------------------------------------------------------------
public:
    
	
	int m_iTraceLevel;

	// the io_service which is the base of the complete serial access
	boost::asio::io_service asioIoService;

	// the object which handles the serial device
	boost::asio::serial_port asioSerialPort;

	// teh worker mechanics. Handles that io_service:: run does not exit the
	// worker thread. 
	// see: http://www.boost.org/doc/libs/1_52_0/doc/html/boost_asio/reference/io_service.html#boost_asio.reference.io_service.stopping_the_io_service_from_running_out_of_work
	boost::asio::io_service::work asioIoServiceWorker;

	// error transfer object
	boost::system::error_code m_error;

	AnsiCharPtr m_serial_port_name;
	
	// pointer to the user module which holds the trace functions
	UserModuleBase* userModulePtr;

	char m_TxBuffer[TXBUFFER_SIZE];
	char RxDatas[RXBUFFER_SIZE];
	
	// mutex to lock access to bufferTxDatas;
	boost::mutex managerMutex;
	
	//-------------------------------------------------------------------------
	// parameters events

	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:
	// constructor
	SerialManager(const std::string& serialPortName, UserModuleBase* pTUserModule);                   // bind IO_SERIVCE to a worker thread pattern, so the run() method is repeated endless 

	  // destructor
	virtual~SerialManager();

	  //-------------------------------------------------------------------------
	  // private members
	  //-------------------------------------------------------------------------
private:
	// the background worker thread which does all the serial work
	boost::thread threadWorker;

	// streambuffer used to collect and handle async_write data
	boost::asio::streambuf bufferTxDatas;
	char m_RxBuffer[RXBUFFER_SIZE];


	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:

	// protected members goes here


	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:

/*
	void destroyServiceAndPort() 
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
		    }
        }
        catch (boost::system::system_error const& e)
        {
            traceError ("SerialManager::destroyServiceAndPort ()", (AnsiCharPtr)e.what());
        }
	}
*/
	/*
	called when async_write to serial is needed.
	- fills the streambuffer with data
	- activates async writing
	*/
	void do_write(std::string strDataOut);

	void do_write(const unsigned char * dataArray, int arrayLen);

	void do_close(const boost::system::error_code& error);
 

	void setBaudrate (int baudrate);
	void setParity   (SerialParity::type serialParity);
	void setCharSize (int charSize = 8);
	void setStopBits (SerialStopBits::type serialStopBits);
	
	//-------------------------------------------------------------------------
	// protected methodes
	//-------------------------------------------------------------------------
protected:

    void do_read() 
    {
		boost::mutex::scoped_lock lock(managerMutex);

        asioSerialPort.async_read_some (
		    boost::asio::buffer(m_RxBuffer, RXBUFFER_SIZE),
            boost::bind(
                &SerialManager::read_complete,
		        this,
                boost::asio::placeholders::error,
                boost::asio::placeholders::bytes_transferred
                )
	    );
    }

    void read_complete(const boost::system::error_code& error, size_t bytes_transferred);

	// Tries to send all async data available in the streambuffer.
	void write_start(void)
	{
		//boost::mutex::scoped_lock lock(managerMutex);

		// compute the data length to send in the stream
		int iTxDataLen = prepareSendBuffer();

		trace("write_start()","iTxDataLen", iTxDataLen);

		// try to send data much as possible

		boost::asio::async_write(
			asioSerialPort,
			boost::asio::buffer(m_TxBuffer, iTxDataLen ),//m_buff,
			boost::bind(
				&SerialManager::write_complete, // called when all data was send.
				this,
				boost::asio::placeholders::error
				)
			);
	} 

	inline void appendDataToTxStreamBuffer( std::string strDataOut ) {

		//boost::mutex::scoped_lock lock(managerMutex);

		bufferTxDatas.sputn(strDataOut.c_str(), strDataOut.length());
	}

	inline void appendDataToTxStreamBuffer(const unsigned char * dataArray, int arrayLen ) {

		//boost::mutex::scoped_lock lock(managerMutex);

		bufferTxDatas.sputn((const char *)dataArray, arrayLen);
	}

	inline int prepareSendBuffer() {

		//boost::mutex::scoped_lock lock(managerMutex);

		// Transfer data from the stream to the send buffer
		// if the data is longer than the buffer, we split the data into buffer size thunks

		// compute the data length to send in the stream
		int iTxDataLen = bufferTxDatas.size();
		trace("write_start()","len stream buffer", iTxDataLen);

		// longer than maximum buffer size?
		if (iTxDataLen > TXBUFFER_SIZE) 
		{
			iTxDataLen = TXBUFFER_SIZE;
		}

		// copy 
		bufferTxDatas.sgetn(m_TxBuffer, iTxDataLen);
		// correct streambuffer
		bufferTxDatas.consume( iTxDataLen );

		return iTxDataLen;
	}

	inline void write_complete(const boost::system::error_code& error)
	{ 
		if (!error)
		{ 
			//boost::mutex::scoped_lock lock(managerMutex);
			int current_buff_size = bufferTxDatas.size();

			if (current_buff_size != 0) // if there is anthing left to be written (or new added?)
			{
				write_start(); // then start sending the next data in the buffer
				return;
			}

		}
		else
		{
			// clear the buffer
			// boost::mutex::scoped_lock lock(managerMutex);
			int size = bufferTxDatas.size();
			if (size !=0) {
				bufferTxDatas.consume(size);
			} 

			trace("SerialManager::write_complete", "!!!!!!!!!!! error.message()", (AnsiCharPtr)error.message().c_str());
		}
	} 

private:
    
	void destroyServiceAndPort();

	inline void trace( AnsiCharPtr fnname, AnsiCharPtr traceMsg) {
		
		if (userModulePtr == NULL || m_iTraceLevel == TL_NONE)
			return;

		std::string s = str(boost::format("[%s()] %s") % fnname % traceMsg);
		userModulePtr->sdkTraceChar( (AnsiCharPtr) s.c_str() );
	}
	inline void trace( AnsiCharPtr fnname, AnsiCharPtr traceMsg, int value ) {
		if (userModulePtr == NULL || m_iTraceLevel == TL_NONE)
			return;

		std::string s = str(boost::format("[%s()] %s => %d") % fnname % traceMsg % value);
		userModulePtr->sdkTraceChar( (AnsiCharPtr) s.c_str() );
	}
	inline void trace( AnsiCharPtr fnname, AnsiCharPtr traceMsg, AnsiCharPtr value ) {
		if (userModulePtr == NULL || m_iTraceLevel == TL_NONE)
			return;

		userModulePtr->sdkTraceChar( (AnsiCharPtr) (boost::format("[%s()] %s => %s") % fnname % traceMsg % value).str().c_str() );
	}

    void traceError (AnsiCharPtr fnname, AnsiCharPtr traceMsg, std::string errorMessage) 
    {
	    //traceError (fnname, traceMsg, errorMessage.c_str());
        if ( ! errorMessage.empty () ) 
	    {    
            std::string stringTrace = str (boost::format("[%s] %s %s") % fnname % traceMsg % errorMessage);
	        userModulePtr->sdkTraceErrorChar ( (AnsiCharPtr) stringTrace.c_str());
	    } 
	    else 
	    {
            std::string stringTrace = str (boost::format("[%s] %s UNKNOWN ERROR") % fnname);
		    userModulePtr->sdkTraceErrorChar( (AnsiCharPtr) stringTrace.c_str() );
	    }
    } 
	inline void traceError(AnsiCharPtr fnname, AnsiCharPtr errorMessage) 
	{
		if (userModulePtr == NULL || m_iTraceLevel == TL_NONE)
			return;

		if ( errorMessage != NULL ) 
		{
			userModulePtr->sdkTraceErrorChar( (AnsiCharPtr) (AnsiCharPtr) (boost::format("[%s()] %s") % fnname % errorMessage).str().c_str() );
		} 
		else 
		{
			userModulePtr->sdkTraceErrorChar( (AnsiCharPtr) (AnsiCharPtr) (boost::format("[%s()] UNKNOWN ERROR") % fnname).str().c_str() );
		}

	}
	
	inline bool isVerbose() {
		return (m_iTraceLevel <= TL_VERBOSE);
	}

	inline bool isInfo() {
		return (m_iTraceLevel <= TL_INFO);
	}

}; // class SerialManager

#endif //__SERIAL_MANAGER_H__