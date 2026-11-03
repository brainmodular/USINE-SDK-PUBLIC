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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "reads and writes Usine objects by their path (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// paths of the objects used by the module, they must exist in the workspace
constexpr AnsiCharPtr PATH_FADER_DATA   = "usine.workspace.rack01.patch01.faderdata";
constexpr AnsiCharPtr PATH_ARRAY_IN     = "usine.workspace.rack01.patch01.arraydata.array-in";
constexpr AnsiCharPtr PATH_ARRAY_OUT    = "usine.workspace.rack01.patch01.arraydata.array-out";
constexpr AnsiCharPtr PATH_COLOR_ARRAY  = "usine.workspace.rack01.patch01.arraycolor.color-array";
constexpr AnsiCharPtr PATH_COLOR_ROWS   = "usine.workspace.rack01.patch01.arraycolor.num-of-rows";
constexpr AnsiCharPtr PATH_COMMA_TEXT   = "usine.workspace.rack01.patch01.string.comma-text";
constexpr AnsiCharPtr PATH_TRIGGER      = "usine.workspace.rack01.patch01.trigger";
constexpr int         OBJECT_ARRAY_SIZE = 16;

inline const int INFO_VIDEO_OBJECTS      = RegisterError(Level::Info, "video objects: ");
inline const int INFO_OBJECTS_HASH       = RegisterError(Level::Info, "objects hash: ");
inline const int INFO_FADER_DATA         = RegisterError(Level::Info, "faderdata = ");
inline const int INFO_ARRAY_OUT          = RegisterError(Level::Info, "array-out");
inline const int INFO_COLOR_ARRAY        = RegisterError(Level::Info, "color-array");
inline const int INFO_COLOR_ARRAY_LENGTH = RegisterError(Level::Info, "color-array length = ");
inline const int INFO_COMMA_TEXT_EXISTS  = RegisterError(Level::Info, "string.comma-text exists");
inline const int INFO_COMMA_TEXT         = RegisterError(Level::Info, "string.comma-text = ");

inline constexpr TParamInfo setArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "array set",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = ""
};

inline constexpr TParamInfo getArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "array get",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNormal
};
