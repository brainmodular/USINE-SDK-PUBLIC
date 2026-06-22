# DataOscilloscope
#example #data #drawing #settings #control #graphics

**Scrolling waveform display with custom drawing.**

| | |
|---|---|
| **Module Name** | `data oscilloscope example` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Source** | `examples/DataOscilloscope/` |

## Description

A visual oscilloscope that displays incoming data as a scrolling waveform. Demonstrates custom drawing with the path API, settings panels, and contextual menus.

## Parameters

| # | Name | Type | I/O | Range | Scale | Default | Callback |
|---|------|------|-----|-------|-------|---------|----------|
| 0 | `y` | `ptDataField` | Input | -1 to 1 | Linear | 0.5 | `ctImmediate` |
| 1 | `speed` | `ptDataFader` | Input | 0 to 0.5 | `scLog` | 0.005 | `ctNormal` |
| 2 | `color` | `ptColor` | Input | — | — | — | `ctImmediate` |

## How It Works

1. **`onCallBack`**: Stores incoming y value, speed offset, and color using `std::atomic` for thread safety
2. **`onProcess`**: On `sdkRefreshSpeedClock()` tick, calls `advance()` which:
   - Adds a new point at x=0 with current y value
   - Shifts all existing points to the right by the speed offset
   - Removes points that have scrolled past x=1.0
   - Calls `sdkRepaintPanel()` to trigger redraw
3. **`onPaint`**: Draws a smooth path through all stored points using the path drawing API

## Key Concepts Demonstrated

- **`mtControl`** module type with custom canvas
- **Path drawing API**: `sdkDrawPathStart()`, `sdkDrawPathMoveTo()`, `sdkDrawPathLineTo()`, `sdkDrawPathClose()`, `sdkDrawPathDraw()`
- **`sdkRefreshSpeedClock()`** for frame-rate-aware animation updates
- **Settings panel**: `sdkAddSettingLineColor()` for persistent color settings
- **Contextual menu**: `onCreateCommands()` for right-click actions
- **`std::atomic`** for safe data sharing between audio and GUI threads

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint`

## Related Examples

- [[examples/data-sampler|DataSampler]] — Data recording with visualization
- [[examples/draw-box|DrawBox]] — Simpler custom drawing
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced path drawing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
