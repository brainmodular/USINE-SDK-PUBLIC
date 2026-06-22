# ArrayDelay
#example #array #data #callback #simple

**Per-channel array delay line.**

| | |
|---|---|
| **Module Name** | `array delay` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Version** | 2.0 |
| **Source** | `examples/ArrayDelay/` |

## Description

Delays each element of an input array by a per-element delay amount (in blocks). Uses a circular buffer delay line to apply independent delay offsets to each array element.

## Parameters

| # | Name | Type | I/O | Range | Callback |
|---|------|------|-----|-------|----------|
| 0 | `array in` | `ptArray` | Input | — | None |
| 1 | `delay in blocs` | `ptArray` | Input | 0 – 1000 | `ctImmediate` |
| 2 | `array out` | `ptArray` | Output | — | None |

## How It Works

1. **`onCallBack`**: When delay input changes, reads per-element delay values into a `delayBuffer[]` array, clamping negatives to 0
2. **`onProcess`**: Uses a circular delay line (`delayLine[MAX_DELAY][MAXBLOC]`):
   - For each element: writes input at `(readPos + delayBuffer[i]) % MAX_DELAY`
   - Reads output from `readPos`
   - Increments `readPos` each block

## Key Concepts Demonstrated

- **`ptArray`** parameter type for variable-size data
- **Circular buffer** delay line implementation
- **Per-element delay offsets** — each array element has independent delay
- **`ctImmediate`** callback for instant parameter updates on audio thread

## SDK Callbacks Used

`onGetModuleInfo` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/audio-volume|AudioVolume]] — Multi-channel audio processing
- [[examples/global-array|GlobalArray]] — Array data manipulation

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
