//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the DatabasePlayer module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "Database Player";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"maps the samples of a Database Creation database on two audio descriptors and resynthesizes the ones "
	"under the cursor from their mean spectrum.\n"
	"[database file] is loaded in the background, the outputs are silent meanwhile or when [on "
	"resynthesis] is OFF.\n"
	"Each sample is placed on a 500 x 500 panel by [first dimension] and [second dimension]: 15 choices "
	"from duration to combined 1 and 2, the t-SNE axes, or random, normalized between their minimum and "
	"maximum.\n"
	"The circle at [cursor X], [cursor Y] and [cursor radius] selects samples, by mouse or [cursor "
	"down].\n"
	"A KD-tree search keeps up to [polyphony] samples, 1 to 20, faded by [attack time] and [release "
	"time].\n"
	"[audio out 1] and [audio out 2] play the gain weighted sum of their mean spectra with independent "
	"random phases, from a work thread.\n"
	"[samples list] and [samples coeffs] describe the 40 slots";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

inline const int ERROR_DATABASE_NOT_FOUND USINE_MODULE_LOCAL = RegisterError(Level::Error, "no database at: ");
inline const int ERROR_OPEN_DATABASE      USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to open the database: ");
inline const int ERROR_CLOSE_DATABASE     USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to close the database: ");
inline const int ERROR_LOAD_FAILED        USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to load the database, it is closed");
inline const int ERROR_NO_DATABASE_OPEN   USINE_MODULE_LOCAL = RegisterError(Level::Error, "no database open, open a database first");
inline const int ERROR_NO_FFT_SIZE        USINE_MODULE_LOCAL = RegisterError(Level::Error, "no FFT size in the FileFFTSize table of the database");
inline const int ERROR_READ_FFT_SIZE      USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to read the FFT size of the database: ");
inline const int ERROR_INVALID_FFT_SIZE   USINE_MODULE_LOCAL = RegisterError(Level::Error, "invalid FFT size in the database (a power of two from 32 to 65536 expected), analyze the sound folder again: ");
inline const int ERROR_CREATE_FFT         USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to create the FFT of the database, size: ");
inline const int ERROR_READ_SAMPLES       USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to read the samples of the database: ");
inline const int ERROR_NO_SAMPLES         USINE_MODULE_LOCAL = RegisterError(Level::Error, "no sample in the database, analyze the sound folder again");
inline const int ERROR_NO_MAGNITUDES      USINE_MODULE_LOCAL = RegisterError(Level::Error, "magnitudes missing in the database, create the database again");
inline const int WARNING_INVALID_MAGNITUDES USINE_MODULE_LOCAL = RegisterError(Level::Warn, "samples not played, their magnitudes do not match the FFT size of the database: ");
inline const int WARNING_NAN_MAGNITUDES     USINE_MODULE_LOCAL = RegisterError(Level::Warn, "samples with magnitudes that are not numbers (silent files?), replaced by 0: ");
inline const int ERROR_PREPARE_REQUEST    USINE_MODULE_LOCAL = RegisterError(Level::Error, "unable to prepare the request to the database: ");
inline const int ERROR_INVALID_SAMPLE_ID  USINE_MODULE_LOCAL = RegisterError(Level::Error, "invalid sample id in the result of the request: ");
inline const int ERROR_REQUEST_FAILED     USINE_MODULE_LOCAL = RegisterError(Level::Error, "the request to the database failed, the database is closed: open it again");
// exceptions caught in the tasks of the work thread, in the callbacks and in onPaint
inline const int ERROR_LOADING            USINE_MODULE_LOCAL = RegisterError(Level::Error, "error while loading the database: ");
inline const int ERROR_REQUEST            USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the request to the database: ");
inline const int ERROR_SEARCH             USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the search of the samples under the cursor: ");
inline const int ERROR_RESYNTHESIS        USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the resynthesis: ");
inline const int ERROR_WORK_THREAD        USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the work thread: ");
inline const int ERROR_SEARCH_THREAD      USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the search thread: ");
inline const int ERROR_CALLBACK           USINE_MODULE_LOCAL = RegisterError(Level::Error, "error in the callback with id: ");
inline const int ERROR_PAINT              USINE_MODULE_LOCAL = RegisterError(Level::Error, "error while drawing the panel: ");
inline const int ERROR_DELETE             USINE_MODULE_LOCAL = RegisterError(Level::Error, "error while deleting the module");

