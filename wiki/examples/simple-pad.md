# SimplePad
#example #drawing #mouse #persistence #automation #control #graphics #errors

**Multi-pad XY controller with automation recording.**

| | |
|---|---|
| **Module Name** | `simple pad` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/SimplePad/` |

## Description

Drag 4 pads in the panel and get their positions as arrays. The positions are saved in the patch, the moves can be recorded as an automation, and the pads follow the quick color of Usine.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `randomize position` | `randomizeBtnIn` | `ptButton` | Input | `ctNormal` (`RANDOMIZE_BTN_CBID`) |
| 1 | `arr pos x` | `posXArrOut` | `ptArray` | Output (DontSave), editable | `ctNormal` (`POS_X_ARR_CBID`) |
| 2 | `arr pos y` | `posYArrOut` | `ptArray` | Output (DontSave), editable | `ctNormal` (`POS_Y_ARR_CBID`) |

The y output goes up while the y axis of the panel goes down: the module converts with `1 - y`.

## How It Works

1. **Mouse down**: hit-tests the pads (each pad is an ellipse in panel coordinates) and selects one
2. **Mouse move**: drags the selected pad, keeps the offset between the mouse and the pad center, and records the move as an automation (pad index, x, y)
3. **Mouse up**: releases the pad and stops the automation recording
4. **`onPaint`**: draws the pads with `sdkDrawPoint`
5. **Randomize**: from the button, the contextual menu or the global randomize of Usine
6. **Chunk**: the positions of the pads are saved, `onSetStateChunk` checks the size of the chunk before restoring them

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `ERROR_CALLBACK` | Error | An exception is thrown in `onCallBack` |
| `ERROR_INVALID_CHUNK` | Error | The saved positions are invalid, they are not restored |
| `ERROR_RECORD_AUTOMATION` | Error | `sdkProcessRecordAutomation` fails |
| `ERROR_STOP_RECORD_AUTOMATION` | Error | `sdkStopRecordAutomation` fails |

## Key Concepts Demonstrated

- **Hit detection** with an elliptical distance test
- **Chunk system**: `onGetStateChunkLen`, `onGetStateChunk`, `onSetStateChunk`
- **Automation recording**: `CanRecord = TRUE`, `sdkProcessRecordAutomation`, `sdkStopRecordAutomation`, `onSetRecordedValue` (which checks the recorded pad index)
- **`onSetQuickColor`**: follows the global quick color
- **Contextual command** sharing the callback id of the button, and settings panel
- **`sdkDrawPoint`** to draw the pads

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onGetStateChunkLen` · `onGetStateChunk` · `onSetStateChunk` · `onCreateSettings` · `onSettingsHasChanged` · `onCreateCommands` · `onPaint` · `onMouseMove` · `onMouseDown` · `onMouseUp` · `onRandomize` · `onSetRecordedValue` · `onSetQuickColor`

## Related Examples

- [[examples/draw-box|DrawBox]] — Simpler mouse interaction
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Advanced path drawing with chunks
- [[examples/granulator|Granulator]] — Another module with automation recording

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
