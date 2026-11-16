//-----------------------------------------------------------------------------
// @file    ModuleDefinition.h
// @author  BrainModular team
// @brief   Module constants, messages, callback ids and parameters of the AudioAnalysis module.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <limits>
#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "Audio Analysis";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC =
	"analyzes [in] and outputs audio descriptors of the last window: [estimated frequency] and [frequency "
	"score] from a YIN pitch detector, the frequency only updating between 40 and 3000 Hz, [spectral "
	"centroid], [spectral spread], [spectral flatness], [RMS] in dB, [total loudness] and [loudness per "
	"bands], an array of 25 Bark bands, plus [latency], time between two analyses.\n"
	"A popup at creation offers basic or advanced analysis, the latter adding spectral, harmonic and "
	"perceptual outputs: skewness, kurtosis, slope, rolloff with [rolloff threshold], crest, tonality, "
	"decrease, entropy, peaks frequencies and amplitudes, [MFCC], inharmonicity, odd to even ratio, "
	"[tristimulus] and [sharpness].\n"
	"[fft size] selects the window from 1024 to 8192 samples, 4096 by default, and clears the input when "
	"changed;\n"
	"[overlap] gives 1, 2 or 4 analyses per window.\n"
	"The analysis runs in a background thread, one at a time;\n"
	"[on analysis] and the section switches freeze the outputs, keeping their last values";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.261112";

inline const int ERROR_CALLBACK USINE_MODULE_LOCAL = RegisterError(Level::Error, "error while handling a parameter change");

constexpr NativeInt ON_ANALYSIS_SW_CBID        = 0x001000F0;
constexpr NativeInt FFT_SIZE_LBOX_CBID         = 0x001000F1;
constexpr NativeInt OVERLAP_LBOX_CBID          = 0x001000F2;
constexpr NativeInt ON_SPECTRAL_SW_CBID        = 0x001000F3;
constexpr NativeInt ROLLOFF_THRESHOLD_FDR_CBID = 0x001000F4;
constexpr NativeInt ON_HARMONIC_SW_CBID        = 0x001000F5;
constexpr NativeInt ON_TEMPORAL_SW_CBID        = 0x001000F6;
constexpr NativeInt ON_PERCEPTUAL_SW_CBID      = 0x001000F7;

// the estimated frequency output is only updated when the estimate is in this range, in Hz
constexpr float ESTIMATED_FREQUENCY_MIN = 40.0f;
constexpr float ESTIMATED_FREQUENCY_MAX = 3000.0f;
constexpr float ROLLOFF_THRESHOLD_MIN   = 0.5f;
// ln(4096), about the entropy of a flat spectrum at an [fft size] of 8192 (ln(4097) bins): top of the displayed range
constexpr float SPECTRAL_ENTROPY_MAX    = 8.317766f;
// number of values of the tristimulus output
constexpr int   NUM_TRISTIMULUS         = 3;

inline constexpr TParamInfo onAnalysisSwInParamInfo {
	.ParamType    = ptSwitch,
	.Caption      = "on analysis",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.DefaultValue = 1.0f,
	.CallBackType = ctNormal,
	.CallBackId   = ON_ANALYSIS_SW_CBID,
	.Description  = "Turns the analysis on or off.\n"
	                "When OFF, no new analysis is started and the outputs keep their last values."
};

inline constexpr TParamInfo audioInParamInfo {
	.ParamType    = ptAudio,
	.Caption      = "in",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.CallBackType = ctNone,
	.Description  = "Audio input to analyze.\n"
	                "Windows of [fft size] samples are analyzed in a background thread, a new one every [fft size]/[overlap] samples."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo estimatedFrequencyFdrOutParamInfo {
	.ParamType        = ptDataField,
	.Caption          = "estimated frequency",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.MinValue         = 0.0f,
	.DefaultValue     = 0.0f,
	.Symbol           = "Hz",
	.Format           = "%.2f",
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Frequency Estimation",
	.Description      = "Fundamental frequency estimated by the probabilistic YIN algorithm on the analyzed window, in Hz.\n"
	                    "A period must fit twice in the window: from 2 x sample rate / [fft size] Hz (86 Hz for 1024 at 44.1 kHz).\n"
	                    "Only updated when the estimate is between 40 and 3000 Hz, else it keeps its last value."
};

inline constexpr TParamInfo frequencyScoreFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "frequency score",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Confidence of the YIN pitch estimate.\n"
	                "Range: [0..1].\n"
	                "About 0.01 when no period is found (noise, silence, too low a pitch).\n"
	                "Updated at each analysis, even when [estimated frequency] is not."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo spectralCentroidFdrOutParamInfo {
	.ParamType        = ptDataField,
	.Caption          = "spectral centroid",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.MinValue         = 0.0f,
	.DefaultValue     = 0.0f,
	.Symbol           = "Hz",
	.Format           = "%.2f",
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Spectral Metrics",
	.Description      = "Center of gravity of the power spectrum, in Hz.\n"
	                    "Higher values mean a brighter sound."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo spectralSpreadFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral spread",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "Hz",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Standard deviation of the power spectrum around the [spectral centroid], in Hz.\n"
	                "Low for a pure tone, high for a noise."
};

inline constexpr TParamInfo spectralFlatnessFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral flatness",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Geometric mean of the power spectrum divided by its arithmetic mean.\n"
	                "Range: [0..1].\n"
	                "Close to 0 for a tonal sound (below 0.000001 for a pure tone), about 0.56 for a white noise, 1 for a perfectly flat spectrum."
};

