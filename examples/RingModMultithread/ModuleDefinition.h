//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the RingModMultithread module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "ring mod multithread";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "ring modulator processing its channels in two threads (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt MIX_FDR_CBID = 0x002300F0;

// audio in/out: the caption depends on the channel, it is set in onCreateParameters
inline constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo modAudioInParamInfo {
	.ParamType    = ptAudio,
	.Caption      = "mod",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo mixFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "mix",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.5f,
	.CallBackType = ctNormal,
	.CallBackId   = MIX_FDR_CBID
};
