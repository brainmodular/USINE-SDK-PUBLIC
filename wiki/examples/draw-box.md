# DrawBox
#example #drawing #mouse #settings #control #graphics

**Interactive rectangle drawing with mouse input.**

| | |
|---|---|
| **Module Name** | `draw box` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Source** | `examples/DrawBox/` |

## Description

Draw and manipulate a rectangle on a canvas using mouse interaction. Supports zoom via mouse wheel, randomization, and contextual menus.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `reset` | `ptButton` | Input | `ctImmediate` |
| 1 | `randomize` | `ptButton` | Input | `ctImmediate` |

## How It Works

- **Mouse down**: Records the starting corner of the rectangle
- **Mouse move**: Updates the opposite corner while dragging
- **Mouse up**: Releases the rectangle
- **Mouse wheel**: Scales the rectangle around the mouse position
- **`onPaint`**: Draws a filled rectangle with `sdkFillRect`
- **`onRandomize`**: Generates random rectangle dimensions

## Key Concepts Demonstrated

- **Full mouse interaction**: `onMouseMove`, `onMouseDown`, `onMouseUp`, `onMouseWheel`
- **`sdkFillRect`** for rectangle drawing
- **`onRandomize`** for preset randomization support
- **`onCreateCommands`** for contextual menu entries
- **Settings panel** with color picker
- **`DontProcess = TRUE`** — no audio processing

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onMouseWheel` · `onRandomize`

## Related Examples

- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced drawing with curves
- [[examples/simple-pad|SimplePad]] — Multi-pad mouse interaction
- [[examples/data-oscilloscope|DataOscilloscope]] — Custom drawing with path API

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
