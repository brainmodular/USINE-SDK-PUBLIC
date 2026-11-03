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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "complete template with every callback (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// messages raised with sdkRaiseError, example:
// inline const int ERROR_EXAMPLE = RegisterError(Level::Error, "something went wrong: ");

// one callback id per parameter that has a callback, example:
// constexpr NativeInt EXAMPLE_FDR_CBID = 0x00100000;

inline constexpr TParamInfo exampleTxtInParamInfo {
	.ParamType    = ptText,
	.Caption      = "text",
	.IsInput      = TRUE,
	.IsOutput     = TRUE,
	.TextValue    = "hello world",
	.CallBackType = ctNone
};
