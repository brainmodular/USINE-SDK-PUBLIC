//-----------------------------------------------------------------------------
//@file  
//	DeviceDMX.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DeviceDMX class.
//
//HISTORIC 
//	2014/12/11
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
#include "DeviceDMX.h"

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    initialiseJuce_GUI ();
	pModule = new DeviceDMX();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((DeviceDMX*)pModule);
    shutdownJuce_GUI ();
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "DMX";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "DMX";
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
DeviceDMX::DeviceDMX () 
	: maxDmxOutputs (4)
{
	initDeviceOpenDMXModule();
}

// destructor
DeviceDMX::~DeviceDMX ()
{	
	try
	{
    stopTimer();
		
	}
	catch (...)
	{
		sdkTraceLogChar("in stopTimer");
	}
	
    for (int i = 0; i < maxDmxOutputs; ++i)
	{
        closeDmxDevice (i);
    }
	listDmxOutputDevicesNames.clear();

}

//-----------------------------------------------------------------------------
// module description
void DeviceDMX::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	maxDmxOutputs = 4;

	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtDeviceDMX;
	pModuleInfo->BackColor          = sdkGetUsineColor(clPluginModuleColor);
	pModuleInfo->NumberOfParams     = PARAMS_BEFORE_DMX_OUTS + maxDmxOutputs;
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
void DeviceDMX::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	rescanDmxDevices();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------
// Parameters description
void DeviceDMX::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
	// m_btnRescanAllDevices
	if (ParamIndex == 0)
	{
        strSepDevices = std::string(sdkGetTrans("devices"));

		pParamInfo->ParamType		 = ptButton;
		pParamInfo->Caption			 = "rescan devices";
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		pParamInfo->ReadOnly		 = FALSE;
		pParamInfo->IsSeparator		 = TRUE;
		pParamInfo->SeparatorCaption = (AnsiCharPtr)strSepDevices.c_str();
        pParamInfo->DontSave		 = TRUE;
		pParamInfo->Translate		 = TRUE;
	}
	// m_btnResetAllDevices
	else if (ParamIndex == 1)
	{
		pParamInfo->ParamType		 = ptButton;
		pParamInfo->Caption			 = "reset devices";
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		pParamInfo->ReadOnly		 = FALSE;
		pParamInfo->IsSeparator		 = TRUE;
        pParamInfo->DontSave		 = TRUE;
		pParamInfo->Translate		 = TRUE;
	}
	// m_ledDmxDeviceNames
	else if (ParamIndex >= (PARAMS_BEFORE_DMX_OUTS) && (ParamIndex < (PARAMS_BEFORE_DMX_OUTS + maxDmxOutputs)))
	{

		pParamInfo->ParamType		 = ptLeftLed;
		pParamInfo->Caption			 = getDmxOutputCaption (ParamIndex - PARAMS_BEFORE_DMX_OUTS);
		pParamInfo->SavedName		 = getDmxOutputsSavedName (ParamIndex - PARAMS_BEFORE_DMX_OUTS);
		pParamInfo->IsInput			 = TRUE;
		pParamInfo->IsOutput		 = TRUE;
		if (ParamIndex == PARAMS_BEFORE_DMX_OUTS)
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
void DeviceDMX::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
	// m_btnRescanAllDevices
	if (ParamIndex == 0)
	{
		m_btnRescanAllDevices = pEvent;
	}
	// m_btnResetAllDevices
	else if (ParamIndex == 1)
	{
		m_btnResetAllDevices = pEvent;
	}
	// m_ledDmxDeviceNames
	else if (ParamIndex >= (PARAMS_BEFORE_DMX_OUTS) && (ParamIndex < (PARAMS_BEFORE_DMX_OUTS + maxDmxOutputs)))
	{
		m_ledDmxDeviceNames.set(ParamIndex - PARAMS_BEFORE_DMX_OUTS, pEvent);
	}
	// default case
	else{}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DeviceDMX::onCallBack (UsineMessage* Message) 
{ 
    if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
    {
		int ParamIndex = (int)Message->wParam;
			
		// m_btnRescanAllDevices
		if ((ParamIndex == 0) && (Message->lParam == MSG_CHANGE))
		{
			rescanDmxDevices();
		}
		// m_btnResetAllDevices
		else if ((ParamIndex == 1) && (Message->lParam == MSG_CHANGE))
		{
			resetDmxDevices();
		}
		// m_ledDmxDeviceNames;
		else if (ParamIndex >= PARAMS_BEFORE_DMX_OUTS && (ParamIndex < (PARAMS_BEFORE_DMX_OUTS + maxDmxOutputs)) && (Message->lParam == MSG_CHANGE))
		{
			int numOutput = ParamIndex - PARAMS_BEFORE_DMX_OUTS;
			setOutputDmxDeviceState (numOutput);
		}
		// default case
		else{}
    }
}

//-----------------------------------------------------------------------------
// called by Usine to send out dmx
void DeviceDMX::onDMXSendOut (int deviceId, char* ByteArray, int len, int universeNum)
{
	try
	{
		if (deviceId > 0 && deviceId <= maxDmxOutputs)
		{
            //sdkTraceLogChar("onDMXSendOut");
            
            DeviceInfos* infos = listDeviceInfos.getUnchecked(deviceId - 1);
            

            if (infos->handle != nullptr && infos->isActive) 
            {
                MemoryBlock* dataBuffer = dataBuffers.getUnchecked(deviceId - 1);
                FTDI_PurgeBuffer (infos->handle);
                switch (infos->deviceType)
                {
                case dtEnttecOpenDMX:
                   {
			            // the scoped lock end automaticly at the end of the block
			            const ScopedLock myScopedLock (bufferLock);

                        // one byte for the start code
                        dataBuffer->ensureSize (len + 1);
                        dataBuffer->operator[](0) = DMX_PACKET_START_CODE;
                        dataBuffer->copyFrom (ByteArray, 1, len);
                        // databuffer is sent by the timer callback
                        // END SCOPED LOCK
                    }
                    break;
                case dtEnttecDMXProUSB:
                    {
                        dataBuffer->ensureSize (len + 1);
                        dataBuffer->operator[](0) = DMX_PACKET_START_CODE;
                        dataBuffer->copyFrom (ByteArray, 1, len);
                    
                        ftStatus = FTDI_SendData (infos->handle, SEND_DMX_PORT1, (unsigned char *)dataBuffer->getData(), (int)dataBuffer->getSize());
                        				
                        if (ftStatus != FT_OK)
				        {
					        sdkTraceErrorChar ("FAILED: Sending to DMX USB PRO");
				        }
                    }
                    break;
                case dtEnttecDMXProMK2:
                    {
                        dataBuffer->ensureSize (len + 1);
                        dataBuffer->operator[](0) = DMX_PACKET_START_CODE;
                        dataBuffer->copyFrom (ByteArray, 1, len);
                    
                        if (universeNum == 2)
                            ftStatus = FTDI_SendData (infos->handle, SEND_DMX_PORT2, (unsigned char *)dataBuffer->getData(), (int)dataBuffer->getSize());
                        else
                            ftStatus = FTDI_SendData (infos->handle, SEND_DMX_PORT1, (unsigned char *)dataBuffer->getData(), (int)dataBuffer->getSize());
                        				
                        if (ftStatus != FT_OK)
				        {
					        sdkTraceErrorChar ("FAILED: Sending to DMX USB PRO MK2");
				        }
                    }
                    break;
                default:
                    // try to send to an unknown device
                    break;
                }
            }
		}		
		else
		{
			//sdkTraceWarningChar(caErrorInvalidMidiOut);
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in onDMXSendOut");
	}
}
void DeviceDMX::hiResTimerCallback()
{
	try 
	{
		
    for (int i = 0; i < maxDmxOutputs; i++)
    {
        DeviceInfos* infos = listDeviceInfos.getUnchecked(i);
        

        if (infos->handle != nullptr && infos->isActive && infos->deviceType == dtEnttecOpenDMX) 
        {
            MemoryBlock* dataBuffer = dataBuffers.getUnchecked(i);
            
            FT_ResetDevice(infos->handle);
            FT_SetDivisor(infos->handle, (char)12);  // set baud rate
            FT_SetDataCharacteristics(infos->handle, FT_BITS_8, FT_STOP_BITS_2, FT_PARITY_NONE);
            FT_SetFlowControl(infos->handle, (char)FT_FLOW_NONE, 0, 0);
            FT_ClrRts(infos->handle);
            FT_Purge(infos->handle, FT_PURGE_TX);
            FT_Purge(infos->handle, FT_PURGE_RX);

            FT_SetBreakOn(infos->handle);
            FT_SetBreakOff(infos->handle);

			// the scoped lock end automaticly at the end of the block
			const ScopedLock myScopedLock (bufferLock);
			
            ftStatus = FT_Write (infos->handle, dataBuffer->getData(), (int)dataBuffer->getSize(), &written);
            if (ftStatus != FT_OK || written != dataBuffer->getSize()) 
            {
				sdkTraceErrorChar ("FAILED: Sending to OPENDMX USB");
            }
            // END SCOPED LOCK
        }
    }
	}
	catch (...)
	{
		sdkTraceErrorChar("exception in openDMX timer");
	}
	
}


//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------
// GetChunkLen 
int DeviceDMX::onGetChunkLen (LongBool Preset)
{
	int ChunkLen = 0;

	int choice = 0;

	// collect outputs choice by name into a quotted string
	StringArray listDmxOutputChoiceNames;

	for (int i = 0; i < maxDmxOutputs; i++)
	{
		choice = (int)sdkGetEvtData (m_ledDmxDeviceNames[i]);
		if (choice == 1)
		{
			listDmxOutputChoiceNames.set(i, listDeviceInfos[i]->name);
		}
		else
		{
			listDmxOutputChoiceNames.set(i, String());
		}
	}

	StringArrayToCommaString (qsChunkDmxOutputChoiceNames, listDmxOutputChoiceNames, false);
	sizeChunkDmxOutputChoiceNames = (int)qsChunkDmxOutputChoiceNames.toUTF8().sizeInBytes();
	caChunkDmxOutputChoiceNames = qsChunkDmxOutputChoiceNames.toUTF8();
	numOfChunkDmxOutputs = maxDmxOutputs;

	// compute size of the cells value array
	ChunkLen = ( sizeof(char) * 4		// the version header
		+ sizeof(int)						// num of midi out
		+ sizeof(int)						// sizeof qsChunkMidiOutputChoiceNames in byte
		+ sizeChunkDmxOutputChoiceNames	// outputs name content
		);

	return ChunkLen;
}

//-----------------------------------------------------------------------------
// GetChunk : store infos for patch/wkp/preset into chnuk
void DeviceDMX::onGetChunk (void* chunk, LongBool Preset)
{
	int charInt = sizeof(int);

	int charHeader = 4;
	
	// -----------------------------------------------------------------------
	// HEADER section
	// copy the header code into the chunk
	memcpy( chunk
		, (void*)CHUNCK_DX_01
		, charHeader 
		);

	// -----------------------------------------------------------------------
	// num of outputs section
	// copy the num of cols into the chunk
	memcpy( (void*)((char*)chunk + charHeader)
		, (void*)&numOfChunkDmxOutputs
		, charInt 
		);
	// -----------------------------------------------------------------------
	// outputs names content size section
	// copy the num of cols into the chunk
	memcpy( (void*)((char*)chunk + charHeader + charInt)
		, (void*)&sizeChunkDmxOutputChoiceNames
		, charInt 
		);
	// -----------------------------------------------------------------------
	// outputs names content section
	// copy the num of cols into the chunk
	memcpy( (void*)((char*)chunk + charHeader + charInt + charInt)
		, (void*)caChunkDmxOutputChoiceNames
		, sizeChunkDmxOutputChoiceNames 
		);
}

//-----------------------------------------------------------------------------
// SetChunk restore info for patch/wkp/preset from chunk
void DeviceDMX::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset)
{
	int charInt = sizeof(int);

	int charHeader = 4;

	char headerChunk[4];

	// extract the header code from the chunk
	memcpy( (void*)headerChunk
		, (void*)((char*)chunk)
		, charHeader 
		);

	//chunk was saved with version 1.6
	if ( String(CHUNCK_DX_01, 4) == String(headerChunk, 4) )
	{		

		// -----------------------------------------------------------------------
		// num of outputs section
		int numOfDmxOuts;
		memcpy( (void*)&numOfDmxOuts
			, (void*)((char*)chunk + charHeader)
			, charInt 
			);	
			
		if (numOfDmxOuts == maxDmxOutputs)
			numOfChunkDmxOutputs = numOfDmxOuts;
		//else
			// error

		// -----------------------------------------------------------------------
		// outputs names content size section
		int sizeOfDmxOutsNames;
		memcpy( (void*)&sizeOfDmxOutsNames
			, (void*)((char*)chunk + charHeader + charInt)
			, charInt 
			);

		// -----------------------------------------------------------------------
		// outputs names content section
		String qsDmxOutsNames = String::createStringFromData((AnsiCharPtr)chunk + charHeader + charInt + charInt, sizeOfDmxOutsNames);
			
		// for outputs
		listDmxOutputWantedNames.clear();
		listDmxOutputWantedNames.addTokens(qsDmxOutsNames, ",", String());
			
		if (listDmxOutputWantedNames.size() != maxDmxOutputs)
			sdkTraceErrorChar("DMX outs corrupted, check setup");

		tryRestoreWantedSettings(true);

	}
}



//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------

//
void DeviceDMX::initDeviceOpenDMXModule()
{
	captionDmxOutputsCaptions.resize (maxDmxOutputs);
	caDmxOutputsCaptions.resize (maxDmxOutputs);
	captionDmxOutputsSavedName.resize (maxDmxOutputs);
	caDmxOutputsSavedName.resize (maxDmxOutputs);
	listDmxOutputDevicesNames.ensureStorageAllocated (maxDmxOutputs);

	for (int i = 0; i < maxDmxOutputs; ++i)
	{
        DeviceInfos* deviceInfos = new DeviceInfos();

        deviceInfos->handle = nullptr;
        deviceInfos->isActive = false;
        deviceInfos->deviceType = dtUnknown;
        deviceInfos->latencyTimer = 0;
        listDeviceInfos.set (i, deviceInfos);

        dataBuffers.add (new MemoryBlock (DMX_UNIVERSE_SIZE + DMX_START_CODE_SIZE, true));
        dataBuffers[i]->operator[](0) = DMX_PACKET_START_CODE;
	}                 
}

//
void DeviceDMX::rescanDmxDevices()
{
	sdkTraceLogChar("begin rescanDmxDevices");

    DWORD numDevs=0;
	ftStatus = FT_ListDevices((PVOID)&numDevs,NULL,FT_LIST_NUMBER_ONLY);

    this->stringTrace = std::string("nb devices FTDI:") + sdkIntToString(numDevs);
    sdkTraceLogChar((AnsiCharPtr) stringTrace.c_str());
   
      for (int i = 0; i < maxDmxOutputs; ++i)
  	  {
        DeviceInfos* infos = listDeviceInfos.getUnchecked(i);

        if (infos->handle != nullptr) 
        {
		    FTDI_PurgeBuffer (infos->handle);
            FT_Close (infos->handle);
        }
		infos->handle = nullptr;
		infos->isActive = false;
		infos->vendor = String();
		infos->description = String();
		infos->deviceType = dtUnknown;
		infos->serial = String();
		infos->name = String();
		infos->params = nullptr;
		infos->latencyTimer = 0;
    }

    if (numDevs != 0)
    {
       openDmxDevices ();
    }
    
	for (int i = 0; i < maxDmxOutputs; i++)
	{
        sdkSetParamCaption(PARAMS_BEFORE_DMX_OUTS + i, getDmxOutputCaption (i));
    }

    sdkTraceLogChar("finish rescanDmxDevices");
}


void DeviceDMX::openDmxDevices()
{
    char cVendor[256];
    char cVendorId[256];
    char cDescription[256];
    char cSerial[256];
        
	long version;
	uint8 major_ver,minor_ver,build_ver;

    FT_PROGRAM_DATA pData;

    listDmxOutputDevicesNames.clear();

    for (int i = 0; i < maxDmxOutputs; ++i)
	{
		try 
		{
        DeviceInfos* infos = listDeviceInfos.getUnchecked(i);

        if (infos->handle != nullptr) 
        {
		    FTDI_PurgeBuffer (infos->handle);
            FT_Close (infos->handle);
        }

		infos->handle = nullptr;
		infos->isActive = false;
		infos->vendor = String();
		infos->description = String();
		infos->deviceType = dtUnknown;
		infos->serial = String();
		infos->name = String();
		infos->params = nullptr;
		infos->latencyTimer = 0;

        //*/
		ftStatus = FT_Open(i, &(infos->handle));
        Thread::getCurrentThread()->sleep (500);
        /*/
        int tries = 0;
 		do  
		{
			// Open the device 
			ftStatus = FT_Open(i, &(infos->handle));
			// delay for next try
			Sleep(750);
			tries ++;
		} while ((ftStatus != FT_OK) && (tries < 3)); 
        //*/

        if (ftStatus == FT_OK) 
        {
            if (i == 0)
            {
                // log infos about the drivers and dll version
		        ftStatus = FT_GetDriverVersion(infos->handle,(LPDWORD)&version);
		        if (ftStatus == FT_OK) 
		        {
			        major_ver = (version >> 16);
			        minor_ver = (version >> 8);
			        build_ver = (version & 0xFF);

	                captionLogMessage = "FTDI D2XX driver version : ";
                    captionLogMessage << (int)major_ver << "." << (int)minor_ver << "." << (int)build_ver;
	                caLogMessage = captionLogMessage.toUTF8().getAddress();
                    sdkTraceLogChar(caLogMessage);
		        }
		        else
                {
			        sdkTraceErrorChar("unable to load FTDI drivers");
                }
            }
            
            pData.Signature1 = 0;
            pData.Signature2 = 0xFFFFFFFF;
            pData.Version = 0x00000005;
            pData.Manufacturer = cVendor;
            pData.ManufacturerId = cVendorId;
            pData.Description = cDescription;
            pData.SerialNumber = cSerial;

            ftStatus = FT_EE_Read(infos->handle, &pData);
                
            if (ftStatus == FT_OK) 
            {
                DeviceInfos* infos = listDeviceInfos.getUnchecked(i);
                infos->vendor = String::fromUTF8 (pData.Manufacturer);
                infos->description = String::fromUTF8 (pData.Description);
                infos->serial = String::fromUTF8 (pData.SerialNumber);

                captionLogMessage = "FTDI D2XX device infos : ";
                captionLogMessage << "deviceId " << (i + 1) << ", " << "vdId:"<< infos->vendor << ", desc:" << infos->description << ", sn:" << infos->serial;
	            caLogMessage = captionLogMessage.toUTF8().getAddress();
                sdkTraceLogChar(caLogMessage);
            }

            identifyDmxDevice (i);

            listDmxOutputDevicesNames.set (i, infos->name);
            setOutputDmxDeviceState (i);
        }
        else
        {
            listDeviceInfos.getUnchecked(i)->vendor = String();
            listDeviceInfos.getUnchecked(i)->description = String();
            listDeviceInfos.getUnchecked(i)->serial = String();

            sdkTraceLogChar("no Dmx devices found");
        }
			
			
		}
		catch (...) 
		{
			sdkTraceLogChar("exeption in Dmx device");
		}
	}
}

//
int DeviceDMX::identifyDmxDevice (int index)
{
    DeviceInfos* infos = listDeviceInfos.getUnchecked(index);
    FT_HANDLE handle = infos->handle;

    int result;
	uint8_t data[4];
    int dataLength = 4;
    String deviceSerial;
	int size = 0;

    // GET Latency Timer
    ftStatus = FT_GetLatencyTimer (handle, (PUCHAR)&(infos->latencyTimer));
    if (ftStatus != FT_OK) infos->latencyTimer = 0;

    // SET Default Read & Write Timeouts (in micro sec ~ 100)
    FT_SetTimeouts (handle, PRO_R_TIMEOUT, PRO_W_TIMEOUT);

    // GET PRO's serial number 
    ftStatus = FTDI_SendData(handle, GET_WIDGET_SN, (unsigned char *)&size, 2);
    result = FTDI_ReceiveData(handle, GET_WIDGET_SN, (unsigned char *)&data, dataLength);
     
    if (result == NO_RESPONSE)
    {
        // maybe an enttec openDMX ?
        if (infos->description.contains ("FT232R"))
        {
			
            infos->deviceType = dtEnttecOpenDMX;
            infos->name = OPEN_DMX_NAME_PREFIX;
            infos->name << " (" << infos->serial << ")";
            
            dataBuffers[index]->ensureSize (513); // start code + 512 channels
            dataBuffers[index]->fillWith (0);

			startTimer (OPEN_DMX_TIMER_SLICE_TIME);
        }
        else
        {
            infos->deviceType = dtUnknown;
            infos->name = String();
        }
    }
    else
    {
        // Get params from device
        int recvd =0;
        unsigned char byte = 0;
        //int res = 0;
        int tries = 0;


        // Purges the buffer
        FT_Purge (handle, FT_PURGE_RX);
        // Send Get Widget Parameters to get Device Info
        ftStatus = FTDI_SendData (handle, GET_WIDGET_PARAMS, (unsigned char *)&size, 2);
        // Check Response
        if (ftStatus != FT_OK)
        {
            // Second try
	        FT_Purge (handle, FT_PURGE_TX);
	        ftStatus = FTDI_SendData (handle,GET_WIDGET_PARAMS,(unsigned char *)&size, 2);
	        if (ftStatus != FT_OK)
            {  
                sdkTraceLogChar("DMX - problem sending GET_WIDGET_PARAMS");
                infos->deviceType = dtUnknown;
                FT_Close (handle);
            }
        }
        // Receive Widget Response
        result = FTDI_ReceiveData(handle, GET_WIDGET_PARAMS_REPLY, (unsigned char *)&(infos->params), sizeof(DMXUSBPROParamsType));
        // Check Response
        if (result == NO_RESPONSE)
        {
            // second try
	        result = FTDI_ReceiveData(handle,GET_WIDGET_PARAMS_REPLY, (unsigned char *)&(infos->params), sizeof(DMXUSBPROParamsType));
	        if (result == NO_RESPONSE) 
            {  
                sdkTraceLogChar("DMX - problem receiving GET_WIDGET_PARAMS");
                infos->deviceType = dtUnknown;
                FT_Close (handle);
            }
        }
        // so it's an Enttec PRO
        if (result != NO_RESPONSE)
        {
            for (int i = dataLength - 1; i >= 0; i--)
            {
                deviceSerial += data[i] / 16;
                deviceSerial += data[i] % 16;
            }
            infos->serial = deviceSerial;
            
            // we have to test if it's an DMX USB PRO MK2
            // we can see that in the infos-d
            if (infos->description.compare ("DMX USB PRO Mk2") == 0)
            {
                infos->name = PRO_MK2_NAME_PREFIX;
                infos->name << " (" << deviceSerial << ")";
                
                infos->deviceType = dtEnttecDMXProMK2;

                unsigned char* MyKey = APIKey;
	            uint8_t PortSet[] = {1,1};
	            BOOL res = 0;

	            FTDI_PurgeBuffer(handle);
	            res = FTDI_SendData(handle, SET_API_KEY_LABEL,MyKey,4);
                Thread::getCurrentThread()->sleep (200);
	            //Sleep(200);
	            //printf("\nPRO Mk2 ... Activated ... ");

	            // Enable Ports to DMX on both 
	            res = FTDI_SendData(handle, SET_PORT_ASSIGNMENT_LABEL,PortSet,2);
                Thread::getCurrentThread()->sleep (200);
	            //Sleep(200);
	            //printf("\nPRO Mk2 ... Ready for DMX on both ports ... ");
            }
            else
            {
                infos->name = PRO_NAME_PREFIX;
                infos->name << " (" << deviceSerial << ")";

                infos->deviceType = dtEnttecDMXProUSB;
            }
            
            captionLogMessage = "deviceId ";
            captionLogMessage << index + 1 << ", name : "<< infos->name;
	        caLogMessage = captionLogMessage.toUTF8().getAddress();
            sdkTraceLogChar(caLogMessage); 

            if (infos->isActive)
            {
                // send a 1 length 0 message to init the device
                dataBuffers[index]->ensureSize (2);
                dataBuffers[index]->operator[](0) = DMX_PACKET_START_CODE;
                dataBuffers[index]->operator[](1) = 0;
                    
                ftStatus = FTDI_SendData (handle, SEND_DMX_PORT1, (unsigned char *)dataBuffers[index]->getData(), (int)dataBuffers[index]->getSize());
            }
        }
        /*

                        				
        if (ftStatus != FT_OK)
        {
            sdkSetEvtData(m_ledDmxDeviceNames[index], 0.0);
			sdkTraceErrorChar ("FAILED: Sending DMX init to PRO");
		}
        */
    
    }

    return result;
}

//
void DeviceDMX::resetDmxDevices()
{
	try 
	{
		
    for (int i = 0; i < maxDmxOutputs; ++i)
	{
        FT_HANDLE handle = listDeviceInfos.getUnchecked(i)->handle;
        
        if (handle != nullptr) 
        {
			try 
			{
				ftStatus = FT_ResetDevice(handle);
				
			}
			catch (...) 
			{
				sdkTraceLogChar("exeption resetDmxDevices for one device");
			}
        }
    }
	}
	catch (...) 
	{
		sdkTraceLogChar("exeption resetDmxDevices");
	}
}


char* DeviceDMX::getDmxOutputCaption (int index)
{
	String name(sdkGetTrans("no device"));

    DeviceInfos* infos = listDeviceInfos[index];
	if (infos != nullptr && infos->name.isNotEmpty())
    {
        name = "";
       name << String (index + 1) << " - "  << infos->name;
    }
	captionDmxOutputsCaptions.set(index, name);
	caDmxOutputsCaptions.set(index, captionDmxOutputsCaptions[index].toUTF8 ());
	return const_cast <char*> (caDmxOutputsCaptions[index]);
}

char* DeviceDMX::getDmxOutputsSavedName (int index)
{
	captionDmxOutputsSavedName.set(index, "Out " + String(index + 1));
	caDmxOutputsSavedName.set(index, captionDmxOutputsSavedName[index].toUTF8 ());
	return const_cast <char*> (caDmxOutputsSavedName[index]);
}

//
void DeviceDMX::setOutputDmxDeviceState (int index)
{
	try
	{
        DeviceInfos* infos = listDeviceInfos.getUnchecked(index);

        
        if (infos->handle != nullptr) 
        {
			int state = (int)sdkGetEvtData (m_ledDmxDeviceNames[index]);
			
			if (state == 1)
			{
                MemoryBlock* dataBuffer = dataBuffers.getUnchecked(index);
                infos->isActive = true;

                // send a 1 length 0 message to init the device
                dataBuffer->ensureSize (2);
                dataBuffer->operator[](0) = DMX_PACKET_START_CODE;
                dataBuffer->operator[](1) = 0;
                    
                ftStatus = FTDI_SendData (infos->handle, SEND_DMX_PORT1, (unsigned char *)dataBuffer->getData(), (int)dataBuffer->getSize());
			}
			else
			{
                infos->isActive = false;

		        FTDI_PurgeBuffer (infos->handle);
    
                // we have to send another command to exit the send mode of the enttec PRO
                // we dont care about the response
                if (infos->deviceType == dtEnttecDMXProUSB)
                {
                    int size = 0;
	                uint8_t data[4];
                    int dataLength = 4;

                    ftStatus = FTDI_SendData(infos->handle, GET_WIDGET_SN, (unsigned char *)&size, 2);
                    FTDI_ReceiveData(infos->handle, GET_WIDGET_SN, (unsigned char *)&data, dataLength);
                }
        
                //sdkSetEvtData(m_ledDmxDeviceNames[index], 0.0);
			}
        }
	}
	catch (...)
	{
		sdkTraceErrorChar("in setOutputDmxDeviceState.");
	}
}

void DeviceDMX::closeDmxDevice(int index)
{
	try
	{
    DeviceInfos* infos = listDeviceInfos.getUnchecked(index);

    if (infos->handle != nullptr) 
    {
        if (infos->deviceType == dtEnttecDMXProUSB)
        {
            int size = 0;
	        uint8_t data[4];
            int dataLength = 4;

            ftStatus = FTDI_SendData(infos->handle, GET_WIDGET_SN, (unsigned char *)&size, 2);
            FTDI_ReceiveData(infos->handle, GET_WIDGET_SN, (unsigned char *)&data, dataLength);
        }

	    FTDI_PurgeBuffer (infos->handle);
        ftStatus = FT_Close (infos->handle);
        infos->handle = nullptr;
    }
}
catch (...)
{
	sdkTraceErrorChar("in closeDmxDevice.");
}
}


//-------------------------------------------------------------------------
// settings
void DeviceDMX::tryRestoreWantedSettings(bool recursive)
{
	// try to retrieve user choice from existing midi devices
	String choice;

	for (int i = 0; i < maxDmxOutputs; i++)
	{
			sdkSetEvtData(m_ledDmxDeviceNames[i], 0.0);
	}


	// for outputs
	for (int i = 0; i < maxDmxOutputs; i++)
	{
		choice = listDmxOutputWantedNames[i].unquoted();
		if (listDmxOutputDevicesNames.contains(choice))
		{
			int newIndex = listDmxOutputDevicesNames.indexOf(choice);
            listDeviceInfos.swap (i, newIndex);
            sdkSetParamCaption(PARAMS_BEFORE_DMX_OUTS + newIndex, getDmxOutputCaption (newIndex));
            sdkSetParamCaption(PARAMS_BEFORE_DMX_OUTS + i, getDmxOutputCaption (i));
			sdkSetEvtData(m_ledDmxDeviceNames[i], 1.0);
			setOutputDmxDeviceState(newIndex);
		}
	}

	//if (missing && recursive)
	//	popupRestoreWantedSettings();
}

void DeviceDMX::popupRestoreWantedSettings()
{
	String popupMessage = sdkGetTrans("Some DMX device are missing:\nconnect them and press Ok\npress Cancel to abort");
	const char* mess = popupMessage.toUTF8();
	int result = sdkDialogConfirmationOKCancel(const_cast <char*> (mess));

	if (result == idOk) // 1 = ok, 2 = cancel
	{
		rescanDmxDevices();
		tryRestoreWantedSettings(false);
	}
}

