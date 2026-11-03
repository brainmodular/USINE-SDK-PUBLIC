//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  Olivier Sens (senso), BrainModular team
// @brief   Module constants and parameters of the VideoDimmer module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "video dimmer";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "dims a video and draws a grid on it (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// no callbacks: the values are read in onProcessVideo
inline constexpr TParamInfo dimmerFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "dimmer",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.DefaultValue = 1,
	.CallBackType = ctNone
};

inline constexpr TParamInfo pixelColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "pixel color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo spaceFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "inter space",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = 20,
	.DefaultValue = 1,
	.Format       = "%.0f", // DEFAULT_FORMAT_INTEGER is not constexpr
	.CallBackType = ctNone
};
