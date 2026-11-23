//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the DataSampler module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "data sampler";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"records the values of [data], one per audio bloc, while [toggle rec] is ON, then plays the recording "
	"back in a loop.\n"
	"Turning [toggle rec] ON erases the previous recording, sets [out] to 0 and starts again from the "
	"first bloc; turning it OFF stores the last chunk and loops over the whole recording.\n"
	"[out] follows [data] during the capture, then outputs one value per audio bloc and wraps at the "
	"end.\n"
	"The capture uses a double buffer of 2000 values: when one is full the audio thread swaps the two and "
	"asks the main thread to append the previous one, so the long copy never runs in onProcess and the "
	"recording length has no limit.\n"
	"[clear rec] stops, erases and sets [out] to 0.\n"
	"[color] and the box color setting of the layout panel set the color of the curve drawn in the panel "
	"(sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

inline const int INFO_REC_STARTED USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "recording started");
inline const int INFO_REC_STOPPED USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "recording stopped");
inline const int INFO_REC_CLEARED USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "record cleared");

constexpr NativeInt REC_SW_CBID  = 0x001400F0;
constexpr NativeInt CLEAR_BTN_CBID  = 0x001400F1;
constexpr NativeInt BOX_COL_CBID    = 0x001400F2;
// not a parameter: sent by the module itself with sdkNotifyUsine when a buffer is full
constexpr NativeInt AGG_BUFFER_CBID = 0x001400FF;

inline constexpr TParamInfo dataFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "data",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.CallBackType = ctNone,
	.Description  = "Value to record, sampled once per audio bloc while [toggle rec] is ON."
};

inline constexpr TParamInfo recSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "toggle rec",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = REC_SW_CBID,
	.Description  = "ON erases the previous recording and records [data].\n"
	                "OFF stops the recording and plays it back in a loop on [out]."
};

inline constexpr TParamInfo clearBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "clear rec",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = CLEAR_BTN_CBID,
	.Description  = "Stops the recording, erases it and sets [out] to 0."
};

inline constexpr TParamInfo boxColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.IsSeparator  = TRUE,
	.CallBackId   = BOX_COL_CBID,
	.Description  = "Color of the recorded curve drawn in the panel."
};

inline constexpr TParamInfo outFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.DefaultValue = 0.0f,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE,
	.Description  = "Follows [data] while recording, then plays the recording back in a loop, one value per audio bloc."
};
