# TemplateModule
#example #template #callback #reference

**Complete template overriding every callback.**

| | |
|---|---|
| **Module Name** | `template` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/TemplateModule/` |

## Description

A template that overrides **every callback** of `UserModuleBase` with an empty body, with the right signatures and `override`. Use it as a reference, or as a starting point for a module that needs chunks, mouse interaction, multi-touch, MIDI output, video, jobs or automation recording. Remove the callbacks you don't use.

`ModuleDefinition.h` shows where to put the messages and the callback ids, with commented examples.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `text` | `exampleTxtIn` | `ptText` | Input/Output | None |

The text parameter is initialized with `"hello world"`.

## Callbacks

The header groups the callbacks by topic:

### Module info, query and init
- `onGetModuleInfo`, `onAfterQuery`, `onCreateParameters`, `onInitModule`

### Parameters and process
- `onCallBack`, `onProcess`, `onProcessVideo`

### Background job
- `onJobBegin`, `onJobProcess`, `onJobEnd`

### Chunks
- `onGetStateChunkLen`, `onGetStateChunk`, `onSetStateChunk`, `onAfterLoading`

### Layout, panel and contextual menu
- `onCreateSettings`, `onSettingsHasChanged`, `onCreateCommands`, `onResize`, `onPaint`

### Mouse and multi-touch
- `onMouseMove`, `onMouseDown`, `onMouseUp`, `onMouseWheel`
- `onMouseMoveMulti`, `onMouseDownMulti`, `onMouseUpMulti` (the `UsineEventClass&` variants)

### Editor window
- `onOpenEditor`, `onBringToFront`, `onCloseEditor`, `onResizeEditor`

### Audio setup
- `onBlocSizeChange`, `onSampleRateChange`

### Global features
- `onSetRecordedValue`, `onRandomize`, `onReset`, `onSetQuickColor` (enabled by `CanRecord`, `CanBeRandomized`, `CanBeReset`)

### MIDI out devices
- `onMidiSendOut`, `onMidiSysexSendOut`

The deprecated `onGetParamInfo`, `onGetNumberOfParams` and `onSetEventAddress` are not listed: new modules create their parameters in `onCreateParameters`.

## Related Examples

- [[getting-started|Getting Started]] — Writing a first module from scratch

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/module-conventions|Module Conventions]]
