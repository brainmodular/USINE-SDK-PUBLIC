# ArrayDelay
#example #array #data #callback #simple

**Delays each element of an array by its own number of blocs.**

| | |
|---|---|
| **Module Name** | `array delay` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Version** | 2.0 |
| **Source** | `examples/ArrayDelay/` |

## Description

Delays each element of an input array by a per-element delay, expressed in blocs. A circular buffer of `MAX_DELAY` arrays stores the past values, each element is written at its own distance from the read position.

## Parameters

Created in `onCreateParameters` from the `TParamInfo` of `ModuleDefinition.h`.

| # | Name | Member | Type | I/O | Range | Callback |
|---|------|--------|------|-----|-------|----------|
| 0 | `array in` | `arrIn` | `ptArray` | Input | — | None |
| 1 | `delay in blocs` | `delayArrIn` | `ptArray` | Input | 0 – 999 | `ctImmediate` (`DELAY_ARR_CBID`) |
| 2 | `array out` | `arrOut` | `ptArray` | Output | — | None |

## How It Works

1. **`onInitModule`**: Clears the delay line and the delays (`delayLine[MAX_DELAY][MAXBLOC]`, `delayBuffer[MAXBLOC]`).
2. **`onCallBack`**: When `delay in blocs` changes, rounds each delay and clamps it to `0 .. MAX_DELAY - 1`.
3. **`onProcess`**: Advances `readPos` by one bloc, then for each element `i`:
   - writes the input at `(readPos + delay[i]) % MAX_DELAY`
   - reads the output at `readPos`

When the delay array is shorter than the input, its values are repeated (`delay[i % lengthDelay]`). An empty delay array means no delay.

## Key Concepts Demonstrated

- **`ptArray`** parameters for variable-size data, limited to `MAXBLOC` elements
- **Circular buffer** delay line
- **`ctImmediate`** callback: the delays are updated in the audio thread as soon as they change
- **`onInitModule`** for the initialization of the module state

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/audio-volume|AudioVolume]] — Multi-channel audio processing
- [[examples/global-array|GlobalArray]] — Array data manipulation

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/module-conventions|Module Conventions]]
