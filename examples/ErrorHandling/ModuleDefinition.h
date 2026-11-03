//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the ErrorHandling module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "error handling";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "registers and raises errors, warnings and infos (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// each message is registered once, RegisterError returns the index to give to sdkRaiseError
inline const int ERROR_WRITE               = RegisterError(Level::Error, "this is an error print");
inline const int WARNING_WRITE             = RegisterError(Level::Warn, "this is a warn print");
inline const int INFO_WRITE                = RegisterError(Level::Info, "this is an info print");
inline const int WARNING_UNKNOWN_VERBOSITY = RegisterError(Level::Warn, "unknown verbosity level, expected None, Error, Warn, Info or All, got: ");

constexpr NativeInt ERROR_BTN_CBID     = 0x00100000;
constexpr NativeInt WARNING_BTN_CBID   = 0x00200000;
constexpr NativeInt INFO_BTN_CBID      = 0x00300000;
constexpr NativeInt VERBOSITY_TXT_CBID = 0x00001000;

inline constexpr TParamInfo errorBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "error btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = ERROR_BTN_CBID
};

inline constexpr TParamInfo warningBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "warning btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = WARNING_BTN_CBID
};

inline constexpr TParamInfo infoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "info btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INFO_BTN_CBID
};

inline constexpr TParamInfo verbosityTxtInParamInfo {
	.ParamType    = ptText,
	.Caption      = "verbosity level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = VERBOSITY_TXT_CBID
};
