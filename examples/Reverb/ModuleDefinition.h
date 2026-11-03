//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the Reverb module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "reverb";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "multichannel Freeverb reverb (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.1";

constexpr NativeInt MODE_SW_CBID                = 0x002100F0;
constexpr NativeInt ROOM_SIZE_FDR_CBID             = 0x002100F1;
constexpr NativeInt DAMPING_FDR_CBID               = 0x002100F2;
constexpr NativeInt CHANNELS_SEPARATION_FDR_CBID   = 0x002100F3;
constexpr NativeInt WET_LEVEL_FDR_CBID             = 0x002100F4;
constexpr NativeInt DRY_LEVEL_FDR_CBID             = 0x002100F5;

// default values, in the unit of the parameters (not between 0 and 1)
constexpr float INITIAL_WET                 = 0.0f;    // dB
constexpr float INITIAL_DRY                 = -200.0f; // dB
constexpr float INITIAL_ROOM                = 95.0f;   // %
constexpr float INITIAL_DAMP                = 75.0f;   // %
constexpr float INITIAL_CHANNELS_SEPARATION = 50.0f;   // %
constexpr float INITIAL_MODE                = 0.0f;    // switch

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

inline constexpr TParamInfo modeSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "freeze",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_MODE,
	.CallBackType = ctNormal,
	.CallBackId   = MODE_SW_CBID
};

inline constexpr TParamInfo roomSizeFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "room size",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scExp,
	.MinValue     = 0.1f,
	.MaxValue     = 100.0f,
	.DefaultValue = INITIAL_ROOM,
	.Symbol       = "%",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = ROOM_SIZE_FDR_CBID
};

inline constexpr TParamInfo dampingFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "damping",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = INITIAL_DAMP,
	.Symbol       = "%",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = DAMPING_FDR_CBID
};

inline constexpr TParamInfo channelsSeparationFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "channels separation",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = INITIAL_CHANNELS_SEPARATION,
	.Symbol       = "%",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = CHANNELS_SEPARATION_FDR_CBID
};

inline constexpr TParamInfo wetLevelFdrInParamInfo {
	.ParamType    = ptGainFader,
	.Caption      = "wet level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_WET,
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = WET_LEVEL_FDR_CBID
};

inline constexpr TParamInfo dryLevelFdrInParamInfo {
	.ParamType    = ptGainFader,
	.Caption      = "dry level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_DRY,
	.CallBackType = ctNormal,
	.CallBackId   = DRY_LEVEL_FDR_CBID
};
