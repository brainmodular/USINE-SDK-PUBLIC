# DrawTrajectoryBox
#example #drawing #mouse #persistence #settings #control #graphics #advanced

**Trajectory drawing with bezier curves and chunk persistence.**

| | |
|---|---|
| **Module Name** | `draw trajectory` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 2.21 |
| **Source** | `examples/DrawTrajectoryBox/` |

## Description

Draw complex trajectories/paths on a canvas by clicking and dragging. Supports smooth quadratic bezier curves, cursor-based interpolation along the path, Lissajous pattern generation, and full chunk-based persistence.

## Parameters

| # | Name | Type | I/O | Details | Callback |
|---|------|------|-----|---------|----------|
| 0 | `nbpoints` | `ptDataField` | Output (ReadOnly) | Number of trajectory points | None |
| 1 | `cursor pos` | `ptDataField` | Input (IsStoredInPreset) | Position along path (0–1) | `ctImmediate` |
| 2 | `pos X` | `ptDataFader` | Output (ReadOnly) | Interpolated X coordinate | None |
| 3 | `pos Y` | `ptDataFader` | Output (ReadOnly) | Interpolated Y coordinate | None |
| 4 | `reset` | `ptButton` | Input | Clear trajectory | `ctImmediate` |
| 5 | `randomize` | `ptButton` | Input | Generate Lissajous curve | `ctImmediate` |
| 6 | `coords in` | `ptArray` | Input (DontSave) | Import trajectory data | `ctImmediate` |
| 7 | `coords out` | `ptArray` | Output (DontSave) | Export trajectory data | `ctImmediate` |

## How It Works

1. **Drawing**: Mouse down starts recording, mouse move appends points (with minimum pixel distance threshold), mouse up stops
2. **Interpolation**: `onProcess` interpolates X/Y position based on cursor position between trajectory points
3. **Rendering**: `onPaint` draws either a polyline or smooth quadratic bezier curves (configurable in settings)
4. **Randomize**: Generates a Lissajous curve with random parameters
5. **Persistence**: Full chunk system saves/restores all trajectory points

## Key Concepts Demonstrated

- **Chunk system** (`onGetChunkLen`, `onGetChunk`, `onSetChunk`) for custom data persistence
- **Quadratic bezier curves** via `sdkDrawPathQuadCurveTo`
- **Lissajous curve generation** for randomization
- **Cursor interpolation** along a path
- **Point distance threshold** for smooth drawing
- **Settings**: smooth line toggle, inverse Y axis, color picker
- **Coords array I/O** for trajectory import/export

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess` · `onGetChunkLen` · `onGetChunk` · `onSetChunk` · `onRandomize` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp`

## Related Examples

- [[examples/draw-box|DrawBox]] — Simpler mouse drawing
- [[examples/simple-pad|SimplePad]] — Draggable pads with chunk persistence
- [[examples/data-oscilloscope|DataOscilloscope]] — Path drawing for waveforms

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
