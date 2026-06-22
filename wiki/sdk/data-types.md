# Data Types & Constants Reference
#sdk #reference #types #enums #structs #constants #midi #color #parameters

All types, structures, enumerations, and constants used by the Usine SDK.

**Defined in:** `sdk/UsineDefinitions.h`

---

## Platform Defines

| Define | Condition |
|--------|-----------|
| `USINE_WIN64` | Windows 64-bit build |
| `USINE_OSX64` | macOS 64-bit build |
| `USINE_LIN64` | Linux 64-bit build |

> **Note:** All SDK structures use `#pragma pack(push, 4)` for Delphi memory alignment compatibility.

---

## SDK Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `SDK_VERSION` | `702002` | Current SDK version number |
| `USINE_AUDIO_CHANNEL_MODULE_MAX` | `144` | Maximum audio input/output channels |
| `USINE_MAX_SOUNDCARD_AUDIO_INPUTS` | `256` | Maximum sound card inputs |
| `USINE_MAX_SOUNDCARD_AUDIO_OUTPUTS` | `256` | Maximum sound card outputs |
| `USINE_MAX_AUDIO_EVT_SIZE` | `1024` | Maximum audio block size |
| `USINE_MAX_MIDI_DEVICES` | `64` | Maximum MIDI devices |
| `FALSE` | `0` | LongBool false value |
| `TRUE` | `1` | LongBool true value |

---

## Basic Types

| Type | Underlying Type | Description |
|------|-----------------|-------------|
| `LongBool` | `int` | 4-byte boolean (Delphi compatibility) |
| `AnsiCharPtr` | `const char*` | Null-terminated string pointer |
| `TPrecision` | `float` | Universal parameter data type (32-bit) |
| `NativeInt` | `int64_t` | Platform-native signed integer |
| `NativeUInt` | `uint64_t` | Platform-native unsigned integer |
| `UINT64` | `uint64_t` | 64-bit unsigned integer |
| `UINT32` | `uint32_t` | 32-bit unsigned integer |
| `Int64` | `int64_t` | 64-bit signed integer |
| `BYTE` | `uint8_t` | Unsigned byte |
| `TUsineColor` | `UINT32` | Color value in ARGB format |

## Handle Types

| Type | Underlying | Description |
|------|------------|-------------|
| `UsineEventPtr` | `UsineEvent*` | Event data handle (prefer `UsineEventClass`) |
| `TAudioFilePtr` | `void*` | Audio file handle |
| `TCriticalSectionPtr` | `void*` | Critical section handle |
| `TThreadPtr` | `void*` | Thread handle |
| `TSyncObjectPtr` | `void*` | Synchronization object handle |
| `TUsineFFTPtr` | `void*` | FFT object handle |
| `TUsineChunkPtr` | `void*` | Chunk handle |
| `TUsinePtr` | `void*` | Generic pointer |

---

## Structures

### TPointF
2D floating-point coordinates.
```cpp
struct TPointF {
    float x;
    float y;
};
```

### T3DPointF
3D floating-point coordinates.
```cpp
struct T3DPointF {
    float x;
    float y;
    float z;
};
```

### TRectF
Floating-point rectangle.
```cpp
struct TRectF {
    float left;
    float top;
    float right;
    float bottom;
};
```

### TPointI
2D integer coordinates.
```cpp
struct TPointI {
    int x;
    int y;
};
```

### TColorArgb
ARGB color components (0.0 to 1.0).
```cpp
struct TColorArgb {
    float a, r, g, b;
};
```

### TColorAhsl
AHSL color components.
```cpp
struct TColorAhsl {
    float a;  // alpha (0-1)
    float h;  // hue (0-360)
    float s;  // saturation (0-1)
    float l;  // lightness (0-1)
};
```

### TUsinePixel
Video pixel (platform-dependent byte order).
```cpp
struct TUsinePixel {
    BYTE B, G, R, A;  // Windows: BGRA order
};
```

### TUsineFrame
Video frame structure.
```cpp
struct TUsineFrame {
    int Width;
    int Height;
    TUsinePixelPtr Pixels;
    LongBool Valid;
    void* intern;
};
```

> **Example:** See [[examples/video-dimmer|VideoDimmer]] for `TUsineFrame` and `TUsinePixel` usage.

### TUsineILDAPoint
ILDA laser point.
```cpp
struct TUsineILDAPoint {
    float x, y, z;
    TUsineColor Color;
};
```

### TUsineMidiCode
MIDI message code.
```cpp
struct TUsineMidiCode {
    unsigned char Channel;  // MIDI channel (0-15)
    unsigned char Msg;      // Message type
    unsigned char Data1;    // First data byte (note number, CC number, etc.)
    unsigned char Data2;    // Second data byte (velocity, CC value, etc.)
};
```

