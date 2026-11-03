//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the DrawTrajectoryBox module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <cfloat>
#include "../../sdk/UserDefinitions.h"
#include "DrawTrajectoryBox.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "draw trajectory box";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "draws a trajectory with the mouse and saves it in the patch (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.21";

inline const int WARNING_MAX_POINTS  = RegisterError(Level::Warn, "maximum number of points reached, the trajectory is truncated");
inline const int ERROR_INVALID_CHUNK = RegisterError(Level::Error, "invalid saved trajectory, it was not restored");

// RESET_BTN_CBID and RANDOMIZE_BTN_CBID are also used by the contextual menu commands, see onCreateCommands
constexpr NativeInt SEL_POINT_INDEX_FDR_CBID = 0x001600F0;
constexpr NativeInt RESET_BTN_CBID           = 0x001600F1;
constexpr NativeInt RANDOMIZE_BTN_CBID       = 0x001600F2;
constexpr NativeInt COORDS_ARR_IN_CBID       = 0x001600F3;
constexpr NativeInt COORDS_ARR_OUT_CBID      = 0x001600F4;

inline constexpr TParamInfo nbPointsFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "nbpoints",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0,
	.MaxValue     = MAXPOINTS,
	.DefaultValue = 0,
	.Symbol       = " pt",
	.Format       = "%.0f",
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

// the integer part selects a point, the fractional part moves the cursor toward the next one
inline constexpr TParamInfo selPointIndexFdrInParamInfo {
	.ParamType        = ptDataField,
	.Caption          = "cursor pos",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 0,
	.Symbol           = "",
	.Format           = "%.2f",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = SEL_POINT_INDEX_FDR_CBID
};

inline constexpr TParamInfo selPointXFdrOutParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "pos x",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.DefaultValue = 0,
	.Symbol       = "",
	.Format       = "%.3f",
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.IsSeparator  = TRUE
};

inline constexpr TParamInfo selPointYFdrOutParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "pos y",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.DefaultValue = 0,
	.Symbol       = "",
	.Format       = "%.3f",
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo resetBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "reset",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.IsSeparator  = TRUE,
	.CallBackId   = RESET_BTN_CBID
};

inline constexpr TParamInfo randomizeBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "randomize",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = RANDOMIZE_BTN_CBID
};

inline constexpr TParamInfo coordsArrInParamInfo {
	.ParamType    = ptArray,
	.Caption      = "coords in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.DontSave     = TRUE,
	.IsSeparator  = TRUE,
	.CallBackId   = COORDS_ARR_IN_CBID
};

// output that the user can also edit in the lines tab, hence the callback
inline constexpr TParamInfo coordsArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "coords out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -FLT_MAX,
	.MaxValue     = FLT_MAX,
	.CallBackType = ctImmediate,
	.DontSave     = TRUE,
	.CallBackId   = COORDS_ARR_OUT_CBID
};
