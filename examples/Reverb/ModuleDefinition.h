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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"multichannel reverb built on the Freeverb model: each audio channel selected in the query gets its "
	"own set of eight parallel comb filters followed by four allpasses in series.\n"
	"[room size] (0.1 to 100%, default 95) sets the comb feedback and so the tail length;\n"
	"[damping] (0 to 100%, default 75) absorbs the high frequencies;\n"
	"[channels separation] (0 to 100%, default 50) sets how much of the other channels feeds each reverb, "
	"at 100% every channel is reverberated alone.\n"
	"[wet level] and [dry level] are gains in dB, default 0 dB and -200 dB, so the outputs only carry the "
	"reverb; both are smoothed bloc by bloc to avoid clicks.\n"
	"[freeze] holds the tail indefinitely and stops feeding it from the inputs.\n"
	"Channel buffers are offset to decorrelate them and the tuning assumes a 44.1 kHz sample rate (sdk "
	"example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

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
constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Audio input of a channel, sent to the reverb and to the matching output."
};

constexpr TParamInfo audioOutParamInfo {
	.ParamType    = ptAudio,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Audio output of a channel: the reverb at [wet level] plus the input at [dry level]."
};

constexpr TParamInfo modeSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "freeze",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_MODE,
	.CallBackType = ctNormal,
	.CallBackId   = MODE_SW_CBID,
	.Description  = "When ON, holds the reverb tail indefinitely and stops feeding it with the inputs."
};

constexpr TParamInfo roomSizeFdrInParamInfo {
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
	.CallBackId   = ROOM_SIZE_FDR_CBID,
	.Description  = "Size of the simulated room: the larger, the longer the reverb tail."
};

constexpr TParamInfo dampingFdrInParamInfo {
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
	.CallBackId   = DAMPING_FDR_CBID,
	.Description  = "Absorption of the high frequencies in the reverb tail: higher values give a darker sound."
};

constexpr TParamInfo channelsSeparationFdrInParamInfo {
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
	.CallBackId   = CHANNELS_SEPARATION_FDR_CBID,
	.Description  = "At 100% each channel is reverberated alone, lower values mix the other channels into the reverb of each one."
};

constexpr TParamInfo wetLevelFdrInParamInfo {
	.ParamType    = ptGainFader,
	.Caption      = "wet level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_WET,
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = WET_LEVEL_FDR_CBID,
	.Description  = "Level of the reverberated signal, in dB."
};

constexpr TParamInfo dryLevelFdrInParamInfo {
	.ParamType    = ptGainFader,
	.Caption      = "dry level",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = INITIAL_DRY,
	.CallBackType = ctNormal,
	.CallBackId   = DRY_LEVEL_FDR_CBID,
	.Description  = "Level of the direct signal, in dB.\n"
	                "Muted by default: the outputs only carry the reverb."
};
