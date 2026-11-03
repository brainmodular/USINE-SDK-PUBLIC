# DrawBox
#example #drawing #mouse #settings #control #graphics

**Interactive rectangle drawing with mouse input.**

| | |
|---|---|
| **Module Name** | `draw box` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/DrawBox/` |

## Description

Draw a rectangle in the panel with the mouse, zoom it with the mouse wheel, randomize or erase it with buttons or with the contextual menu.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `reset` | `resetBtnIn` | `ptButton` | Input | `ctImmediate` (`RESET_BTN_CBID`) |
| 1 | `randomize` | `randomizeBtnIn` | `ptButton` | Input | `ctImmediate` (`RANDOMIZE_BTN_CBID`) |

## How It Works

- **Mouse down**: stores the anchor corner of the rectangle
- **Mouse move**: while the left button is down, moves the opposite corner
- **Mouse up**: releases the rectangle
- **Mouse wheel**: scales the rectangle around the mouse position, clamped to the panel
- **`onPaint`**: fills the rectangle with `sdkFillRect`, a negative `top` means there is no rectangle
- **`onRandomize`**: draws a random rectangle, called by the `randomize` button and by the global randomize of Usine (`CanBeRandomized = TRUE`)
- **`onCreateCommands`**: adds `cmd_reset` and `cmd_randomize` to the contextual menu, with the callback ids of the buttons: a command calls `onCallBack` like the matching button

## Key Concepts Demonstrated

- **Mouse interaction**: `onMouseMove`, `onMouseDown`, `onMouseUp`, `onMouseWheel`
- **`sdkFillRect`** for rectangle drawing
- **`onRandomize`** for the global randomize feature
- **`onCreateCommands`** sharing the callback ids of the buttons
- **Settings panel** with a color setting
- **`DontProcess = TRUE`**: no audio processing

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onMouseWheel` · `onRandomize`

## Related Examples

- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced drawing with curves
- [[examples/simple-pad|SimplePad]] — Multi-pad mouse interaction
- [[examples/data-oscilloscope|DataOscilloscope]] — Custom drawing with the path API

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
