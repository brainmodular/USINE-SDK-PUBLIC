//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the AudioVolume module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "audio volume";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "gain and mute on multichannel audio (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

constexpr NativeInt GAIN_FDR_CBID = 0x001100F0;
constexpr NativeInt MUTE_SW_CBID = 0x001100FF;

// audio in/out: the caption depends on the channel, it is set in onCreateParameters
inline constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo gainFdrInParamInfo {
	.ParamType        = ptGainFader,
	.Caption          = "gain",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctImmediate,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "volume",
	.CallBackId       = GAIN_FDR_CBID
};

inline constexpr TParamInfo muteSwInParamInfo {
	.ParamType    = ptLeftLed,
	.Caption      = "mute",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = MUTE_SW_CBID
};
