//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the BackgroundJob module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "background job";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "runs a long computation in a background thread (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt ONE_FDR_CBID   = 0x001800F0;
constexpr NativeInt TWO_FDR_CBID   = 0x001800F1;
constexpr NativeInt START_BTN_CBID = 0x001800F2;

inline constexpr TParamInfo oneFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "data1",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = ONE_FDR_CBID
};

inline constexpr TParamInfo twoFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "data2",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = TWO_FDR_CBID
};

inline constexpr TParamInfo startBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "start job",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = START_BTN_CBID
};

inline constexpr TParamInfo outFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.DefaultValue = 0.0f,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE
};
