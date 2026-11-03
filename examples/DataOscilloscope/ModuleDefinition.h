//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the DataOscilloscope module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "data oscilloscope";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "draws a scrolling curve of a data input (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt Y_FDR_CBID     = 0x001300F0;
constexpr NativeInt SPEED_FDR_CBID = 0x001300F1;
constexpr NativeInt BOX_COL_CBID   = 0x001300F2;

inline constexpr TParamInfo yFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "y",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = -1.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.5f,
	.CallBackType = ctImmediate,
	.CallBackId   = Y_FDR_CBID
};

inline constexpr TParamInfo speedFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "speed",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scLog,
	.MinValue     = 0.0f,
	.MaxValue     = 0.5f,
	.DefaultValue = 0.005f,
	.CallBackType = ctNormal,
	.CallBackId   = SPEED_FDR_CBID
};

inline constexpr TParamInfo boxColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = BOX_COL_CBID
};
