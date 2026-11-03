//-----------------------------------------------------------------------------
// @file    ErrorHandling.h
// @author  BrainModular team
// @brief   Registers errors, warnings and infos, raises them with sdkRaiseError
//          and filters them with a verbosity level.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class ErrorHandling : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass errorBtnIn;
	UsineEventClass warningBtnIn;
	UsineEventClass infoBtnIn;
	UsineEventClass verbosityTxtIn;

	void changeVerbosityLevel();
};
