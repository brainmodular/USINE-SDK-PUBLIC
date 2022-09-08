//-----------------------------------------------------------------------------
//@file  
//	DeviceLaser.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the DeviceLaserclass.
//
//  A template include to start from for new user module.
//
//@historic 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//@IMPORTANT
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
#include "DeviceLaser.h"
#include "HeliosDac.h"

#include "../../sdk/UsineDefinitions.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DeviceLaser();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((DeviceLaser*)pModule);
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
DeviceLaser::DeviceLaser()
{
	//
}

// destructor
DeviceLaser::~DeviceLaser()
{
	helios.CloseDevices();
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "Device Laser";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Device for Laser ILDA DAC";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

void DeviceLaser::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtDeviceLaser;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = 1+ MAX_HELIOS_DAC;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= TRUE;
}

//-----------------------------------------------------------------------------
// query system and init
int  DeviceLaser::onGetNumberOfParams( int QIdx) {return 0;}
void DeviceLaser::onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int QIdx) {}

//-----------------------------------------------------------------------------
void DeviceLaser::rescanDevices()
{
	helios.CloseDevices();
	numDevs = helios.OpenDevices();
	
	char* name = new char[32];
	for (int i=0; i < MAX_HELIOS_DAC; i++)
	{
		if (helios.GetName(i, name) == HELIOS_SUCCESS)
		{
			sdkSetEvtPChar(m_deviceName[i], name);
		}
		else
		{
			sdkSetEvtPChar(m_deviceName[i],"");

		}
	}
}


//-----------------------------------------------------------------------------
void DeviceLaser::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo)
{
	rescanDevices();
}

//-----------------------------------------------------------------------------
// parameters and process
void DeviceLaser::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
		// m_rescan
  	   case 0:
			pParamInfo->ParamType = ptButton;
			pParamInfo->Caption = "rescan";
			pParamInfo->IsInput = TRUE;
			pParamInfo->IsOutput = FALSE;
			pParamInfo->CallBackType = ctAsynchronous;
			pParamInfo->CallBackId = 0xFFB;
			pParamInfo->EventPtr = &m_rescan;
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = "helios devices";

			break;

        // m_deviceName

	        // default case
        default:
			if (ParamIndex <= MAX_HELIOS_DAC)
			{
				
				ParamIndex--;
				pParamInfo->ParamType = ptTextField;
				deviceIDstring[ParamIndex] = std::string("ID "+ std::to_string(ParamIndex));			
				pParamInfo->Caption = (AnsiCharPtr)deviceIDstring[ParamIndex].c_str();
				pParamInfo->TextValue = "";
				pParamInfo->ReadOnly = TRUE;
				pParamInfo->IsInput = FALSE;
				pParamInfo->IsOutput = TRUE;
				pParamInfo->CallBackId = -1;
				pParamInfo->EventPtr = &m_deviceName[ParamIndex];
				pParamInfo->DontSave = true;
				if (ParamIndex == 0)
				{
					pParamInfo->IsSeparator = TRUE;
					pParamInfo->SeparatorCaption = "available devices";
				}
				break;
			}
			else break;
	}
}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DeviceLaser::onCallBack (UsineMessage *Message)
{
	// filter only message specific to this user module
	if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
	{

		// FileName,SizeFactor,Predelay
		if ((Message->wParam == 0xFFB) && (Message->lParam == MSG_CHANGE))
		{
			rescanDevices();
		}
	}

}

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
void DeviceLaser::onLaserSendOut(int DeviceID, TUsineILDAPoint** arrayPoint, int arraySize, int speedPPS)
{
	HeliosPoint frame[MAX_NUMBER_OF_POINTS];
	arraySize = min(MAX_NUMBER_OF_POINTS, arraySize);
	if ((DeviceID < numDevs) && (DeviceID >= 0))
	{
		for (int i = 0; i < arraySize; i++)
		{
			TUsinePixel pix;
			pix = sdkColorToPixel((*arrayPoint)[i].Color);
			frame[i].x = 0xFFF * (*arrayPoint)[i].x;
			frame[i].y = 0xFFF * (*arrayPoint)[i].y;
			frame[i].r = pix.R;
			frame[i].g = pix.G;
			frame[i].b = pix.B;
			frame[i].i = pix.A;
		}
		//wait for ready status
		for (unsigned int k = 0; k < 512; k++)
		{
			if (helios.GetStatus(DeviceID) == 1)
			{
				helios.WriteFrame(DeviceID, speedPPS, HELIOS_FLAGS_DEFAULT, &frame[0], arraySize); //send the next frame
				break;
			}
		}
	}
}

