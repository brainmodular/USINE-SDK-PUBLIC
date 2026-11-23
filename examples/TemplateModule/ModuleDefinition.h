//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the TemplateModule module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "template";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"starting point for a new module: it overrides most of the callbacks of UserModuleBase with an empty "
	"body, from module info, parameters and process to the video process, the background job, the chunk "
	"methods, the layout, the mouse and multi touch events, the editor window, the audio setup, the "
	"randomization and the midi outputs, so the list can be trimmed to what the module needs.\n"
	"A single parameter, [text], shows the pattern of a TParamInfo block: a text inlet and outlet "
	"initialized to hello world, without callback.\n"
	"The callback handler already ignores everything but change notifications and reads the callback id "
	"in wParam, and the chunk methods report an empty state.\n"
	"The module is declared as a simple module with processing enabled; its process functions stay empty "
	"until code is added (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// messages raised with sdkReport, example:
// inline const int ERROR_EXAMPLE USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "something went wrong: ");

// one callback id per parameter that has a callback, example:
// constexpr NativeInt EXAMPLE_FDR_CBID = 0x00100000;

inline constexpr TParamInfo exampleTxtInParamInfo {
	.ParamType    = ptText,
	.Caption      = "text",
	.IsInput      = TRUE,
	.IsOutput     = TRUE,
	.TextValue    = "hello world",
	.CallBackType = ctNone,
	.Description  = "Example text with an inlet and an outlet, initialized to hello world.\n"
	                "Replace it with the parameters of your module."
};
