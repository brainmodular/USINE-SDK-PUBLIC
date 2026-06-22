# RingMod
#example #audio #query #events #simple

**Ring modulator audio effect.**

| | |
|---|---|
| **Module Name** | `ring mod example` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Source** | `examples/RingMod/` |

## Description

A ring modulator that multiplies audio inputs by a modulation signal with a dry/wet mix control. Supports multi-channel audio via the query system.

## Parameters

Dynamic count (N = number of audio channels):

| # | Name | Type | I/O | Details |
|---|------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `ptAudio` | Input | Audio inputs |
| N..2N-1 | `out 1`, `out 2`, ... | `ptAudio` | Output (ReadOnly) | Audio outputs |
| 2N | `mod` | `ptAudio` | Input | Modulation signal |
| 2N+1 | `mix` | `ptDataFader` | Input | Dry/wet mix (0–1, default 0.5) |

## How It Works

- **`onCallBack`**: Stores mix coefficient in `std::atomic<float>`
- **`onProcess`**: For each channel: `output = input * (1-mix) + (input * mod) * mix`
- Uses `createEvent` for a temporary multiplication buffer

## Key Concepts Demonstrated

- **Ring modulation** via event multiplication
- **Temporary events** with `createEvent`/`destroyEvent`
- **Dry/wet mixing** pattern
- **`std::atomic`** for thread-safe parameter sharing

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod-multithread|RingModMultithread]] — Same effect with parallel processing
- [[examples/audio-volume|AudioVolume]] — Simpler multi-channel audio

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