constexpr NativeInt DATABASE_FILE_CBID         = 0x00F200C0;
constexpr NativeInt RELOAD_DATABASE_BTN_CBID   = 0x00F200C1;
constexpr NativeInt FIRST_DIMENSION_LBOX_CBID  = 0x00F200C2;
constexpr NativeInt SECOND_DIMENSION_LBOX_CBID = 0x00F200C3;
constexpr NativeInt POLYPHONY_FDR_CBID         = 0x00F200C4;
constexpr NativeInt ATTACK_TIME_FDR_CBID       = 0x00F200C5;
constexpr NativeInt RELEASE_TIME_FDR_CBID      = 0x00F200C6;
constexpr NativeInt ON_RESYNTHESIS_SW_CBID     = 0x00F200C7;
constexpr NativeInt CURSOR_DOWN_SW_CBID        = 0x00F200C8;
constexpr NativeInt CURSOR_X_FDR_CBID          = 0x00F200C9;
constexpr NativeInt CURSOR_Y_FDR_CBID          = 0x00F200CA;
constexpr NativeInt CURSOR_RADIUS_FDR_CBID     = 0x00F200CB;
// settings: a change of the points radius or of a spread factor requests the positions of the samples again
constexpr NativeInt POINTS_RADIUS_SETTING_CBID    = 90;
constexpr NativeInt SPREAD_FACTOR_X_SETTING_CBID  = 91;
constexpr NativeInt SPREAD_FACTOR_Y_SETTING_CBID  = 92;
// the work thread reads a copy of the setting, made by onCallBack
constexpr NativeInt NORMALIZE_COEFFS_SETTING_CBID = 93;

// columns of the FileResults table, one per item of [first dimension] and [second dimension]
constexpr std::array<const char*, 15> FIRST_DIMENSION_COLUMNS {
	"duration",
	"spectralCentroid_mean",
	"spectralCentroid_std",
	"spectralSpread_mean",
	"spectralSpread_std",
	"spectralFlatness_mean",
	"spectralFlatness_std",
	"RMS_mean",
	"RMS_std",
	"f0_frequency_mean",
	"f0_frequency_std",
	"f0_probability_mean",
	"f0_probability_std",
	"tsne_x",
	"random"
};
constexpr std::array<const char*, 15> SECOND_DIMENSION_COLUMNS {
	"duration",
	"spectralCentroid_mean",
	"spectralCentroid_std",
	"spectralSpread_mean",
	"spectralSpread_std",
	"spectralFlatness_mean",
	"spectralFlatness_std",
	"RMS_mean",
	"RMS_std",
	"f0_frequency_mean",
	"f0_frequency_std",
	"f0_probability_mean",
	"f0_probability_std",
	"tsne_y",
	"random"
};
// default item of the dimension list boxes: the axes of the t-SNE map (combined 1 and combined 2)
constexpr float TSNE_DIMENSION_ITEM = 13.0f;

// the mouse wheel keeps the radius of the cursor in the range of [cursor radius]
constexpr float CURSOR_RADIUS_MIN = 1.0f;
constexpr float CURSOR_RADIUS_MAX = 20.0f;

inline constexpr TParamInfo databaseFileInParamInfo {
	.ParamType        = ptFileName,
	.Caption          = "database file",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "database",
	.CallBackId       = DATABASE_FILE_CBID,
	.Description      = "Database file made by the Database Creation module.\n"
	                    "It is loaded in the background with the mean spectrum of each sample and the FFT size of the analysis, the outputs are silent meanwhile.\n"
	                    "Then the samples are placed on the panel according to [first dimension] and [second dimension].\n"
	                    "A sample whose mean spectrum does not match the FFT size of the database is drawn but not played, with a warning."
};

inline constexpr TParamInfo reloadDatabaseBtnInParamInfo {
	.ParamType    = ptButton,
	.Caption      = "reload database",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNormal,
	.CallBackId   = RELOAD_DATABASE_BTN_CBID,
	.Description  = "Loads [database file] again, for example after the database has been created again.\n"
	                "Ignored, with an error message, when no database is open."
};

inline constexpr TParamInfo firstDimensionLboxInParamInfo {
	.ParamType        = ptListBox,
	.Caption          = "first dimension",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = TSNE_DIMENSION_ITEM,
	.ListBoxStrings   = "duration,spectral centroid mean,spectral centroid std,spectral spread mean,spectral spread std,spectral flatness mean,spectral flatness std,RMS mean,RMS std,fund frequency mean,fund frequency std,frequency probability mean,frequency probability std,combined 1,random",
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "2D representation",
	.CallBackId       = FIRST_DIMENSION_LBOX_CBID,
	.Description      = "Descriptor of the horizontal axis: its values are normalized between their minimum and maximum in the database, then raised to the power of the spread factor X setting.\n"
	                    "Combined 1 is the X axis of the t-SNE map computed by Database Creation, and random places the samples at random."
};

inline constexpr TParamInfo secondDimensionLboxInParamInfo {
	.ParamType      = ptListBox,
	.Caption        = "second dimension",
	.IsInput        = TRUE,
	.IsOutput       = FALSE,
	.DefaultValue   = TSNE_DIMENSION_ITEM,
	.ListBoxStrings = "duration,spectral centroid mean,spectral centroid std,spectral spread mean,spectral spread std,spectral flatness mean,spectral flatness std,RMS mean,RMS std,frequency mean,frequency std,frequency probability mean,frequency probability std,combined 2,random",
	.CallBackType   = ctNormal,
	.CallBackId     = SECOND_DIMENSION_LBOX_CBID,
	.Description    = "Descriptor of the vertical axis, high values at the top, normalized like [first dimension] with the spread factor Y setting.\n"
	                  "Combined 2 is the Y axis of the t-SNE map computed by Database Creation, and random places the samples at random."
};

