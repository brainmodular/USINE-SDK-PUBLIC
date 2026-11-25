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
	"[verbosity level], in the Report section shown with all the parameters, filters these messages for this "
	"instance only: none traces nothing, error only errors, warn errors and warnings, all traces everything. "
	"It is saved with the patch.\n"
	"Messages are registered once when the module loads and raised by index, and the module does no audio "
	"or data processing, it only reacts to its buttons (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

// each message is registered once, registerReport returns the index to give to sdkReport
inline const int ERROR_WRITE               USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "this is an error print");
inline const int WARNING_WRITE             USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "this is a warn print");
inline const int INFO_WRITE                USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "this is an info print");

constexpr NativeInt ERROR_BTN_CBID     = 0x00100000;
constexpr NativeInt WARNING_BTN_CBID   = 0x00200000;
constexpr NativeInt INFO_BTN_CBID      = 0x00300000;

constexpr TParamInfo errorBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "error btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = ERROR_BTN_CBID,
	.Description  = "Raises the example error message in the Usine trace, followed by an additional text."
};

constexpr TParamInfo warningBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "warning btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = WARNING_BTN_CBID,
	.Description  = "Raises the example warning message in the Usine trace and also writes it in the Usine log file."
};

constexpr TParamInfo infoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "info btn",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INFO_BTN_CBID,
	.Description  = "Raises the example info message in the Usine trace."
};
