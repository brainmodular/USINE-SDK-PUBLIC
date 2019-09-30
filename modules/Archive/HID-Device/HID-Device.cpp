//-----------------------------------------------------------------------------
//@file  
//	HID-Device.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the HIDDevice class.
//
//  A template include to start from for new user module.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
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
#include "HID-Device.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    initialiseJuce_GUI ();
	pModule = new HIDDevice();
}

// destroy
void DestroyModule(void* pModule) 
{    try 
    {
		// cast is important to call the good destructor
		delete ((HIDDevice*)pModule);
		shutdownJuce_GUI (); 
    }
    catch (...)
    {
    }
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "HID Device";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "HID Device";
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
HIDDevice::HIDDevice()
    : wantTraceData (false)
    , infoHidDevices (nullptr)
    , currentHidDevice (nullptr)
{

	//hid_init ();
 //   constructHidDevicesList ();

}

// destructor
HIDDevice::~HIDDevice()
{ 
    try 
    {
		arrayHidDevicesInfos.clear ();
		hid_free_enumeration(infoHidDevices);
		
		if (currentHidDevice != nullptr)
		{
			stopTimer ();
			hid_close(currentHidDevice);
		}
    
		hid_exit (); 
    }
    catch (...)
    {
    }
}

void HIDDevice::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = 4;
	pModuleInfo->DontProcess		= FALSE;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init
int  HIDDevice::onGetNumberOfParams( int QIdx) {return 0;}
void HIDDevice::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int QIdx) {}
void HIDDevice::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
    try 
    {
        sdkTraceLogChar ("start HID enumeration :");
        for (int i = 0; i < arrayHidDevicesInfos.size(); i++) 
        {
            hid_device_info* device = arrayHidDevicesInfos[i];
            String vendorDesc (device->manufacturer_string);
            String productDesc (device->product_string);

            if (vendorDesc == String::empty)
                vendorDesc = "unknown vendor";
        
            if (productDesc == String::empty)
                productDesc = "unknown product";
        
            String vendorId = String::toHexString (device->vendor_id).paddedLeft ('0', 4);
            String productId  = String::toHexString(device->product_id).paddedLeft ('0', 4);
        
            String usageId = String::toHexString (device->usage).paddedLeft ('0', 4);
            String usagePageId  = String::toHexString(device->usage_page).paddedLeft ('0', 4);

            traceString = String::empty;
            traceString << "idx=" << i << " vid=0x"<< vendorId.trim () << " pid=0x" << productId.trim () << " upid=0x"<< usagePageId.trim () << " uid=0x" << usageId.trim () << " vdesc=" << vendorDesc.trim () << " pdesc=" << productDesc.trim ();
        
            sdkTraceLogChar (traceString.toUTF8 ().getAddress ());
        }
        sdkTraceLogChar ("end HID enumeration :");
    }
    catch (...)
    {
        //sdkTraceErrorChar("in createPlugin");
    }
}

//-----------------------------------------------------------------------------
// parameters and process
void HIDDevice::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
        // Add all parameters declared in the module class, example :

        // cboxHidDevices
        case 0:
            hid_init ();
            constructHidDevicesList ();
		    pParamInfo->ParamType			= ptListBox;
		    pParamInfo->Caption				= "devices";
		    pParamInfo->IsInput				= TRUE;
		    pParamInfo->IsOutput			= FALSE;
            pParamInfo->ListBoxStrings		= (AnsiCharPtr)commaHidDevicesList.toUTF8().getAddress();
		    pParamInfo->Symbol				= "";
		    pParamInfo->Format				= "%.0f";
			pParamInfo->DontSave			= FALSE;
			pParamInfo->IsStoredInPreset	= TRUE;
		    break;

	        break;
			// txtHidDevicesCommaText
        case 1:
		    pParamInfo->ParamType		= ptTextField;
		    pParamInfo->Caption			= "comma text";
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
			pParamInfo->DontSave		= TRUE;
			pParamInfo->IsStoredInPreset	= FALSE;
		    //pParamInfo->ReadOnly		= TRUE;
		    pParamInfo->TextValue		= (AnsiCharPtr)commaHidDevicesList.toUTF8().getAddress();
	        break;
        // arrayBytesOut
        case 2:
		    pParamInfo->ParamType		= ptArray;
		    pParamInfo->Caption			= "bytes out";
		    pParamInfo->IsInput			= FALSE;
		    pParamInfo->IsOutput		= TRUE;
		    pParamInfo->ReadOnly		= TRUE;
            pParamInfo->MinValue		= - FLT_MAX;
		    pParamInfo->MaxValue		= FLT_MAX;
	        break;
        // switchTraceData
        case 3:
			pParamInfo->ParamType		= ptSwitch;
			pParamInfo->Caption			= "trace data";
			pParamInfo->DefaultValue	= 1.0f;
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= FALSE;
	        break;

	        // default case
        default:
	        break;
	}
}
void HIDDevice::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent)
{
	switch (ParamIndex) 
    {
        // cboxHidDevices
        case 0:
	        cboxHidDevices = pEvent;
	        break;
		// txtHidDevicesCommaText
        case 1:
	        txtHidDevicesCommaText = pEvent;
	        break;
        // arrayBytesOut
        case 2:
	        arrayBytesOut = pEvent;
	        break;
        // switchTraceData
        case 3:
	        switchTraceData = pEvent;
	        break;

        // default case
        default:
	        break;
	}
}

void HIDDevice::onCallBack (UsineMessage *Message) 
{
	// cboxHidDevices
	if ( (Message->wParam == 0) && (Message->lParam == MSG_CHANGE) ) 
	{
        connectHidDevice ((int)sdkGetEvtData (cboxHidDevices));
	}
    // switchTraceData
	else if ( (Message->wParam == 3) && (Message->lParam == MSG_CHANGE) ) 
	{
		float result = sdkGetEvtData (switchTraceData);

        if (result == 1)
            wantTraceData = true;
        else
            wantTraceData = false;
	}
}

