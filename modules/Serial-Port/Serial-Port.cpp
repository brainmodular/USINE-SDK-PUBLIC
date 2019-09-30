//-----------------------------------------------------------------------------
//@file  
//	Serial-Port.cpp
//
//@author
//	Peter Heﬂ alias "hesspet"
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the SerialPort class.
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
#include "Serial-Port.h"
#include <cmath>
#include <limits>

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new SerialPort();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((SerialPort*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Serial Port";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Serial Port";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
SerialPort::SerialPort()
    : serialManagerPtr (nullptr)
    , serialIsConnected (false)
    , serialPortName ()
    , serialBaudrate (57600)
    , serialRxBufferSize (0)
    , serialParity (SerialParity::none)
    , serialCharSize (8)
    , serialStopBits (SerialStopBits::one)
    , settingParity (0)
    , settingCharSize (8)
    , settingStopBits (0)
    , settingTraceLevel (TL_NONE)
    , serialRxCircularBuffer (CIRCULAR_BUFFER_SIZE)
    , serialRxAvailableSize (0)
    , serialRxBufferDelimiter (0)
    , serialRxBufferOverflow (0)
    , nextDelimiterIndex (-1)
{
}

// destructor
SerialPort::~SerialPort()
{
	destroySerialManager(); 

    //if (serialRxCircularBuffer != nullptr)
    //{
    //    delete serialRxCircularBuffer;
    //    serialRxCircularBuffer = nullptr;
    //}
}

//-------------------------------------------------------------------------
// public methodes inherited from UserModule
//------------------------------------------------------------------------
void SerialPort::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = piNumOfParams;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= false;
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init
void SerialPort::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
}

//-----------------------------------------------------------------------------
// parameters and process
void SerialPort::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
		// txtPortname
		case piPort:
			pParamInfo->ParamType		= ptTextField;
			pParamInfo->Caption			= "port";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
			break;
        // cboxBaudrate
		case piBaudrate:
			pParamInfo->ParamType		= ptListBox ;
			pParamInfo->Caption			= "baudrate";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
			pParamInfo->ListBoxStrings	= "\"300\",\"600\",\"1200\",\"2400\",\"4800\",\"9600\",\"14400\",\"19200\",\"28800\",\"38400\",\"57600\",\"115200\"";
			pParamInfo->DefaultValue    = 10;
            break;
        // fdrRxBufferSize
		case piRxBufferSize:
		    pParamInfo->ParamType		= ptDataFader;
		    pParamInfo->Caption			= "rx buffer size";
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
		    pParamInfo->MinValue		= 0; 
		    pParamInfo->MaxValue		= 128;
		    pParamInfo->DefaultValue	= 0;
		    pParamInfo->Format			= "%.0f";
			break;
        // dtfRxBufferDelimiter
		case piRxBufferDelim:
			pParamInfo->ParamType		= ptDataField;
			pParamInfo->Caption			= "rx byte delimiter";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
		    pParamInfo->MinValue		= -1; 
		    pParamInfo->MaxValue		= 255;
		    pParamInfo->DefaultValue	= -1;
			pParamInfo->ReadOnly		= FALSE;
		    pParamInfo->Format			= "%g";
			pParamInfo->CallBackType	= ctImmediate;
			break;
        // swtchConnect
		case piConnect:
		    pParamInfo->ParamType		= ptSwitch;
		    pParamInfo->Caption			= "on";
		    pParamInfo->IsInput			= TRUE;
		    pParamInfo->IsOutput		= FALSE;
            pParamInfo->IsSeparator     = TRUE;
			break;
        // ledIsConnected
		case piIsConnected:
		    pParamInfo->ParamType		= ptTriggerLed;
		    pParamInfo->Caption			= "connected";
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
		    pParamInfo->ReadOnly		= TRUE;
			break;
        // txtTxDataString
		case piTxDataString:
			pParamInfo->ParamType		= ptTextField;
			pParamInfo->Caption			= "tx data (string)";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
            pParamInfo->IsSeparator     = TRUE;
			break;
        // arrTxDataArray
		case piTxDataArray:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "tx data (array)";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
			//pParamInfo->CallBackType	= ctImmediate;
			break;
        // midifTxDataMidi
		case piTxDataMidi:
			pParamInfo->ParamType		= ptMidi;
			pParamInfo->Caption			= "tx data (midi/firmata)";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
			//pParamInfo->MinValue		= - FLT_MAX;
			//pParamInfo->MaxValue		= FLT_MAX;
			pParamInfo->CallBackType	= ctImmediate;
			break;        
        // btnClearRxBuffer
		case piClearRxBuffer:
			pParamInfo->ParamType		= ptButton;
			pParamInfo->Caption			= "clear rx buffer";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
			pParamInfo->IsSeparator		= TRUE;
			break;
        // arrRxDataArray
		case piRxData:
			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= "rx data";
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->ReadOnly		= FALSE;
			//pParamInfo->MinValue		= - FLT_MAX;
			//pParamInfo->MaxValue		= FLT_MAX;
			break;
        // ledRxBufferOverflow
		case piRxBufferOverflow:
		    pParamInfo->ParamType		= ptTriggerLed;
		    pParamInfo->Caption			= "rx buffer overflow";
		    pParamInfo->Color       	= sdkGetUsineColor(clCursor);
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
		    pParamInfo->ReadOnly		= TRUE;
			break;

	        // default case
        default:
	        break;
	}
}

