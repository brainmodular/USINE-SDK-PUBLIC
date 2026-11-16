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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"granular synthesis on the audio channels chosen at creation.\n"
	"Each [in 1] is recorded in its own delay line, read by 16 grains reused in a round robin and summed "
	"in [out 1].\n"
	"[duration] sets the grain length from 0.05 to 2000 ms;\n"
	"[pitch] transposes it by -48 to +48 halftones, with cubic interpolation from -48 to -24 and linear "
	"above.\n"
	"In Interval mode [interval] sets the time before the next grain, -1000 to 20000 ms, negative values "
	"letting grains overlap.\n"
	"In Play Button mode [play] starts one grain, on the first sample of a bloc.\n"
	"[grain env] picks the envelope and [env var] shapes it, with no effect on Hanning.\n"
	"The curve is painted in the panel, where a drag also edits duration, envelope variable and, with "
	"Shift, pitch, and records an automation.\n"
	"[trig out] blinks at each grain start, with a warning when grains overlap (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.261112";

inline const int ERROR_RECORD_AUTOMATION      USINE_MODULE_LOCAL = RegisterError(Level::Error, "failed to record automation");
inline const int ERROR_STOP_RECORD_AUTOMATION USINE_MODULE_LOCAL = RegisterError(Level::Error, "failed to stop automation recording");
inline const int WARNING_GRAIN_OVERLAP        USINE_MODULE_LOCAL = RegisterError(Level::Warn, "too many overlapping grains, the oldest ones are cut");

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
	.CallBackType = ctNone,
	.Description  = "Audio input of a channel, continuously recorded in the delay line where the grains are read."
};

inline constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Audio output of a channel: the sum of the grains taken from the matching input."
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
	.IsSeparator  = TRUE,
	.Description  = "Blinks each time a grain starts."
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
	.CallBackId       = GRAIN_INTERVAL_FDR_CBID,
	.Description      = "Time between the end of a grain and the start of the next one in Interval mode, in ms.\n"
	                    "Negative values overlap the grains."
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
	.CallBackId       = GRAIN_ENVELOPE_LBOX_CBID,
	.Description      = "Amplitude envelope of the grains: Linear ASR, Bezier, Raised Cosine Bell, Hanning or Gaussian.\n"
	                    "Its curve is drawn in the panel."
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
	.CallBackId       = GRAIN_ENV_VAR_FDR_CBID,
	.Description      = "Shape of the envelope set by [grain env]: attack and release length (Linear ASR, Raised Cosine Bell), curvature (Bezier), narrowness (Gaussian).\n"
	                    "No effect on Hanning."
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
	.CallBackId       = GRAIN_DURATION_FDR_CBID,
	.Description      = "Duration of each grain in ms.\n"
	                    "A horizontal drag in the panel also changes it."
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
	.CallBackId       = GRAIN_PITCH_FDR_CBID,
	.Description      = "Transposition of the grains in halftones.\n"
	                    "A vertical drag with Shift in the panel also changes it."
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
	.CallBackId       = GRAIN_MODE_LBOX_CBID,
	.Description      = "How the grains start: Interval starts them continuously, separated by [interval]; Play Button starts one grain at each [play]."
};

// starts a grain in the Play Button mode
inline constexpr TParamInfo grainGoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "play",
	.IsInput      = TRUE,
	.IsOutput     = TRUE,
	.CallBackType = ctImmediate,
	.CallBackId   = GRAIN_GO_BTN_CBID,
	.Description  = "Starts one grain when [grain mode] is Play Button."
};
