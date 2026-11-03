# UsineChunks
#example #chunks #persistence #errors #advanced

**Shares data with other modules through a Usine chunk.**

| | |
|---|---|
| **Module Name** | `usine chunks` |
| **Type** | `mtSimple` |
| **Color** | `clChunkModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/UsineChunks/` |

## Description

Demonstrates the Usine chunks: named stores of items shared by all the modules that use the same name, saved with the workspace, which notify their subscribers when they change. Add two instances of the module to see one write and the other read.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `set item1` | `setItemArrIn` | `ptArray` | Input | `ctImmediate` (`SET_ITEM_ARR_CBID`) |
| 1 | `commatxt idx 1` | `setCommaTxtIn` | `ptText` | Input | `ctImmediate` (`SET_COMMA_TXT_CBID`) |
| 2 | `get item1` | `getItemArrOut` | `ptArray` | Output | None |
| 3 | `get commatxt` | `getCommaTxtOut` | `ptText` | Output (DontSave) | None |
| 4 | `read from chunk` | `getChunkDataBtnIn` | `ptButton` | Input | `ctNormal` (`GET_CHUNK_DATA_BTN_CBID`) |
| 5 | `push to chunk` | `setChunkDataBtnIn` | `ptButton` | Input | `ctNormal` (`SET_CHUNK_DATA_BTN_CBID`) |

The chunk name (`CHUNK_NAME`), the item names and the subscription ids are defined in `ModuleDefinition.h`.

## How It Works

1. **`onInitModule`**: gets the chunk `"mychunk"` with `sdkGetChunk` (it is created if needed) and subscribes to:
   - the chunk itself (`CHUNK_SUB_CBID`): destroyed, or its item list changed
   - the item `"item1"` (`CHUNK_SUB_ITEM_CBID`)
   - the item `"commatxt"` (`CHUNK_SUB_TEXT_CBID`)

   It also writes a default comma text in `"commatxt"`.
2. **Writing**: when an input changes, `onCallBack` writes it in its item with `sdkChunkOperation(..., coAddOrModifyItem)`. `commatxt idx 1` only writes the value at index 1 of the comma text.
3. **Reading**: when a subscribed item changes, possibly written by another module, `onCallBack` reads it into the output with `sdkChunkOperation(..., coGetItem)`.
4. **Bulk transfer**: `push to chunk` writes a buffer of 10000 random floats, filled once in `onInitModule`, in the item `"data_bulk"` with a single `sdkSetChunkFromFloatPointer` call. `read from chunk` asks the size of `"data_bulk"` with `sdkGetChunkItemSize`, resizes a `std::vector<float>` to it and fills it with `sdkGetChunkToFloatPointer`, then writes the test buffer in the item `"data_bulk_from_module"`.
5. **Cleanup**: the destructor calls `sdkChunkUnSubscribeAll()`.

## Chunk API Pattern

```cpp
// check existence without creating anything
LongBool chunkFound = sdkChunkExist("mychunk");
LongBool itemFound  = sdkChunkItemExist("mychunk", "item1");

// get the chunk, it is created if needed
TUsineChunkPtr chunk = sdkGetChunk("mychunk");

// write a float array, and the value at index 1 of a comma text
sdkChunkOperation(chunk, "item1", ftDataFloat, setItemArrIn, coAddOrModifyItem);
sdkChunkOperation(chunk, "commatxt", ftTextGeneral, setCommaTxtIn, coAddOrModifyItem, 1);

// subscribe: onCallBack receives the id when the item changes, "" subscribes to the chunk itself
sdkChunkSubscribe(chunk, "item1", CHUNK_SUB_ITEM_CBID, TRUE);

// read an item
sdkChunkOperation(chunk, "item1", ftDataFloat, getItemArrOut, coGetItem);

// write a whole float buffer in one call, without an event
sdkSetChunkFromFloatPointer(chunk, "data_bulk", data_bulk.data(), static_cast<UINT32>(data_bulk.size()));

// read it back: the destination must hold the size of the item
data_buffer.resize(sdkGetChunkItemSize(chunk, "data_bulk"));
sdkGetChunkToFloatPointer(chunk, "data_bulk", data_buffer.data());

sdkChunkUnSubscribeAll();
```

The chunk notifications arrive in `onCallBack` with their own `lParam`: `MSG_CHANGE` for an item, `MSG_CHUNK_DESTROYED` and `MSG_CHUNK_ITEM_LIST_CHANGED` for the chunk.

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `WARNING_CHUNK_DESTROYED` | Warn | The chunk is destroyed |
| `INFO_CHUNK_ITEMS_CHANGED` | Info | Items are added to or removed from the chunk |

## Key Concepts Demonstrated

- **`sdkGetChunk`**: get or create a named chunk
- **`sdkChunkExist` / `sdkChunkItemExist`**: check a chunk or an item without creating it
- **`sdkChunkOperation`**: read and write items with `coAddOrModifyItem` and `coGetItem`
- **`sdkChunkSubscribe`**: notifications when the chunk or an item changes
- **`sdkSetChunkFromFloatPointer` / `sdkGetChunkToFloatPointer` / `sdkGetChunkItemSize`**: a float buffer written or read in one call
- **Flow types**: `ftDataFloat` for floats, `ftTextGeneral` for text
- **Data shared between modules** through named chunks

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onCallBack`

## Related Examples

- [[examples/global-array|GlobalArray]] — Simpler data sharing between modules
- [[examples/usine-objects|UsineObjects]] — Object access by path
- [[examples/draw-trajectory-box|DrawTrajectoryBox]] — Saving the state of a module itself (`onGetStateChunk`)

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