> **Example:** See [[examples/midi-transpose|MidiTranspose]] for MIDI code usage.

### TUsineMessage
Internal message structure for callbacks.
```cpp
struct TUsineMessage {
    NativeInt message;  // Message type
    NativeInt wParam;   // CallBackId of the parameter
    NativeInt lParam;   // Sub-message type
    NativeInt result;   // Return value
};
```

### TVstTimeInfo
VST-compatible time information.
```cpp
struct TVstTimeInfo {
    double samplePos;
    double sampleRate;
    double nanoSeconds;
    double PpqPos;
    double tempo;
    double barStartPos;
    double cycleStartPos;
    double cycleEndPos;
    int timeSigNumerator;
    int timeSigDenominator;
    int smpteOffset;
    int smpteFrameRate;
    int samplesToNextClock;
    int flags;
    double cyclePpqPos;
};
```

---

## Enumerations

### TModuleType
Module category types.

| Value | Description |
|-------|-------------|
| `mtSimple` | Standard data/audio module |
| `mtControl` | Module with visual canvas |
| `mtVideo` | Video processing module |
| `mtDeviceAudio` | Audio device driver |
| `mtDeviceMidi` | MIDI device driver |
| `mtDeviceDMX` | DMX lighting device |
| `mtDeviceLaser` | ILDA laser device |
| `mtDeviceOther` | Other device type |
| `mtPluginWrapper` | VST/AU plugin wrapper |
| `mtPluginLister` | Plugin lister |
| `mtOther` | Other module type |

### TParamType
Parameter types determine how a parameter appears and behaves.

| Value | Description |
|-------|-------------|
| `ptText` | Text display/input |
| `ptColor` | Color picker |
| `ptMidi` | MIDI data stream |
| `ptGainFader` | Gain fader (dB scale) |
| `ptAudio` | Audio signal stream |
| `ptDataField` | Numeric data field |
| `ptDataFader` | Data fader with range |
| `ptButton` | Push button |
| `ptListBox` | Dropdown list box |
| `ptSwitch` | Toggle switch |
| `ptArray` | Data array |
| `ptIpAddress` | IP address field |
| `ptSmpte` | SMPTE timecode |
| `ptMidiNoteFader` | MIDI note fader |
| `ptPointerBitMap` | Bitmap pointer |
| `ptPointer` | Raw typed pointer |
| `ptRightLed` | LED indicator (right side) |
| `ptLeftLed` | LED indicator (left side) |
| `ptTriggerLed` | Trigger LED |
| `ptChooseFolder` | Folder picker |
| `ptFileName` | File name picker |
| `ptBitwise` | Bitwise data |
| `ptCardinalHexa` | Hexadecimal value |
| `ptOther` | Other parameter type |

> See [[sdk/user-module-base|UserModuleBase]] for how to use `TParamType` in `onGetParamInfo`.

### TScale
Parameter value scaling.

| Value | Description |
|-------|-------------|
| `scLinear` | Linear scaling |
| `scLog` | Logarithmic scaling |
| `scExp` | Exponential scaling |

### TFastCallBackType
Callback processing modes (`typedef enum TFastCallBackType : int32_t`).

| Value | Thread | Description |
|-------|--------|-------------|
| `ctNormal` | Main thread | Processed in Usine's normal message handler (latency set in Setup) |
| `ctImmediate` | Audio thread | Fired immediately on change — no memory allocation allowed |
| `ctAsynchronous` | Window message handler | Slower than `ctNormal`, use for long processes or modal windows |
| `cttVideo` | Video thread | Processed in the video rendering thread |
| `cbtRealTime` | Real-time thread | Fast but not immediate real-time processing |
| `cbtGraphic` | Main thread (slower) | Lower priority graphic thread updates |
| `cbtNetwork` | Network thread | Dedicated real-time thread for network operations |
| `ctNone` | — | No callback, parameter changes are not notified |

### TThreadPriority
Thread priority levels.

| Value | Description |
|-------|-------------|
| `tpIDLE` | Idle priority |
| `tpLow` | Low priority |
| `tpMedium` | Medium priority |
| `tpRealTime` | Real-time priority |
| `tpTimeCritical` | Time-critical priority |

### TMouseButton
Mouse button identifiers.

| Value | Description |
|-------|-------------|
| `mbLeft` | Left mouse button |
| `mbRight` | Right mouse button |
| `mbMiddle` | Middle mouse button |

### TTextAlign
Text alignment options.

