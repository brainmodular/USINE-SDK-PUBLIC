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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "records a data input and plays it back in a loop (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline const int INFO_REC_STARTED = RegisterError(Level::Info, "recording started");
inline const int INFO_REC_STOPPED = RegisterError(Level::Info, "recording stopped");
inline const int INFO_REC_CLEARED = RegisterError(Level::Info, "record cleared");

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
	.CallBackType = ctNone
};

inline constexpr TParamInfo recSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "toggle rec",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = REC_SW_CBID
};

inline constexpr TParamInfo clearBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "clear rec",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 0.0f,
	.CallBackType = ctImmediate,
	.CallBackId   = CLEAR_BTN_CBID
};

inline constexpr TParamInfo boxColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.IsSeparator  = TRUE,
	.CallBackId   = BOX_COL_CBID
};

inline constexpr TParamInfo outFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.DefaultValue = 0.0f,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE
};
