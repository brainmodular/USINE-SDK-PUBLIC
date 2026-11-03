//-----------------------------------------------------------------------------
// @file    DataSampler.cpp
// @author  BrainModular team
// @brief   Records a data input and plays it back in a loop, with a double buffer.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DataSampler.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataSampler();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DataSampler*>(pModule);
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
	initBuffers();
	resetBuffers();
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
		case REC_SW_CBID:
			isRec = (recSwIn.getData() != 0.0f);
			if (isRec)
			{
				resetBuffers();
				outFdrOut.setData(0.0f);
				sdkRaiseError(INFO_REC_STARTED);
			}
			else
			{
				aggCurrentBuffer();
				bufferLength = static_cast<int>(bufRecord.size());
				sdkRaiseError(INFO_REC_STOPPED);
			}
			break;
		case CLEAR_BTN_CBID:
			isRec = false;
			resetBuffers();
			outFdrOut.setData(0.0f);
			sdkRaiseError(INFO_REC_CLEARED);
			break;
		case BOX_COL_CBID:
			colorBox = boxColIn.getColor();
			break;
		case AGG_BUFFER_CBID:
			aggPreviousBuffer();
			break;
		default:
			break;
	}
}

void DataSampler::onProcess()
{
	if (isRec)
	{
		if (currentBuf->size() + 10 > BUFFER_SIZE)
		{
			swapBuffers();
			triggerBufferCallback();
		}
		currentBuf->push_back(dataFdrIn.getData());
		outFdrOut.setData(dataFdrIn.getData());
	}
	else if (!bufRecord.empty())
	{
		if (static_cast<size_t>(bufferIdx) >= bufRecord.size())
			bufferIdx = 0;
		outFdrOut.setData(bufRecord[bufferIdx]);
		bufferIdx++;
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

void DataSampler::onPaint()
{
	if (bufferLength == 0)
		return;

	const TPrecision delta = 1.0f / bufferLength;

	sdkDrawPathStart();
	TPointF previous {};
	for (int i = 0; i < bufferLength; i++)
	{
		const TPointF current { .x = i * delta, .y = 1.0f - bufRecord[(bufferIdx + i) % bufferLength] };
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

void DataSampler::initBuffers()
{
	bufA.reserve(BUFFER_SIZE);
	bufB.reserve(BUFFER_SIZE);
	currentBuf  = &bufA;
	previousBuf = &bufB;
}

void DataSampler::resetBuffers()
{
	bufA.clear();
	bufB.clear();
	bufRecord.clear();
	bufferIdx    = 0;
	bufferLength = 0;
}

void DataSampler::swapBuffers()
{
	std::swap(currentBuf, previousBuf);
}

void DataSampler::aggPreviousBuffer()
{
	bufRecord.insert(bufRecord.end(), previousBuf->begin(), previousBuf->end());
	previousBuf->clear();
}

void DataSampler::aggCurrentBuffer()
{
	bufRecord.insert(bufRecord.end(), currentBuf->begin(), currentBuf->end());
	currentBuf->clear();
}

void DataSampler::triggerBufferCallback() const
{
	sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, AGG_BUFFER_CBID, MSG_CHANGE, TNotifyUsineMode::numRealTime, 0);
}
