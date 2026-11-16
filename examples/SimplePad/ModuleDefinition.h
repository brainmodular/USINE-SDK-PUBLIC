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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"four XY pads of this control module are moved with the mouse and each move is recorded as "
	"automation: the pad index goes in X, its position in Y and Z, and recording stops on mouse up.\n"
	"[randomize position] sends the four pads to random coordinates, also available as a command of the "
	"contextual menu.\n"
	"[arr pos x] and [arr pos y] are editable arrays of four values from 0 to 1 that follow the pads and "
	"move them when set, the y axis being inverted in the outlet.\n"
	"Positions are saved in the patch and in presets through the chunk system; a chunk shorter than "
	"expected raises an error and keeps the pads on their diagonal.\n"
	"The edit layout panel sets the pad color and the ellipse size in pixels, from 1 to 30, default 20, "
	"and the quick color replaces the pad color (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

inline const int ERROR_CALLBACK               USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in callback");
inline const int ERROR_INVALID_CHUNK          USINE_MODULE_LOCAL = RegisterError(Level::Error, "invalid saved pad positions, they were not restored");
inline const int ERROR_RECORD_AUTOMATION      USINE_MODULE_LOCAL = RegisterError(Level::Error, "failed to record automation");
inline const int ERROR_STOP_RECORD_AUTOMATION USINE_MODULE_LOCAL = RegisterError(Level::Error, "failed to stop automation recording");

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
	.CallBackId   = RANDOMIZE_BTN_CBID,
	.Description  = "Moves the four pads to random positions."
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
	.CallBackId   = POS_X_ARR_CBID,
	.Description  = "X positions of the four pads, from 0 (left) to 1 (right).\n"
	                "Setting the array moves the pads."
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
	.CallBackId   = POS_Y_ARR_CBID,
	.Description  = "Y positions of the four pads, from 0 (bottom) to 1 (top).\n"
	                "Setting the array moves the pads."
};
