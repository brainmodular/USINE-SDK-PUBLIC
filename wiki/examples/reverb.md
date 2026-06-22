# Reverb
#example #audio #query #advanced

**Freeverb-based multi-channel reverb processor.**

| | |
|---|---|
| **Module Name** | `Reverb` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Version** | 1.1 |
| **Source** | `examples/Reverb/` |

## Description

A full reverb effect based on the Freeverb algorithm. Features room size, damping, channel separation, wet/dry mix, and freeze mode. Supports multi-channel audio via the query system.

## Parameters

Dynamic count (N = number of channels):

| # | Name | Type | I/O | Range | Details |
|---|------|------|-----|-------|---------|
| 0..N-1 | `in 1/2...` | `ptAudio` | Input | — | Audio inputs |
| N..2N-1 | `out 1/2...` | `ptAudio` | Output | — | Audio outputs |
| 2N | `freeze` | `ptSwitch` | Input | — | Freeze reverb tail |
| 2N+1 | `room size` | `ptDataFader` | Input | 0.1 – 100% | Room size (`scExp`) |
| 2N+2 | `damping` | `ptDataFader` | Input | 0 – 100% | High frequency damping |
| 2N+3 | `channels separation` | `ptDataFader` | Input | 0 – 100% | Stereo width / channel spread |
| 2N+4 | `wet level` | `ptGainFader` | Input | — | Wet signal level (dB) |
| 2N+5 | `dry level` | `ptGainFader` | Input | — | Dry signal level (dB) |

## How It Works

Implements the classic Freeverb algorithm:
1. **8 comb filters** in parallel per channel (with tuned delay lengths)
2. **4 allpass filters** in series (for diffusion)
3. **Stereo spread** tuning for channel separation
4. **Wet/dry mixing** with smoothing for glitch-free parameter changes

### Freeze Mode
When enabled, comb filter feedback is set to 1.0 and damping to 0, creating an infinite sustain.

## Key Concepts Demonstrated

- **Complex DSP algorithm** with helper classes (Comb filter, Allpass filter)
- **Audio query** for dynamic multi-channel support
- **`createEvent`/`destroyEvent`** for temporary audio buffers
- **Parameter smoothing** for click-free parameter changes
- **Multiple helper classes** within a single module

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/granulator|Granulator]] — Another complex audio processor
- [[examples/audio-volume|AudioVolume]] — Simpler audio processing
- [[examples/ring-mod|RingMod]] — Multi-channel audio effect

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
