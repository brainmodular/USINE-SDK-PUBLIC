# SimplePad
#example #drawing #mouse #persistence #automation #control #graphics

**Multi-pad XY controller with automation recording.**

| | |
|---|---|
| **Module Name** | `simple pad` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Source** | `examples/SimplePad/` |

## Description

An interactive multi-pad XY controller. Drag 4 pads on a 2D canvas and output their X/Y coordinates as arrays. Supports automation recording, randomization, chunk persistence, and quick color changes.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `randomize position` | `ptButton` | Input | `ctNormal` |
| 1 | `arr pos x` | `ptArray` | Output (DontSave) | `ctNormal` |
| 2 | `arr pos y` | `ptArray` | Output (DontSave) | `ctNormal` |

## How It Works

1. **Mouse down**: Hit-tests against all 4 pads (elliptical detection) to select one
2. **Mouse move**: Drags the selected pad with offset compensation
3. **Mouse up**: Releases the pad
4. **`onPaint`**: Draws 4 points using `sdkDrawPoint`
5. **Randomize**: Repositions all pads randomly

## Key Concepts Demonstrated

- **Hit detection**: Elliptical distance test for pad selection
- **Chunk system**: `onGetChunkLen/onGetChunk/onSetChunk` for pad position persistence
- **Automation recording**: `sdkProcessRecordAutomation`/`sdkStopRecordAutomation` + `onSetRecordedValue`
- **`CanRecord = TRUE`** — enables Usine's automation recording
- **`onSetQuickColor`** — responds to global quick color change
- **Contextual commands** and settings panel
- **`sdkDrawPoint`** for rendering pads

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onGetChunkLen` · `onGetChunk` · `onSetChunk` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onRandomize` · `onSetRecordedValue` · `onSetQuickColor`

## Related Examples

- [[examples/draw-box|DrawBox]] — Simpler mouse interaction
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced path drawing with chunks
- [[examples/granulator|Granulator]] — Another module with automation recording

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
