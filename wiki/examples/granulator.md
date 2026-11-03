# Granulator
#example #audio #drawing #mouse #automation #query #control #errors #advanced

**Multi-channel granular synthesis with visual feedback.**

| | |
|---|---|
| **Module Name** | `granulator` |
| **Type** | `mtControl` |
| **Color** | `clAudioModuleColor` |
| **Version** | 2.3 |
| **Source** | `examples/Granulator/` |

## Description

A granular synthesis module with several envelope types, pitch shifting, a delay line per channel, a display of the envelope and automation recording. Inspired from the granulator of the Fragmental VST plugin by Niall Moody, whose original notice is kept in `Grain.h` and `Grain.cpp`. One of the most complex examples of the SDK.

## Files

| File | Contains |
|------|----------|
| `Granulator.h/.cpp` | The module |
| `Grain.h/.cpp` | A single grain: reads the delay line through an envelope |
| `Envelopes.h/.cpp` | The five envelope shapes, `EnvelopeType` and the `makeEnvelope` factory |
| `ModuleDefinition.h` | Module constants, messages, callback ids and parameters |

## Parameters

Created in `onCreateParameters`, N is the number of channels chosen in the query popup:

| # | Name | Member | Type | I/O | Details |
|---|------|--------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `audioInputs[]` | `ptAudio` | Input | Audio source |
| N..2N-1 | `out 1`, `out 2`, ... | `audioOutputs[]` | `ptAudio` | Output | Grains |
| 2N | `trig out` | `grainStartSwOut` | `ptTriggerLed` | Output | Blinks when a grain starts |
| 2N+1 | `interval` | `grainIntervalFdrIn` | `ptDataFader` | Input | -1000 – 20000 ms (`scLog`), a negative interval overlaps the grains |
| 2N+2 | `grain env` | `grainEnvelopeLboxIn` | `ptListBox` | Input | Linear ASR / Bezier / Raised Cosine Bell / Hanning / Gaussian, read as an `EnvelopeType` |
| 2N+3 | `env var` | `grainEnvVarFdrIn` | `ptDataFader` | Input | Shape of the envelope |
| 2N+4 | `duration` | `grainDurationFdrIn` | `ptDataFader` | Input | 0.05 – 2000 ms (`scLog`) |
| 2N+5 | `pitch` | `grainPitchFdrIn` | `ptDataFader` | Input | -48 – +48 halftones |
| 2N+6 | `grain mode` | `grainModeLboxIn` | `ptListBox` | Input | Interval / Play Button, read as a `GrainMode` |
| 2N+7 | `play` | `grainGoBtnIn` | `ptButton` | Input/Output | Starts a grain in Play Button mode |

All the inputs have a `ctImmediate` callback, their ids are named after the members (`GRAIN_PITCH_FDR_CBID`...).

## How It Works

1. **Delay line**: each input is written in a circular buffer, long enough for the longest grain read at the highest pitch. The buffers are `std::vector`, reallocated in `onSampleRateChange`.
2. **Grain scheduling**: in Interval mode, a countdown starts a grain every `duration + interval`. In Play Button mode, `play` starts a grain at the beginning of the next bloc.
3. **Grains**: each channel has `NUM_OF_GRAIN` grains, reused in a round robin. A grain reads the delay line with an increment depending on the pitch (cubic interpolation below -24 halftones, linear above), applies its envelope and adds its sample to the output. A grain pitched up starts earlier in the delay line so it never passes the write position.
4. **Panel**: `onPaint` draws the current envelope and writes the pitch and the duration.
5. **Mouse**: dragging in the panel changes the duration (horizontal), the envelope variable (vertical) or the pitch (Shift + vertical), and records these moves as an automation, replayed by `onSetRecordedValue`.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `ERROR_RECORD_AUTOMATION` | Error | `sdkProcessRecordAutomation` fails |
| `ERROR_STOP_RECORD_AUTOMATION` | Error | `sdkStopRecordAutomation` fails |
| `WARNING_GRAIN_OVERLAP` | Warn | A grain still playing is cut by a new one, raised once until the grains stop overlapping |

## Key Concepts Demonstrated

- **Audio query** for multi-channel support
- **Sample-by-sample processing** in `onProcess`
- **Circular buffer** as grain source
- **Pitch shifting** with a variable read increment and interpolation
- **Custom drawing** (`onPaint`) of the envelope
- **Automation recording**: `CanRecord = TRUE`, `sdkProcessRecordAutomation`, `sdkStopRecordAutomation`, `onSetRecordedValue`
- **`onSampleRateChange`** to resize the buffers and convert the durations in samples
- **Modern C++**: `std::vector` and `std::array` for the buffers and the grains, `std::unique_ptr` for the envelopes, `std::span` for the delay line and the curves, `enum class` for the list boxes
- **Factory function**: `makeEnvelope()` creates the envelope of a type, used by the grains and by the painted curves
- **Raising a warning once** instead of at every grain

## SDK Callbacks Used

`onGetModuleInfo` · `onAfterQuery` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onSampleRateChange` · `onSetRecordedValue`

## Related Examples

- [[examples/reverb|Reverb]] — Another complex audio processor
- [[examples/audio-volume|AudioVolume]] — Simpler multi-channel audio
- [[examples/simple-pad|SimplePad]] — Automation recording example

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
