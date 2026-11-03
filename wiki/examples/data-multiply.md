# DataMultiply
#example #data #callback #simple #events

**Multiplies two data inputs.**

| | |
|---|---|
| **Module Name** | `data multiply` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/DataMultiply/` |

## Description

Multiplies two data inputs and outputs the result. The simplest example of a module that updates its outputs when its inputs change, without audio processing.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `a` | `aFdrIn` | `ptDataField` | Input | `ctImmediate` (`INPUT_FDR_CBID`) |
| 1 | `b` | `bFdrIn` | `ptDataField` | Input | `ctImmediate` (`INPUT_FDR_CBID`) |
| 2 | `result` | `resultFdrOut` | `ptDataField` | Output (DontSave) | None |

## How It Works

- Both inputs share the same callback id, so a change of either one recomputes the result
- **`onCallBack`**: `resultFdrOut.mult(aFdrIn, bFdrIn)`, the multiplication of two events into a third one
- **`onProcess`**: not used (`DontProcess = TRUE`)

## Key Concepts Demonstrated

- **Shared callback id**: several parameters triggering the same code
- **`UsineEventClass::mult()`**: event-level multiplication
- **`DontSave`**: output excluded from the saved values
- **`DontProcess = TRUE`**: callback-only module

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onCallBack`

## Related Examples

- [[examples/error-handling|ErrorHandling]] — Another callback-only module
- [[examples/midi-transpose|MidiTranspose]] — Data processing with MIDI

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