void SerialPort::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
	switch (ParamIndex) 
    {
		// txtPortname
		case piPort:
			txtPortname = pEvent;
			break;
        // cboxBaudrate
		case piBaudrate:
			cboxBaudrate = pEvent;
			break;
        // fdrRxBufferSize
		case piRxBufferSize:
			fdrRxBufferSize = pEvent;
			break;
		// arrRxBufferDelimiter
        // dtfRxBufferDelimiter
		case piRxBufferDelim:
			dtfRxBufferDelimiter = pEvent;
			break;
        // swtchConnect
		case piConnect:
			swtchConnect = pEvent;
			break;
        // ledIsConnected
		case piIsConnected:
			ledIsConnected = pEvent;
			break;
        // txtTxDataString
		case piTxDataString:
			txtTxDataString = pEvent;
			break;
        // arrTxDataArray
		case piTxDataArray:
			arrTxDataArray = pEvent;
			break;
        // midifTxDataMidi
		case piTxDataMidi:
			midifTxDataMidi = pEvent;
			break;
        // btnClearRxBuffer
		case piClearRxBuffer:
			btnClearRxBuffer = pEvent;
			break;
        // arrRxDataArray
		case piRxData:
			arrRxDataArray = pEvent;
			break;
        // ledRxBufferOverflow
		case piRxBufferOverflow:
			ledRxBufferOverflow = pEvent;
			break;
        // default case
        default:
	        break;
	}
}
void SerialPort::onCallBack (UsineMessage *Message) 
{
    
    if ( Message->message == MSG_SERIAL_READ_COMPLETE ) 
	{
        writeRxDataToBuffer (Message->wParam);
        return;
    }

    if ( Message->lParam == MSG_CHANGE ) 
	{
	    switch (Message->wParam) // ParamIndex
        {

		    // txtPortname
		    case piPort:
		        {
			        serialPortName = sdkGetEvtPChar (txtPortname);
			        boost::trim (serialPortName);

			        if (isVerbose())
				        trace ("SerialPort", "CallBack(port)", serialPortName);
		        }
			    break;
            // cboxBaudrate
		    case piBaudrate:
		        {	
			        if (serialManagerPtr == nullptr) 
			            return;
                    
                    int baudrateId = (int) sdkGetEvtData( cboxBaudrate );

			        serialManagerPtr->setBaudrate(getBaudRateFromIndex (baudrateId));
			        
                    if (isVerbose())
				        trace("SerialPort", "CallBack(baudrate)", serialBaudrate );
		        }
			    break;
            // fdrRxBufferSize 
		    case piRxBufferSize:
                {
			        serialRxBufferSize = (int) sdkGetEvtData (fdrRxBufferSize);
                    if (isVerbose())
				        trace("SerialPort", "CallBack(rx buffer size)", serialRxBufferSize );
                }
			    break;
		    // arrRxBufferDelimiter
		    case piRxBufferDelim:
                {
                    serialRxBufferDelimiter = (int)sdkGetEvtData (dtfRxBufferDelimiter);
                    if (isVerbose())
				        trace("SerialPort", "CallBack(rx delimiter)", serialRxBufferDelimiter );
                }
			    break;
                // swtchConnect
		    case piConnect:
			    {
					if (serialPortName.empty())
					{
						trace ("SerialPort", "fill port name to connect");
					}
					
                    int start = (int) sdkGetEvtData (swtchConnect);
                    
                    if (! serialPortName.empty() && start == 1) 
                    {
                        createSerialManager();
                    }
                    else if (serialPortName.empty() || start == 0)
                    {
                        destroySerialManager();
                    }
                }
			    break;
 
                	
            // txtTxDataString
		    case piTxDataString:
                {
                    
			        if (serialManagerPtr == nullptr) 
			            return;
                   		
                    std::string strDataOut = sdkGetEvtPChar( txtTxDataString );

			        try 
			        {
				        serialManagerPtr->do_write(strDataOut);
				        if (isVerbose())
					        trace ("SerialPort", "CallBack(tx data (string))", (AnsiCharPtr)strDataOut.c_str());
			        }
			        catch(std::exception& e) 
			        {
				        traceError("SerialPort", "CallBack(tx data (string))", (AnsiCharPtr)e.what());
			        }
                    
                }
			    break;
            // arrTxDataArray
		    case piTxDataArray:
                {
                    
			        if (serialManagerPtr == nullptr) 
			            return;
                    			
                    //TPrecision* dataOutPtr = sdkGetEvtDataAddr (arrTxDataArray);
			        int dataOutSize = sdkGetEvtSize (arrTxDataArray);
                    TPrecision tmp;
			        // Array will be treated as byte array, so for the values of the array only values 0x00-0xFF are allowed!
			        // We must convert now the Data to a new Array();
			
			        unsigned char * byteArray = nullptr;
			        try 
			        {
				        byteArray = new unsigned char[dataOutSize];

				        for (int i = 0; i < dataOutSize; i++) 
                        {
                            tmp = sdkGetEvtArrayData (arrTxDataArray, i);
					        byteArray[i] = (const unsigned char)tmp;
				        }

				        serialManagerPtr->do_write(byteArray, dataOutSize);
				        
                        if ( isVerbose() )  {
					        ("CallBack(tx data (array))", "do_write tx data len = ", dataOutSize);
				        }
			        }
			        catch (std::exception& e) 
			        {
				        traceError("SerialPort", "CallBack(tx data (array))", (AnsiCharPtr)e.what());
			        }

			        if (byteArray != nullptr) 
			        {
				        delete byteArray;
			        }
                    
                }
			    break;
            // midifTxDataMidi
		    case piTxDataMidi:
                {
                    
			        if (serialManagerPtr == nullptr) 
			            return;

			        unsigned char * byteArray = nullptr;
			        int byteArrayWritePos = 0;

                    // how many midi messages we get?
			        int midiMessagesSize = sdkGetEvtSize( midifTxDataMidi );

			        int bufLenSendbuffer = 3  * midiMessagesSize;

			        if ( bufLenSendbuffer != 0 ) 
                    {
				        try 
                        {
					        byteArray =  new unsigned char[bufLenSendbuffer];

					        // Collect all message to a buffer and send this buffer
					        for( int iMidiMessageIndex=0; iMidiMessageIndex < midiMessagesSize; iMidiMessageIndex++ ) 
                            {
			
						        UsineMidiCode txMidi = sdkGetEvtArrayMidi( midifTxDataMidi, iMidiMessageIndex );

						        // we ignore the channel, we look only to the Command!
						        byteArray[byteArrayWritePos++] =  txMidi.Msg;
						        byteArray[byteArrayWritePos++] =  txMidi.Data1;
						        byteArray[byteArrayWritePos++] =  txMidi.Data2;
						        
                                if ( isVerbose() )
							        trace("CallBack(tx data (midi))", "buffer = ", txMidi.Msg, txMidi.Data1, txMidi.Data2);
					        }

					        serialManagerPtr->do_write(byteArray, bufLenSendbuffer);

					        if ( isVerbose() ) 
                                trace("CallBack(tx data (midi))", "do_write tx data len = ", bufLenSendbuffer);


				        }
						catch (std::exception& e) 
                        {
					        traceError("SerialPort", "CallBack(tx data (midi))", (AnsiCharPtr)e.what());
				        }

				        if (byteArray!=NULL) 
				        {
					        delete byteArray;
				        }
			        }
                    
                }
			    break;
            // btnClearRxBuffer
		    case piClearRxBuffer:
			    {
                    serialRxCircularBuffer.clear();
                }
			    break;

            // default case
            default:
	            break;
	    }
    } 
}
void SerialPort::onProcess () 
{

    if (serialIsConnected)
    {
        if (serialRxBufferOverflow != (int)sdkGetEvtData (ledRxBufferOverflow))
            sdkSetEvtData (ledRxBufferOverflow, (float)serialRxBufferOverflow);
        
        boost::mutex::scoped_lock lockBuffer(serialRxBufferMutex);

        serialRxAvailableSize = serialRxCircularBuffer.size();

        // something to put into Rx ?
        if (serialRxAvailableSize > 0)
        {
            // are we in rx delimiter mode ?
            if (serialRxBufferSize == 0)
            {      
                // if we have a defined delimiter
                if (serialRxBufferDelimiter > 0)
                {
                    // have we got an identified delimiter
                    if (nextDelimiterIndex >= 0)
                    {
                        sdkSetEvtSize (arrRxDataArray, nextDelimiterIndex); 
                        // fill the rx buffer until the delimiter
                        for (unsigned int i = 0; i < nextDelimiterIndex; i++)
                        {
                            int tmp = serialRxCircularBuffer.front();
                            serialRxCircularBuffer.pop_front();
                            sdkSetEvtArrayData (arrRxDataArray, i, tmp);
                        }
                        sdkRepaintParam (piRxData);
                        // just get ride of the delimiter
                        serialRxCircularBuffer.pop_front();
                        serialRxAvailableSize = serialRxCircularBuffer.size();

                        // and try to find the next one
                        findNextRxDelimiter ();
                    }
                }
                else
                {
                    // fill the rx buffer with all we have
                    sdkSetEvtSize (arrRxDataArray, serialRxAvailableSize);
                    for (unsigned int i = 0; i < serialRxAvailableSize; ++i)
                    {
                        int tmp = serialRxCircularBuffer.front();
                        serialRxCircularBuffer.pop_front();
                        sdkSetEvtArrayData (arrRxDataArray, i, tmp);
                    }
                    sdkRepaintParam (piRxData);
                    serialRxAvailableSize = serialRxCircularBuffer.size();
                }

            }
            // so in buffer mode, have we got enough to fill the buffer ?
            else if (serialRxAvailableSize >= serialRxBufferSize)
            {
                sdkSetEvtSize (arrRxDataArray, serialRxBufferSize);
                for (unsigned int i = 0; i < serialRxBufferSize; ++i)
                {
                    sdkSetEvtArrayData (arrRxDataArray, i, serialRxCircularBuffer.front());
                    serialRxCircularBuffer.pop_front();
                }
            }
        }




/*

        if (serialRxBufferSize == 0 &&  serialRxAvailableSize > 0)
        {
            if (nextDelimiterIndex >= 0)
            {     
                for (unsigned int i = 0; i < serialRxAvailableSize; i++)
                {
                    int tmp = serialRxCircularBuffer.front();
                    serialRxCircularBuffer.pop_front();
                    if (tmp != serialRxBufferDelimiter)
                    {
                        sdkSetEvtSize (arrRxDataArray, i+1); 
                        sdkSetEvtArrayData (arrRxDataArray, i, tmp);
                    }
                    else
                    {
                        sdkRepaintParam (piRxData);
                        nextDelimiterIndex = -1;
                        i = serialRxAvailableSize;
                        return;
                    }
                }
            }
        }
        else if (serialRxAvailableSize >= serialRxBufferSize &&  serialRxAvailableSize > 0)
        {
            sdkSetEvtSize (arrRxDataArray, serialRxBufferSize);
            for (unsigned int i = 0; i < serialRxBufferSize; ++i)
            {
                sdkSetEvtArrayData (arrRxDataArray, i, serialRxCircularBuffer.front());
                serialRxCircularBuffer.pop_front();
            }
        }
        
    */

    }
    
}

