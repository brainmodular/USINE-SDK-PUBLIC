# Utility Functions Reference
#sdk #reference #functions #color #geometry #math #audio

Helper functions for color conversion, geometry, and numeric operations.

**Defined in:** `sdk/UserUtils.h`

---

## Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `PI` | `3.14159265...` | Pi (`std::atan(1.0f)*4`) |
| `COEFF_TO_DB_MIN` | `-200` | Minimum dB value for coefficient conversion |
| `GAIN_MIN` | `-80` | Minimum gain in dB |
| `LN10S20` | `0.115129254f` | ln(10)/20, used in dB conversion |
| `MID_NOTE_ON` | `144` | MIDI Note On message byte |
| `MID_VOL_RANGE` | `127` | MIDI volume range |

---

## Color Conversion Functions

### Between Color Spaces

| Function | Input | Output | Description |
|----------|-------|--------|-------------|
| `sdkColorArgbToColorAhsl(TColorArgb)` | ARGB floats | `TColorAhsl` | ARGB to AHSL conversion |
| `sdkColorAhslToColorArgb(TColorAhsl)` | AHSL floats | `TColorArgb` | AHSL to ARGB conversion |
| `sdkColorArgbToAlphaColor(TColorArgb)` | ARGB floats | `TUsineColor` | ARGB to packed color |
| `sdkAlphaColorToColorArgb(TUsineColor)` | Packed color | `TColorArgb` | Packed color to ARGB |
| `sdkColorAhslToAlphaColor(TColorAhsl)` | AHSL floats | `TUsineColor` | AHSL to packed color |
| `sdkAlphaColorToColorAhsl(TUsineColor)` | Packed color | `TColorAhsl` | Packed color to AHSL |

### Color Constructors

| Function | Description |
|----------|-------------|
| `sdkColorAgrb(float r, g, b, a)` | Create `TColorArgb` from floats (0.0-1.0) |
| `sdkColorAhsl(float h, s, l, a)` | Create `TColorAhsl` (h: 0-360, s/l/a: 0-1) |
| `sdkAlphaColor(float r, g, b, a)` | Create `TUsineColor` from floats (0.0-1.0) |
| `sdkAlphaColor(int r, g, b, a)` | Create `TUsineColor` from ints (0-255) |

### Pixel Conversion

| Function | Description |
|----------|-------------|
| `sdkColorToPixel(TUsineColor)` | Convert color to video pixel (`TUsinePixel`) |
| `sdkPixelToColor(TUsinePixel)` | Convert video pixel to color (`TUsineColor`) |

### Usage
```cpp
// Create a semi-transparent red
TUsineColor red = sdkAlphaColor(1.0f, 0.0f, 0.0f, 0.5f);

// Convert to HSL, modify hue, convert back
TColorAhsl hsl = sdkAlphaColorToColorAhsl(red);
hsl.h += 120.0f;  // shift to green
TUsineColor green = sdkColorAhslToAlphaColor(hsl);
```

> **Example:** See [[examples/video-dimmer|VideoDimmer]] for color conversion in practice.

---

## Geometry Functions

### Point Construction

```cpp
TPointF sdkPointF(float x, float y);
```
Create a `TPointF` from x, y coordinates.

### Point-in-Rectangle Tests

| Function | Parameters | Description |
|----------|------------|-------------|
| `sdkPointInRect(int X, Y, TPointI LeftTop, RightBottom)` | Integer coords | Test if point is inside rectangle |
| `sdkPointInRect(TPointI Coords, TPointI LeftTop, RightBottom)` | Integer points | Test with TPointI |
| `sdkPointInRect(float X, Y, TRectF rect)` | Float coords | Test with TRectF |

### Bezier Interpolation

```cpp
// Bezier interpolation with 4 control points
TPointF sdkBezier4(TPointF p1, p2, p3, p4, float mu);  // Returns 2D point
float   sdkBezier4(float p1, p2, p3, p4, float mu);     // Returns float value
```

`mu` is the interpolation parameter (0.0 to 1.0).

---

## Numeric Functions

### Audio dB Conversion

```cpp
float sdkCoeffToDb(float coeff);  // Linear coefficient to decibels
float sdkDbToCoeff(float dB);     // Decibels to linear coefficient
```

**Formulas:**
- `sdkCoeffToDb`: Returns `20 * log10(coeff)`, clamped to `COEFF_TO_DB_MIN` (-200 dB)
- `sdkDbToCoeff`: Returns `10^(dB/20)`, returns 0 if dB <= `GAIN_MIN` (-80 dB)

> **Example:** See [[examples/audio-volume|AudioVolume]] for dB conversion usage.

### Math

```cpp
float sqr(float x);  // Returns x * x (square)
```

### String Conversion

```cpp
std::string sdkFloatToString(float value, int precision, int width);
std::string sdkIntToString(int value);
```

---

## Related Pages

- [[sdk/sdk-functions|SDK Functions]] — Global utility functions
- [[sdk/data-types|Data Types & Constants]] — Color types, TPointF, TRectF
- [[sdk/usine-event-class|UsineEventClass]] — Event manipulation
- [[sdk/user-module-base|UserModuleBase Class]] — Module base class and callbacks
- [[sdk/module-architecture|Module Architecture]] — How modules are structured
