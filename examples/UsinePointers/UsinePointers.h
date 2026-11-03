//-----------------------------------------------------------------------------
// @file    UsinePointers.h
// @author  BrainModular team
// @brief   Copies the data of a pointer input to a pointer output, shows typed pointers.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class UsinePointers : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass ptrIn;
	UsineEventClass ptrOut;

	void copyPointer();
};
