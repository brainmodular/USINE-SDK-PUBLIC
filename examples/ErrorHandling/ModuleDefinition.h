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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"raises errors, warnings and infos in the Usine trace to show how the error system works.\n"
	"[error btn] raises a registered error followed by an additional text, [warning btn] raises a warning "
	"and also writes it in the Usine log file, [info btn] raises a plain info.\n"
	"[verbosity level] filters these messages: None traces nothing, Error only errors, Warn errors and "
	"warnings, Info or All traces everything, since a message is traced when its level is lower than or "
	"equal to the setting.\n"
	"An empty text is ignored and any other text raises a warning that names the received value.\n"
	"Messages are registered once when the module loads and raised by index, and the module does no audio "
	"or data processing, it only reacts to its buttons and text field (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// each message is registered once, RegisterError returns the index to give to sdkRaiseError
inline const int ERROR_WRITE               USINE_MODULE_LOCAL = RegisterError(Level::Error, "this is an error print");
inline const int WARNING_WRITE             USINE_MODULE_LOCAL = RegisterError(Level::Warn, "this is a warn print");
inline const int INFO_WRITE                USINE_MODULE_LOCAL = RegisterError(Level::Info, "this is an info print");
inline const int WARNING_UNKNOWN_VERBOSITY USINE_MODULE_LOCAL = RegisterError(Level::Warn, "unknown verbosity level, expected None, Error, Warn, Info or All, got: ");

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
	.CallBackId   = ERROR_BTN_CBID,
	.Description  = "Raises the example error message in the Usine trace, followed by an additional text."
};

inline constexpr TParamInfo warningBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "warning btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = WARNING_BTN_CBID,
	.Description  = "Raises the example warning message in the Usine trace and also writes it in the Usine log file."
};

inline constexpr TParamInfo infoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "info btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INFO_BTN_CBID,
	.Description  = "Raises the example info message in the Usine trace."
};

inline constexpr TParamInfo verbosityTxtInParamInfo {
	.ParamType    = ptText,
	.Caption      = "verbosity level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = VERBOSITY_TXT_CBID,
	.Description  = "Messages traced by the module: None, Error (errors only), Warn (errors and warnings), Info or All (every message).\n"
	                "Other texts raise a warning."
};
