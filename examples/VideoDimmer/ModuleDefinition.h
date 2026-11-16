//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  Olivier Sens (senso), BrainModular team
// @brief   Module constants and parameters of the VideoDimmer module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "video dimmer";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"a video effect with one video input and one video output, processed in the video thread: [dimmer] "
	"scales the brightness of the whole frame, from 0 for black to 1 for the unchanged image, and [inter "
	"space] sets the grid step N in pixels, from 0 to 20, default 1.\n"
	"With a step of 0 the input frame is copied untouched; with a larger step the output is cleared to "
	"opaque black, one row out of N is drawn and, on that row, one pixel out of N is kept, followed by a "
	"pixel of [pixel color], red by default, so the image is reduced to a grid of samples.\n"
	"A step of 1 keeps every pixel and the grid color never appears.\n"
	"The dimmer is applied after the grid, then the frame is sent to the output at each video tick (sdk "
	"example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// no callbacks: the values are read in onProcessVideo
inline constexpr TParamInfo dimmerFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "dimmer",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = 1,
	.DefaultValue = 1,
	.CallBackType = ctNone,
	.Description  = "Brightness of the output video, from 0 (black) to 1 (unchanged)."
};

inline constexpr TParamInfo pixelColInParamInfo {
	.ParamType    = ptColor,
	.Caption      = "pixel color",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Color of the pixel drawn right after each pixel kept by the grid.\n"
	                "Red by default."
};

inline constexpr TParamInfo spaceFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "inter space",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0,
	.MaxValue     = 20,
	.DefaultValue = 1,
	.Format       = "%.0f", // DEFAULT_FORMAT_INTEGER is not constexpr
	.CallBackType = ctNone,
	.Description  = "Grid step N in pixels: one pixel out of N, on one row out of N, is kept and followed by a pixel of [pixel color], the rest is black.\n"
	                "0 or 1 keeps the whole image."
};