| Value | Description |
|-------|-------------|
| `taCenter` | Center aligned |
| `taLeading` | Left aligned |
| `taTrailing` | Right aligned |

### TNotifyUsineMode
Notification delivery modes.

| Value | Description |
|-------|-------------|
| `numMainThread` | Deliver on main thread |
| `numAsync` | Deliver asynchronously |
| `numVideo` | Deliver on video thread |
| `numRealTime` | Deliver on audio thread |
| `numGraphic` | Deliver on graphic thread |
| `numNetwork` | Deliver on network thread |

### TChunkOperation
Chunk data operations.

| Value | Description |
|-------|-------------|
| `coAddOrModifyItem` | Add or modify a chunk item |
| `coGetItem` | Read a chunk item |
| `coClearItem` | Clear a chunk item |
| `coDeleteItem` | Delete a chunk item |
| `coClearChunk` | Clear entire chunk |
| `coDeleteChunk` | Delete entire chunk |

### TFlowType
Data flow types for chunks.

| Value | Code | Description |
|-------|------|-------------|
| `ftNone` | `0` | No type |
| `ftDataFloat` | `1` | Float data |
| `ftDataInteger` | `8` | Integer data |
| `ftTextGeneral` | `2048` | Text data |
| `ftColor` | `131072` | Color data |

### TSettingInOutletMode
Setting inlet/outlet modes.

| Value | Description |
|-------|-------------|
| `siomInOut` | Both input and output |
| `siomIn` | Input only |
| `siomOut` | Output only |
| `siomNone` | No inlet/outlet |
| `sioOther` | Other mode |

---

## Message Constants

### Callback lParam Values

| Constant | Value | Description |
|----------|-------|-------------|
| `MSG_CHANGE` | `0` | Parameter value changed |
| `MSG_CLICK` | `1` | Mouse click |
| `MSG_DBLCLICK` | `2` | Double click |
| `MSG_SETFOCUS` | `3` | Parameter gained focus |
| `MSG_KILLFOCUS` | `4` | Parameter lost focus |
| `MSG_COMMATEXT` | `5` | Comma-text changed |
| `MSG_RESET` | `10` | Reset requested |
| `MSG_RANDOMIZE` | `11` | Randomize requested |
| `MSG_AFTERLOADING` | `12` | After loading complete |
| `MSG_CHUNK_DESTROYED` | `16` | Chunk was destroyed |
| `MSG_CHUNK_ITEM_LIST_CHANGED` | `17` | Chunk item list changed |

### Notification Targets

| Constant | Description |
|----------|-------------|
| `NOTIFY_TARGET_USINE` | Send to Usine core |
| `NOTIFY_TARGET_SETUP` | Send to setup |
| `NOTIFY_TARGET_USER_MODULE` | Send back to this module |

### Special Callback IDs

| Constant | Value | Description |
|----------|-------|-------------|
| `CALLBACK_JOB_END` | `0xFABA00` | Background job finished |
| `CALLBACK_WPARAM_LIMIT` | `0xF000000` | Max callback ID value |

---

## Shift State Bitmask

For `TShiftState` in mouse callbacks:

| Constant | Bit | Description |
|----------|-----|-------------|
| `ssShift` | `1` | Shift key |
| `ssAlt` | `2` | Alt key |
| `ssCtrl` | `4` | Ctrl key |
| `ssLeft` | `8` | Left button |
| `ssRight` | `16` | Right button |
| `ssMiddle` | `32` | Middle button |
| `ssDouble` | `64` | Double click |
| `ssTouch` | `128` | Touch input |
| `ssPen` | `256` | Pen input |
| `ssCommand` | `512` | Command key (macOS) |
| `ssHorizontal` | `1024` | Horizontal scroll |

---

## Color Constants

### Standard Colors

| Constant | Description |
|----------|-------------|
| `clWhite` | White |
| `clBlack` | Black |
| `clRed` | Red |
| `clGreen` | Green |
| `clBlue` | Blue |
| `clYellow` | Yellow |

### Usine Theme Colors

| Constant | Description |
|----------|-------------|
| `clMainBack` | Main background color |
| `clDataFlow` | Data flow color |
| `clMIDIFlow` | MIDI flow color |
| `clAudioFlow` | Audio flow color |
| `clVideoFlow` | Video flow color |
| `clArrayFlow` | Array flow color |

### Module Type Colors

| Constant | Description |
|----------|-------------|
| `clDataModuleColor` | Data module background |
| `clMIDIModuleColor` | MIDI module background |
| `clAudioModuleColor` | Audio module background |
| `clVideoModuleColor` | Video module background |
| `clArrayModuleColor` | Array module background |
| `clChunkModuleColor` | Chunk module background |
| `clInterfaceDesignModuleColor` | Interface/control module background |

