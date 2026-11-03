//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the ArrayDelay module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <cfloat>
#include "../../sdk/UserDefinitions.h"
#include "ArrayDelay.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "array delay";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "delays each element of an array (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

constexpr NativeInt DELAY_ARR_CBID = 0x001000FF;

inline constexpr TParamInfo arrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.CallBackType = ctNone
};

inline constexpr TParamInfo delayArrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "delay in blocs",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = MAX_DELAY - 1,
	.CallBackType = ctImmediate,
	.CallBackId   = DELAY_ARR_CBID
};

inline constexpr TParamInfo arrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.CallBackType = ctNone
};