inline constexpr TParamInfo rmsFdrOutParamInfo {
	.ParamType        = ptDataField,
	.Caption          = "RMS",
	.IsInput          = FALSE,
	.IsOutput         = TRUE,
	.MinValue         = -std::numeric_limits<float>::infinity(),
	.MaxValue         = 12.0f,
	.DefaultValue     = 0.0f,
	.Symbol           = "dB",
	.Format           = "%.2f",
	.CallBackType     = ctNone,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Level Metrics",
	.Description      = "RMS level of the analyzed window, in dB.\n"
	                    "A silence gives -240 dB."
};

inline constexpr TParamInfo totalLoudnessFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "total loudness",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Sum of the [loudness per bands] divided by 10.\n"
	                "The spectrum is normalized first: the value follows the spectral balance of the sound, not its level."
};

inline constexpr TParamInfo loudnessPerBandsArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "loudness per bands",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Array of 25 values, one per Bark band from 0 to 20.5 kHz: the share of the spectrum energy in the band, raised to the power 0.23.\n"
	                "Range: [0..1].\n"
	                "The bands above the Nyquist frequency (sample rate / 2) are 0."
};

inline constexpr TParamInfo fftSizeLboxInParamInfo {
	.ParamType        = ptListBox,
	.Caption          = "fft size",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.MinValue         = 0.0f,
	.MaxValue         = 3.0f,
	.DefaultValue     = 2.0f,
	.ListBoxStrings   = "1024,2048,4096,8192",
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Analysis Parameters",
	.CallBackId       = FFT_SIZE_LBOX_CBID,
	.Description      = "Size of the analyzed window, in samples: larger sizes give a finer frequency resolution and a slower response.\n"
	                    "A change clears the input buffer."
};

inline constexpr TParamInfo overlapLboxInParamInfo {
	.ParamType      = ptListBox,
	.Caption        = "overlap",
	.IsInput        = TRUE,
	.IsOutput       = FALSE,
	.MinValue       = 0.0f,
	.MaxValue       = 2.0f,
	.DefaultValue   = 0.0f,
	.ListBoxStrings = "1,2,4",
	.CallBackType   = ctNormal,
	.CallBackId     = OVERLAP_LBOX_CBID,
	.Description    = "Number of analyses per window: 1 analyzes every [fft size] samples, 2 every half window, 4 every quarter window."
};

inline constexpr TParamInfo latencyFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "latency",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1000.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "ms",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Time elapsed between the last two analyses of this module, in ms."
};

inline constexpr TParamInfo onSpectralSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "on spectral",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 1.0f,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Spectral",
	.CallBackId       = ON_SPECTRAL_SW_CBID,
	.Description      = "Turns on the spectral descriptors: [spectral centroid], [spectral spread], [spectral flatness] and the outputs of this section.\n"
	                    "When OFF, they keep their last values."
};

inline constexpr TParamInfo spectralSkewnessFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral skewness",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -100.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Asymmetry of the power spectrum around the [spectral centroid]: 0 for a symmetric spectrum, positive when the energy lies below the centroid with a tail towards the high frequencies."
};

inline constexpr TParamInfo spectralKurtosisFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral kurtosis",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -100.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Peakedness of the power spectrum around the [spectral centroid]: about 3 for a gaussian shape, higher for a peaky spectrum, lower for a flat one."
};

inline constexpr TParamInfo spectralSlopeFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral slope",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 10.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "1/Hz",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Slope of the linear regression of the power spectrum over the frequency.\n"
	                "Usually negative, the energy decreasing towards the high frequencies."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo spectralRolloffFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral rolloff",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "Hz",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Frequency below which lies the part of the spectrum energy set by [rolloff threshold], in Hz."
};

inline constexpr TParamInfo rolloffThresholdFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "rolloff threshold",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = ROLLOFF_THRESHOLD_MIN,
	.MaxValue     = 0.99f,
	.DefaultValue = 0.95f,
	.Symbol       = "%",
	.Format       = "%.2f",
	.CallBackType = ctNormal,
	.CallBackId   = ROLLOFF_THRESHOLD_FDR_CBID,
	.Description  = "Part of the spectrum energy looked for by [spectral rolloff], as a fraction.\n"
	                "Range: [0.5..0.99]. 0.95 means 95 percent."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo spectralCrestFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral crest",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Highest bin of the power spectrum divided by the mean of the bins: high for a tonal sound, low for a noise."
};

