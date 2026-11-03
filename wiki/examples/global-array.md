# GlobalArray
#example #array #data #advanced

**Writes and reads Usine global arrays.**

| | |
|---|---|
| **Module Name** | `global array` |
| **Type** | `mtSimple` |
| **Color** | `clArrayModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/GlobalArray/` |

## Description

Demonstrates the Global Array API, which shares data between all the modules of a workspace. The module writes random values in four global arrays of different types and reads them back. Create the global arrays `GA-FLOAT`, `GA-COLOR`, `GA-STRING` and `GA-EVENT` in Usine to see it work.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `array set` | `setArrIn` | `ptArray` | Input | None |
| 1 | `array get` | `getArrOut` | `ptArray` | Output | None |

## How It Works

In `onProcess`, on each `sdkRefreshSpeedClock()` tick, for each global array found (a hash of 0 means it doesn't exist):

1. **`GA-FLOAT`**: writes `GA_SIZE` random floats, reads back index 0
2. **`GA-COLOR`**: writes `GA_SIZE` random colors, reads back index 0
3. **`GA-STRING`**: writes `GA_SIZE` random strings, reads back index 0
4. **`GA-EVENT`**: writes `array set` filled with random values, reads it back into `array get`

On `sdkSlowClock()`, raises the names of all the global arrays.

## API Pattern

```cpp
// a hash of 0 means the global array does not exist
UINT64 hash = sdkGlobalArrayGetHash("GA-FLOAT");

// the last argument is the name used in the error messages of Usine
sdkGlobalArraySetValueFloat(hash, index, value, "GA-FLOAT");
sdkGlobalArraySetValueColor(hash, index, color, "GA-COLOR");
sdkGlobalArraySetValueAnsiChar(hash, index, "text", "GA-STRING");
sdkGlobalArraySetValueEvt(hash, setArrIn, "GA-EVENT");

TPrecision  val = sdkGlobalArrayGetValueFloat(hash, index, "GA-FLOAT");
TUsineColor col = sdkGlobalArrayGetValueColor(hash, index, "GA-COLOR");
AnsiCharPtr str = sdkGlobalArrayGetValueAnsiChar(hash, index, "GA-STRING");
sdkGlobalArrayGetValueEvt(hash, "GA-EVENT", getArrOut);

AnsiCharPtr names = sdkGlobalArrayGetAllNames();
```

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `INFO_GA_NAMES` | Info | On each slow clock, with the names of the global arrays |
| `INFO_GA_FLOAT`, `INFO_GA_COLOR`, `INFO_GA_STRING` | Info | On each refresh tick, with the value read at index 0 |

Set the verbosity level to `Warn` to hide them, see [[examples/error-handling|ErrorHandling]].

## Key Concepts Demonstrated

- **`sdkGlobalArrayGetHash`**: hash-based identification of a global array
- **Several data types**: float, color, string, event
- **Data shared between modules**: any module can read and write the same named array
- **`sdkSlowClock`** / **`sdkRefreshSpeedClock`**: two update rates in `onProcess`
- **`sdkRaiseError` with an extra text** to trace values

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onProcess`

## Related Examples

- [[examples/usine-objects|UsineObjects]] — Object access by path, another sharing mechanism
- [[examples/usine-chunks|UsineChunks]] — Data shared through chunks

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/sdk-functions|SDK Functions]] — Global array functions
