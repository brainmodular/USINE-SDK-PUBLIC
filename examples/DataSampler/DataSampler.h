//-----------------------------------------------------------------------------
// @file    DataSampler.h
// @author  BrainModular team
// @brief   Records a data input and plays it back in a loop, with a double buffer.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <vector>
#include "../../sdk/UserDefinitions.h"

constexpr int DEFAULT_MODULE_SIZE = 134;

class DataSampler : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onPaint() override;

private:
	UsineEventClass dataFdrIn;
	UsineEventClass recSwIn;
	UsineEventClass clearBtnIn;
	UsineEventClass boxColIn;
	UsineEventClass outFdrOut;

	TUsineColor colorBox = 0;
	bool        isRec    = false;

	int bufferIdx    = 0;
	int bufferLength = 0;

	// onProcess fills currentBuf, when it is full the buffers are swapped
	// and previousBuf is appended to bufRecord outside of the audio process
	static constexpr size_t BUFFER_SIZE = 2000;
	std::vector<TPrecision>  bufA;
	std::vector<TPrecision>  bufB;
	std::vector<TPrecision>  bufRecord;
	std::vector<TPrecision>* currentBuf  = &bufA;
	std::vector<TPrecision>* previousBuf = &bufB;

	void initBuffers();
	void resetBuffers();
	void swapBuffers();
	void aggPreviousBuffer();
	void aggCurrentBuffer();
	void triggerBufferCallback() const;
};
