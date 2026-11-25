//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants and parameters of the MidiTranspose module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "midi transpose";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"transposes the notes of a midi flow.\n"
	"[in] receives the midi messages and [out] returns a copy of them in which only the note on and note "
	"off messages are changed: their note number is shifted by [pitch] and kept in 0..127, so a note "
	"pushed out of the range stops on the nearest limit instead of being dropped.\n"
	"Controllers, pitch bend, clock and sysex pass through untouched, on the same channel and at the same "
	"position in the bloc.\n"
	"[pitch] is a fader from -127 to 127 halftones, 0 by default, rounded to the nearest integer.\n"
	"It has no callback, its value is read at each bloc in onProcess, and it is marked as stored in "
	"preset.\n"
	"When [in] is empty the module clears [out] and does nothing else, and it keeps no state between "
	"blocs (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

constexpr TParamInfo midiInParamInfo {
	.ParamType    = ptMidi,
	.Caption      = "in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone,
	.Description  = "MIDI flow to transpose."
};

constexpr TParamInfo midiOutParamInfo {
	.ParamType    = ptMidi,
	.Caption      = "out",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Copy of [in] with the note on and note off messages transposed by [pitch]."
};

// no callback: the value is read in onProcess
constexpr TParamInfo pitchFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "pitch",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = -127.0f,
	.MaxValue         = 127.0f,
	.DefaultValue     = 0.0f,
	.Symbol           = "halftone",
	.Format           = "%.0f",
	.CallBackType     = ctNone,
	.IsStoredInPreset = TRUE,
	.Description      = "Transposition in halftones.\n"
	                    "The transposed notes are kept within 0..127."
};
