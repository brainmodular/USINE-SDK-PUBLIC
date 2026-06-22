# UserModuleBase Class Reference
#sdk #reference #class #callbacks #parameters #audio #drawing #threading #persistence #settings

`UserModuleBase` is the base class that every Usine user module inherits from. It provides virtual callback methods that Usine calls at various points in the module's lifecycle.

**Defined in:** `sdk/UserModule.h`  
**Implemented in:** `sdk/UserModule.cpp`

## Static Constants

| Constant | Type | Description |
|----------|------|-------------|
| `MODULE_NAME` | `AnsiCharPtr` | Module display name |
| `MODULE_DESC` | `AnsiCharPtr` | Module description |
| `MODULE_VERSION` | `AnsiCharPtr` | Module version string |

---

## Mandatory Callbacks

These must be implemented by every module.

### onGetModuleInfo
```cpp
virtual void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) = 0;
```
Fill in the `TModuleInfo` structure to describe your module's characteristics. Called once during module creation.

**Key fields to set:**
| Field | Type | Description |
|-------|------|-------------|
| `Name` | `AnsiCharPtr` | Module name shown in Usine |
| `Description` | `AnsiCharPtr` | Module description |
| `ModuleType` | `TModuleType` | `mtSimple`, `mtControl`, `mtVideo`, etc. |
| `BackColor` | `TUsineColor` | Module background color |
| `NumberOfParams` | `int` | Total number of parameters |
| `Version` | `AnsiCharPtr` | Version string |
| `DontProcess` | `LongBool` | `TRUE` to skip `onProcess()` calls |
| `CanRecord` | `LongBool` | `TRUE` to enable automation recording |
| `CanBeRandomized` | `LongBool` | `TRUE` to enable randomize feature |
| `CanBeReset` | `LongBool` | `TRUE` to enable reset feature |
| `CanBeSavedInPreset` | `LongBool` | `TRUE` to enable preset saving |
| `DefaultWidth` | `int` | Default module width |
| `DefaultHeight` | `int` | Default module height |
| `NumberOfVideoInputs` | `int` | Number of video inputs (for `mtVideo`) |
| `NumberOfVideoOutputs` | `int` | Number of video outputs (for `mtVideo`) |

**Query system fields:**
| Field | Description |
|-------|-------------|
| `QueryListString` | Comma-separated query option labels |
| `QueryListValues` | Comma-separated query values |
| `QueryListDefaultIdx` | Default selected index |
| `QueryLabel1` / `QueryLabel2` | Labels for query inputs |
| `QueryDefaultValue1` / `QueryDefaultValue2` | Default query values |
| `QueryMaxValue1` / `QueryMaxValue2` | Max query values |
| `QueryMinValue1` / `QueryMinValue2` | Min query values |

### onGetParamInfo
```cpp
virtual void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) = 0;
```
Fill in the `TParamInfo` structure for each parameter. Called once per parameter during setup.

**Key TParamInfo fields:**
| Field | Type | Description |
|-------|------|-------------|
| `ParamType` | `TParamType` | Parameter type (see Data Types) |
| `Caption` | `AnsiCharPtr` | Parameter label |
| `IsInput` | `LongBool` | `TRUE` for input parameter |
| `IsOutput` | `LongBool` | `TRUE` for output parameter |
| `MinValue` | `TPrecision` | Minimum value |
| `MaxValue` | `TPrecision` | Maximum value |
| `DefaultValue` | `TPrecision` | Default value |
| `Scale` | `TScale` | `scLinear`, `scLog`, or `scExp` |
| `Symbol` | `AnsiCharPtr` | Unit symbol (e.g., "Hz", "dB") |
| `Format` | `AnsiCharPtr` | Display format (e.g., "%.2f") |
| `Color` | `TUsineColor` | Parameter color |
| `ReadOnly` | `LongBool` | `TRUE` for output-only parameters |
| `CallBackType` | `TFastCallBackType` | `ctNormal`, `ctImmediate`, `ctAsynchronous`, `cttVideo`, `cbtRealTime`, `cbtGraphic`, `cbtNetwork`, `ctNone` |
| `CallBackId` | `NativeInt` | ID passed in `wParam` during callback |
| `DontSave` | `LongBool` | `TRUE` to exclude from save |
| `IsStoredInPreset` | `LongBool` | `TRUE` to include in presets |
| `IsSeparator` | `LongBool` | `TRUE` to display a section separator |
| `SeparatorCaption` | `AnsiCharPtr` | Separator label text |
| `IsInvisible` | `LongBool` | `TRUE` to hide the parameter |
| `DisplayOrder` | `int` | Display order override |
| `ListBoxStrings` | `AnsiCharPtr` | Comma-text for `ptListBox` |
| `TextValue` | `AnsiCharPtr` | Default text for `ptText` |
| `Translate` | `LongBool` | `TRUE` to translate caption |
| `FileNameFilter` | `AnsiCharPtr` | Filter for `ptFileName` |
| `PointerDataType` | `int` | Type for `ptPointer` params |
| `PointerDataInitialSize` | `int` | Initial size for pointer data |
| `AllowZeroLenAsInput` | `LongBool` | Allow zero-length events |

