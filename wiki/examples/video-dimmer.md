# VideoDimmer
#example #video #advanced

**Video frame processing and pixel manipulation.**

| | |
|---|---|
| **Module Name** | `video dimmer` |
| **Type** | `mtVideo` |
| **Color** | `clVideoModuleColor` |
| **Source** | `examples/VideoDimmer/` |

## Description

A video processing module that applies a dimmer effect and an optional pixel grid pattern. Demonstrates the `mtVideo` module type with frame input/output and pixel-level manipulation.

## Parameters

| # | Name | Type | I/O | Range | Default | Callback |
|---|------|------|-----|-------|---------|----------|
| 0 | `dimmer` | `ptDataFader` | Input | 0–1 | 1 | None |
| 1 | `pixel color` | `ptColor` | Input | — | Red (0xFFFF0000) | None |
| 2 | `inter space` | `ptDataFader` | Input | 0–20 (integer) | 1 | None |

## How It Works

Processing happens in **`onProcessVideo`** (not `onProcess`):

1. Gets input frame: `sdkGetInputFrame(0, &inputFrame)`
2. Creates new output frame: `sdkGetNewFrame(&outputFrame, w, h, FALSE)`
3. If `inter space == 0`: copies frame directly with `sdkCopyFrame`
4. If `inter space > 0`: creates a grid effect:
   - Every N-th pixel: copied from input
   - Every (N+1)-th pixel: replaced with custom color
   - Other pixels: set to black
5. Applies dimmer: `sdkSetDimmerFrame(&outputFrame, dimmerValue)`
6. Sets output: `sdkSetOutputFrame(0, &outputFrame)`

## Key Concepts Demonstrated

- **`mtVideo`** module type
- **`NumberOfVideoInputs = 1`**, **`NumberOfVideoOutputs = 1`** in module info
- **`onProcessVideo`** instead of `onProcess` for video modules
- **Video frame API**:
  - `sdkGetInputFrame` — get input frame
  - `sdkGetNewFrame` — allocate output frame
  - `sdkCopyFrame` — copy frame data
  - `sdkClearFrame` — clear frame to color
  - `sdkGetFramePixel` / `sdkSetFramePixel` — pixel access
  - `sdkSetDimmerFrame` — apply brightness
  - `sdkSetOutputFrame` — set output frame
- **`sdkColorToPixel`** — convert `TUsineColor` to `TUsinePixel`

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcessVideo`

## Related Examples

This is the only video processing example in the SDK. For other module types, see:
- [[examples/audio-volume|AudioVolume]] — Audio processing
- [[examples/data-oscilloscope|DataOscilloscope]] — Custom visual rendering (mtControl)

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
