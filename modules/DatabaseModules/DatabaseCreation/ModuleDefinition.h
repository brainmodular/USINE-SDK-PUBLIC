//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the DatabaseCreation module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "Database Creation";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"analyzes the audio files of a folder and writes their descriptors and t-SNE map coordinates to an "
	"SQLite database for the Database Player.\n"
	"[sounds folder] is scanned recursively, subfolders included: .wav, .aif and .aiff files are "
	"analyzed, hidden files ignored.\n"
	"[max file duration] skips longer files, range 100..60000 ms, default 2000.\n"
	"[start analysis] runs in a background thread with the Usine progress bar, refused while one is "
	"running: frames of 2048 samples give spectral centroid, spread, flatness, RMS, loudness, MFCC and "
	"pYIN pitch as mean and standard deviation, plus a mean magnitude spectrum at [fft size], then "
	"Z-score statistics and t-SNE coordinates; below 4 files the samples are placed on a diagonal.\n"
	"[database folder] and [database name] give the .usine-analysis file, a previous one is deleted at "
	"start.\n"
	"The files are committed every 50, so [stop analysis] keeps the partial results.\n"
	"No audio or data output";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

// parameters
inline const int ERROR_INVALID_SOUNDS_FOLDER   USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "invalid sounds folder, it must exist and hold .wav, .aif or .aiff files: ");
inline const int ERROR_INVALID_DATABASE_FOLDER USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "invalid database folder: ");
inline const int ERROR_NO_DATABASE_NAME        USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "empty database name, the analysis is not started");
inline const int ERROR_ALREADY_PROCESSING      USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "an analysis is already running");
// scan of the sounds folder
inline const int ERROR_SCAN_FOLDER             USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to scan the folder: ");
inline const int ERROR_LIST_FILE               USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to add the file to the analysis: ");
inline const int ERROR_UNREPRESENTABLE_NAMES   USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "files skipped, Usine can not open their names (characters outside the ANSI code page): ");
// creation of the database
inline const int ERROR_DELETE_DATABASE         USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to delete the previous database: ");
inline const int INFO_DATABASE_DELETED         USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "previous database deleted: ");
inline const int ERROR_CREATE_DATABASE         USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to create the database: ");
inline const int ERROR_CREATE_TABLE            USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to create a table: ");
inline const int ERROR_PREPARE_INSERT          USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to prepare an insert: ");
inline const int ERROR_INSERT_FFT_SIZE         USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to insert the fft size: ");
// analysis of the files
inline const int INFO_FILE_LOADED              USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "file loaded: ");
inline const int ERROR_EMPTY_FILE              USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "empty file, skipped: ");
inline const int ERROR_FILE_TOO_LONG           USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "file longer than [max file duration], skipped: ");
inline const int ERROR_NO_SAMPLES              USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to read the samples, file skipped: ");
inline const int ERROR_INSERT_FILE             USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to insert or update the results of a file: ");
inline const int ERROR_STORE_FILE              USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to store the results in the database, the analysis stops at: ");
inline const int INFO_ANALYSIS_STOPPED         USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "analysis stopped");
// statistics of the features, used for the Z-score normalization
inline const int ERROR_BEGIN_TRANSACTION       USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to begin a transaction: ");
inline const int ERROR_COMMIT_TRANSACTION      USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to commit a transaction: ");
inline const int ERROR_PREPARE_FEATURE_QUERY   USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to prepare the query of a feature: ");
inline const int ERROR_PREPARE_STATS_INSERT    USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to prepare the insert of the feature statistics: ");
inline const int ERROR_INSERT_FEATURE_STATS    USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to insert the statistics of a feature: ");
inline const int ERROR_FEATURE_STATS           USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to compute and store the statistics of the features: ");
inline const int ERROR_READ_DATABASE           USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to read the database: ");
// t-SNE map of the files (the combined dimensions)
inline const int INFO_TSNE_STARTED             USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "finishing the analysis (t-SNE)...");
inline const int INFO_TSNE_STEP                USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "t-SNE step: ");
inline const int INFO_TSNE_TOO_FEW_FILES       USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "no t-SNE below 4 files, the files are placed on the diagonal of the map: ");
inline const int ERROR_PREPARE_TSNE_QUERY      USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to prepare the t-SNE query: ");
inline const int ERROR_TSNE_DIMENSIONS         USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "invalid t-SNE data dimensions: ");
inline const int INFO_TSNE_STOPPED             USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "analysis stopped during the t-SNE computation, start it again to compute the combined dimensions");
inline const int ERROR_TSNE                    USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "error during the t-SNE computation of the combined dimensions");
inline const int ERROR_STORE_TSNE              USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to store the t-SNE results in the database");
inline const int ERROR_TSNE_SIZE               USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "the number of t-SNE results does not match the number of files");
inline const int ERROR_PREPARE_TSNE_UPDATE     USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to prepare the update of the t-SNE coordinates: ");
inline const int ERROR_UPDATE_TSNE             USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to update the t-SNE coordinates: ");
// end of the analysis
inline const int ERROR_CLOSE_DATABASE          USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "unable to close the database at the end of the analysis");
inline const int INFO_ANALYSIS_COMPLETED       USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "analysis completed: ");

