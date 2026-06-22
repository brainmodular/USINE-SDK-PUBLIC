# DataMultiply
#example #data #callback #simple #events

**Simple data multiplication module.**

| | |
|---|---|
| **Module Name** | `data multiply` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/DataMultiply/` |

## Description

Multiplies two data inputs and outputs the result. The simplest example of callback-driven data processing with no audio processing.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `a` | `ptDataField` | Input | `ctNormal` (shared ID) |
| 1 | `b` | `ptDataField` | Input | `ctNormal` (shared ID) |
| 2 | `result` | `ptDataField` | Output (DontSave) | None |

## How It Works

- Both inputs share the same `CallBackId` so any change triggers the computation
- **`onCallBack`**: Calls `sdkMultEvt3(a, b, result)` to multiply A * B
- **`onProcess`**: Not used (`DontProcess = TRUE`)

## Key Concepts Demonstrated

- **Shared callback IDs**: Multiple parameters triggering the same callback
- **`sdkMultEvt3`**: Event-level multiplication (deprecated — use `UsineEventClass::mult()` in new code)
- **`DontSave`**: Output parameter excluded from saves
- **`DontProcess = TRUE`**: No audio processing needed

## SDK Callbacks Used

`onGetModuleInfo` · `onGetParamInfo` · `onCallBack`

## Related Examples

- [[examples/template-module-simple|TemplateModuleSimple]] — Another simple data module
- [[examples/midi-transpose|MidiTranspose]] — Data processing with MIDI

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