//-----------------------------------------------------------------------------
// chunk system
int  SerialPort::onGetChunkLen (LongBool Preset) {return 0;}
void SerialPort::onGetChunk (void* chunk, LongBool Preset) {}
void SerialPort::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}

//-----------------------------------------------------------------------------
// layout
void SerialPort::onCreateSettings() 
{
    
    sdkAddSettingLineCaption (PROPERTIES_TAB_NAME, "protocol");
	sdkAddSettingLineCombobox (PROPERTIES_TAB_NAME, &(settingParity), "parity", "\"none\",\"odd\",\"even\"");
	sdkAddSettingLineInteger (PROPERTIES_TAB_NAME, &(settingCharSize), "char size", 4, 8, scLinear, "", 8);
	sdkAddSettingLineCombobox (PROPERTIES_TAB_NAME, &(settingStopBits), "stop bits", "\"one\",\"one point five\",\"two\"");
    sdkAddSettingLineCaption (PROPERTIES_TAB_NAME, "trace");
	sdkAddSettingLineCombobox (PROPERTIES_TAB_NAME, &(settingTraceLevel), "level", "\"none\",\"info\",\"debug\",\"verbose\"");
    
}

void SerialPort::onSettingsHasChanged() 
{
	if (serialManagerPtr != nullptr )
    {

        if (serialParity != SerialParity::type (settingParity))
        serialManagerPtr->setParity (getParityIndex (settingParity));

        if (serialCharSize != settingTraceLevel)
        serialManagerPtr->setCharSize (serialCharSize = settingCharSize);

        if (serialStopBits != getStopBitsIndex (settingStopBits))
        serialManagerPtr->setStopBits (getStopBitsIndex (settingStopBits));

        if (serialManagerPtr->m_iTraceLevel != settingTraceLevel)
		    serialManagerPtr->m_iTraceLevel = settingTraceLevel;
    }
}