constexpr NativeInt SOUNDS_DIR_CBID         = 0x00F100B0;
constexpr NativeInt DATABASE_DIR_CBID       = 0x00F100B1;
constexpr NativeInt START_ANALYSIS_BTN_CBID = 0x00F100B3;
constexpr NativeInt STOP_ANALYSIS_BTN_CBID  = 0x00F100B4;

// added to [database name] to make the file name of the database
constexpr const char* DATABASE_EXTENSION = ".usine-analysis";
// iterations of the t-SNE, shown by the progress traces
constexpr int TSNE_MAX_ITERATIONS = 1000;
// the t-SNE needs a perplexity of at least 1, it is (files - 1) / 3 below 91 files
constexpr int TSNE_MIN_FILES = 4;
// the inserted files are committed by groups: one transaction per file is slow on slow disks
constexpr int FILES_PER_TRANSACTION = 50;

inline constexpr TParamInfo soundsDirInParamInfo {
	.ParamType        = ptChooseFolder,
	.Caption          = "sounds folder",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Create a database",
	.CallBackId       = SOUNDS_DIR_CBID,
	.Description      = "Folder of the sounds to analyze, subfolders included: its .wav, .aif and .aiff files are analyzed, hidden files are ignored.\n"
	                    "[start analysis] is refused while it holds no audio file."
};

inline constexpr TParamInfo databaseDirInParamInfo {
	.ParamType    = ptChooseFolder,
	.Caption      = "database folder",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = DATABASE_DIR_CBID,
	.Description  = "Folder where the database is saved, as [database name].usine-analysis.\n"
	                "A previous database of the same name is deleted when the analysis starts."
};

inline constexpr TParamInfo databaseNameTxtInParamInfo {
	.ParamType    = ptTextField,
	.Caption      = "database name",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Name of the database file, without extension: .usine-analysis is added.\n"
	                "[start analysis] is refused while it is empty."
};

inline constexpr TParamInfo maxFileDurationFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "max file duration",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.Scale            = scLog,
	.MinValue         = 100.0f,
	.MaxValue         = 60000.0f,
	.DefaultValue     = 2000.0f,
	.Symbol           = "ms",
	.Format           = "%.0f",
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Parameters",
	.Description      = "Longest file analyzed, in ms: the longer files are skipped.\n"
	                    "Range: [100..60000]."
};

inline constexpr TParamInfo fftSizeLboxInParamInfo {
	.ParamType      = ptListBox,
	.Caption        = "fft size",
	.IsInput        = TRUE,
	.IsOutput       = FALSE,
	.MinValue       = 0.0f,
	.MaxValue       = 4.0f,
	.DefaultValue   = 2.0f,
	.ListBoxStrings = "1024,2048,4096,8192,16384",
	.CallBackType   = ctNone,
	.Description    = "Size of the FFT of the mean magnitude spectrum stored for each file, used by the Database Player to resynthesize the sounds.\n"
	                  "The audio descriptors always use frames of 2048 samples."
};

inline constexpr TParamInfo startAnalysisBtnInParamInfo {
	.ParamType        = ptButton,
	.Caption          = "start analysis",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Analysis",
	.CallBackId       = START_ANALYSIS_BTN_CBID,
	.Description      = "Starts the analysis in a background thread, with its progress in the Usine progress bar: the descriptors of each file, then the t-SNE map coordinates, are written to the database.\n"
	                    "Refused while an analysis is running."
};

inline constexpr TParamInfo stopAnalysisBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "stop analysis",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = STOP_ANALYSIS_BTN_CBID,
	.Description  = "Stops the analysis after the current file, or during the t-SNE computation: the database keeps the files already analyzed, without t-SNE coordinates."
};
