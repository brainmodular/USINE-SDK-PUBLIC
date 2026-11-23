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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"draws a trajectory in the module panel with the left mouse button and outputs the position of a "
	"cursor along it.\n"
	"Points are appended while the button is down, up to 5000, where a warning is raised;\n"
	"[nbpoints] counts them.\n"
	"[cursor pos] selects a point: its integer part is the point index, its fractional part interpolates "
	"toward the next point, -1 hides the cursor, and [pos x] and [pos y] output the position from 0 to 1 "
	"while the cursor is in range.\n"
	"[coords in] replaces the trajectory from an array of three values per point, [coords out] exports "
	"the same layout and editing it moves the points.\n"
	"[reset] clears everything, [randomize] builds a random Lissajous curve.\n"
	"Settings handle smooth and closed lines, Y inversion and colors.\n"
	"The trajectory is saved in the state chunk of workspaces, patches and presets, and a corrupted chunk "
	"raises an error (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.261112";

inline const int WARNING_MAX_POINTS  USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "maximum number of points reached, the trajectory is truncated");
inline const int ERROR_INVALID_CHUNK USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "invalid saved trajectory, it was not restored");

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
	.CallBackType = ctNone,
	.Description  = "Number of points of the trajectory, 5000 at most."
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
	.CallBackId       = SEL_POINT_INDEX_FDR_CBID,
	.Description      = "Position of the cursor: the integer part is the index of a point, the fractional part moves toward the next one.\n"
	                    "-1 hides the cursor."
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
	.IsSeparator  = TRUE,
	.Description  = "X coordinate of the cursor, from 0 (left) to 1 (right)."
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
	.CallBackType = ctNone,
	.Description  = "Y coordinate of the cursor, from 0 (top) to 1 (bottom)."
};

inline constexpr TParamInfo resetBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "reset",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.IsSeparator  = TRUE,
	.CallBackId   = RESET_BTN_CBID,
	.Description  = "Clears the trajectory and hides the cursor."
};

inline constexpr TParamInfo randomizeBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "randomize",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = RANDOMIZE_BTN_CBID,
	.Description  = "Replaces the trajectory with a random Lissajous curve."
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
	.CallBackId   = COORDS_ARR_IN_CBID,
	.Description  = "Replaces the trajectory with the points of the array, 3 values per point: x and y from 0 to 1, then a third value copied to [coords out]."
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
	.CallBackId   = COORDS_ARR_OUT_CBID,
	.Description  = "Points of the trajectory, 3 values per point: x, y and a third value (0.5 for drawn points).\n"
	                "Editing it moves the points."
};
