//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the RingMod module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "ring mod";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"ring modulator: one audio input and one audio output per channel chosen in the audio query, captions "
	"in 1, out 1 and so on.\n"
	"Every input is multiplied sample by sample with the [mod] audio input, which is common to all "
	"channels, then blended with the untouched input by [mix], a fader from 0 to 1 whose default 0.5 "
	"gives an equal balance: 0 passes the dry signal only, 1 the ring-modulated signal only.\n"
	"[mix] is read when its callback fires and applies from the next bloc, without smoothing.\n"
	"The module works on the Usine audio buffers and uses a single temporary bloc buffer, allocated once "
	"at creation with the bloc size and never resized afterwards.\n"
	"One [mod] input drives all channels; feeding it an audio signal produces the ring modulation (sdk "
	"example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

constexpr NativeInt MIX_FDR_CBID = 0x002200F0;

// audio in/out: the caption depends on the channel, it is set in onCreateParameters
constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Audio input of a channel, multiplied by [mod]."
};

constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Audio output of a channel: the input multiplied by [mod], mixed with the dry input by [mix]."
};

constexpr TParamInfo modAudioInParamInfo {
	.ParamType    = ptAudio,
	.Caption      = "mod",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Modulation signal, multiplied with every audio input."
};

constexpr TParamInfo mixFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "mix",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.5f,
	.CallBackType = ctNormal,
	.CallBackId   = MIX_FDR_CBID,
	.Description  = "Dry/wet mix balance.\n"
	                "Range: [0..1]. 0=dry input only, 0.5=equal mix, 1=ring-modulated signal only."
};
