//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants and parameters of the MidiTranspose module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "midi transpose";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "transposes midi notes (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline constexpr TParamInfo midiInParamInfo {
	.ParamType    = ptMidi,
	.Caption      = "in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo midiOutParamInfo {
	.ParamType    = ptMidi,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone
};

// no callback: the value is read in onProcess
inline constexpr TParamInfo pitchFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "pitch",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = -127.0f,
	.MaxValue         = 127.0f,
	.DefaultValue     = 0.0f,
	.Symbol           = "halftone",
	.Format           = "%.0f",
	.CallBackType     = ctNone,
	.IsStoredInPreset = TRUE
};
