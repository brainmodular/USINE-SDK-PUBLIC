//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the DataMultiply module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "data multiply";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "multiplies two data inputs (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// shared by both inputs: any change recomputes the result
constexpr NativeInt INPUT_FDR_CBID = 0x001200FF;

inline constexpr TParamInfo aFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "a",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INPUT_FDR_CBID
};

inline constexpr TParamInfo bFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "b",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INPUT_FDR_CBID
};

inline constexpr TParamInfo resultFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "result",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE
};
