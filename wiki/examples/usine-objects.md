# UsineObjects
#example #data #advanced

**Reads and writes Usine objects by their path.**

| | |
|---|---|
| **Module Name** | `usine objects` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/UsineObjects/` |

## Description

Demonstrates the Usine Objects API, which reads and writes any named object of the workspace. Objects are addressed by their full path, like `"usine.workspace.rack01.patch01.faderdata"`. The paths used by the module are constants of `ModuleDefinition.h`: create the matching objects in the first patch of the first rack to see it work.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `array set` | `setArrIn` | `ptArray` | Input | `ctNormal` |
| 1 | `array get` | `getArrOut` | `ptArray` | Output | `ctNormal` |

## How It Works

In `onProcess`, on each `sdkSlowClock()` tick:

- **Find**: `sdkUsineObjectFind("video")`, objects whose name contains a text
- **List**: `sdkUsineObjectListHash()`
- **Float**: `sdkUsineObjectSetFloat` / `sdkUsineObjectGetFloat` on `faderdata`
- **Data array**: `sdkUsineObjectSetLength` and `sdkUsineObjectSetArray` on the input of `arraydata`, `sdkUsineObjectGetArray` on its output
- **Color array**: `sdkUsineObjectSetArrayColor` / `sdkUsineObjectGetArrayColor`, `sdkUsineObjectSetInteger` for the number of rows, `sdkUsineObjectGetLength`
- **Text**: `sdkUsineObjectExists`, `sdkUsineObjectSetAnsiChar` / `sdkUsineObjectGetAnsiChar`
- **Trigger**: `sdkUsineObjectSetTrigger`

Each value read is raised as an info message.

## Object API Pattern

```cpp
sdkUsineObjectSetFloat("usine.workspace.rack01.patch01.faderdata", 0.5f);
TPrecision val = sdkUsineObjectGetFloat("usine.workspace.rack01.patch01.faderdata");

// arrays are written one element at a time
sdkUsineObjectSetLength(PATH_ARRAY_IN, 16);
sdkUsineObjectSetArray(PATH_ARRAY_IN, index, value);

LongBool    exists  = sdkUsineObjectExists(PATH_COMMA_TEXT);
AnsiCharPtr results = sdkUsineObjectFind("video");
```

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `INFO_VIDEO_OBJECTS`, `INFO_OBJECTS_HASH` | Info | Results of the find and list functions |
| `INFO_FADER_DATA`, `INFO_ARRAY_OUT`, `INFO_COLOR_ARRAY`, `INFO_COLOR_ARRAY_LENGTH`, `INFO_COMMA_TEXT` | Info | Values read back from the objects |
| `INFO_COMMA_TEXT_EXISTS` | Info | The text object exists |

Set the verbosity level to `Warn` to hide them, see [[examples/error-handling|ErrorHandling]].

## Key Concepts Demonstrated

- **Object access by path**: read and write any named object of the workspace
- **Several data types**: float, integer, array, color array, text, trigger
- **Object discovery**: find and list objects
- **Paths as named constants** instead of repeated strings

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onProcess`

## Related Examples

- [[examples/global-array|GlobalArray]] — Global array sharing
- [[examples/usine-chunks|UsineChunks]] — Data shared through chunks

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
