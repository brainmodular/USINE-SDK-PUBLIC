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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "writes and reads Usine global arrays (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// names of the global arrays, create them in Usine to see the module work
constexpr AnsiCharPtr GA_FLOAT  = "GA-FLOAT";
constexpr AnsiCharPtr GA_COLOR  = "GA-COLOR";
constexpr AnsiCharPtr GA_STRING = "GA-STRING";
constexpr AnsiCharPtr GA_EVENT  = "GA-EVENT";
constexpr int         GA_SIZE   = 16;

inline const int INFO_GA_NAMES  = RegisterError(Level::Info, "global arrays: ");
inline const int INFO_GA_FLOAT  = RegisterError(Level::Info, "ga-float[0] = ");
inline const int INFO_GA_COLOR  = RegisterError(Level::Info, "ga-color[0] = ");
inline const int INFO_GA_STRING = RegisterError(Level::Info, "ga-string[0] = ");

inline constexpr TParamInfo setArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "array set",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = ""
};

inline constexpr TParamInfo getArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array get",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone
};
