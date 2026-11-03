//-----------------------------------------------------------------------------
// @file    ArrayDelay.h
// @author  BrainModular team
// @brief   Delays each element of an array by its own number of blocs.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

constexpr int MAXBLOC   = 512;
constexpr int MAX_DELAY = 1000;

class ArrayDelay : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

private:
	UsineEventClass arrIn;
	UsineEventClass delayArrIn;
	UsineEventClass arrOut;

	// one array of MAXBLOC values per bloc of delay
	std::array<std::array<float, MAXBLOC>, MAX_DELAY> delayLine;
	std::array<int, MAXBLOC>                          delayBuffer;
	int                                               readPos = 0;
};
