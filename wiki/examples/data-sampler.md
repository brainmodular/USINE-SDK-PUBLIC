# DataSampler
#example #data #drawing #control #graphics #errors

**Records a data input and plays it back in a loop.**

| | |
|---|---|
| **Module Name** | `data sampler` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/DataSampler/` |

## Description

Records the values of a data input, then plays them back in a loop and draws the recorded curve in the panel. Demonstrates double buffering and a module that sends a notification to itself.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `data` | `dataFdrIn` | `ptDataField` | Input, 0 – 1 | None |
| 1 | `toggle rec` | `recSwIn` | `ptSwitch` | Input | `ctImmediate` (`REC_SW_CBID`) |
| 2 | `clear rec` | `clearBtnIn` | `ptButton` | Input | `ctImmediate` (`CLEAR_BTN_CBID`) |
| 3 | `color` | `boxColIn` | `ptColor` | Input | `ctImmediate` (`BOX_COL_CBID`) |
| 4 | `out` | `outFdrOut` | `ptDataField` | Output (ReadOnly, DontSave) | None |

## How It Works

1. **Recording**: while `toggle rec` is on, `onProcess` pushes the input in the current buffer and copies it to the output.
2. **Double buffering**: when the current buffer is nearly full, `onProcess` swaps the two buffers and sends `AGG_BUFFER_CBID` to the module itself with `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ...)`. `onCallBack` then appends the full buffer to the recording, outside of the audio process.
3. **Playback**: when recording stops, the last buffer is appended and the recording is played back in a loop on the output.
4. **Drawing**: on `sdkRefreshSpeedClock()`, the panel is repainted, `onPaint` draws the recording as a path that scrolls with the playback position.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `INFO_REC_STARTED` | Info | Recording starts |
| `INFO_REC_STOPPED` | Info | Recording stops |
| `INFO_REC_CLEARED` | Info | `clear rec` is clicked |

## Key Concepts Demonstrated

- **Double buffering** to record without blocking the audio process
- **Self notification**: `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ...)` with a callback id that is not a parameter
- **Path drawing** of the recorded curve
- **Settings panel** with a color setting
- **`sdkRaiseError`** for informative messages

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onPaint`

## Related Examples

- [[examples/data-oscilloscope|DataOscilloscope]] — Real-time scrolling curve
- [[examples/error-handling|ErrorHandling]] — The error system
- [[examples/simple-pad|SimplePad]] — Another interactive control module

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