//-------------------------------------------------------------------------
// public methodes
//------------------------------------------------------------------------
void SerialPort::createSerialManager() 
{	
	if (serialManagerPtr != nullptr)
        destroySerialManager();

	try 
    {
        serialRxCircularBuffer.clear();
		serialManagerPtr = new SerialManager (serialPortName, this);
		serialManagerPtr->setBaudrate (serialBaudrate);
		serialManagerPtr->setParity (serialParity);
		serialManagerPtr->setCharSize (settingCharSize);
		serialManagerPtr->setStopBits (serialStopBits);
        
	    trace ("SerialPort", "connect to", serialPortName);

        serialIsConnected = true;
        sdkSetEvtData (ledIsConnected, 1);
	}
    catch(...) // std::exception& e
    {
		traceError("SerialPort", "connection failed to", serialPortName);	
		
        if (serialManagerPtr) 
        {
            destroySerialManager();
		}
	}

}

void SerialPort::destroySerialManager()
{
	if (serialManagerPtr == nullptr) 
		return;
    
	trace("SerialPort", "disconnect ", serialPortName);
    
    serialIsConnected = false;
    sdkSetEvtData (ledIsConnected, 0);

	delete serialManagerPtr;
	serialManagerPtr = nullptr; 
}

// implements SerialBasicsCallbackHelper
void SerialPort::writeRxDataToBuffer( int bytes_transferred) 
{
    try
    {
        if (bytes_transferred > 0)
        {
	        boost::mutex::scoped_lock lockManager (serialManagerPtr->managerMutex);
            boost::mutex::scoped_lock lockBuffer(serialRxBufferMutex);
        
            unsigned int rxSize = serialRxCircularBuffer.size();
            unsigned int newSize = rxSize + bytes_transferred;

            if (newSize >= CIRCULAR_BUFFER_SIZE)
                serialRxBufferOverflow = 1;
            else if (serialRxBufferOverflow == 1 && newSize < CIRCULAR_BUFFER_SIZE)
                serialRxBufferOverflow = 0;

            for (int i = 0; i < bytes_transferred; ++i)
            {
                int tmp = serialManagerPtr->RxDatas[i];
                serialRxCircularBuffer.push_back(tmp);
            }
            serialRxAvailableSize = serialRxCircularBuffer.size();

            // if we are in rx delimiter mode and have a defined delimiter
            if (serialRxBufferSize == 0 && serialRxBufferDelimiter > 0)
            {
                findNextRxDelimiter ();
            }

            if (settingTraceLevel >= TL_VERBOSE) 
	        {
                serialManagerPtr->RxDatas[bytes_transferred] = 0;
		        trace ("SerialPort", "receiving");
		        trace ("SerialPort", "len :", bytes_transferred);
		        trace ("SerialPort", "data:", serialManagerPtr->RxDatas);
	        }
        }
    }
    catch (...)
    {
        traceError ("writeRxDataToBuffer", "", "");
    }

}