inline constexpr TParamInfo polyphonyFdrInParamInfo {
	.ParamType        = ptDataFader,
	.Caption          = "polyphony",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = 1.0f,
	.MaxValue         = 20.0f,
	.DefaultValue     = 10.0f,
	.Format           = "%.0f",
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "closest samples",
	.CallBackId       = POLYPHONY_FDR_CBID,
	.Description      = "Maximum number of samples played at the same time, chosen among the samples inside the cursor circle at regular steps from the closest to the farthest.\n"
	                    "Up to twice as many sound while the released ones fade out.\n"
	                    "Lowering it moves the samples of the slots above 2 x [polyphony] to the free slots below with their gain, and stops those finding no free slot.\n"
	                    "Range: [1..20]."
};

inline constexpr TParamInfo samplesListTxtOutParamInfo {
	.ParamType    = ptTextField,
	.Caption      = "samples list",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Paths of the samples in the 2 x [polyphony] slots of the player, as a comma text of quoted items: the samples playing or fading out, and an empty item for each free slot."
};

inline constexpr TParamInfo samplesCoeffsArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "samples coeffs",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Current gain of each slot of the player in the order of [samples list], always 40 values.\n"
	                "The target gain of a sample is 1/(number of selected samples) when the normalize coeffs setting is ON, else its squared closeness to the cursor center (1 at the center, 0 on the circle)."
};

inline constexpr TParamInfo attackTimeFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "attack time",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scLog,
	.MinValue     = 0.0f,
	.MaxValue     = 2000.0f,
	.DefaultValue = 50.0f,
	.Symbol       = "ms",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = ATTACK_TIME_FDR_CBID,
	.Description  = "Time for the gain of a sample to rise to a higher target, in ms: the samples entering the selection fade in over this time."
};

inline constexpr TParamInfo releaseTimeFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "release time",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scLog,
	.MinValue     = 0.0f,
	.MaxValue     = 2000.0f,
	.DefaultValue = 200.0f,
	.Symbol       = "ms",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = RELEASE_TIME_FDR_CBID,
	.Description  = "Time for the gain of a sample to fall to a lower target, in ms: the samples leaving the cursor circle fade out to 0 over this time."
};

inline constexpr TParamInfo onResynthesisSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "on resynthesis",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 1.0f,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "resynthesis",
	.CallBackId       = ON_RESYNTHESIS_SW_CBID,
	.Description      = "Turns the resynthesis ON or OFF.\n"
	                    "When ON, the audio outputs play the mean spectra of the selected samples weighted by [samples coeffs], with random phases (overlap-add of inverse FFTs computed in the background, a bloc and 10 to 50 ms ahead of the outputs).\n"
	                    "When OFF, the outputs are silent and [samples list] and [samples coeffs] keep updating."
};

// the two outputs play the same spectrum with independent random phases
inline constexpr TParamInfo audioOut1ParamInfo {
	.ParamType    = ptAudio,
	.Caption      = "audio out 1",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Resynthesis of the selected samples, see [on resynthesis].\n"
	                "Its random phases differ from the ones of [audio out 2]: the two outputs make a decorrelated pair."
};

inline constexpr TParamInfo audioOut2ParamInfo {
	.ParamType    = ptAudio,
	.Caption      = "audio out 2",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.CallBackType = ctNone,
	.Description  = "Same resynthesis as [audio out 1] with other random phases: the two outputs make a decorrelated pair."
};

inline constexpr TParamInfo cursorDownSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "cursor down",
	.IsInput          = TRUE,
	.IsOutput         = TRUE,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "controls",
	.CallBackId       = CURSOR_DOWN_SW_CBID,
	.Description      = "When ON, the cursor circle at [cursor X], [cursor Y] selects the samples to play, like the left mouse button held down on the panel.\n"
	                    "When OFF and the mouse button is up, the playing samples fade out over [release time]."
};

inline constexpr TParamInfo cursorXFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "cursor X",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = CURSOR_X_FDR_CBID,
	.Description  = "Horizontal position of the cursor center on the panel.\n"
	                "Range: [0..1]. 0=left, 1=right.\n"
	                "Ignored while the left mouse button is held down on the panel: the mouse moves the cursor."
};

inline constexpr TParamInfo cursorYFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "cursor Y",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = CURSOR_Y_FDR_CBID,
	.Description  = "Vertical position of the cursor center on the panel.\n"
	                "Range: [0..1]. 0=top, 1=bottom.\n"
	                "Ignored while the left mouse button is held down on the panel: the mouse moves the cursor."
};

inline constexpr TParamInfo cursorRadiusFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "cursor radius",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = CURSOR_RADIUS_MIN,
	.MaxValue     = CURSOR_RADIUS_MAX,
	.DefaultValue = 5.0f,
	.Symbol       = "pix",
	.Format       = "%.0f",
	.CallBackType = ctNormal,
	.CallBackId   = CURSOR_RADIUS_FDR_CBID,
	.Description  = "Radius of the cursor circle in percent of the panel size, despite its pix symbol: the samples inside the circle are played.\n"
	                "Range: [1..20].\n"
	                "While the cursor is active, the mouse wheel also changes the radius, without moving this fader."
};
