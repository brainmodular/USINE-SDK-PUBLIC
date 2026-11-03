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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "draws a box with the mouse (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

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
