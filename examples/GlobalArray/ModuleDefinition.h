//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages and parameters of the GlobalArray module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "global array";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"writes random values in Usine global arrays and reads one of them back.\n"
	"At each refresh the module looks for the global arrays GA-FLOAT, GA-COLOR, GA-STRING and GA-EVENT, "
	"fills 16 elements of each existing one with random floats, random colors, random 5-character strings "
	"or random event values, then traces the first value of the float, color and string arrays.\n"
	"A missing global array is silently skipped, so the arrays must be created in Usine first.\n"
	"GA-EVENT is written from [array set], whose content is overwritten with 16 random values at each "
	"refresh, and read back into [array get].\n"
	"Every second the module also traces the list of all global array names.\n"
	"Nothing is stored in presets and there is no audio input or output (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// names of the global arrays, create them in Usine to see the module work
constexpr AnsiCharPtr GA_FLOAT  = "GA-FLOAT";
constexpr AnsiCharPtr GA_COLOR  = "GA-COLOR";
constexpr AnsiCharPtr GA_STRING = "GA-STRING";
constexpr AnsiCharPtr GA_EVENT  = "GA-EVENT";
constexpr int         GA_SIZE   = 16;

inline const int INFO_GA_NAMES  USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "global arrays: ");
inline const int INFO_GA_FLOAT  USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "ga-float[0] = ");
inline const int INFO_GA_COLOR  USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "ga-color[0] = ");
inline const int INFO_GA_STRING USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "ga-string[0] = ");

inline constexpr TParamInfo setArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "array set",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "",
	.Description      = "Array written in the global array GA-EVENT.\n"
	                    "The module fills it with 16 random values at each refresh, overwriting any input."
};

inline constexpr TParamInfo getArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array get",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Content of the global array GA-EVENT, read back after each write.\n"
	                "GA-EVENT must exist in Usine."
};
