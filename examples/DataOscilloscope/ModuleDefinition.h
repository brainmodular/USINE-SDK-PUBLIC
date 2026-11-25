//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, callback ids and parameters of the DataOscilloscope module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "data oscilloscope";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"draws a scrolling curve of a data input, refreshed on display ticks, not on every audio bloc.\n"
	"[y] is the value plotted, a data field from -1 to 1, default 0.5: 0 sits at the bottom of the panel "
	"and 1 at the top.\n"
	"Each refresh appends the current [y] at the left edge, shifts every stored point to the right by "
	"[speed] and drops the points that leave the panel, so the history covers one panel width.\n"
	"[speed] is a logarithmic fader from 0 to 0.5, default 0.005, read as a fraction of the panel width "
	"per refresh; 0 freezes the curve.\n"
	"[color] sets the path color, also editable as box color in the edit layout settings, and each new "
	"point or settings change repaints the panel.\n"
	"The default panel is 134 by 134 and points sit in a vector cleared at start (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";

constexpr NativeInt Y_FDR_CBID     = 0x001300F0;
constexpr NativeInt SPEED_FDR_CBID = 0x001300F1;
constexpr NativeInt BOX_COL_CBID   = 0x001300F2;

constexpr TParamInfo yFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "y",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = -1.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.5f,
	.CallBackType = ctImmediate,
	.CallBackId   = Y_FDR_CBID,
	.Description  = "Value drawn by the curve: 0 at the bottom of the panel, 1 at the top."
};

constexpr TParamInfo speedFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "speed",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scLog,
	.MinValue     = 0.0f,
	.MaxValue     = 0.5f,
	.DefaultValue = 0.005f,
	.CallBackType = ctNormal,
	.CallBackId   = SPEED_FDR_CBID,
	.Description  = "Scrolling speed of the curve, as a fraction of the panel width per display refresh.\n"
	                "0 freezes the curve."
};

constexpr TParamInfo boxColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctImmediate,
	.CallBackId   = BOX_COL_CBID,
	.Description  = "Color of the curve."
};
