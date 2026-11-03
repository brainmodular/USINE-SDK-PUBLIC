# Reverb
#example #audio #query #advanced

**Freeverb-based multi-channel reverb.**

| | |
|---|---|
| **Module Name** | `reverb` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Version** | 1.1 |
| **Source** | `examples/Reverb/` |

## Description

A reverb based on the Freeverb algorithm, written by Jezar at Dreampoint and placed in the public domain. Room size, damping, channel separation, wet and dry levels, and a freeze mode. Supports multi-channel audio via the query system: each channel also feeds the reverb of the other channels.

## Files

| File | Contains |
|------|----------|
| `Reverb.h/.cpp` | The module |
| `comb.hpp/.cpp`, `allpass.hpp/.cpp` | The Freeverb filters |
| `tuning.h` | The Freeverb tuning values |
| `denormals.h` | `undenormalise()`, flushes the denormal floats to zero |
| `ModuleDefinition.h` | Module constants, callback ids, default values and parameters |

## Parameters

Created in `onCreateParameters`, N is the number of channels chosen in the query popup:

| # | Name | Member | Type | I/O | Details |
|---|------|--------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `audioInputs[]` | `ptAudio` | Input | Audio inputs |
| N..2N-1 | `out 1`, `out 2`, ... | `audioOutputs[]` | `ptAudio` | Output | Audio outputs |
| 2N | `freeze` | `modeSwIn` | `ptSwitch` | Input | Freezes the reverb tail |
| 2N+1 | `room size` | `roomSizeFdrIn` | `ptDataFader` | Input | 0.1 – 100 % (`scExp`) |
| 2N+2 | `damping` | `dampingFdrIn` | `ptDataFader` | Input | 0 – 100 % |
| 2N+3 | `channels separation` | `channelsSeparationFdrIn` | `ptDataFader` | Input | 0 – 100 % |
| 2N+4 | `wet level` | `wetLevelFdrIn` | `ptGainFader` | Input | dB |
| 2N+5 | `dry level` | `dryLevelFdrIn` | `ptGainFader` | Input | dB |

All the inputs have a `ctNormal` callback, their ids are named after the members (`ROOM_SIZE_FDR_CBID`...). Every callback ends in `reverbParamsUpdate()`.

## How It Works

The Freeverb algorithm, for each channel:
1. **8 comb filters** in parallel, with tuned buffer sizes
2. **4 allpass filters** in series, for diffusion
3. Each channel gets slightly longer buffers (`stereospread`) to decorrelate the channels
4. **Wet and dry levels** smoothed at each bloc to avoid clicks

The channel separation splits the wet level between the reverb of the channel itself (`wet1`) and the reverb of the other channels (`wet2`).

### Freeze Mode

When enabled, the comb feedback is set to 1 and the damping to 0, and the input is muted: the reverb tail sustains forever.

## Key Concepts Demonstrated

- **DSP helper classes** (comb and allpass filters) in their own files
- **Audio query** for a dynamic number of channels
- **Work events** with `createEvent` / `destroyEvent`
- **Parameter smoothing** for click-free changes
- **Third party code**: public domain code adapted to the SDK conventions

## SDK Callbacks Used

`onGetModuleInfo` · `onAfterQuery` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/granulator|Granulator]] — Another complex audio processor
- [[examples/audio-volume|AudioVolume]] — Simpler audio processing
- [[examples/ring-mod|RingMod]] — Multi-channel audio effect

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
