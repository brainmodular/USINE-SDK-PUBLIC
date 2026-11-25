//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the DataMultiply module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "data multiply";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"multiplies two data values and keeps [result] up to date with no process callback: [a] and [b] are "
	"data inputs that share a single callback id, so any change of either one recomputes the product and "
	"sends it to [result] at once, without waiting for an audio bloc.\n"
	"The product is computed by the mult method of the Usine event class, so the module only routes the "
	"callback: it is a compact reference for event-driven data modules.\n"
	"The parameter declarations set no range and no default value, and [result] is never clamped, so the "
	"product grows freely.\n"
	"[result] is an output only, with no callback and marked as not saved, so the host does not restore "
	"it from the file, and the whole module is excluded from presets (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

// shared by both inputs: any change recomputes the result
constexpr NativeInt INPUT_FDR_CBID = 0x001200FF;

constexpr TParamInfo aFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "a",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INPUT_FDR_CBID,
	.Description  = "First factor: each change recomputes [result]."
};

constexpr TParamInfo bFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "b",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = INPUT_FDR_CBID,
	.Description  = "Second factor: each change recomputes [result]."
};

constexpr TParamInfo resultFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "result",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE,
	.Description  = "Product of [a] and [b], updated as soon as one of them changes."
};