**Binding events:** Call `pParamInfo->setEventClass(myEvent)` to bind a `UsineEventClass` to the parameter.

### onCallBack
```cpp
virtual void onCallBack(TUsineMessage* Message);
```
Called when a parameter changes or a UI event occurs.

**TUsineMessage fields:**
| Field | Description |
|-------|-------------|
| `message` | Always `MSG_CHANGE` |
| `wParam` | The `CallBackId` of the parameter |
| `lParam` | Message type: `MSG_CHANGE`, `MSG_CLICK`, `MSG_DBLCLICK`, `MSG_SETFOCUS`, `MSG_KILLFOCUS`, `MSG_COMMATEXT`, `MSG_RESET`, `MSG_RANDOMIZE`, `MSG_AFTERLOADING`, `MSG_CHUNK_DESTROYED`, `MSG_CHUNK_ITEM_LIST_CHANGED` |

---

## Query System Callbacks

### onGetNumberOfParams
```cpp
virtual int onGetNumberOfParams(int queryResult1, int queryResult2);
```
Return the number of parameters based on user query popup selection. Used for dynamic parameter counts (e.g., multi-channel audio modules).

### onAfterQuery
```cpp
virtual void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo,
                          int queryResult1, int queryResult2);
```
Called after the query popup closes. Store query results for later use.

---

## Initialization

### onInitModule
```cpp
virtual void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
```
Called after all parameters are set up. Initialize resources, create threads, allocate buffers here.

---

## Real-Time Processing

### onProcess
```cpp
virtual void onProcess();
```
Called on the audio thread at every block tick. This is where audio/data processing happens.

**Critical rules:**
- No memory allocation
- No blocking I/O
- No locks (unless absolutely necessary and very brief)
- Keep as fast as possible

### onProcessVideo
```cpp
virtual void onProcessVideo();
```
Called by the video thread for `mtVideo` modules. Use `sdkGetInputFrame()` and `sdkSetOutputFrame()` here.

---

## Background Job System

### startJob
```cpp
void startJob();
```
Launches (or restarts) the background job thread. Call this to trigger async processing.

### onJobBegin
```cpp
virtual void onJobBegin();
```
Called on the job thread before processing begins. Set up job state here.

### onJobProcess
```cpp
virtual void onJobProcess();
```
Called on the job thread. Execute your long-running task here.

### onJobEnd
```cpp
virtual void onJobEnd();
```
Called on the **main thread** after the job thread finishes. Collect results and update outputs here.

> **Example:** See [[examples/background-job|BackgroundJob]] for a complete background job implementation.

---

## Audio Configuration

### onBlocSizeChange
```cpp
virtual void onBlocSizeChange(int BlocSize);
```
Called when the audio block size changes. Resize buffers as needed.

### onSampleRateChange
```cpp
virtual void onSampleRateChange(double SampleRate);
```
Called when the sample rate changes. Update timing calculations.

---

## Chunk System (Persistence)

### onGetChunkLen
```cpp
virtual int onGetChunkLen(LongBool Preset);
```
Return the estimated byte size of your chunk data. `Preset` indicates if saving a preset vs. full save.

### onGetChunk
```cpp
virtual void onGetChunk(void* chunk, LongBool Preset);
```
Write your module state into the chunk buffer.

### onSetChunk
```cpp
virtual void onSetChunk(const void* chunk, int sizeInBytes, LongBool Preset);
```
Restore your module state from the chunk buffer.

### onAfterLoading
```cpp
virtual void onAfterLoading();
```
Called after the module is fully loaded and all chunks are restored.

> **Examples:** See [[examples/draw-trajectory-box|DrawTrajectoryBox]] and [[examples/usine-chunks|UsineChunks]] for chunk system usage.

---

## Settings Panel

### onCreateSettings
```cpp
virtual void onCreateSettings();
```
Add entries to the module's Settings Panel using `sdkAddSettingLine*` methods.

### onSettingsHasChanged
```cpp
virtual void onSettingsHasChanged();
```
Called when any setting value changes. React accordingly.

---

## Graphics Canvas (mtControl)

### onResize
```cpp
virtual void onResize(float contentWidth, float contentHeight);
```
Called when the module canvas is resized.

### onPaint
```cpp
virtual void onPaint();
```
Draw the module canvas content. Use `sdkDraw*` and `sdkFill*` methods.

> **Example:** See [[examples/data-oscilloscope|DataOscilloscope]] for a graphics canvas implementation.

---

## Mouse Interaction

### onMouseMove
```cpp
virtual void onMouseMove(TShiftState Shift, float X, float Y);
```
Mouse moved over canvas. Coordinates are normalized (0.0 to 1.0).

### onMouseDown
```cpp
virtual void onMouseDown(TMouseButton Button, TShiftState Shift, float X, float Y);
```
Mouse button pressed.

### onMouseUp
```cpp
virtual void onMouseUp(TMouseButton Button, TShiftState Shift, float X, float Y);
```
Mouse button released.

### onMouseWheel
```cpp
virtual void onMouseWheel(TShiftState Shift, int WheelDelta);
```
Mouse wheel scrolled.

