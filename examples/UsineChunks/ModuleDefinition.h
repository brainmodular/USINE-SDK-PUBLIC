//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the UsineChunks module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2025-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"
#include <array>

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "usine chunks";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "shares data with other modules through a Usine chunk (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// the chunk and its items, shared by every module that uses the same names
constexpr AnsiCharPtr CHUNK_NAME            = "mychunk";
constexpr AnsiCharPtr ITEM_FLOAT_NAME       = "item1";
constexpr AnsiCharPtr ITEM_TEXT_NAME        = "commatxt";
constexpr AnsiCharPtr DATA_BULK_ITEM_NAME   = "data_bulk";
constexpr AnsiCharPtr DATA_BULK_ITEM_NAME_COPY   = "data_bulk_from_module";
constexpr AnsiCharPtr DEFAULT_COMMA_TEXT    = "value1,value2,value3,value4";

inline const int WARNING_CHUNK_DESTROYED  = RegisterError(Level::Warn, "the chunk has been destroyed: ");
inline const int INFO_CHUNK_ITEMS_CHANGED = RegisterError(Level::Info, "the item list of the chunk has changed: ");

constexpr NativeInt SET_ITEM_ARR_CBID        = 0x00000AC0;
constexpr NativeInt SET_COMMA_TXT_CBID       = 0x00000AC1;
constexpr NativeInt GET_CHUNK_DATA_BTN_CBID  = 0x00000FF6;
constexpr NativeInt SET_CHUNK_DATA_BTN_CBID  = 0x00000FF7;
// not parameters: ids given to sdkChunkSubscribe, sent back to onCallBack when the chunk changes
constexpr NativeInt CHUNK_SUB_CBID           = 0x00000FF0;
constexpr NativeInt CHUNK_SUB_ITEM_CBID      = 0x00000FF1;
constexpr NativeInt CHUNK_SUB_TEXT_CBID      = 0x00000FF2;



inline constexpr TParamInfo setItemArrInParamInfo {
	.ParamType        = ptArray,
	.Caption          = "set item1",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctImmediate,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "",
	.CallBackId       = SET_ITEM_ARR_CBID
};

inline constexpr TParamInfo setCommaTxtInParamInfo {
	.ParamType        = ptText,
	.Caption          = "commatxt idx 1",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctImmediate,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "",
	.CallBackId       = SET_COMMA_TXT_CBID
};

inline constexpr TParamInfo getItemArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "get item1",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone
};

inline constexpr TParamInfo getCommaTxtOutParamInfo {
	.ParamType    = ptText,
	.Caption      = "get commatxt",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.DontSave     = TRUE
};

inline constexpr TParamInfo getChunkDataBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "read from chunk",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = GET_CHUNK_DATA_BTN_CBID
};

inline constexpr TParamInfo setChunkDataBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "push to chunk",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = SET_CHUNK_DATA_BTN_CBID
};
