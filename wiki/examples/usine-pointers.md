# UsinePointers
#example #data #array #advanced

**Raw typed pointer data passing between modules.**

| | |
|---|---|
| **Module Name** | `pointer example` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Source** | `examples/UsinePointers/` |

## Description

Demonstrates the `ptPointer` parameter type for passing raw typed pointer data between modules. Pointer parameters allow direct memory sharing between connected modules.

## Parameters

| # | Name | Type | I/O | Details | Callback |
|---|------|------|-----|---------|----------|
| 0 | `pointer in` | `ptPointer` | Input | `PointerDataType = usinePtrFloat` | None |
| 1 | `pointer out` | `ptPointer` | Output | `PointerDataType = usinePtrFloat` | None |

## How It Works

1. **Type check**: Verifies input pointer type matches `usinePtrFloat`
2. **Size sync**: Gets input size with `getPointerDataSize()`, sets output size with `setPointerDataSize()`
3. **Data copy**: Copies raw pointer data via `memcpy(getPointerData(), ...)`
4. **Notification**: Calls `setPointerDataChanged()` to notify downstream modules
5. **Error handling**: If type mismatch, logs error with `sdkTraceErrorChar()`

## Pointer API Pattern

```cpp
// In onGetParamInfo:
pParamInfo->ParamType = ptPointer;
pParamInfo->PointerDataType = usinePtrFloat;  // typed pointer

// In callback:
int type = inputPtr.getPointerDataType();     // check type
int size = inputPtr.getPointerDataSize();     // get size in bytes
void* data = inputPtr.getPointerData();       // get raw data

outputPtr.setPointerDataSize(size);           // set output size
memcpy(outputPtr.getPointerData(), data, size); // copy data
outputPtr.setPointerDataChanged();            // notify downstream
```

## Pointer Data Types

| Constant | Value | Description |
|----------|-------|-------------|
| `usinePtrFloat` | 1 | Float array |
| `usinePtrInt` | 2 | Integer array |
| `usinePtrString` | 3 | String data |
| `usinePtrMidi` | 4 | MIDI data |
| `usinePtrColor` | 5 | Color data |
| `usinePtrAudio` | 6 | Audio data |
| `usinePtrBitwise` | 7 | Bitwise data |
| `usinePtrChunk` | 8 | Chunk data |

## Key Concepts Demonstrated

- **`ptPointer`** parameter type
- **`PointerDataType`** specification in `TParamInfo`
- **Raw memory access** via `getPointerData()`
- **Size management** and change notification
- **Type safety** with runtime type checking

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack`

## Related Examples

- [[examples/global-array|GlobalArray]] — Higher-level data sharing
- [[examples/usine-chunks|UsineChunks]] — Chunk-based data sharing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
