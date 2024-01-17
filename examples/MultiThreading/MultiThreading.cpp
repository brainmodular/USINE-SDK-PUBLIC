//-----------------------------------------------------------------------------
//@file  
//	MultiThreading.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the MultiThreading class.
//
//  Example user module to show how to process audio buffers.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
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
#include "MultiThreading.h"
#include <string> 

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MultiThreading();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((MultiThreading*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "multithreading";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "multithreading sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
MultiThreading::MultiThreading()
 
{
	

}

// destructor
MultiThreading::~MultiThreading()
{
	sdkThreadDestroy(thread1);
	sdkThreadDestroy(thread2);
	sdkCriticalSectionDestroy(criticalSection);
}

void MultiThreading::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;  
	pModuleInfo->CanBeSavedInPreset = FALSE;

}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int MultiThreading::onGetNumberOfParams (int queryResult1, int queryResult2)
{
	int result = 1;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void MultiThreading::onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}

//-----------------------------------------------------------------------------
// process for thread1
void processThread1(void* pModule, TThreadPtr pThread)
{
	MultiThreading* module = static_cast<MultiThreading*>(pModule);
	try
	{
		module->sdkCriticalSectionLock(module->criticalSection);
		module->sdkTraceChar("thread 1");
	}
	catch (...)
	{
	}
	module->sdkCriticalSectionUnLock(module->criticalSection);
}

//-----------------------------------------------------------------------------
// process for thread1
void processThread2(void* pModule, TThreadPtr pThread)
{
	MultiThreading* module = static_cast<MultiThreading*>(pModule);
	try
	{
		module->sdkCriticalSectionLock(module->criticalSection);
		module->sdkTraceChar("thread 2");
	}
	catch (...)
	{
	}
	module->sdkCriticalSectionUnLock(module->criticalSection);
}

//-----------------------------------------------------------------------------
// initialisation
void MultiThreading::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
    //syncObject = sdkSyncObjectCreate();
    criticalSection = sdkCriticalSectionCreate("critical section",true);
	thread1 = sdkThreadCreate("thread 1", &processThread1, tpMedium, 100);
	thread2 = sdkThreadCreate("thread 2", &processThread2, tpMedium, 100);
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void MultiThreading::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
	if (ParamIndex == 0)
	{
	pParamInfo->ParamType			= ptArray;
	pParamInfo->Caption				= "output";
	pParamInfo->IsInput				= FALSE;
	pParamInfo->IsOutput			= TRUE;
	pParamInfo->IsSeparator			= TRUE;
	pParamInfo->CallBackType		= ctNormal;
	pParamInfo->SeparatorCaption	= "";
	pParamInfo->setEventClass		(output);

	}
}


//-----------------------------------------------------------------------------
// Parameters callback
void MultiThreading::onCallBack (TUsineMessage *Message) 
{
    // filter only message specific to this user module
    if (Message->message == NOTIFY_MSG_USINE_CALLBACK)
    {
    }
}


//-----------------------------------------------------------------------------
// Parameters callback
void MultiThreading::onProcess () 
{
	


}
