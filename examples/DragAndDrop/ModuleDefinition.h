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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "shows the names of the files dropped on it (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";
