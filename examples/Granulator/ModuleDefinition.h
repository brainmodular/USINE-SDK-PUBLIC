//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the Granulator module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"
#include "Granulator.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "granulator";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "granular synthesis on live audio inputs (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.3";

inline const int ERROR_RECORD_AUTOMATION      = RegisterError(Level::Error, "failed to record automation");
inline const int ERROR_STOP_RECORD_AUTOMATION = RegisterError(Level::Error, "failed to stop automation recording");
inline const int WARNING_GRAIN_OVERLAP        = RegisterError(Level::Warn, "too many overlapping grains, the oldest ones are cut");

constexpr NativeInt GRAIN_INTERVAL_FDR_CBID  = 0x001700F0;
constexpr NativeInt GRAIN_ENVELOPE_LBOX_CBID = 0x001700F1;
constexpr NativeInt GRAIN_ENV_VAR_FDR_CBID   = 0x001700F2;
constexpr NativeInt GRAIN_DURATION_FDR_CBID  = 0x001700F3;
constexpr NativeInt GRAIN_PITCH_FDR_CBID     = 0x001700F4;
constexpr NativeInt GRAIN_MODE_LBOX_CBID     = 0x001700F5;
constexpr NativeInt GRAIN_GO_BTN_CBID        = 0x001700F6;

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

// blinks each time a grain starts
inline constexpr TParamInfo grainStartSwOutParamInfo {
	.ParamType    = ptTriggerLed,
	.Caption      = "trig out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.Symbol       = "",
	.Format       = "%.0f",
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.IsSeparator  = TRUE
};

inline constexpr TParamInfo grainIntervalFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "interval",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.Scale            = scLog,
	.MinValue         = -1000.0f, // a negative interval makes the grains overlap
	.MaxValue         = 20000.0f,
	.DefaultValue     = 100.0f,
	.Symbol           = " ms",
	.Format           = "%.3f",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_INTERVAL_FDR_CBID
};

// the order of the items must match Grain::EnvType
inline constexpr TParamInfo grainEnvelopeLboxInParamInfo {
	.ParamType        = ptListBox,
	.Caption          = "grain env",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 0,
	.ListBoxStrings   = "\"Linear ASR\",\"Bezier\",\"Raised Cosine Bell\",\"Hanning\",\"Gaussian\"",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_ENVELOPE_LBOX_CBID
};

inline constexpr TParamInfo grainEnvVarFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "env var",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = ENV_VAR_MIN,
	.MaxValue         = ENV_VAR_MAX,
	.DefaultValue     = 0.5f,
	.Format           = "%.3f",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_ENV_VAR_FDR_CBID
};

inline constexpr TParamInfo grainDurationFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "duration",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.Scale            = scLog,
	.MinValue         = GRAIN_DUR_MIN,
	.MaxValue         = GRAIN_DUR_MAX,
	.DefaultValue     = 100.0f,
	.Symbol           = " ms",
	.Format           = "%.3f",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_DURATION_FDR_CBID
};

inline constexpr TParamInfo grainPitchFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "pitch",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = GRAIN_PITCH_MIN,
	.MaxValue         = GRAIN_PITCH_MAX,
	.DefaultValue     = 0.0f,
	.Symbol           = " ht",
	.Format           = "%.0f",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_PITCH_FDR_CBID
};

// the order of the items must match GRAIN_MODE_INTERVAL and GRAIN_MODE_PLAY
inline constexpr TParamInfo grainModeLboxInParamInfo {
	.ParamType        = ptListBox,
	.Caption          = "grain mode",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 0,
	.ListBoxStrings   = "\"Interval\",\"Play Button\"",
	.ReadOnly         = FALSE,
	.CallBackType     = ctImmediate,
	.IsStoredInPreset = TRUE,
	.CallBackId       = GRAIN_MODE_LBOX_CBID
};

// starts a grain in the Play Button mode
inline constexpr TParamInfo grainGoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "play",
	.IsInput      = TRUE,
	.IsOutput     = TRUE,
	.CallBackType = ctImmediate,
	.CallBackId   = GRAIN_GO_BTN_CBID
};