### Grayscale

`clg0` through `clg11` — grayscale values from dark to light.

### Palette Colors

`cl0` through `cl23` — 24-color palette.

---

## MIDI Message Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `MIDI_ALLNOTESOFF` | `0x7B` | All Notes Off |
| `MIDI_NOTEON` | `0x90` | Note On |
| `MIDI_NOTEOFF` | `0x80` | Note Off |
| `MIDI_KEYAFTERTOUCH` | `0xA0` | Polyphonic Key Pressure |
| `MIDI_CONTROLCHANGE` | `0xB0` | Control Change |
| `MIDI_PROGRAMCHANGE` | `0xC0` | Program Change |
| `MIDI_CHANAFTERTOUCH` | `0xD0` | Channel Pressure |
| `MIDI_PITCHBEND` | `0xE0` | Pitch Bend |
| `MIDI_SYSTEMMESSAGE` | `0xF0` | System Exclusive |
| `MIDI_BEGINSYSEX` | `0xF0` | Begin SysEx |
| `MIDI_MTCQUARTERFRAME` | `0xF1` | MTC Quarter Frame |
| `MIDI_SONGPOSPTR` | `0xF2` | Song Position Pointer |
| `MIDI_SONGSELECT` | `0xF3` | Song Select |
| `MIDI_ENDSYSEX` | `0xF7` | End SysEx |
| `MIDI_TIMINGCLOCK` | `0xF8` | Timing Clock |
| `MIDI_START` | `0xFA` | Start |
| `MIDI_CONTINUE` | `0xFB` | Continue |
| `MIDI_STOP` | `0xFC` | Stop |
| `MIDI_ACTIVESENSING` | `0xFE` | Active Sensing |
| `MIDI_SYSTEMRESET` | `0xFF` | System Reset |

---

## Pointer Type Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `usinePtrFloat` / `USINE_POINTER_FLOAT` | `1` | Float pointer |
| `usinePtrInt` / `USINE_POINTER_INT` | `2` | Integer pointer |
| `usinePtrString` / `USINE_POINTER_STRING` | `3` | String pointer |
| `usinePtrMidi` / `USINE_POINTER_MIDI` | `4` | MIDI pointer |
| `usinePtrColor` / `USINE_POINTER_COLOR` | `5` | Color pointer |
| `usinePtrAudio` / `USINE_POINTER_AUDIO` | `6` | Audio pointer |
| `usinePtrBitwise` / `USINE_POINTER_BITWISE` | `7` | Bitwise pointer |
| `usinePtrChunk` | `8` | Chunk pointer |

---

## Settings Tab Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `PROPERTIES_TAB_NAME` | `"properties"` | Properties tab |
| `DESIGN_TAB_NAME` | `"design"` | Design tab |
| `COLORS_TAB_NAME` | `"colors"` | Colors tab |
| `MOUSE_TAB_NAME` | `"mouse"` | Mouse tab |
| `OTHER_TAB_NAME` | `"other"` | Other tab |
| `SCALE_TAB_NAME` | `"scale"` | Scale tab |
| `LAYOUT_TAB_NAME` | `"layout"` | Layout tab |
| `SIZE_TAB_NAME` | `"size"` | Size tab |

---

## Numeric Format Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `DEFAULT_FORMAT_FLOAT_1` | `"%.1f"` | 1 decimal place |
| `DEFAULT_FORMAT_FLOAT_2` | `"%.2f"` | 2 decimal places |
| `DEFAULT_FORMAT_FLOAT_3` | `"%.3f"` | 3 decimal places |
| `DEFAULT_FORMAT_FLOAT_4` | `"%.4f"` | 4 decimal places |
| `DEFAULT_FORMAT_FLOAT_5` | `"%.5f"` | 5 decimal places |
| `DEFAULT_FORMAT_FLOAT_6` | `"%.6f"` | 6 decimal places |
| `DEFAULT_FORMAT_INTEGER` | `"%.0f"` | Integer (no decimals) |
| `DEFAULT_FORMAT_GENERAL` | `"%g"` | General float format |

---

## Related Pages

- [[sdk/usine-event-class|UsineEventClass]] — Event data manipulation
- [[sdk/sdk-functions|SDK Functions]] — Utility functions
- [[sdk/utility-functions|Utility Functions]] — Color, geometry, math helpers
- [[sdk/user-module-base|UserModuleBase Class]] — Module base class
- [[sdk/module-architecture|Module Architecture]] — How modules are structured
