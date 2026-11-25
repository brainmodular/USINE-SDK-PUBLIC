//-----------------------------------------------------------------------------
// @file    DataSampler.cpp
// @author  BrainModular team
// @brief   Records a data input and plays it back in a loop, in chunks allocated outside of the audio thread.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DataSampler.h"
#include "ModuleDefinition.h"

#include <algorithm>

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataSampler();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DataSampler*>(pModule);
}

// the main thread destroys the module, once the audio thread no longer processes it
DataSampler::~DataSampler()
{
	for (auto& chunk : chunks)
		delete chunk.exchange(nullptr);
}

void DataSampler::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth       = DEFAULT_MODULE_SIZE;
	pModuleInfo->DefaultHeight      = DEFAULT_MODULE_SIZE;
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void DataSampler::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorBox = sdkGetUsineColor(clCursor);
	// the first two chunks: a recording can start at the next block
	allocateChunks();
}

void DataSampler::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(dataFdrInParamInfo, dataFdrIn);
	sdkCreateParam(recSwInParamInfo, recSwIn);
	sdkCreateParam(clearBtnInParamInfo, clearBtnIn);
	sdkCreateParam(boxColInParamInfo, boxColIn);
	sdkCreateParam(outFdrOutParamInfo, outFdrOut);
}

void DataSampler::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		// ctImmediate: possibly the audio thread, so only an atomic store
		case REC_SW_CBID:
			request = (recSwIn.getData() != 0.0f) ? START_REC : STOP_REC;
			break;
		case CLEAR_BTN_CBID:
			request = CLEAR_REC;
			break;
		case BOX_COL_CBID:
			colorBox = boxColIn.getColor();
			break;
		// sent by onProcess with numMainThread: the main thread allocates and reports
		case NEXT_CHUNK_CBID:
			allocateChunks();
			break;
		case REC_STARTED_CBID:
			sdkReport(INFO_REC_STARTED);
			break;
		case REC_STOPPED_CBID:
			sdkRepaintPanel();
			sdkReport(INFO_REC_STOPPED);
			break;
		case REC_CLEARED_CBID:
			sdkRepaintPanel();
			sdkReport(INFO_REC_CLEARED);
			break;
		default:
			break;
	}
}

// audio thread: atomics and the preallocated chunks only, no allocation, no lock
void DataSampler::onProcess()
{
	switch (request.exchange(NO_REQUEST))
	{
		case START_REC:
			// erases the previous recording: its chunks are written again from the first one
			recordLength = 0;
			playPosition = 0;
			recording = true;
			notifyMainThread(REC_STARTED_CBID);
			break;
		case STOP_REC:
			if (recording)
			{
				recording = false;
				playPosition = 0;
				notifyMainThread(REC_STOPPED_CBID);
			}
			break;
		case CLEAR_REC:
			recording = false;
			recordLength = 0;
			playPosition = 0;
			outFdrOut.setData(0.0f);
			notifyMainThread(REC_CLEARED_CBID);
			break;
		default:
			break;
	}

	const size_t length = recordLength.load(std::memory_order_relaxed);
	if (recording)
	{
		const TPrecision value = dataFdrIn.getData();
		Chunk* chunk = (length / CHUNK_SIZE < MAX_CHUNKS) ? chunks[length / CHUNK_SIZE].load(std::memory_order_acquire) : nullptr;
		// no chunk: the maximum length is reached, or the main thread did not allocate it yet (it had 2000 blocks)
		if (chunk != nullptr)
		{
			(*chunk)[length % CHUNK_SIZE].store(value, std::memory_order_relaxed);
			// release: onPaint reads the values below the length it loads
			recordLength.store(length + 1, std::memory_order_release);
			// a chunk begins: the main thread allocates the next one meanwhile
			if (length % CHUNK_SIZE == 0)
				notifyMainThread(NEXT_CHUNK_CBID);
		}
		outFdrOut.setData(value);
	}
	else if (length > 0)
	{
		size_t position = playPosition.load(std::memory_order_relaxed);
		if (position >= length)
			position = 0;
		outFdrOut.setData(recordedValue(position));
		playPosition.store(position + 1, std::memory_order_relaxed);
	}

	if (sdkRefreshSpeedClock())
		sdkRepaintPanel();
}

void DataSampler::onCreateSettings()
{
	// adds our own section in the 'edit layout' panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorBox, "box color");
}

void DataSampler::onSettingsHasChanged()
{
	sdkRepaintPanel();
}

// main thread: the curve of the recording, scrolling with the playback position
void DataSampler::onPaint()
{
	const size_t length = recordLength.load(std::memory_order_acquire);
	if (recording || length == 0)
		return;

	// at most 2000 points, whatever the length of the recording
	const size_t points = std::min<size_t>(length, 2000);
	const size_t start = playPosition.load(std::memory_order_relaxed);
	const TPrecision delta = 1.0f / static_cast<TPrecision>(points);

	sdkDrawPathStart();
	TPointF previous {};
	for (size_t i = 0; i < points; i++)
	{
		const size_t index = (start + i * length / points) % length;
		const TPointF current { .x = static_cast<TPrecision>(i) * delta, .y = 1.0f - recordedValue(index) };
		if (i > 0)
		{
			sdkDrawPathMoveTo(previous);
			sdkDrawPathLineTo(current);
		}
		previous = current;
	}
	sdkDrawPathClose();
	sdkDrawPathDraw(colorBox, 0.5f);
}

// main thread: keeps one chunk allocated ahead of the one being recorded
void DataSampler::allocateChunks()
{
	const size_t wanted = std::min(MAX_CHUNKS, recordLength.load(std::memory_order_acquire) / CHUNK_SIZE + 2);
	while (allocatedChunks < wanted)
	{
		// value-initialized: every value 0
		chunks[allocatedChunks].store(new Chunk(), std::memory_order_release);
		allocatedChunks++;
	}
}

// index below a length loaded with acquire (or below the length the audio thread wrote): its chunk exists
TPrecision DataSampler::recordedValue(const size_t index) const
{
	const Chunk* chunk = chunks[index / CHUNK_SIZE].load(std::memory_order_acquire);
	return chunk != nullptr ? (*chunk)[index % CHUNK_SIZE].load(std::memory_order_relaxed) : 0.0f;
}

// numMainThread: the callback runs in the main thread, with the other main thread callbacks of the module
void DataSampler::notifyMainThread(const NativeInt callbackId) const
{
	sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, callbackId, MSG_CHANGE, TNotifyUsineMode::numMainThread, 0);
}
