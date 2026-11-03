# VideoDimmer
#example #video #advanced

**Video frame processing and pixel manipulation.**

| | |
|---|---|
| **Module Name** | `video dimmer` |
| **Type** | `mtVideo` |
| **Color** | `clVideoModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/VideoDimmer/` |

## Description

A video module that dims its input and can draw a grid pattern on it. Demonstrates the `mtVideo` module type, with a video input, a video output and pixel access. Written by Olivier Sens (senso).

## Parameters

| # | Name | Member | Type | I/O | Range | Default | Callback |
|---|------|--------|------|-----|-------|---------|----------|
| 0 | `dimmer` | `dimmerFdrIn` | `ptDataFader` | Input | 0 – 1 | 1 | None |
| 1 | `pixel color` | `pixelColIn` | `ptColor` | Input | — | Red (0xFFFF0000) | None |
| 2 | `inter space` | `spaceFdrIn` | `ptDataFader` | Input | 0 – 20 (integer) | 1 | None |

The parameters have no callback: their values are read in `onProcessVideo`.

## How It Works

Processing happens in **`onProcessVideo`**, called by the video thread, not in `onProcess`:

1. Gets the input frame: `sdkGetInputFrame(1, &frameIn)`
2. Gets a new output frame of the same size: `sdkGetNewFrame(&frameOut, w, h, FALSE)`
3. If `inter space` is 0: copies the frame with `sdkCopyFrame`
4. Otherwise, draws a grid on one line out of `inter space`:
   - one pixel out of `inter space` is copied from the input
   - the next pixel takes the grid color
   - the other pixels stay black
5. Applies the dimmer: `sdkSetDimmerFrame(&frameOut, dimmer)`
6. Sets the output: `sdkSetOutputFrame(1, &frameOut)`

## Key Concepts Demonstrated

- **`mtVideo`** module type
- **`NumberOfVideoInputs = 1`**, **`NumberOfVideoOutputs = 1`** in the module info
- **`onProcessVideo`** instead of `onProcess`
- **Video frame API**:
  - `sdkGetInputFrame` — get the input frame
  - `sdkGetNewFrame` — allocate an output frame
  - `sdkCopyFrame` — copy a frame
  - `sdkClearFrame` — fill a frame with a color
  - `sdkGetFramePixel` / `sdkSetFramePixel` — pixel access
  - `sdkSetDimmerFrame` — apply a brightness
  - `sdkSetOutputFrame` — set the output frame
- **`sdkColorToPixel`** — convert a `TUsineColor` to a `TUsinePixel`

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters` · `onProcessVideo`

## Related Examples

This is the only video example of the SDK. For other module types, see:
- [[examples/audio-volume|AudioVolume]] — Audio processing
- [[examples/data-oscilloscope|DataOscilloscope]] — Custom drawing (`mtControl`)

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
