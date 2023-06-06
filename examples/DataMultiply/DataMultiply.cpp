//-----------------------------------------------------------------------------
//@file  
//	DataMultiply.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the DataMultiplyExample class.
//
//  Example user module to show how to update outputs from inputs change.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs  :
//  support@brainmodular.com
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 BrainModular
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
#include "DataMultiply.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataMultiplyExample ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((DataMultiplyExample*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "data multiply example";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "data multiply sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void DataMultiplyExample::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{

	//identification of the module
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= TRUE;
	pModuleInfo->CanBeSavedInPreset = FALSE;

}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
// query system not used

//void DataMultiplyExample::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
//{
//}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void DataMultiplyExample::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{
	// all parameters declared in the module class		
	switch (ParamIndex) 
	{
	// dtfInputA
	case 0:
		pParamInfo->ParamType		= ptDataField;
		pParamInfo->Caption			= "a";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->CallBackType	= ctImmediate;
		pParamInfo->EventPtr        = &dtfInputA;

		break;

	// dtfInputB
	case 1:
		pParamInfo->ParamType		= ptDataField;
		pParamInfo->Caption			= "b";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->CallBackType	= ctImmediate;
		pParamInfo->EventPtr        = &dtfInputB;
		break;

	// dftOutput
	case 2:
		pParamInfo->ParamType		= ptDataField;
		pParamInfo->Caption			= "result";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->DontSave		= TRUE;
		pParamInfo->EventPtr        = &dftOutput;

		break;

	// default case
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void DataMultiplyExample::onCallBack (TUsineMessage* Message) 
{
	try
	{
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
        {
            int paramIndex = (int)Message->wParam;

		    if ((paramIndex == 0 || paramIndex == 1) && (Message->lParam == MSG_CHANGE))
		    {
			    sdkMultEvt3 (dtfInputA, dtfInputB, dftOutput);
		    }
        }
	}
	catch (...)
	{
		//sdkTraceErrorChar("error");
	}
}