void SerialPort::findNextRxDelimiter ()
{
     nextDelimiterIndex = -1;
    for (unsigned int i = 0; i < serialRxAvailableSize; i++)
    {
        int tmp = serialRxCircularBuffer[i];
        if (tmp == serialRxBufferDelimiter)
        {
            // collect the next delimiter and stop the loop
            nextDelimiterIndex = i;
            i = serialRxAvailableSize;
        }
    }
}

//------------------------------------------------------------------------
// trace helpers
void SerialPort::trace (std::string traceMsg) 
{
    sdkTraceChar ((AnsiCharPtr) traceMsg.c_str());

	if (settingTraceLevel == TL_VERBOSE)
        sdkTraceLogChar ((AnsiCharPtr) traceMsg.c_str());
}

void SerialPort::trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg) 
{
	if (settingTraceLevel == TL_NONE) return;

	std::string s = str(boost::format("[%s] %s") % fnname % traceMsg);
    
    trace (s);
}

void SerialPort::trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, int value) 
{
	if (settingTraceLevel == TL_NONE) return;

	std::string s = str(boost::format("[%s] %s %d") % fnname % traceMsg % value);
    
    trace (s);
}
    
void SerialPort::trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, std::string value) 
{
	if (settingTraceLevel == TL_NONE) return;

    std::string s = str (boost::format("[%s] %s %s") % fnname % traceMsg % value);
    
    trace (s);
}

