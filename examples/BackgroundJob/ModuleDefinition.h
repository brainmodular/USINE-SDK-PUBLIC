//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the BackgroundJob module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "background job";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"runs a deliberately long computation outside the audio and interface threads, so Usine keeps playing "
	"while the job works.\n"
	"[data1] and [data2] are data fields from 0 to 1, default 0, cached when they change;\n"
	"[start job] launches the job and the [processing] and [processing time] outputs added by the SDK "
	"report its state.\n"
	"A press during a job is not lost: another job starts when the current one ends.\n"
	"The job body loops one hundred million times over nested sines of [data1] and [data2] and multiplies "
	"the last value by both, so the result follows the inputs without being a plain product.\n"
	"onJobBegin is empty, onJobProcess runs in a background thread, and onJobEnd, called by Usine once "
	"the job is done, writes the result to [out], which is not saved with the module.\n"
	"There is no process callback (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

constexpr NativeInt ONE_FDR_CBID   = 0x001800F0;
constexpr NativeInt TWO_FDR_CBID   = 0x001800F1;
constexpr NativeInt START_BTN_CBID = 0x001800F2;

constexpr TParamInfo oneFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "data1",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = ONE_FDR_CBID,
	.Description  = "First input of the computation, read when the job starts."
};

constexpr TParamInfo twoFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "data2",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = TWO_FDR_CBID,
	.Description  = "Second input of the computation, read when the job starts."
};

constexpr TParamInfo startBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "start job",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = START_BTN_CBID,
	.Description  = "Starts the computation in a background thread, [processing] stays ON until it ends.\n"
	                "Pressed during a job, another job starts when it ends."
};

constexpr TParamInfo outFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.DefaultValue = 0.0f,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE,
	.Description  = "Result of the computation, a deliberately long loop of sines on [data1] and [data2], set when the job ends."
};
