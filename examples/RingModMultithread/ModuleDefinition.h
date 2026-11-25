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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"ring modulator with the same parameters as the plain version but processing its channels in two "
	"threads: the audio inputs are multiplied by the [mod] audio input and blended with the dry signal by "
	"[mix] (0 to 1, default 0.5, 0 dry only, 1 modulated only), without smoothing.\n"
	"Thread 1 handles the even channels and thread 2 the odd ones, each walking the channel list with a "
	"step of two.\n"
	"Both threads are created once at initialization with a medium priority and stay alive; at each bloc "
	"onProcess sets a counter to two, restarts them and waits on a sync object for up to 500 ms, and the "
	"last thread to finish signals it.\n"
	"A critical section protects the counter.\n"
	"Each channel owns a temporary bloc buffer allocated at creation and never resized (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

constexpr NativeInt MIX_FDR_CBID = 0x002300F0;

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
