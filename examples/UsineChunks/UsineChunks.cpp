//-----------------------------------------------------------------------------
//@file  
//	UsineChunks.cpp
//
//@author
//	BrainModular team
//
//@brief 
//	Implementation of the UsineChunks class.
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
#include "UsineChunks.h"
#include "../../sdk/UsineDefinitions.h"  

constexpr NativeInt CHUNK_SUB_DEL_CBID  = 0x00000FF0;
constexpr NativeInt CHUNK_SUB_ITEM_CBID = 0x00000FF1;
constexpr NativeInt CHUNK_SUB_TEXT_CBID = 0x00000FF2;
constexpr NativeInt SET_ITEM_CBID		= 0x00000AC0;
constexpr NativeInt SET_TEXT_CBID		= 0x00000AC1;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsineChunks();
}

// destroy
void DestroyModule(void* pModule) 
{
	// cast is important to call the good destructor
	delete ((UsineChunks*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "usine chunks";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "usine chunks sdk module example";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr AnsiCharPtr CHUNK_NAME = "mychunk";

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
UsineChunks::UsineChunks()
{
	//
}

// destructor
UsineChunks::~UsineChunks()
{
	// we unsubscribe from all chunk notifications
	sdkChunkUnSubscribeAll();
}

void UsineChunks::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clChunkModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;    
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int UsineChunks::onGetNumberOfParams(int queryResult1, int queryResult2)
{
	int result = 4;
    return result;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void UsineChunks::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}

//-----------------------------------------------------------------------------
// initialisation
void UsineChunks::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	// we get the chunk pointer and create the chunk if it doesn't exist
	chunkPtr = sdkGetChunk(CHUNK_NAME);
	// we subscribe to chunk notifications to check if it has been deleted or cleared
	sdkChunkSubscribe(chunkPtr, "", CHUNK_SUB_DEL_CBID, TRUE);
	// we subscribe to chunk notifications for the item "item1"
	sdkChunkSubscribe(chunkPtr, "item1", CHUNK_SUB_ITEM_CBID, TRUE);
	// we subscribe to chunk notifications for the item "commatxt"
	sdkChunkSubscribe(chunkPtr, "commatxt", CHUNK_SUB_TEXT_CBID, TRUE);
	// fill output parameter with a default comma text
	chunkStringGet.setPChar(dummyCommaText);
	// we write into the chunk "commatxt" the default comma text
	sdkChunkOperation(chunkPtr, "commatxt", ftTextGeneral, chunkStringGet, coAddOrModifyItem);
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void UsineChunks::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{	
	if (ParamIndex == 0)
	{
		pParamInfo->ParamType = ptArray;
		pParamInfo->Caption = "set item1";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = SET_ITEM_CBID; // you can choose a value up to 0x00000FFE
		pParamInfo->SeparatorCaption = "";
		pParamInfo->setEventClass(chunkFloatSet);

	}
	else if (ParamIndex == 1)
	{
		pParamInfo->ParamType = ptText;
		pParamInfo->Caption = "commatxt idx 1";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = SET_TEXT_CBID; // you can choose a value up to 0x00000FFE
		pParamInfo->SeparatorCaption = "";
		pParamInfo->setEventClass(chunkStringSet);
	}
	else if (ParamIndex == 2)
	{
		pParamInfo->ParamType = ptArray;
		pParamInfo->Caption = "get item1";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->CallBackType = ctNone;
		pParamInfo->setEventClass(chunkFloatGet);
	}
	else if (ParamIndex == 3)
	{
		pParamInfo->ParamType = ptText;
		pParamInfo->Caption = "get commatxt";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->CallBackType = ctNone;
		pParamInfo->setEventClass(chunkStringGet);
		pParamInfo->DontSave = TRUE; // we don't want to save this param
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void UsineChunks::onCallBack(TUsineMessage *Message) 
{
    // filter only message specific to this user module
	if (Message->message != NOTIFY_MSG_USINE_CALLBACK)
		return;

	if (Message->lParam == MSG_CHANGE)
	{
		// check the callback id to identify the parameter : input "set item1"
		if (Message->wParam == SET_ITEM_CBID)
		{
			// we write into the chunk "item1" the values of the input parameter "set item1"
			sdkChunkOperation(chunkPtr, "item1", ftDataFloat, chunkFloatSet, coAddOrModifyItem);
		}
		else if (Message->wParam == SET_TEXT_CBID)
		{
			// we write into the chunk "commatxt" the values of the input parameter "commatxt idx 1"
			// but only then value at the index 1
			sdkChunkOperation(chunkPtr, "commatxt", ftTextGeneral, chunkStringSet, coAddOrModifyItem, 1);
		}
		else if (Message->wParam == CHUNK_SUB_ITEM_CBID)
		{
			// the chunk item "item1" has changed
			// we copy the chunk "item1" values to the output parameter "get item1"
			sdkChunkOperation(chunkPtr, "item1", ftDataFloat, chunkFloatGet, coGetItem);
		}
		else if (Message->wParam == CHUNK_SUB_TEXT_CBID)
		{
			// the chunk item "commatxt" has changed
			// we copy the chunk "commatxt" values to the output parameter "get commatxt"
			sdkChunkOperation(chunkPtr, "commatxt", ftTextGeneral, chunkStringGet, coGetItem);
		}
	}
	else if (Message->lParam == MSG_CHUNK_DESTROYED)
	{
		if (Message->wParam == CHUNK_SUB_DEL_CBID)
		{
			// do something if needed like unsubscribe ?
		}
	}
	else if (Message->lParam == MSG_CHUNK_ITEM_LIST_CHANGED)
	{
		if (Message->wParam == CHUNK_SUB_DEL_CBID)
		{
			// do something if needed like unsubscribe ?
		}
	}
}
