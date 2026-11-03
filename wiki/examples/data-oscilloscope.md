# DataOscilloscope
#example #data #drawing #settings #control #graphics

**Scrolling curve of a data input.**

| | |
|---|---|
| **Module Name** | `data oscilloscope` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/DataOscilloscope/` |

## Description

Draws the values of a data input as a curve that scrolls in the panel. Demonstrates custom drawing with the path API, the settings panel and the contextual menu.

## Parameters

| # | Name | Member | Type | I/O | Range | Default | Callback |
|---|------|--------|------|-----|-------|---------|----------|
| 0 | `y` | `yFdrIn` | `ptDataField` | Input | -1 – 1 | 0.5 | `ctImmediate` (`Y_FDR_CBID`) |
| 1 | `speed` | `speedFdrIn` | `ptDataFader` | Input | 0 – 0.5, `scLog` | 0.005 | `ctNormal` (`SPEED_FDR_CBID`) |
| 2 | `color` | `boxColIn` | `ptColor` | Input | — | — | `ctImmediate` (`BOX_COL_CBID`) |

## How It Works

1. **`onCallBack`**: stores the y value, the scroll speed and the color
2. **`onProcess`**: on each `sdkRefreshSpeedClock()` tick, `advance()`:
   - adds a point at x = 0 with the current y value
   - moves all the points to the right by the speed
   - removes the points that scrolled past x = 1
   - asks a repaint with `sdkRepaintPanel()`
3. **`onPaint`**: draws a segment between each pair of consecutive points with the path API

The points are stored in a `std::vector`, the simplest container for an example. A circular buffer would be more efficient.

## Key Concepts Demonstrated

- **`mtControl`** module type with a custom panel
- **Path drawing API**: `sdkDrawPathStart()`, `sdkDrawPathMoveTo()`, `sdkDrawPathLineTo()`, `sdkDrawPathClose()`, `sdkDrawPathDraw()`
- **`sdkRefreshSpeedClock()`** to animate at the refresh rate of Usine
- **Settings panel**: `sdkAddSettingLineColor()` for the color of the curve
- **Contextual menu**: `onCreateCommands()`

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint`

## Related Examples

- [[examples/data-sampler|DataSampler]] — Data recording with drawing
- [[examples/draw-box|DrawBox]] — Simpler custom drawing
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced path drawing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
