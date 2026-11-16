//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the ArrayDistance module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "Array Distance";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"computes the dynamic time warping distance between [array 1] and [array 2], which can have different "
	"sizes: the closer the shapes of the two arrays, the lower [distance].\n"
	"Nothing is computed in the audio thread, press [calculate] to update [distance] from the current "
	"arrays and settings.\n"
	"[min] and [max] give the range of the array values and scale [distance] to 0..1, 0 for identical "
	"arrays; only the gap between them counts, so swapped bounds give the same result, and equal bounds "
	"leave the distance unscaled.\n"
	"[distance] is -1 when an array is empty.\n"
	"The cost matrix is computed row by row, its memory growing with the size of [array 2] only; the time "
	"grows with the product of the two sizes and the calculation blocks the interface until it ends";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "0.261112";

constexpr NativeInt CALCULATE_BTN_CBID = 0x003200F0;

// range of values given to Usine for the array inputs, the calculation doesn't use it
constexpr float ARRAY_MIN_VALUE = -16384.0f;
constexpr float ARRAY_MAX_VALUE = 16384.0f;

inline constexpr TParamInfo array1ArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "array 1",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = ARRAY_MIN_VALUE,
	.MaxValue         = ARRAY_MAX_VALUE,
	.ReadOnly         = FALSE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "data in",
	.Description      = "First array of values, compared with [array 2] when [calculate] is pressed."
};

inline constexpr TParamInfo array2ArrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array 2",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = ARRAY_MIN_VALUE,
	.MaxValue     = ARRAY_MAX_VALUE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Second array of values, compared with [array 1] when [calculate] is pressed.\n"
	                "The two arrays can have different sizes."
};

inline constexpr TParamInfo distanceFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "distance",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Dynamic time warping distance between [array 1] and [array 2], set when [calculate] is pressed.\n"
	                "Range: [0..1] when every value is between [min] and [max], 0=identical arrays.\n"
	                "-1 when one of the arrays is empty."
};

inline constexpr TParamInfo minFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "min",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Lowest possible value of the arrays, used with [max] to scale [distance] to [0..1].\n"
	                "Only the gap between [min] and [max] counts: swapped, they give the same [distance].\n"
	                "When [min] equals [max], [distance] is not scaled."
};

inline constexpr TParamInfo maxFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "max",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 1.0f,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Highest possible value of the arrays, used with [min] to scale [distance] to [0..1].\n"
	                "Only the gap between [min] and [max] counts: swapped, they give the same [distance].\n"
	                "When [min] equals [max], [distance] is not scaled."
};

inline constexpr TParamInfo calculateBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "calculate",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = CALCULATE_BTN_CBID,
	.Description  = "Calculates [distance] from the current [array 1], [array 2], [min] and [max]: changing them doesn't update [distance].\n"
	                "The calculation time grows with the product of the two array sizes, seconds for two arrays of 16384 values, and the interface of Usine waits for it."
};
