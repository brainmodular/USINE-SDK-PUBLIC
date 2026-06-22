# TemplateModule
#example #template #callback #reference

**Full template with all callback stubs implemented.**

| | |
|---|---|
| **Module Name** | `template` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/TemplateModule/` |

## Description

A comprehensive template that implements **every available SDK callback** as an empty stub. Use this as a reference or starting point when you need to implement advanced features like chunk persistence, mouse interaction, multi-touch, MIDI output, video, or automation recording.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `text` | `ptText` | Input/Output | None |

The text parameter is initialized with `"hello world"`.

## Key Concepts Demonstrated

This template provides empty implementations for every callback category:

### Module Lifecycle
- `onGetModuleInfo` — Module description
- `onGetNumberOfParams` / `onAfterQuery` — Query system
- `onInitModule` — Post-initialization
- `onBlocSizeChange` / `onSampleRateChange` — Audio configuration changes

### Processing
- `onProcess` — Real-time audio/data processing
- `onCallBack` — Parameter change handling

### Persistence
- `onGetChunkLen` / `onGetChunk` / `onSetChunk` — Custom state serialization

### User Interface
- `onCreateSettings` / `onSettingsHasChanged` — Settings panel
- `onPaint` — Custom canvas drawing
- `onMouseMove` / `onMouseDown` / `onMouseUp` — Mouse interaction
- `onMouseMoveMulti` / `onMouseDownMulti` / `onMouseUpMulti` — Multi-touch

### External
- `onMidiSendOut` / `onMidiSysexSendOut` — MIDI output
- `onOpenEditor` / `onBringToFront` — Child window management
- `onSetRecordedValue` — Automation playback

## Usage

Copy this template as a starting point and remove the callbacks you don't need. Keep only what your module actually uses.

## SDK Callbacks Used

All callbacks are implemented as empty stubs.

## Related Examples

- [[examples/template-module-simple|TemplateModuleSimple]] — Minimal template (recommended starting point)
- [[examples/my-module|MyModule]] — Simple working example

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
