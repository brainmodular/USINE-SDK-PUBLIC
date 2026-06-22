# UsineObjects
#example #data #advanced

**Remote object access API for reading/writing workspace values.**

| | |
|---|---|
| **Module Name** | `usine objects` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/UsineObjects/` |

## Description

Demonstrates the Usine Objects API for remotely reading and writing values of any named object in the Usine workspace hierarchy. Objects are addressed by their full path (e.g., `"usine.workspace.rack01.patch01.faderdata"`).

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `array set` | `ptArray` | Input | `ctNormal` |
| 1 | `array get` | `ptArray` | Output | `ctNormal` |

## How It Works

In `onProcess`, on each `sdkSlowClock` tick, demonstrates all object operations:

- **Find**: `sdkUsineObjectFind("video")` — search objects by pattern
- **List**: `sdkUsineObjectListHash()` — get all object hashes
- **Float**: `sdkUsineObjectSetFloat/GetFloat` — set/get single values
- **Array**: `sdkUsineObjectSetArray/GetArray/SetLength` — set/get arrays
- **Color**: `sdkUsineObjectSetArrayColor/GetArrayColor` — color arrays
- **String**: `sdkUsineObjectSetAnsiChar/GetAnsiChar` — text values
- **Integer**: `sdkUsineObjectSetInteger/GetInteger` — integer values
- **Trigger**: `sdkUsineObjectSetTrigger` — trigger objects
- **Exists**: `sdkUsineObjectExists` — check if object exists
- **Length**: `sdkUsineObjectGetLength` — get array length

## Object API Pattern

```cpp
// Set a float value on a named object
sdkUsineObjectSetFloat("usine.workspace.rack01.patch01.fader", 0.5f);

// Get a float value
float val = sdkUsineObjectGetFloat("usine.workspace.rack01.patch01.fader");

// Set an array
sdkUsineObjectSetLength("my.array.object", 16);
sdkUsineObjectSetArray("my.array.object", myEvent);

// Check existence
LongBool exists = sdkUsineObjectExists("some.object.name");

// Find objects by pattern
AnsiCharPtr results = sdkUsineObjectFind("video");
```

## Key Concepts Demonstrated

- **Remote object access** — read/write any named object in the workspace
- **Object path hierarchy** — dot-separated path notation
- **Multiple data types** — float, integer, array, color, string, trigger
- **Object discovery** — find and list objects by name pattern

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/global-array|GlobalArray]] — Global array sharing
- [[examples/usine-chunks|UsineChunks]] — Chunk-based data sharing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
