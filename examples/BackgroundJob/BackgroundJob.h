//-----------------------------------------------------------------------------
// @file    BackgroundJob.h
// @author  BrainModular team
// @brief   Runs a long computation in a background thread with the job API.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class BackgroundJob : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

	void onJobBegin() override;
	void onJobProcess() override;
	void onJobEnd() override;

private:
	UsineEventClass oneFdrIn;
	UsineEventClass twoFdrIn;
	UsineEventClass startBtnIn;
	UsineEventClass outFdrOut;

	TPrecision valueOne = 0.0f;
	TPrecision valueTwo = 0.0f;
	TPrecision retValue = 0.0f;
};
