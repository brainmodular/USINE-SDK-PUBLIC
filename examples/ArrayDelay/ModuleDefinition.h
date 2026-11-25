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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"delays each element of [array in] by its own number of audio blocs, up to 999 blocs, so that the "
	"array becomes a bank of independent delay lines.\n"
	"The delay of element i is read from element i of [delay in blocs], rounded to the nearest bloc and "
	"clamped between 0 and 999; when that array is shorter than [array in], its values repeat.\n"
	"A delay of 0 lets the element through without delay.\n"
	"Changing [delay in blocs] only changes where the next blocs are written, so the new delay applies "
	"from the following blocs on and the content already in the lines is not moved.\n"
	"The module handles up to 512 elements and allocates one delay line per element when it is created, "
	"whatever the number of elements actually used.\n"
	"[array out] carries the delayed elements (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.261125";

constexpr NativeInt DELAY_ARR_CBID = 0x001000FF;

constexpr TParamInfo arrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.CallBackType = ctNone,
	.Description  = "Array to delay, up to 512 elements: each element has its own delay line."
};

constexpr TParamInfo delayArrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "delay in blocs",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = MAX_DELAY - 1,
	.CallBackType = ctImmediate,
	.CallBackId   = DELAY_ARR_CBID,
	.Description  = "Delay of each element of [array in], in audio blocs.\n"
	                "Element i uses value i of this array, which repeats when it is shorter."
};

constexpr TParamInfo arrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.CallBackType = ctNone,
	.Description  = "[array in] with each element delayed by its own number of blocs."
};
