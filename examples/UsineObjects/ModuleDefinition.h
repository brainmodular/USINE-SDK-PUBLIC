//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages and parameters of the UsineObjects module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "usine objects";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"reads and writes the objects of the patch by their path on each tick of the slow clock: one value in "
	"a faderdata object, the sixteen elements of a data array through the array-in path, the array-out "
	"read back, a color array with its number of rows, a comma-text string and a trigger.\n"
	"Most writes are followed by a trace of the value read back from the object, and the module also "
	"traces the number of video objects and the hash of the objects list.\n"
	"The paths are fixed in the code and point to usine.workspace.rack01.patch01, so these objects must "
	"exist in the workspace; colors are computed in double to stay in the range of TUsineColor and the "
	"text is fifty random alphanumeric characters.\n"
	"[array set] and [array get] are declared but never used, the objects being written directly (sdk "
	"example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// paths of the objects used by the module, they must exist in the workspace
constexpr AnsiCharPtr PATH_FADER_DATA   = "usine.workspace.rack01.patch01.faderdata";
constexpr AnsiCharPtr PATH_ARRAY_IN     = "usine.workspace.rack01.patch01.arraydata.array-in";
constexpr AnsiCharPtr PATH_ARRAY_OUT    = "usine.workspace.rack01.patch01.arraydata.array-out";
constexpr AnsiCharPtr PATH_COLOR_ARRAY  = "usine.workspace.rack01.patch01.arraycolor.color-array";
constexpr AnsiCharPtr PATH_COLOR_ROWS   = "usine.workspace.rack01.patch01.arraycolor.num-of-rows";
constexpr AnsiCharPtr PATH_COMMA_TEXT   = "usine.workspace.rack01.patch01.string.comma-text";
constexpr AnsiCharPtr PATH_TRIGGER      = "usine.workspace.rack01.patch01.trigger";
constexpr int         OBJECT_ARRAY_SIZE = 16;

inline const int INFO_VIDEO_OBJECTS      USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "video objects: ");
inline const int INFO_OBJECTS_HASH       USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "objects hash: ");
inline const int INFO_FADER_DATA         USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "faderdata = ");
inline const int INFO_ARRAY_OUT          USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "array-out");
inline const int INFO_COLOR_ARRAY        USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "color-array");
inline const int INFO_COLOR_ARRAY_LENGTH USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "color-array length = ");
inline const int INFO_COMMA_TEXT_EXISTS  USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "string.comma-text exists");
inline const int INFO_COMMA_TEXT         USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "string.comma-text = ");

inline constexpr TParamInfo setArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "array set",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "",
	.Description      = "Not used: the module writes the objects of the patch directly by their path."
};

inline constexpr TParamInfo getArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array get",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNormal,
	.Description  = "Not used: the values read back from the objects are written in the Usine trace."
};
