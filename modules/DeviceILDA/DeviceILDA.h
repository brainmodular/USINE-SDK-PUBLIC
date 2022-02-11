//-----------------------------------------------------------------------------
//@file  
//	DeviceILDA.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the DeviceILDA class.
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

// include once, no more
#ifndef __TEMPLATE_MODULE_H__
#define __TEMPLATE_MODULE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"
#include "HeliosDac.h"

#include <iostream>
#include <cmath>
#include <limits>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define MAX_HELIOS_DAC 8
#define MAX_NUMBER_OF_POINTS 4096
//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class DeviceILDA : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	DeviceILDA();

    // destructor
	virtual ~DeviceILDA();

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	int  onGetNumberOfParams( int QIdx);
	void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int QIdx);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (UsineMessage *Message);	
	void onILDASendOut(int DeviceID, TUsineILDAPoint** arrayPoint, int arraySize, int speedPPS);

	//-------------------------------------------------------------------------
	// public methodes
	//-------------------------------------------------------------------------
public:

    // public methodes goes here

	//-------------------------------------------------------------------------
	// protected members
	//-------------------------------------------------------------------------
protected:
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr m_rescan;          // rescan devices
	UsineEventPtr m_deviceName[MAX_HELIOS_DAC];       // name of the device

    // protected members goes here
	int numDevs;
	HeliosDac helios;
	void rescanDevices();
	std::string deviceIDstring[MAX_HELIOS_DAC];

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:  

    // private methodes goes here

}; // class DeviceILDA

#endif //__TEMPLATE_MODULE_H__