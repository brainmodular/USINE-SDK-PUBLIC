# GlobalArray
#example #array #data #advanced

**Shared global array manipulation across modules.**

| | |
|---|---|
| **Module Name** | `global array` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Source** | `examples/GlobalArray/` |

## Description

Demonstrates the Global Array API for sharing data across modules in a workspace. Shows how to create, read, and write global arrays of different types: float, color, string, and event.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `array set` | `ptArray` | Input | None |
| 1 | `array get` | `ptArray` | Output | None |

## How It Works

In `onProcess`, on each `sdkRefreshSpeedClock` tick:

1. **Float array** (`GA-FLOAT`): Sets 16 random floats, reads back index 0
2. **Color array** (`GA-COLOR`): Sets 16 random colors, reads back index 0
3. **String array** (`GA-STRING`): Sets 16 random strings, reads back index 0
4. **Event array** (`GA-EVENT`): Sets entire event with 16 random values, gets entire event back

On `sdkSlowClock`, traces all global array names via `sdkGlobalArrayGetAllNames()`.

## API Pattern

```cpp
// Get or create a named global array
UINT64 hash = sdkGlobalArrayGetHash("my-array-name");

// Write values
sdkGlobalArraySetValueFloat(hash, index, value);
sdkGlobalArraySetValueColor(hash, index, color);
sdkGlobalArraySetValueAnsiChar(hash, index, "text");
sdkGlobalArraySetValueEvt(hash, eventPtr);

// Read values
float val = sdkGlobalArrayGetValueFloat(hash, index);
TUsineColor col = sdkGlobalArrayGetValueColor(hash, index);
AnsiCharPtr str = sdkGlobalArrayGetValueAnsiChar(hash, index);
sdkGlobalArrayGetValueEvt(hash, eventPtr);

// Query
int size = sdkGlobalArrayGetSize(hash);
AnsiCharPtr names = sdkGlobalArrayGetAllNames();
```

## Key Concepts Demonstrated

- **`sdkGlobalArrayGetHash`** — hash-based global array identification
- **Multiple data types** — float, color, string, event
- **Cross-module data sharing** — any module can read/write the same named array
- **`sdkSlowClock`** / **`sdkRefreshSpeedClock`** — different update rates

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onProcess`

## Related Examples

- [[examples/usine-objects|UsineObjects]] — Remote object access (another sharing mechanism)
- [[examples/usine-chunks|UsineChunks]] — Chunk-based shared data

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
