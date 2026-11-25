# DataSampler
#example #data #drawing #control #graphics #errors

**Records a data input and plays it back in a loop.**

| | |
|---|---|
| **Module Name** | `data sampler` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.261125 |
| **Source** | `examples/DataSampler/` |

## Description

Records the values of a data input, then plays them back in a loop and draws the recorded curve in the panel. Demonstrates how the audio thread, the main thread and the immediate callbacks share data without lock and without allocation in `onProcess`, and a module that sends notifications to itself.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `data` | `dataFdrIn` | `ptDataField` | Input, 0 – 1 | None |
| 1 | `toggle rec` | `recSwIn` | `ptSwitch` | Input | `ctImmediate` (`REC_SW_CBID`) |
| 2 | `clear rec` | `clearBtnIn` | `ptButton` | Input | `ctImmediate` (`CLEAR_BTN_CBID`) |
| 3 | `color` | `boxColIn` | `ptColor` | Input | `ctImmediate` (`BOX_COL_CBID`) |
| 4 | `out` | `outFdrOut` | `ptDataField` | Output (ReadOnly, DontSave) | None |

## How It Works

1. **Requests**: `toggle rec` and `clear rec` are `ctImmediate`. Their callback runs in the thread that changes the value: the audio thread when the value comes through a wire, the main thread when the user clicks. It only stores a request in an atomic, which `onProcess` applies at the start of its next block.
2. **Recording**: while recording, `onProcess` writes the input into the current chunk of 2000 values and copies it to the output. The chunks are allocated by the main thread: when a chunk begins, `onProcess` sends `NEXT_CHUNK_CBID` to the module itself with `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ..., TNotifyUsineMode::numMainThread)`, and `onCallBack` allocates the next one, 2000 blocks before the audio thread needs it. `onProcess` never allocates, and the recorded values never move.
3. **Playback**: when recording stops, the recording is played back in a loop on the output.
4. **Drawing**: on `sdkRefreshSpeedClock()`, the panel is repainted, `onPaint` (main thread) draws the recording as a path that scrolls with the playback position. The values are `std::atomic<TPrecision>`: the main thread can read them while a new recording writes them.
5. **Messages**: `onProcess` notifies the main thread (`REC_STARTED_CBID`, `REC_STOPPED_CBID`, `REC_CLEARED_CBID`), which calls `sdkReport`: a report allocates, never in the audio thread.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `INFO_REC_STARTED` | Info | Recording starts |
| `INFO_REC_STOPPED` | Info | Recording stops |
| `INFO_REC_CLEARED` | Info | The recording is cleared |

## Key Concepts Demonstrated

- **Chunks allocated ahead** by the main thread, to record without allocating in the audio process
- **Atomics** between the audio thread, the main thread and the immediate callbacks, without lock
- **Self notification**: `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ...)` with `numMainThread` and callback ids that are not parameters
- **Path drawing** of the recorded curve
- **Settings panel** with a color setting
- **`sdkReport`** for informative messages

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onPaint` · destructor

## Related Examples

- [[examples/data-oscilloscope|DataOscilloscope]] — Real-time scrolling curve
- [[examples/error-handling|ErrorHandling]] — The error system
- [[examples/simple-pad|SimplePad]] — Another interactive control module

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
