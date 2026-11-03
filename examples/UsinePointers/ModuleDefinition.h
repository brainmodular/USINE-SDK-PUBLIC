//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the UsinePointers module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "usine pointers";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "copies the data of a pointer input to a pointer output (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline const int ERROR_POINTER_NOT_FLOAT = RegisterError(Level::Error, "the pointer is not type float");

constexpr NativeInt PTR_IN_CBID = 0x002300F0;

// PointerDataType can be any UINT32, see UsinePointerTypes for the types known by Usine
inline constexpr TParamInfo ptrInParamInfo {
	.ParamType       = ptPointer,
	.Caption         = "pointer in",
	.IsInput         = TRUE,
	.IsOutput        = FALSE,
	.CallBackType    = ctImmediate,
	.CallBackId      = PTR_IN_CBID,
	.PointerDataType = usinePtrFloat
};

inline constexpr TParamInfo ptrOutParamInfo {
	.ParamType       = ptPointer,
	.Caption         = "pointer out",
	.IsInput         = FALSE,
	.IsOutput        = TRUE,
	.CallBackType    = ctNone,
	.PointerDataType = usinePtrFloat
};
