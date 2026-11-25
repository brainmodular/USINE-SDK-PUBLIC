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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"runs two Usine threads that share a critical section.\n"
	"[output] is the only parameter, an array output that is never written.\n"
	"The module has no audio, midi or data processing and no process callback, it only demonstrates the "
	"thread functions of the SDK.\n"
	"In onInitModule it creates a critical section, declared shared so that other modules can lock it "
	"too, and starts two Usine threads named thread 1 and thread 2, both at medium priority with a 100 ms "
	"execution timeout.\n"
	"Each of them locks the section, writes its own info message in the Usine trace, then unlocks it, so "
	"the two messages never interleave.\n"
	"Both threads and the section are destroyed with the module, and the module is not saved in presets "
	"(sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

inline const int INFO_THREAD_1 USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "thread 1");
inline const int INFO_THREAD_2 USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "thread 2");

constexpr TParamInfo arrOutParamInfo {
	.ParamType        = ptArray,
	.Caption          = "output",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "",
	.Description      = "Not used: the two threads only write their messages in the Usine trace."
};
