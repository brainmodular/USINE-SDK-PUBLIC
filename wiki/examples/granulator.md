# Granulator
#example #audio #drawing #mouse #automation #query #control #advanced

**Multi-channel granular synthesis engine with visual feedback.**

| | |
|---|---|
| **Module Name** | `Granulator` |
| **Type** | `mtControl` |
| **Color** | `clAudioModuleColor` |
| **Version** | 2.3 |
| **Source** | `examples/Granulator/` |

## Description

A fully-featured granular synthesis module with multiple envelope types, pitch shifting, circular audio buffer, multi-channel support, visual envelope display, and automation recording. One of the most complex examples in the SDK.

## Parameters

Dynamic count (N = number of audio channels):

| # | Name | Type | I/O | Range | Details |
|---|------|------|-----|-------|---------|
| 0..N-1 | `in 1/2...` | `ptAudio` | Input | — | Audio source |
| N..2N-1 | `out 1/2...` | `ptAudio` | Output | — | Granulated output |
| 2N | `trig out` | `ptTriggerLed` | Output | — | Grain trigger indicator |
| 2N+1 | `interval` | `ptDataFader` | Input | -1000 – 20000 ms | Time between grains (`scLog`) |
| 2N+2 | `grain env` | `ptListBox` | Input | — | ASR / Bezier / RCB / Hanning / Gaussian |
| 2N+3 | `env var` | `ptDataFader` | Input | — | Envelope shape variable |
| 2N+4 | `duration` | `ptDataFader` | Input | 0.05 – 2000 ms | Grain duration (`scLog`) |
| 2N+5 | `pitch` | `ptDataFader` | Input | -48 – +48 | Pitch shift in halftones |
| 2N+6 | `grain mode` | `ptListBox` | Input | — | Interval / Play Button |
| 2N+7 | `play` | `ptButton` | Input/Output | — | Manual grain trigger |

## How It Works

1. **Audio buffering**: Incoming audio is written to a circular buffer
2. **Grain scheduling**: In Interval mode, a countdown triggers new grains automatically. In Play mode, grains are triggered manually.
3. **Grain processing**: Each grain reads from the circular buffer with a pitch-dependent playback increment, applies an envelope, and accumulates to the output. Up to 16 simultaneous grains per channel.
4. **Envelope types**: ASR (linear), Bezier (curved), RCB (raised cosine bell), Hanning, Gaussian — each with a variable shape parameter.

## Key Concepts Demonstrated

- **Audio query** for multi-channel support
- **Sample-by-sample processing** in `onProcess` (not block-based)
- **Circular audio buffer** for grain source material
- **Multiple envelope types** with mathematical generation
- **Pitch shifting** via variable playback increment
- **Custom canvas drawing** (`onPaint`) for envelope visualization
- **Automation recording**: `sdkProcessRecordAutomation`, `sdkStopRecordAutomation`, `onSetRecordedValue`
- **`CanRecord = TRUE`** — automation recording support
- **Mouse interaction** for real-time control (Y = env var or pitch, X = duration)
- **`onSampleRateChange`** for buffer resizing

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onSampleRateChange` · `onSetRecordedValue`

## Related Examples

- [[examples/reverb|Reverb]] — Another complex audio processor
- [[examples/audio-volume|AudioVolume]] — Simpler multi-channel audio
- [[examples/simple-pad|SimplePad]] — Automation recording example

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