void HIDDevice::onProcess () {}

//-----------------------------------------------------------------------------
// midi out callbacks
void HIDDevice::onMidiSendOut (int DeviceID, UsineMidiCode Code) {}
void HIDDevice::onMidiSysexSendOut (int DeviceID, char* Sysex) {}

//-----------------------------------------------------------------------------
// chunk system
int  HIDDevice::onGetChunkLen (LongBool Preset) {return 0;}
void HIDDevice::onGetChunk (void* chunk, LongBool Preset) {}
void HIDDevice::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) {}

//-----------------------------------------------------------------------------
// layout
void HIDDevice::onCreateSettings() {}
void HIDDevice::onSettingsHasChanged() {}
void HIDDevice::onPaint () {}

//-----------------------------------------------------------------------------
// mouse and multi touch interaction
void HIDDevice::onMouseMove(TShiftState Shift, float X, float Y) {}
void HIDDevice::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
void HIDDevice::onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y) {}
void HIDDevice::onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
void HIDDevice::onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y) {}
void HIDDevice::onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y) {}
void HIDDevice::onOpenEditor() {}
void HIDDevice::onBringToFront() {}

//-----------------------------------------------------------------------------
// audio setup update
void HIDDevice::onBlocSizeChange (int BlocSize) {}
void HIDDevice::onSampleRateChange (double SampleRate) {}

//-----------------------------------------------------------------------------
// recording 
void HIDDevice::onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z) {}

void HIDDevice::connectHidDevice (int indexDevice)
{
    try 
	{
		if (indexDevice >= 0 && indexDevice < arrayHidDevicesInfos.size())
		{
			if (currentHidDevice != nullptr)
			{
				stopTimer ();
				hid_close(currentHidDevice);
			}

			currentHidDevice =  hid_open_path(arrayHidDevicesInfos[indexDevice]->path);
	
	    if (!currentHidDevice) 
        {
			traceString = String::empty;
			traceString << "HID module, unable to connect to device " << arrayHidDevicesInfos[indexDevice]->product_string;
		    sdkTraceWarningChar (traceString.toUTF8().getAddress());
	    }
        else
        {
			traceString = String::empty;
			traceString << "HID module, connected to device " << arrayHidDevicesInfos[indexDevice]->product_string;
		    sdkTraceChar (traceString.toUTF8().getAddress());
			
	        hid_set_nonblocking(currentHidDevice, 1);
				startTimer (5);
			}
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("in connectHidDevice");
	}
}

void HIDDevice::timerCallback ()
{
    try 
    {
        if (currentHidDevice != nullptr)
        {
            unsigned char buf[256];
            int res = hid_read(currentHidDevice, buf, sizeof(buf));
            bool wantUpdate = false;

            if (res > 0) 
            {
                sdkSetEvtSize (arrayBytesOut, res);
                TPrecision* outputBuffer = sdkGetEvtDataAddr(arrayBytesOut);

                for (int i = 0; i < res; i++) 
                {
                    if (outputBuffer[i] != buf[i])
                    {
                        outputBuffer[i] = buf[i];
                        wantUpdate = true;
                    }
                }

                if (wantTraceData && wantUpdate)
                {
                    traceString = String::empty;

                    for (int j = 0; j < res; j++) 
                    {
                        traceString << (int)buf[j] << " ";
                    }

                    sdkTraceChar (traceString.toUTF8().getAddress());
                }
            }
            if (res < 0) 
            {
                // hid_read() returned error
            }
        }
    }
    catch (...)
    {
        //sdkTraceErrorChar("in createPlugin");
    }
}

void HIDDevice::constructHidDevicesList ()
{
    try 
    {
        hid_free_enumeration(infoHidDevices);
        arrayHidDevicesInfos.clear ();
        // enumerate all vendors and all products 
        infoHidDevices = hid_enumerate(0x0, 0x0);
        commaHidDevicesList = String::empty;

        hid_device_info* devicesInfos = infoHidDevices;
        // put them in an array
        while (devicesInfos) 
        {
            arrayHidDevicesInfos.add (devicesInfos);
            devicesInfos = devicesInfos->next;
        }

        // sort the array 
        HIDDeviceComparator comp;
        arrayHidDevicesInfos.sort (comp);

        // construct the comma text used by the cbox param
        for (int i = 0; i < arrayHidDevicesInfos.size(); i++) 
        {

            hid_device_info* device = arrayHidDevicesInfos[i];
            String vendorDesc (device->manufacturer_string);
            String productDesc (device->product_string);

            if (vendorDesc == String::empty)
                vendorDesc = "unknown vendor";
        
            if (productDesc == String::empty)
                productDesc = "unknown product";
        
            String vendorId = String::toHexString (device->vendor_id).paddedLeft ('0', 4);
            String productId  = String::toHexString(device->product_id).paddedLeft ('0', 4);
        
            String usageId = String::toHexString (device->usage).paddedLeft ('0', 4);
            String usagePageId  = String::toHexString(device->usage_page).paddedLeft ('0', 4);

            traceString = String::empty;
            traceString << productDesc.trim () << " - " << vendorDesc.trim ();



            commaHidDevicesList.append (String("\""), 256);
            commaHidDevicesList.append (traceString, traceString.length ());
            commaHidDevicesList.append (String("\""), 256);

            if (i < arrayHidDevicesInfos.size() - 1)
                commaHidDevicesList.append (String(","), 256);
        }
    }
    catch (...)
    {
        //sdkTraceErrorChar("in createPlugin");
    }

}

