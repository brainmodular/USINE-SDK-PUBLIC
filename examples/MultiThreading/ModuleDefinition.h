//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages and parameters of the MultiThreading module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "multithreading";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "two Usine threads sharing a critical section (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline const int INFO_THREAD_1 = RegisterError(Level::Info, "thread 1");
inline const int INFO_THREAD_2 = RegisterError(Level::Info, "thread 2");

inline constexpr TParamInfo arrOutParamInfo {
	.ParamType        = ptArray,
	.Caption          = "output",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = ""
};
