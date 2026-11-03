# DragAndDrop
#example #drag-and-drop #files #mouse #control #graphics

**Accepts the files dragged on the panel and shows the names of the dropped ones.**

| | |
|---|---|
| **Module Name** | `drag and drop` |
| **Type** | `mtControl` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/DragAndDrop/` |

## Description

Drag files from the file explorer or from the Usine browser over the panel: it turns green and accepts them. Drop them: the panel lists their names, one per line, the last line counting the files that do not fit.

## Parameters

None: `ModuleDefinition.h` only holds the module constants, and the module doesn't override `onCreateParameters`.

## How It Works

- **`onAcceptDragOver`**: called at each move of a drag over the panel, returns `TRUE` and paints the panel green when the drag holds files, `FALSE` otherwise (a module, a wire...)
- **`onDragDrop`**: splits the file list, keeps the names without their folder and repaints the panel
- **`onPaint`**: fills the panel in green during the drag, then draws one line of text per file with `sdkFillText`
- **`onMouseMove`**: Usine has no drag leave callback, a drag that leaves the panel without a drop keeps it green until the mouse moves over it again

## The File List

Both callbacks receive the files as a UTF-8 Delphi comma-text, only valid during the call:

```
"C:\My Files\kick 01.wav",C:\Samples\snare.wav
```

A name holding a space, a comma or a quote is quoted, and its quotes are doubled. `splitCommaText` in `DragAndDrop.cpp` splits it into `std::string` copies.

## Key Concepts Demonstrated

- **Drag and drop**: `onAcceptDragOver`, `onDragDrop`
- **`sdkFillRect`** and **`sdkFillText`** for the panel content
- **`sdkRepaintPanel`** only when the state changes, `onAcceptDragOver` being called at each move of the drag
- **`DontProcess = TRUE`**: no audio processing

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onPaint` · `onMouseMove` · `onAcceptDragOver` · `onDragDrop`

## Related Examples

- [[examples/draw-box|DrawBox]] — Mouse interaction and drawing
- [[examples/simple-pad|SimplePad]] — Multi-pad mouse interaction

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Drag and Drop]]
- [[sdk/user-module-base|UserModuleBase Drawing Methods]]
