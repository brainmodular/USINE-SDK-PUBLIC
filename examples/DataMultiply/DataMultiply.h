//-----------------------------------------------------------------------------
// @file    DataMultiply.h
// @author  BrainModular team
// @brief   Multiplies two data inputs, shows how to update outputs when inputs change.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class DataMultiply : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass aFdrIn;
	UsineEventClass bFdrIn;
	UsineEventClass resultFdrOut;
};