### Multi-Touch Variants
```cpp
virtual void onMouseMoveMulti(TShiftState Shift, UsineEventClass& touchPosX,
                               UsineEventClass& touchPosY, UsineEventClass& mouseBtn);
virtual void onMouseDownMulti(TShiftState Shift, UsineEventClass& touchPosX,
                               UsineEventClass& touchPosY, UsineEventClass& mouseBtn);
virtual void onMouseUpMulti(TShiftState Shift, UsineEventClass& touchPosX,
                             UsineEventClass& touchPosY, UsineEventClass& mouseBtn);
```

> **Examples:** See [[examples/draw-box|DrawBox]] and [[examples/simple-pad|SimplePad]] for mouse interaction.

---

## Contextual Menu

### onCreateCommands
```cpp
virtual void onCreateCommands();
```
Add entries using `sdkAddCommand()` and `sdkAddCommandSeparator()`.

---

## Automation Recording

### onSetRecordedValue
```cpp
virtual void onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z);
```
Called during playback of recorded automation curves.

---

## Randomize / Reset

### onRandomize
```cpp
virtual void onRandomize();
```
Called when the user triggers randomize. Randomize your module state.

### onReset
```cpp
virtual void onReset();
```
Called when the user triggers reset. Reset your module to default state.

---

## Quick Color

### onSetQuickColor
```cpp
virtual void onSetQuickColor(TUsineColor color);
```
Called when the user changes the module's global quick color.

---

## Child Window

### onOpenEditor / onBringToFront / onCloseEditor / onResizeEditor
```cpp
virtual void onOpenEditor();
virtual void onBringToFront();
virtual void onCloseEditor();
virtual void onResizeEditor(int width, int height);
```
Manage external child windows created with `sdkCreateDeskWindow()`.

---

## MIDI/DMX/Laser Output (Internal)

### onMidiSendOut
```cpp
virtual void onMidiSendOut(int DeviceID, TUsineMidiCode Code);
```

### onDMXSendOut
```cpp
virtual void onDMXSendOut(int deviceId, char* ByteArray, int len, int universeNum);
```

### onLaserSendOut
```cpp
virtual void onLaserSendOut(int DeviceID, TUsineILDAPoint** pointsArray,
                            int numOfPoints, int convergenceAngle);
```

---

## Drawing Utility Methods

These non-virtual methods are available for use in `onPaint()`:

| Method | Description |
|--------|-------------|
| `sdkDrawPoint(pos, color, size, rounded)` | Draw a single point |
| `sdkDrawMultiPoints(points, colors, size, rounded, count)` | Draw multiple points |
| `sdkDrawLine(p1, p2, color, width)` | Draw a line |
| `sdkDrawPolyLine(points, count, color, width)` | Draw a polyline |
| `sdkFillPolyLine(points, count, color)` | Fill a polygon |
| `sdkFillRect(rect, color, borderWidth, borderColor, radius)` | Fill a rectangle |
| `sdkFillText(rect, text, color, fontSize, bold, wordwrap, hAlign, vAlign)` | Draw text |
| `sdkBitBlit(pixels, W, H, rect)` | Blit a pixel buffer |

### Path Drawing
| Method | Description |
|--------|-------------|
| `sdkDrawPathStart()` | Begin a new path |
| `sdkDrawPathMoveTo(point)` | Move path cursor |
| `sdkDrawPathLineTo(point)` | Add line segment |
| `sdkDrawPathQuadCurveTo(control, end)` | Add quadratic bezier curve |
| `sdkDrawPathClose()` | Close the path |
| `sdkDrawPathDraw(color, width)` | Stroke the path |
| `sdkDrawPathFill(color)` | Fill the path |

### Other Utility Methods

| Method | Description |
|--------|-------------|
| `sdkRepaintPanel()` | Request canvas repaint |
| `sdkSetListBoxCommaText(param, text)` | Update listbox options |
| `sdkSetParamCaption(param, caption)` | Update parameter label |
| `sdkSetParamVisible(param, visible)` | Show/hide parameter |
| `sdkRecreateParam(param, paramInfo)` | Recreate a parameter |
| `sdkSetParamValueText(param, text)` | Set parameter display text |
| `sdkRepaintParam(param)` | Repaint a specific parameter |

---

## Protected Members

| Member | Type | Description |
|--------|------|-------------|
| `panelWidth` | `float` | Current canvas width |
| `panelHeight` | `float` | Current canvas height |
| `m_moduleInfo` | `TModuleInfo*` | Pointer to module info |
| `m_masterInfo` | `static TMasterInfo*` | Pointer to master info (shared) |

---

## Related Pages

- [[sdk/usine-event-class|UsineEventClass]] — Event data manipulation
- [[sdk/data-types|Data Types & Constants]] — Type definitions and enumerations
- [[sdk/sdk-functions|SDK Functions]] — Global utility functions
- [[sdk/module-architecture|Module Architecture]] — How modules are structured
- [[sdk/utility-functions|Utility Functions]] — Color, geometry, math helpers
- [[getting-started|Getting Started]] — Getting started with the SDK
