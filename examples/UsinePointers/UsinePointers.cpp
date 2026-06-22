//-----------------------------------------------------------------------------
//@file  
//	UsinePointers.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the UsinePointers class.
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
#include "UsinePointers.h"

constexpr NativeInt USINE_POINTER_CBID = 0x002300F0;
//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsinePointers();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((UsinePointers*)pModule);
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
UsinePointers::UsinePointers()
{
	//
}

// destructor
UsinePointers::~UsinePointers()
{
	// 
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "pointer example";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "example of pointers";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//void UsinePointers::onCreate(AinsiCharPtr optionalString);
//void UsinePointers::onDestroy(); 
void UsinePointers::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->NumberOfParams     = 2;
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->DontProcess		= TRUE;
}

//-----------------------------------------------------------------------------
// init
void UsinePointers::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	doSomething();
}

//-----------------------------------------------------------------------------
// parameters and process
void UsinePointers::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
	switch (ParamIndex) 
    {
        // fdrFrequence
        case 0:
		    pParamInfo->ParamType           = ptPointer;
		    pParamInfo->Caption             = "pointer in";
		    pParamInfo->IsInput             = TRUE;
		    pParamInfo->IsOutput            = FALSE;
			pParamInfo->setEventClass   	(fdrPointerIn);
			pParamInfo->CallBackType		= ctNone;
			pParamInfo->PointerDataType     = usinePtrFloat;  // type of the pointer can be any uint
	        break;  
        // fdrPeriod
        case 1:
		    pParamInfo->ParamType           = ptPointer;
		    pParamInfo->Caption             = "pointer out";
		    pParamInfo->IsInput             = FALSE;
		    pParamInfo->IsOutput            = TRUE;
			pParamInfo->setEventClass	    (fdrPointerOut);
			pParamInfo->CallBackType		= ctNone;
			pParamInfo->PointerDataType		= usinePtrFloat;  // type of the pointer can be any uint
			break;
	        // default case
        default:
	        break;
	}
}

void UsinePointers::onCallBack(TUsineMessage *Message) 
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;
	// WParam contains the Param Number
	switch (Message->wParam) 
	{
	case USINE_POINTER_CBID:
        doSomething();
		break;
	// default case
	default:
		// do nothing
		break;
	}
}

void UsinePointers::doSomething()
{
	// do something with the pointer
	// for example, copy the pointer to the output
	if (fdrPointerIn.getPointerDataType() == usinePtrFloat)
	{
		UINT32 size = fdrPointerIn.getPointerDataSize();
		fdrPointerOut.setPointerDataSize(size);
		if (size != 0)
		{
			memcpy(fdrPointerOut.getPointerData(), fdrPointerIn.getPointerData(), size);
		}
		// set the pointer data changed flag to notify Usine that the pointer data has been updated
		fdrPointerOut.setPointerDataChanged();
	}
	else
	{
		// handle error, pointer data type is not correct
		sdkTraceErrorChar("the pointer is not type float");
	}	
}