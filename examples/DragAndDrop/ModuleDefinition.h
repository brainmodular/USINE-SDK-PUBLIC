//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants of the DragAndDrop module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "drag and drop";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"accepts the files dragged from the Finder on its panel and lists the names of the last dropped ones, "
	"one per line; it has no parameter and no audio process.\n"
	"Each drop replaces the previous list, keeps only the file name of the full paths Usine sends and "
	"decodes the comma-text quoting, so a name holding a space, a comma or a quote stays whole.\n"
	"The panel shows a drop files here placeholder while the list is empty, turns green while a file is "
	"dragged over it and shows up to seven names; when more files are dropped the last line becomes + N "
	"more.\n"
	"There is no drag leave callback: a drag that leaves the panel without a drop keeps it green until "
	"the mouse moves over it again.\n"
	"Dropped files are only displayed, never opened or loaded (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261125";