inline constexpr TParamInfo tonalityCoeffFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "tonality coeff",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "%",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Tonality coefficient: the [spectral flatness] in dB divided by -60, limited to 1.\n"
	                "Range: [0..1]. 0 for a noise, 1 for a pure tone."
};

inline constexpr TParamInfo spectralDecreaseFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral decrease",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Symbol       = "1/Hz",
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Decrease of the spectrum from its first bin, emphasizing the low frequencies: 1 when the energy is in the lowest bin, smaller as it moves towards the high frequencies."
};

inline constexpr TParamInfo spectralEntropyFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "spectral entropy",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = SPECTRAL_ENTROPY_MAX,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Shannon entropy of the normalized power spectrum, in nats (natural logarithm): 0 when the energy is in a single bin, up to ln(number of bins) for a flat spectrum (white noise): 6.2, 6.9, 7.6 and 8.3 for an [fft size] of 1024, 2048, 4096 and 8192."
};

inline constexpr TParamInfo peaksAmplitudesArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "peaks amplitudes",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Normalized power of the spectral peaks: the local maxima holding more than 1 percent of the spectrum energy.\n"
	                "Same order as [peaks frequencies]."
};

// MaxValue: the Nyquist frequency, set by onCreateParameters
inline constexpr TParamInfo peaksFrequenciesArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "peaks frequencies",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Frequencies of the spectral peaks, in Hz, in increasing order.\n"
	                "Same order as [peaks amplitudes]."
};

inline constexpr TParamInfo onHarmonicSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "on harmonic",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 1.0f,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Harmonic",
	.CallBackId       = ON_HARMONIC_SW_CBID,
	.Description      = "Turns on the harmonic descriptors: [estimated frequency], [frequency score] and the outputs of this section.\n"
	                    "When OFF, they keep their last values."
};

inline constexpr TParamInfo mfccArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "MFCC",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -5.0f,
	.MaxValue     = 10.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "13 mel-frequency cepstral coefficients, a compact description of the spectral envelope (timbre): the DCT of the log amplitudes of 30 mel bands from 0 Hz to the Nyquist frequency, with liftering."
};

inline constexpr TParamInfo inharmonicityFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "inharmonicity",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Deviation of the spectral peaks from the harmonics of the estimated fundamental, weighted by their power.\n"
	                "Range: [0..1]. 0 for a harmonic sound, or when no fundamental is found."
};

inline constexpr TParamInfo oddToEvenRatioFdrOutParamInfo {
	.ParamType    = ptDataField,
	.Caption      = "odd to even ratio",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = -1.0f,
	.MaxValue     = 1.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Balance between the odd and the even harmonics, from the power of the spectral peaks on harmonics 2 and above.\n"
	                "Range: [-1..1]. 1 = odd harmonics only, -1 = even harmonics only, 0 when no fundamental is found."
};

inline constexpr TParamInfo tristimulusArrOutParamInfo {
	.ParamType    = ptArray,
	.Caption      = "tristimulus",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.1f",
	.CallBackType = ctNone,
	.Description  = "Tristimulus after Pollard and Jansson, 3 values between 0 and 1: the share of the fundamental, of the harmonics 2 to 4 and of the harmonics 5 and above in the sum of the amplitudes of the harmonics 1 to 20.\n"
	                "0 when no fundamental is found."
};

inline constexpr TParamInfo onTemporalSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "on temporal",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 1.0f,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Temporal",
	.CallBackId       = ON_TEMPORAL_SW_CBID,
	.Description      = "Turns on the computation of [RMS].\n"
	                    "When OFF, it keeps its last value."
};

inline constexpr TParamInfo onPerceptualSwInParamInfo {
	.ParamType        = ptSwitch,
	.Caption          = "on perceptual",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.DefaultValue     = 1.0f,
	.CallBackType     = ctNormal,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "Perceptual",
	.CallBackId       = ON_PERCEPTUAL_SW_CBID,
	.Description      = "Turns on the perceptual descriptors: [total loudness], [loudness per bands] and [sharpness].\n"
	                    "When OFF, they keep their last values."
};

inline constexpr TParamInfo sharpnessFdrOutParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "sharpness",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.MinValue     = 0.0f,
	.MaxValue     = 100.0f,
	.DefaultValue = 0.0f,
	.Format       = "%.2f",
	.CallBackType = ctNone,
	.Description  = "Sharpness after Zwicker: weighted centroid of the [loudness per bands], the high bands weighing more.\n"
	                "Higher for a brighter sound."
};
