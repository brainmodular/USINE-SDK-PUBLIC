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
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"copies the float data carried by a pointer from the inlet to the outlet without processing it: the "
	"number of bytes reported by the input is copied unchanged, the same size is set on the output and "
	"Usine is told that the data changed, once when the module is created and again each time [pointer "
	"in] changes through an immediate callback.\n"
	"A pointer inlet accepts any pointer type, so the data type is checked first: anything else than "
	"float raises an error and copies nothing.\n"
	"A size of zero is allowed and only the change notification is sent.\n"
	"[pointer out] always declares float data, so it can feed any other pointer inlet expecting floats.\n"
	"The module is a simple module with processing disabled and the copy happens only in the callback "
	"(sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

inline const int ERROR_POINTER_NOT_FLOAT USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "the pointer is not type float");

constexpr NativeInt PTR_IN_CBID = 0x002300F0;

// PointerDataType can be any UINT32, see UsinePointerTypes for the types known by Usine
inline constexpr TParamInfo ptrInParamInfo {
	.ParamType       = ptPointer,
	.Caption         = "pointer in",
	.IsInput         = TRUE,
	.IsOutput        = FALSE,
	.CallBackType    = ctImmediate,
	.CallBackId      = PTR_IN_CBID,
	.PointerDataType = usinePtrFloat,
	.Description     = "Pointer to float data, copied to [pointer out] each time it changes.\n"
	                   "Another pointer type raises an error."
};

inline constexpr TParamInfo ptrOutParamInfo {
	.ParamType       = ptPointer,
	.Caption         = "pointer out",
	.IsInput         = FALSE,
	.IsOutput        = TRUE,
	.CallBackType    = ctNone,
	.PointerDataType = usinePtrFloat,
	.Description     = "Copy of the float data of [pointer in], same size."
};
