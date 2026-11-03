# RingMod
#example #audio #query #events #simple

**Ring modulator audio effect.**

| | |
|---|---|
| **Module Name** | `ring mod` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/RingMod/` |

## Description

A ring modulator that multiplies audio inputs by a modulation signal, with a dry/wet mix. Supports multi-channel audio through the query system.

## Parameters

Created in `onCreateParameters`, N is the number of channels chosen in the query popup:

| # | Name | Member | Type | I/O | Details |
|---|------|--------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `audioInputs[]` | `ptAudio` | Input | Separator "audio in" |
| N..2N-1 | `out 1`, `out 2`, ... | `audioOutputs[]` | `ptAudio` | Output | Separator "audio out" |
| 2N | `mod` | `modAudioIn` | `ptAudio` | Input | Modulation signal |
| 2N+1 | `mix` | `mixFdrIn` | `ptDataFader` | Input | Dry/wet, 0 – 1, default 0.5, `ctNormal` (`MIX_FDR_CBID`) |

## How It Works

- **`onAfterQuery`**: Stores the number of channels, clamped to `USINE_AUDIO_CHANNEL_MODULE_MAX`.
- **`onInitModule`**: Creates `bufferTemp`, a work event of one bloc, with `createEvent`. The destructor destroys it.
- **`onCallBack`**: Stores the mix coefficient.
- **`onProcess`**: Copies the mix in a local so the whole bloc uses the same value, then for each channel: `output = (input * mod) * mix + input * (1 - mix)`, with `mult`, `copyfrom` and `add` on the events.

## Key Concepts Demonstrated

- **Ring modulation** with event multiplication
- **Work events** with `createEvent` / `destroyEvent`
- **Dry/wet mixing** pattern
- **Query system** for the number of channels

## SDK Callbacks Used

`onGetModuleInfo` · `onAfterQuery` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod-multithread|RingModMultithread]] — Same effect processed in two threads
- [[examples/audio-volume|AudioVolume]] — Simpler multi-channel audio

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
