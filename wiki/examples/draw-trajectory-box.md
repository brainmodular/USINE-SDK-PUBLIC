# DrawTrajectoryBox
#example #drawing #mouse #persistence #settings #control #graphics #errors #advanced

**Trajectory drawing with bezier curves and chunk persistence.**

| | |
|---|---|
| **Module Name** | `draw trajectory box` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 2.21 |
| **Source** | `examples/DrawTrajectoryBox/` |

## Description

Draw a trajectory in the panel by clicking and dragging. The trajectory can be drawn as lines or smooth quadratic curves, followed by a cursor, generated as a Lissajous curve, imported and exported as arrays, and it is saved in the patch with the chunk system.

## Parameters

| # | Name | Member | Type | I/O | Details | Callback |
|---|------|--------|------|-----|---------|----------|
| 0 | `nbpoints` | `nbPointsFdrOut` | `ptDataField` | Output (ReadOnly) | Number of points | None |
| 1 | `cursor pos` | `selPointIndexFdrIn` | `ptDataField` | Input (IsStoredInPreset) | Index of the cursor point, the fractional part moves it toward the next point | `ctImmediate` (`SEL_POINT_INDEX_FDR_CBID`) |
| 2 | `pos x` | `selPointXFdrOut` | `ptDataFader` | Output (ReadOnly) | Cursor X | None |
| 3 | `pos y` | `selPointYFdrOut` | `ptDataFader` | Output (ReadOnly) | Cursor Y | None |
| 4 | `reset` | `resetBtnIn` | `ptButton` | Input | Clears the trajectory | `ctImmediate` (`RESET_BTN_CBID`) |
| 5 | `randomize` | `randomizeBtnIn` | `ptButton` | Input | Generates a Lissajous curve | `ctImmediate` (`RANDOMIZE_BTN_CBID`) |
| 6 | `coords in` | `coordsArrIn` | `ptArray` | Input (DontSave) | Imports a trajectory | `ctImmediate` (`COORDS_ARR_IN_CBID`) |
| 7 | `coords out` | `coordsArrOut` | `ptArray` | Output (DontSave), editable | Exports the trajectory | `ctImmediate` (`COORDS_ARR_OUT_CBID`) |

`coords in` and `coords out` store 3 lines per point (`LINES_PER_POINT`): x, y and a third value.

## How It Works

1. **Drawing**: mouse down starts the trajectory, mouse move appends a point when the mouse moved more than one pixel, mouse up stops. The trajectory holds up to `MAXPOINTS` points.
2. **Cursor**: `onProcess` interpolates the position between the cursor point and the next one, with the fractional part of `cursor pos`.
3. **Rendering**: `onPaint` draws lines or quadratic curves (`sdkDrawPathQuadCurveTo`), with a control point computed for each segment.
4. **Randomize**: generates a Lissajous curve with random frequencies and phases, until it comes back near its first point.
5. **Persistence**: the chunk holds the number of points followed by the points. `onSetStateChunk` checks the size and the number of points before restoring them.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `WARNING_MAX_POINTS` | Warn | The trajectory reaches `MAXPOINTS`, or `coords in` has more points |
| `ERROR_INVALID_CHUNK` | Error | The saved trajectory is invalid, it is not restored |

## Key Concepts Demonstrated

- **Chunk system** (`onGetStateChunkLen`, `onGetStateChunk`, `onSetStateChunk`) with validation of the loaded data
- **Quadratic curves** with `sdkDrawPathQuadCurveTo`
- **Contextual menu** commands sharing the callback ids of the buttons
- **Settings**: inverse Y axis, line and cursor colors and widths, closed line, smooth line
- **Array import/export** of the trajectory
- **`sdkRaiseError`** for limits and invalid data

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess` · `onGetStateChunkLen` · `onGetStateChunk` · `onSetStateChunk` · `onRandomize` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp`

## Related Examples

- [[examples/draw-box|DrawBox]] — Simpler mouse drawing
- [[examples/simple-pad|SimplePad]] — Draggable pads with chunk persistence
- [[examples/data-oscilloscope|DataOscilloscope]] — Path drawing for curves

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
