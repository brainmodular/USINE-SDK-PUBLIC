//-----------------------------------------------------------------------------
// @file    GlobalArray.h
// @author  BrainModular team
// @brief   Writes random values in Usine global arrays and reads them back.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class GlobalArray : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onProcess() override;

private:
	UsineEventClass setArrIn;
	UsineEventClass getArrOut;
};