void SerialPort::trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, AnsiCharPtr value) 
{
	if (settingTraceLevel == TL_NONE) return;

	std::string s = str (boost::format("[%s] %s %s") % fnname % traceMsg % value);
    
    trace (s);
}

void SerialPort::trace (AnsiCharPtr fnname, AnsiCharPtr traceMsg, int v1, int v2, int v3) 
{
	if (settingTraceLevel == TL_NONE) return;

	std::string s = str (boost::format("[%s] %s %d %d %d") % fnname % traceMsg % v1 % v2 %v3);
    
    trace (s);
}


void SerialPort::traceError (AnsiCharPtr fnname, AnsiCharPtr traceMsg, std::string errorMessage) 
{
    if (! errorMessage.empty ()) 
	{    
        stringTrace = str (boost::format("[%s] %s %s") % fnname % traceMsg % errorMessage);
	    sdkTraceErrorChar ((AnsiCharPtr) stringTrace.c_str());
	} 
	else 
	{
        stringTrace = str (boost::format("[%s] %s UNKNOWN ERROR") % fnname);
		sdkTraceErrorChar((AnsiCharPtr) stringTrace.c_str());
	}
} 

void SerialPort::traceError (AnsiCharPtr fnname, AnsiCharPtr traceMsg, AnsiCharPtr errorMessage) 
{
	if (errorMessage != NULL) 
	{    
        stringTrace = str (boost::format("[%s] %s %s") % fnname % traceMsg % errorMessage);
	    sdkTraceErrorChar ((AnsiCharPtr) stringTrace.c_str());
	} 
	else 
	{
        stringTrace = str (boost::format("[%s] %s UNKNOWN ERROR") % fnname);
		sdkTraceErrorChar((AnsiCharPtr) stringTrace.c_str());
	}
} 

//-------------------------------------------------------------------------
// private methodes
//------------------------------------------------------------------------
int SerialPort::getBaudRateFromIndex (int index)
{
    switch (index)
    {
		case 0:
            serialBaudrate = 300;
			break;
		case 1:
            serialBaudrate = 600;
			break;
		case 2:
            serialBaudrate = 1200;
			break;
		case 3:
            serialBaudrate = 2400;
			break;
		case 4:
            serialBaudrate = 4800;
			break;
		case 5:
            serialBaudrate = 9600;
			break;
		case 6:
            serialBaudrate = 14400;
			break;
		case 7:
            serialBaudrate = 19200;
			break;
		case 8:
            serialBaudrate = 28800;
			break;
		case 9:
            serialBaudrate = 38400;
			break;
		case 10:
            serialBaudrate = 57600;
			break;
		case 11:
            serialBaudrate = 115200;
			break;
        // default case
        default:
            serialBaudrate = 57600;
	        break;
    }

    return serialBaudrate;
}

SerialParity::type SerialPort::getParityIndex (int index)
{
    switch (index)
    {
		case 0:
            serialParity = SerialParity::none;
			break;
		case 1:
            serialParity = SerialParity::odd;
			break;
		case 2:
            serialParity = SerialParity::even;
			break;
        // default case
        default:
            serialParity = SerialParity::none;
	        break;
    }

    return serialParity;
}

SerialStopBits::type SerialPort::getStopBitsIndex (int index)
{
    switch (index)
    {
		case 0:
            serialStopBits = SerialStopBits::one;
			break;
		case 1:
            serialStopBits = SerialStopBits::onepointfive;
			break;
		case 2:
            serialStopBits = SerialStopBits::two;
			break;
        // default case
        default:
            serialStopBits = SerialStopBits::one;
	        break;
    }

    return serialStopBits;
}


