//-----------------------------------------------------------------------------
// @file    UsineObjects.h
// @author  BrainModular team
// @brief   Reads and writes Usine objects (faders, arrays, texts, triggers) by their path.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class UsineObjects : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onProcess() override;

private:
	UsineEventClass setArrIn;
	UsineEventClass getArrOut;
};
