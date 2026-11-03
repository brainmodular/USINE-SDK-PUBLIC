//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the SimplePad module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "simple pad";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "pads moved with the mouse, saved and recorded (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline const int ERROR_CALLBACK               = RegisterError(Level::Error, "error in callback");
inline const int ERROR_INVALID_CHUNK          = RegisterError(Level::Error, "invalid saved pad positions, they were not restored");
inline const int ERROR_RECORD_AUTOMATION      = RegisterError(Level::Error, "failed to record automation");
inline const int ERROR_STOP_RECORD_AUTOMATION = RegisterError(Level::Error, "failed to stop automation recording");

// RANDOMIZE_BTN_CBID is also used by the contextual menu command, see onCreateCommands
constexpr NativeInt RANDOMIZE_BTN_CBID = 0x002400F0;
constexpr NativeInt POS_X_ARR_CBID     = 0x002400F1;
constexpr NativeInt POS_Y_ARR_CBID     = 0x002400F2;

inline constexpr TParamInfo randomizeBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "randomize position",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = RANDOMIZE_BTN_CBID
};

// outputs that the user can also edit, hence the callbacks
inline constexpr TParamInfo posXArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "arr pos x",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.CallBackType = ctNormal,
	.DontSave     = TRUE,
	.CallBackId   = POS_X_ARR_CBID
};

inline constexpr TParamInfo posYArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "arr pos y",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.CallBackType = ctNormal,
	.DontSave     = TRUE,
	.CallBackId   = POS_Y_ARR_CBID
};
