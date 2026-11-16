//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the DrawBox module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "draw box";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"draws a box on its panel with the mouse and shows painting, mouse, command and randomization "
	"handling; it has no audio process and the box coordinates stay internal.\n"
	"Pressing the left button sets the anchor, moving with the button down stretches the box between the "
	"anchor and the pointer, both clamped to the panel, and releasing ends the gesture.\n"
	"The wheel scales the whole box by 1.1 up or 0.9 down around the mouse position, every edge staying "
	"inside the panel.\n"
	"[reset] erases the box, [randomize] draws a new one with a random position and size, and the "
	"randomization of the module does the same.\n"
	"The box color setting of the layout panel, by default the cursor color, sets the fill color, and the "
	"contextual menu offers the cmd_reset and cmd_randomize commands, which use the same ids as the "
	"buttons (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// also used by the contextual menu commands, see onCreateCommands
constexpr NativeInt RESET_BTN_CBID     = 0x001500F0;
constexpr NativeInt RANDOMIZE_BTN_CBID = 0x001500F1;

inline constexpr TParamInfo resetBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "reset",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.IsSeparator  = TRUE,
	.CallBackId   = RESET_BTN_CBID,
	.Description  = "Erases the rectangle."
};

inline constexpr TParamInfo randomizeBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "randomize",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = RANDOMIZE_BTN_CBID,
	.Description  = "Draws a rectangle with a random position and size."
};
