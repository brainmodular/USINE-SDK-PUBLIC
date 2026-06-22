# UsineChunks
#example #chunks #persistence #advanced

**Chunk-based shared data persistence between modules.**

| | |
|---|---|
| **Module Name** | `usine chunks` |
| **Type** | `mtSimple` |
| **Color** | `clChunkModuleColor` |
| **Source** | `examples/UsineChunks/` |

## Description

Demonstrates the Usine Chunks system for sharing named data between modules. Chunks are persistent key-value stores that survive save/load cycles and can notify subscribers when data changes.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `set item1` | `ptArray` | Input | `ctImmediate` |
| 1 | `commatxt idx 1` | `ptText` | Input | `ctImmediate` |
| 2 | `get item1` | `ptArray` | Output | None |
| 3 | `get commatxt` | `ptText` | Output (DontSave) | None |

## How It Works

1. **`onInitModule`**: Gets/creates chunk `"mychunk"` via `sdkGetChunk`. Subscribes to notifications for chunk deletion, `"item1"` changes, and `"commatxt"` changes.
2. **Writing**: On input change, writes data to chunk items using `sdkChunkOperation(..., coAddOrModifyItem)`
3. **Reading**: On subscription notification, reads data from chunk using `sdkChunkOperation(..., coGetItem)`
4. **Cleanup**: Destructor calls `sdkChunkUnSubscribeAll()`

## Chunk API Pattern

```cpp
// Get or create a chunk
TUsineChunkPtr chunk = sdkGetChunk("mychunk");

// Write float array to chunk item
sdkChunkOperation(chunk, "item1", ftDataFloat,
    inputEvent, coAddOrModifyItem, 0);

// Write text to chunk item at index
sdkChunkOperation(chunk, "commatxt", ftTextGeneral,
    textEvent, coAddOrModifyItem, 1);

// Subscribe to changes (fires callbackID when item changes)
sdkChunkSubscribe(chunk, "item1", MY_CALLBACK_ID, TRUE);

// Read chunk item
sdkChunkOperation(chunk, "item1", ftDataFloat,
    outputEvent, coGetItem, 0);

// Cleanup
sdkChunkUnSubscribeAll();
```

## Key Concepts Demonstrated

- **`sdkGetChunk`** — get or create a named chunk
- **`sdkChunkOperation`** — read/write chunk items with `coAddOrModifyItem` and `coGetItem`
- **`sdkChunkSubscribe`** — receive notifications when chunk data changes
- **Flow types**: `ftDataFloat` for float data, `ftTextGeneral` for text
- **`MSG_CHUNK_DESTROYED` / `MSG_CHUNK_ITEM_LIST_CHANGED`** — lifecycle notifications
- **Cross-module data sharing** via named chunks

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack`

## Related Examples

- [[examples/global-array|GlobalArray]] — Simpler cross-module data sharing
- [[examples/usine-objects|UsineObjects]] — Remote object access
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Module-local chunk persistence

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
