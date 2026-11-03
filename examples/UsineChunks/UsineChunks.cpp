//-----------------------------------------------------------------------------
// @file    UsineChunks.cpp
// @author  BrainModular team
// @brief   Shares data with other modules through a Usine chunk and its notifications.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "UsineChunks.h"
#include "ModuleDefinition.h"
#include <random>

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new UsineChunks();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<UsineChunks*>(pModule);
}

UsineChunks::~UsineChunks()
{
	sdkChunkUnSubscribeAll();
}

void UsineChunks::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clChunkModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void UsineChunks::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	// gets the chunk, it is created if it doesn't exist yet
	chunkPtr = sdkGetChunk(CHUNK_NAME);

	// an empty item name subscribes to the chunk itself: destroyed or item list changed
	sdkChunkSubscribe(chunkPtr, "", CHUNK_SUB_CBID, TRUE);
	sdkChunkSubscribe(chunkPtr, ITEM_FLOAT_NAME, CHUNK_SUB_ITEM_CBID, TRUE);
	sdkChunkSubscribe(chunkPtr, ITEM_TEXT_NAME, CHUNK_SUB_TEXT_CBID, TRUE);

	getCommaTxtOut.setPChar(DEFAULT_COMMA_TEXT);
	sdkChunkOperation(chunkPtr, ITEM_TEXT_NAME, ftTextGeneral, getCommaTxtOut, coAddOrModifyItem);
	fillBulkTestData();
}

void UsineChunks::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(setItemArrInParamInfo, setItemArrIn);
	sdkCreateParam(setCommaTxtInParamInfo, setCommaTxtIn);
	sdkCreateParam(getItemArrOutParamInfo, getItemArrOut);
	sdkCreateParam(getCommaTxtOutParamInfo, getCommaTxtOut);
	sdkCreateParam(getChunkDataBtnInParamInfo, getChunkDataBtnIn);
	sdkCreateParam(setChunkDataBtnInParamInfo, setChunkDataBtnIn);
}

void UsineChunks::onCallBack(TUsineMessage* Message)
{
	if (Message->message != NOTIFY_MSG_USINE_CALLBACK)
		return;

	// the chunk notifications come with their own lParam
	if (Message->lParam == MSG_CHUNK_DESTROYED && Message->wParam == CHUNK_SUB_CBID)
	{
		sdkRaiseError(WARNING_CHUNK_DESTROYED, CHUNK_NAME);
		return;
	}
	if (Message->lParam == MSG_CHUNK_ITEM_LIST_CHANGED && Message->wParam == CHUNK_SUB_CBID)
	{
		sdkRaiseError(INFO_CHUNK_ITEMS_CHANGED, CHUNK_NAME);
		return;
	}
	if (Message->lParam != MSG_CHANGE)
		return;

	switch (Message->wParam)
	{
		// inputs: write in the chunk
		case SET_ITEM_ARR_CBID:
			sdkChunkOperation(chunkPtr, ITEM_FLOAT_NAME, ftDataFloat, setItemArrIn, coAddOrModifyItem);
			break;
		case SET_COMMA_TXT_CBID:
			// only writes the value at index 1 of the comma text
			sdkChunkOperation(chunkPtr, ITEM_TEXT_NAME, ftTextGeneral, setCommaTxtIn, coAddOrModifyItem, 1);
			break;

		// subscriptions: an item changed, possibly written by another module, read it back
		case CHUNK_SUB_ITEM_CBID:
			sdkChunkOperation(chunkPtr, ITEM_FLOAT_NAME, ftDataFloat, getItemArrOut, coGetItem);
			break;
		case CHUNK_SUB_TEXT_CBID:
			sdkChunkOperation(chunkPtr, ITEM_TEXT_NAME, ftTextGeneral, getCommaTxtOut, coGetItem);
			break;
		case GET_CHUNK_DATA_BTN_CBID:
		{
			UINT32 bulk_size = sdkGetChunkItemSize(chunkPtr, DATA_BULK_ITEM_NAME);
			data_buffer.clear();
			data_buffer.resize(bulk_size);
			sdkGetChunkToFloatPointer(chunkPtr, DATA_BULK_ITEM_NAME, data_buffer.data());
			sdkSetChunkFromFloatPointer(chunkPtr, DATA_BULK_ITEM_NAME_COPY, data_bulk.data(), static_cast<UINT32>(bulk_size));
			break;
		}
		case SET_CHUNK_DATA_BTN_CBID:
			sdkSetChunkFromFloatPointer(chunkPtr, DATA_BULK_ITEM_NAME, data_bulk.data(), static_cast<UINT32>(kBulkSize));
			break;
		default:
			break;
	}
}



void UsineChunks::fillBulkTestData()
{
    std::mt19937 rng(12345);                                  // fixed seed = reproducible
    std::uniform_real_distribution<float> dist(-1.f, 1.f);
    for (auto& v : data_bulk)
        v = dist(rng);
}
