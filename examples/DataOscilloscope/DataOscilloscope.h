//-----------------------------------------------------------------------------
// @file    DataOscilloscope.h
// @author  BrainModular team
// @brief   Draws a scrolling curve of a data input in the module panel.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <vector>
#include "../../sdk/UserDefinitions.h"

constexpr int DEFAULT_MODULE_SIZE = 134;

class DataOscilloscope : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onCreateCommands() override;
	void onPaint() override;

private:
	UsineEventClass yFdrIn;
	UsineEventClass speedFdrIn;
	UsineEventClass boxColIn;

	TUsineColor colorBox   = 0;
	TPrecision  moveOffset = 0.005f;
	TPrecision  yValue     = 0.0f;

	// a vector is the simplest container for this example,
	// a circular buffer would be more cpu efficient
	std::vector<TPointF> points;

	void eraseBox();
	void advance();
};
