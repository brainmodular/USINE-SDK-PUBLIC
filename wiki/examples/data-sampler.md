# DataSampler
#example #data #drawing #control #graphics

**Data recording and playback with visualization.**

| | |
|---|---|
| **Module Name** | `data sampler test` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Source** | `examples/DataSampler/` |

## Description

Records incoming data values and plays them back in a loop with a visual waveform display. Demonstrates double-buffering, self-notification, and recording/playback toggle.

## Parameters

| # | Name | Type | I/O | Range | Callback |
|---|------|------|-----|-------|----------|
| 0 | `data` | `ptDataField` | Input | 0–1 | None |
| 1 | `toggle Rec` | `ptSwitch` | Input | — | `ctImmediate` |
| 2 | `clear Rec` | `ptButton` | Input | — | `ctImmediate` |
| 3 | `color` | `ptColor` | Input | — | `ctImmediate` |
| 4 | `out` | `ptDataField` | Output (ReadOnly, DontSave) | — | None |

## How It Works

1. **Recording**: When toggled on, pushes incoming data to the current buffer each `onProcess` tick
2. **Double-buffering**: When a buffer nears capacity, swaps to a second buffer and triggers aggregation via `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ...)`
3. **Playback**: When recording stops, plays back the recorded data in a continuous loop
4. **Visualization**: Draws the recorded waveform as a path on the canvas

## Key Concepts Demonstrated

- **Double-buffering** pattern for safe recording without blocking
- **Self-notification**: `sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, ...)` to send messages back to the module
- **Recording/playback toggle** with state management
- **Path drawing** for waveform visualization
- **Settings panel** with color picker

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onPaint`

## Related Examples

- [[examples/data-oscilloscope|DataOscilloscope]] — Real-time scrolling waveform
- [[examples/simple-pad|SimplePad]] — Another interactive control module

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
