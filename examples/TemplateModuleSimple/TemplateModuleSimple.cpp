//-----------------------------------------------------------------------------
//@file  
//	TemplateModuleSimple.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the TemplateModuleSimple class.
//
//  A very basic user module with minimal stuff to make it work.
//
//@historic 
//	2013/06/28
//    first release for Hollyhock CPP SDK 6.01.002 
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.com 
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
#include "TemplateModuleSimple.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TemplateModuleSimple();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((TemplateModuleSimple*)pModule);
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
TemplateModuleSimple::TemplateModuleSimple()
{
	//
}

// destructor
TemplateModuleSimple::~TemplateModuleSimple()
{
	// 
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "template simple";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "template simple example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//void TemplateModuleSimple::onCreate(AinsiCharPtr optionalString);
//void TemplateModuleSimple::onDestroy(); 
void TemplateModuleSimple::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = 2;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= TRUE;
}

//-----------------------------------------------------------------------------
// init
void TemplateModuleSimple::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {}

//-----------------------------------------------------------------------------
// parameters and process
void TemplateModuleSimple::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
        // fdrFrequence
        case 0:
		    pParamInfo->ParamType       = ptDataFader;
		    pParamInfo->Caption         = "freq";
		    pParamInfo->IsInput         = TRUE;
		    pParamInfo->IsOutput        = FALSE;
		    pParamInfo->MinValue        = 0.1f;
		    pParamInfo->MaxValue        = 20000.0f;
		    pParamInfo->DefaultValue    = 50.0f;
		    pParamInfo->Symbol          = "Hz";
		    pParamInfo->Format          = "%.2f";
            pParamInfo->Scale           = scLog;
			pParamInfo->setEventClass	(fdrFrequence);

	        break;  
        // fdrPeriod
        case 1:
		    pParamInfo->ParamType       = ptDataFader;
		    pParamInfo->Caption         = "period";
		    pParamInfo->IsInput         = FALSE;
		    pParamInfo->IsOutput        = TRUE;
		    pParamInfo->MinValue        = 0.05f;
		    pParamInfo->MaxValue        = 10000.0f;
		    pParamInfo->DefaultValue    = 0.0f;
            pParamInfo->Symbol          = "mS";
		    pParamInfo->Format          = "%.2f";
            pParamInfo->Scale           = scLog;
		    pParamInfo->ReadOnly		= TRUE; // can't change the field
			pParamInfo->setEventClass	(fdrPeriod);
			break;

	        // default case
        default:
	        break;
	}
}


void TemplateModuleSimple::onCallBack (TUsineMessage *Message) 
{
	// WParam contains the Param Number
	switch (Message->wParam) 
	{
	// Selected point index data field input/output
	case 0:
		if (Message->lParam == MSG_CHANGE) 
            computePeriod ();
		break;

	// default case
	default:
		// do nothing
		break;
	}
}

void TemplateModuleSimple::onProcess () 
{

}


void TemplateModuleSimple::computePeriod ()
{
    fdrPeriod.setData(1000.0f / fdrFrequence.getData());
}