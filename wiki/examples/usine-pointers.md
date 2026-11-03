# UsinePointers
#example #data #array #errors #advanced

**Copies the data of a pointer input to a pointer output.**

| | |
|---|---|
| **Module Name** | `usine pointers` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/UsinePointers/` |

## Description

Demonstrates the `ptPointer` parameter type, which passes a typed block of memory between connected modules.

## Parameters

| # | Name | Member | Type | I/O | Details | Callback |
|---|------|--------|------|-----|---------|----------|
| 0 | `pointer in` | `ptrIn` | `ptPointer` | Input | `PointerDataType = usinePtrFloat` | `ctImmediate` (`PTR_IN_CBID`) |
| 1 | `pointer out` | `ptrOut` | `ptPointer` | Output | `PointerDataType = usinePtrFloat` | None |

## How It Works

When the input changes, and once in `onInitModule`, `copyPointer()`:
1. **Checks the type**: the input must be `usinePtrFloat`, otherwise raises `ERROR_POINTER_NOT_FLOAT`
2. **Sizes the output**: `getPointerDataSize()` on the input, `setPointerDataSize()` on the output, in bytes
3. **Copies the data**: `std::memcpy` from `getPointerData()` to `getPointerData()`
4. **Notifies**: `setPointerDataChanged()` tells Usine that the output changed

## Pointer API Pattern

```cpp
// ModuleDefinition.h
inline constexpr TParamInfo ptrInParamInfo {
	.ParamType       = ptPointer,
	// ...
	.PointerDataType = usinePtrFloat
};

// in the callback
const UINT32 size = ptrIn.getPointerDataSize();   // in bytes
ptrOut.setPointerDataSize(size);
std::memcpy(ptrOut.getPointerData(), ptrIn.getPointerData(), size);
ptrOut.setPointerDataChanged();
```

## Pointer Data Types

| Constant | Value | Description |
|----------|-------|-------------|
| `usinePtrFloat` | 0 | Floats |
| `usinePtrInt` | 1 | Integers |
| `usinePtrString` | 2 | Strings |
| `usinePtrMidi` | 3 | MIDI codes |
| `usinePtrColor` | 4 | Colors |
| `usinePtrAudio` | 5 | Audio data |
| `usinePtrBitwise` | 6 | Bitwise data |
| `usinePtrChunk` | 6 | Chunk data |

> **Note:** `usinePtrBitwise` and `usinePtrChunk` have the same value in `UsineDefinitions.h`. `PointerDataType` accepts any `UINT32`, so modules can also define their own types.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `ERROR_POINTER_NOT_FLOAT` | Error | The input pointer is not `usinePtrFloat` |

## Key Concepts Demonstrated

- **`ptPointer`** parameter type
- **`PointerDataType`** in `TParamInfo`
- **Raw memory access** with `getPointerData()`
- **Size management** and change notification
- **Type checking** at runtime, with an error message

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack`

## Related Examples

- [[examples/global-array|GlobalArray]] — Higher-level data sharing
- [[examples/usine-chunks|UsineChunks]] — Data shared through chunks

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
