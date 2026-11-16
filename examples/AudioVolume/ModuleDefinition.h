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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"audio gain stage for a user-chosen number of channels: the query popup shown at creation picks the "
	"audio channel layout, up to the maximum the SDK allows, and the module then creates one audio input "
	"and one audio output per channel, captioned in 1, out 1 and so on.\n"
	"[gain] is a fader in dB applied to every channel, 0 dB leaving the signal unchanged, and [mute] "
	"silences all channels without moving [gain].\n"
	"Both are merged into one linear coefficient, recomputed when either changes, so [mute] zeroes the "
	"coefficient and not the dB value.\n"
	"onProcess copies each input to its matching output and multiplies the copy, bloc by bloc, with no "
	"latency.\n"
	"The channel count is fixed at creation and the module is never stored in presets (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.261112";

constexpr NativeInt GAIN_FDR_CBID = 0x001100F0;
constexpr NativeInt MUTE_SW_CBID = 0x001100FF;

// audio in/out: the caption depends on the channel, it is set in onCreateParameters
inline constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Audio input of a channel.\n"
	                "The number of channels is chosen when the module is created."
};

inline constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Audio output of a channel: the matching input multiplied by [gain], silent when [mute] is ON."
};

inline constexpr TParamInfo gainFdrInParamInfo {
	.ParamType        = ptGainFader,
	.Caption          = "gain",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctImmediate,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "volume",
	.CallBackId       = GAIN_FDR_CBID,
	.Description      = "Volume applied to all the channels, in dB.\n"
	                    "0 dB leaves the signal unchanged."
};

inline constexpr TParamInfo muteSwInParamInfo {
	.ParamType    = ptLeftLed,
	.Caption      = "mute",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = MUTE_SW_CBID,
	.Description  = "Silences all the channels when ON, without changing [gain]."
};
