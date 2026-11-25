//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the MotionRecognizer module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "ML Motion Recognizer";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"records up to 8 movements on its data inputs and recognizes them in real time.\n"
	"[in 1], [in 2] and so on are read every [sampling period], 1 to 10000 ms (30 by default), and "
	"normalized by the mean and standard deviation of their last 100 values.\n"
	"[record] stores the normalized points of the movement chosen in [select movement], 2000 points max, "
	"and pauses recognition; the recording is added when [record] goes OFF.\n"
	"[undo] removes the last recording, [clear] all of them.\n"
	"[threshold] (0 to 1, 0.5 by default) sets the tolerance of a recognition comparing the last 32 "
	"points with the recordings by dynamic time warping in a background thread every 6 points.\n"
	"[detection out] gives the movement number or 0;\n"
	"[sample out 1] and the following arrays carry the last recording of the selected movement;\n"
	"[latency] gives the recognition time in ms.\n"
	"Recordings are saved in the patch";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "0.261125";

inline const int WARNING_NOTHING_TO_UNDO      USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "nothing to undo");
inline const int ERROR_CALLBACK_FAILED        USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unexpected error while handling a parameter change");
inline const int ERROR_UNKNOWN_CHUNK_VERSION  USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unknown version of the saved recordings, they were not restored: ");
inline const int WARNING_TOO_MANY_MOVEMENTS   USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "too many movements in the saved recordings, the extra ones are ignored: ");
inline const int ERROR_WRONG_NUMBER_OF_INPUTS USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "the saved recordings have another number of inputs, they were not restored: ");
inline const int ERROR_CORRUPTED_CHUNK        USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "saved recordings truncated or corrupted, the remaining ones were not restored");
inline const int WARNING_INVALID_SAVED_VALUES USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "invalid values (NaN or infinite) in the saved recordings were replaced by 0");
inline const int ERROR_SAVE_FAILED            USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unexpected error while saving the recordings, they were not saved");
inline const int ERROR_RESTORE_FAILED         USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unexpected error while restoring the saved recordings");
inline const int WARNING_EMPTY_RECORDING      USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "nothing recorded: [record] was ON for less than [sampling period], the recording is ignored");
inline const int WARNING_RECORDING_TRUNCATED  USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "recording truncated: 2000 points max");
inline const int WARNING_RECORDING_START_LOST USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "the start of a recording was lost: the previous recordings were not stored in time");
inline const int WARNING_RECORDING_LOST       USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "a recording was lost: the previous recordings were not stored in time");
inline const int ERROR_NO_RECOGNITION_THREAD  USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "the recognition thread could not be created, the movements are not recognized");
inline const int WARNING_IGNORED_RECORDINGS   USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "saved recordings with an invalid movement number were ignored: ");

constexpr NativeInt SELECT_MOVEMENT_LBOX_CBID = 0x00A01;
constexpr NativeInt UNDO_BTN_CBID             = 0x00A03;
constexpr NativeInt CLEAR_BTN_CBID            = 0x00A04;
// not a parameter: sent by the module itself when a recording ends (onProcess) and when the recordings are restored
// (onSetStateChunk, maybe out of the main thread); in the main thread it stores the finished recordings and draws [sample out]
constexpr NativeInt UPDATE_RECORDINGS_CBID    = 0x00A05;

// version written in the chunk, see onGetStateChunk
constexpr int CHUNK_VERSION = 2;

// query of the number of inputs, its maximum is DATA_INS_OUTS_MAX
constexpr int QUERY_DEFAULT_NB_INPUTS = 3;
constexpr int QUERY_MIN_NB_INPUTS     = 1;

// each input is normalized with the mean and the standard deviation of its last NORMALIZATION_WINDOW values
constexpr int        NORMALIZATION_WINDOW = 100;
// number of last points compared with the recordings
constexpr int        BATCH_SIZE           = 32;
// a recording stops growing after MAX_RECORDED_POINTS points
constexpr int        MAX_RECORDED_POINTS  = 2000;
// local outlier factor under which the last points are recognized, when [threshold] is 1
constexpr TPrecision LOF_THRESHOLD_MAX    = 4.0f;

// data inputs: the caption depends on the input, it is set in onCreateParameters
constexpr TParamInfo dataFdrInParamInfo {
	.ParamType    = ptDataField,
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Value of one axis of the motion, for example one axis of an accelerometer.\n"
	                "It is read every [sampling period], then normalized with the mean and the standard deviation of its last 100 values (of the values read so far, during the first 100 points).\n"
	                "A NaN or infinite value is replaced by the last valid one."
};

constexpr TParamInfo detectionFdrOutParamInfo {
	.ParamType        = ptDataField,
	.Caption          = "detection out",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.ReadOnly         = FALSE,
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "data out",
	.Description      = "Number of the movement recognized in the last 32 points, from 1 to 8, or 0 when they match no recorded movement.\n"
	                    "The recognition runs in the background every 6 points, and stops while [record] is ON.\n"
	                    "Its result is sent at the first point after its end, the next point when it lasts less than [sampling period]; a recognition is skipped when the previous one is still running."
};

constexpr TParamInfo samplingPeriodFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "sampling period",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 1.0f,
	.MaxValue     = 10000.0f,
	.DefaultValue = 30.0f,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Time between two points read on the inputs, in ms.\n"
	                "Range: [1..10000].\n"
	                "Each point is read in the audio bloc where its time falls, so the average period is exact; when it is shorter than an audio bloc, one point is read per bloc."
};

constexpr TParamInfo thresholdFdrInParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "threshold",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.5f,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Tolerance of the recognition.\n"
	                "Range: [0..1].\n"
	                "The last points are recognized when their local outlier factor among the recordings is below 4 x [threshold]: higher values recognize the movements more easily."
};

constexpr TParamInfo selectMovementLboxInParamInfo {
	.ParamType        = ptListBox,
	.Caption          = "select movement",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 0.0f,
	.ListBoxStrings   = "movement 1,movement 2,movement 3,movement 4,movement 5,movement 6,movement 7,movement 8",
	.ReadOnly         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "movement handling",
	.CallBackId       = SELECT_MOVEMENT_LBOX_CBID,
	.Description      = "Movement to which the recording is added when [record] is switched OFF.\n"
	                    "The last recording of this movement is sent to the [sample out] arrays."
};

constexpr TParamInfo recordSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "record",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "While ON, records the normalized points of the inputs (2000 points max) and pauses the recognition.\n"
	                "When switched OFF, the recording is added to the movement selected by [select movement], unless it has no point.\n"
	                "Read at each audio bloc."
};

constexpr TParamInfo undoBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "undo",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = UNDO_BTN_CBID,
	.Description  = "Removes the last recording made with [record], whatever its movement.\n"
	                "The recordings restored with the patch can't be undone."
};

constexpr TParamInfo clearBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "clear",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = CLEAR_BTN_CBID,
	.Description  = "Removes the recordings of all the movements.\n"
	                "Can't be undone."
};

// sample outputs, one per input: the caption is set in onCreateParameters
constexpr TParamInfo sampleArrOutParamInfo {
	.ParamType    = ptArray,
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.ReadOnly     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Last recording of the movement selected by [select movement], for the input of the same number: its normalized values, one per point."
};

constexpr TParamInfo latencyFdrOutParamInfo {
	.ParamType          = ptDataField,
	.Caption            = "latency",
	.IsInput            = FALSE,
	.IsOutput           = TRUE,
	.ReadOnly           = TRUE,
	.CallBackType       = ctNone,
	.IsVisibleByDefault = FALSE,
	.Description        = "Duration of the last recognition in the background thread, in ms."
};
