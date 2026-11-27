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

The constants are declared by the SDK and defined by each module, in its `ModuleDefinition.h` (see [[sdk/module-conventions|Module Conventions]]):

```cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "my module";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "converts a frequency to a period";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";
```

The SDK uses them to fill `Name`, `Description` and `Version` of `TModuleInfo`: in `GetBrowserInfo`, which the SDK implements, and before calling `onGetModuleInfo`. The module doesn't set these fields itself. The Usine trace also shows `MODULE_NAME` in front of the messages of the module.

The three constants are declared `USINE_MODULE_LOCAL` (hidden visibility on macOS and Linux, nothing on Windows). A `constexpr` static data member is an inline variable, hence a weak symbol, and the dynamic linker unifies the weak definitions of the whole process: without the attribute, `GetBrowserInfo` — compiled in `UserModule.cpp`, another translation unit of the same library — binds to the copy of the first module loaded, and every module reports its name, description and version.

---

## Module Description and Parameters

`onGetModuleInfo` must be implemented by every module, `onCreateParameters` by every module that has parameters.

### onGetModuleInfo
```cpp
virtual void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) = 0;
```
Fill in the `TModuleInfo` structure to describe your module's characteristics. Called once during module creation.

`Name`, `Description` and `Version` are already set by the SDK from the `MODULE_*` constants. Every field has a default value: set only the ones that differ.

```cpp
void AudioVolume::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType          = mtSimple;
	pModuleInfo->BackColor           = sdkGetUsineColor(clAudioModuleColor);
	pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx = 1;
	pModuleInfo->CanBeSavedInPreset  = FALSE;
}
```

**Key fields:**
| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `ModuleType` | `TModuleType` | — | `mtSimple`, `mtControl`, `mtVideo`, `mtBackgroundJob`... always set it |
| `BackColor` | `TUsineColor` | `0` | Module background color |
| `DontProcess` | `LongBool` | `FALSE` | `TRUE` to skip `onProcess()` calls |
| `CanRecord` | `LongBool` | `FALSE` | `TRUE` to enable automation recording |
| `CanBeRandomized` | `LongBool` | `FALSE` | `TRUE` to enable randomize feature |
| `CanBeReset` | `LongBool` | `FALSE` | `TRUE` to enable reset feature |
| `CanBeSavedInPreset` | `LongBool` | `TRUE` | `FALSE` to keep the module out of the presets |
| `DefaultWidth` | `float` | `100` | Default panel width in pixels (`mtControl`) |
| `DefaultHeight` | `float` | `100` | Default panel height in pixels (`mtControl`) |
| `NumberOfVideoInputs` | `int` | `0` | Number of video inputs (for `mtVideo`), 0 to 2 |
| `NumberOfVideoOutputs` | `int` | `0` | Number of video outputs (for `mtVideo`), 0 to 2 |
| `Name` | `AnsiCharPtr` | `MODULE_NAME` | Set by the SDK |
| `Description` | `AnsiCharPtr` | `MODULE_DESC` | Set by the SDK |
| `Version` | `AnsiCharPtr` | `MODULE_VERSION` | Set by the SDK |
| `NumberOfParams` | `int` | `0` | Not set with `onCreateParameters`, only used by the legacy `onGetParamInfo` |

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

### onCreateParameters
```cpp
virtual void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
```
Create the parameters of the module with `sdkCreateParam`. Called after `onAfterQuery`, so the number of parameters can depend on the query.

```cpp
void sdkCreateParam(const TParamInfo& paramInfo, UsineEventClass& eventClass);
```
Creates a parameter from its description and binds it to `eventClass`. The parameters appear in the order of the calls.

```cpp
void AudioVolume::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(gainFdrInParamInfo, gainFdrIn);
	sdkCreateParam(muteSwInParamInfo, muteSwIn);
}
```

The `TParamInfo` are `constexpr` structures written with designated initializers in `ModuleDefinition.h`, never `inline`: each module must keep its own copy (see [[sdk/module-conventions|Module Conventions]]). The fields must follow their declaration order, the ones left out keep their default value:

```cpp
constexpr TParamInfo gainFdrInParamInfo {
	.ParamType        = ptGainFader,
	.Caption          = "gain",
	.IsInput          = TRUE,
	.IsOutput         = FALSE,
	.CallBackType     = ctImmediate,
	.IsSeparator      = TRUE,
	.SeparatorCaption = "volume",
	.CallBackId       = GAIN_FDR_CBID
};
```

**Key TParamInfo fields**, in their declaration order:
| Field | Type | Default | Description |
|-------|------|---------|-------------|
| `ParamType` | `TParamType` | `ptText` | Parameter type (see Data Types) |
| `Caption` | `AnsiCharPtr` | `nullptr` | Parameter label |
| `IsInput` | `LongBool` | `FALSE` | `TRUE` for input parameter |
| `IsOutput` | `LongBool` | `FALSE` | `TRUE` for output parameter |
| `Scale` | `TScale` | `scLinear` | `scLinear`, `scLog`, or `scExp` |
| `Color` | `TUsineColor` | `0` | Parameter color |
| `OffColor` | `TUsineColor` | `0` | Off color of a switch |
| `MinValue` | `TPrecision` | `0` | Minimum value |
| `MaxValue` | `TPrecision` | `1` | Maximum value |
| `DefaultValue` | `TPrecision` | `0` | Default value |
| `Symbol` | `AnsiCharPtr` | `nullptr` | Unit symbol (e.g., "Hz", "dB") |
| `Format` | `AnsiCharPtr` | `nullptr` | Display format (e.g., "%.2f") |
| `ComboBoxStrings` | `AnsiCharPtr` | `nullptr` | Comma-text for `ptComboBox` |
| `TextValue` | `AnsiCharPtr` | `nullptr` | Default text for `ptText` |
| `ReadOnly` | `LongBool` | `FALSE` | `TRUE` for output-only parameters |
| `CallBackType` | `TFastCallBackType` | `ctNormal` | `ctNormal`, `ctImmediate`, `ctAsynchronous`, `cttVideo`, `cbtRealTime`, `cbtGraphic`, `cbtNetwork`, `ctNone` |
| `DontSave` | `LongBool` | `FALSE` | `TRUE` to exclude from save |
| `DisplayOrder` | `int` | `-1` | **Deprecated**: the parameters are ordered by creation |
| `IsSeparator` | `LongBool` | `FALSE` | `TRUE` to display a section separator |
| `IsInvisible` | `LongBool` | `FALSE` | `TRUE` to hide the parameter |
| `IsStoredInPreset` | `LongBool` | `FALSE` | `TRUE` to include in presets |
| `SavedName` | `AnsiCharPtr` | `nullptr` | Name used to save the value, `Caption` if `nullptr` |
| `SeparatorCaption` | `AnsiCharPtr` | `nullptr` | Separator label text |
| `CallBackId` | `NativeInt` | `-1` | ID passed in `wParam` during callback, below `0xF000000` |
| `IsVisibleByDefault` | `LongBool` | `TRUE` | Visible when the module is created |
| `Translate` | `LongBool` | `FALSE` | `TRUE` to translate caption |
| `FileNameFilter` | `AnsiCharPtr` | `nullptr` | Filter for `ptFileName` |
| `AllowZeroLenAsInput` | `LongBool` | `FALSE` | Allow zero-length events |
| `PointerDataType` | `UINT32` | `0` | Type for `ptPointer` params |
| `PointerDataInitialSize` | `UINT32` | `0` | Initial size for pointer data |

A parameter without callback must say it with `.CallBackType = ctNone`: the default is `ctNormal`.

### onGetParamInfo (legacy)
```cpp
virtual void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo);
```
The former way to declare parameters: Usine calls it `NumberOfParams` times and the module binds each event with `pParamInfo->setEventClass(myEvent)`. Still supported, new modules use `onCreateParameters`.

### onCallBack
```cpp
virtual void onCallBack(TUsineMessage* Message);
```
Called when a parameter changes or a UI event occurs.

**TUsineMessage fields:**
| Field | Description |
|-------|-------------|
| `message` | `NOTIFY_MSG_USINE_CALLBACK` for the messages sent to the module |
| `wParam` | The `CallBackId` of the parameter, or the id given to `sdkAddCommand`, `sdkChunkSubscribe` or `sdkNotifyUsine` |
| `lParam` | Message type: `MSG_CHANGE`, `MSG_CLICK`, `MSG_DBLCLICK`, `MSG_SETFOCUS`, `MSG_KILLFOCUS`, `MSG_COMMATEXT`, `MSG_RESET`, `MSG_RANDOMIZE`, `MSG_AFTERLOADING`, `MSG_CHUNK_DESTROYED`, `MSG_CHUNK_ITEM_LIST_CHANGED` |

```cpp
void MyModule::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == FREQUENCY_FDR_CBID)
		computePeriod();
}
```

> **Note:** an exception thrown in `onCallBack` is caught by the SDK and traced as an error with the callback id.

---

## Query System Callbacks

### onAfterQuery
```cpp
virtual void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo,
                          int queryResult1, int queryResult2);
```
Called after the query popup closes, before `onCreateParameters`. Store the query results, they decide how many parameters to create.

### onGetNumberOfParams (legacy)
```cpp
virtual int onGetNumberOfParams(int queryResult1, int queryResult2);
```
Returns the number of parameters for the legacy `onGetParamInfo` system. Not needed with `onCreateParameters`.

---

## Initialization

### onInitModule
```cpp
virtual void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
```
Called after all parameters are set up. Initialize resources, create threads, allocate buffers here: everything that needs Usine (`sdkGetUsineColor`, `sdkGetBlocSize`, `createEvent`...) belongs here rather than in the constructor.

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

### Video frames

A `TUsineFrame` and its `Pixels` are valid only during the call of `onProcessVideo` that obtained them, unless the frame is permanent.

- **Input frames** (`sdkGetInputFrame`) and **frames that are not permanent** (`sdkGetNewFrame` with `permanent = FALSE`) belong to the frame pool of Usine. Once `onProcessVideo` has returned, Usine hands the frame to other modules, which overwrite its pixels. When the frame has stayed unused for a few seconds, Usine gives its pixels back to the system: `Pixels` then points to memory that no longer exists, and reading or writing it crashes Usine. Never keep such a frame, or its `Pixels` pointer, for a later call of `onProcessVideo`, for `onProcess`, `onPaint` (`sdkBitBlit`) or another thread.
- **To keep a picture from one call to the next** (the previous frame, a delay, a freeze), copy it into a **permanent frame**: `sdkGetNewFrame` with `permanent = TRUE`, then `sdkCopyFrame`. Release it with `sdkReleaseFrame` when it is replaced and in the destructor of the module: a permanent frame that is never released stays in memory until Usine quits.
- `sdkReleaseFrame` is only for the permanent frames of the module. Never release an input frame: it belongs to the module that sends it. Read the pixels of an input, never write them.
- **Test the size** before reading or writing the pixels. An input that receives no picture gives `Valid = FALSE`, `Width = Height = 0` and `Pixels = nullptr`. A new frame whose allocation failed has a size of 0 x 0. `sdkGetFramePixel` and `sdkSetFramePixel` check nothing.
- A new frame may be one that Usine used before: **its pixels are not cleared**. Write all of them, or clear it with `sdkClearFrame`.
- Once a frame is set with `sdkSetOutputFrame`, the next modules and the video outputs read it: do not change its pixels any more, take a new frame for the next picture. A frame of 0 x 0 pixels tells them that there is no picture.

Keeping the previous frame, for a frame difference or a feedback:

```cpp
// in the class: TUsineFrame prevFrame{}; bool hasPrev = false;

void MyVideoModule::onProcessVideo()
{
    TUsineFrame frameIn;
    TUsineFrame frameOut;
    sdkGetInputFrame(1, &frameIn);
    if (!frameIn.Valid || frameIn.Width <= 0 || frameIn.Height <= 0)
    {
        // no picture: an empty output, rather than the frame of a former call
        sdkGetNewFrame(&frameOut, 0, 0, FALSE);
        sdkSetOutputFrame(1, &frameOut);
        return;
    }

    sdkGetNewFrame(&frameOut, frameIn.Width, frameIn.Height, FALSE);
    if (frameOut.Width != frameIn.Width || frameOut.Height != frameIn.Height)
        return; // the allocation failed
    // ... compute frameOut from frameIn and prevFrame (when hasPrev and of the same size) ...
    sdkSetOutputFrame(1, &frameOut);

    // keep a copy of this input for the next call, in a permanent frame of the module
    if (!hasPrev || prevFrame.Width != frameIn.Width || prevFrame.Height != frameIn.Height)
    {
        if (hasPrev)
            sdkReleaseFrame(&prevFrame);
        sdkGetNewFrame(&prevFrame, frameIn.Width, frameIn.Height, TRUE);
        hasPrev = true;
    }
    sdkCopyFrame(&frameIn, &prevFrame);
}

MyVideoModule::~MyVideoModule()
{
    if (hasPrev)
        sdkReleaseFrame(&prevFrame);
}
```

> **Example:** [[examples/video-dimmer|VideoDimmer]] processes its frames this way: local to `onProcessVideo`, never kept.

---

## Background Job System

### startJob
```cpp
virtual void startJob();
```
Asks Usine to start the background job: the SDK then runs `onJobBegin` and `onJobProcess` on the job thread. A start requested while a job is running waits for its end.

### onJobBegin
```cpp
virtual void onJobBegin();
```
Called on the job thread before processing begins. Set up job state here.

### onJobProcess
```cpp
virtual void onJobProcess();
```
Called on the job thread. Execute your long-running work here.

### onJobEnd
```cpp
virtual void onJobEnd();
```
Called after the job thread finishes, through a notification the SDK sends to Usine on the main thread queue (`numMainThread`), delayed by the duration of one audio block (`sdkGetBlockDuration`). It runs on the main thread, one at a time with the `ctNormal` parameter callbacks and never during the destruction of the module; it is neither the job thread nor the audio thread. Collect results and update outputs here.

### Automatic job parameters (`mtBackgroundJob`)
When `onGetModuleInfo` sets `pModuleInfo->ModuleType = mtBackgroundJob`, the SDK creates two output parameters, **after** the parameters created in `onCreateParameters` (user param indices don't shift):

| Name | Type | Behaviour |
|------|------|-----------|
| `processing` | `ptRightLed` | Goes to 1 when the job starts and back to 0 after `onJobEnd`. |
| `processing time` | `ptDataField` (ms) | Duration of the last job (`onJobBegin` + `onJobProcess`), written once at job end. |

Both are read-only and not saved. The module must not create them itself.

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

### onGetStateChunkLen
```cpp
virtual int onGetStateChunkLen(LongBool Preset);
```
Return the byte size of your chunk data. `Preset` indicates if saving a preset vs. full save.

### onGetStateChunk
```cpp
virtual void onGetStateChunk(void* chunk, LongBool Preset);
```
Write your module state into the chunk buffer.

### onSetStateChunk
```cpp
virtual void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset);
```
Restore your module state from the chunk buffer. The chunk comes from a file: check `sizeInBytes` and every count read from it before using them, and raise an error if they are invalid (see [[examples/draw-trajectory-box|DrawTrajectoryBox]]).

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
virtual void onMouseMoveMulti(TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed);
virtual void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift,
                              UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed);
virtual void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift,
                            UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed);
```
Each variant also exists with `UsineEventPtr` arguments, Usine calls both: override only one of them.

> **Examples:** See [[examples/draw-box|DrawBox]] and [[examples/simple-pad|SimplePad]] for mouse interaction.

---

## Drag and Drop

### onAcceptDragOver
```cpp
virtual LongBool onAcceptDragOver(AnsiCharPtr FilesList, float X, float Y);
```
Called at each move of a drag over the panel. Return `TRUE` to accept the drop, the default returns `FALSE`. `FilesList` is empty when the drag holds no file.

### onDragDrop
```cpp
virtual void onDragDrop(AnsiCharPtr FilesList, float X, float Y);
```
Called when the drag is dropped on the panel, if `onAcceptDragOver` accepted it.

`FilesList` is a UTF-8 Delphi comma-text, only valid during the call: a name holding a space, a comma or a quote is quoted, and its quotes are doubled. `X` and `Y` are normalized (0.0 to 1.0), like the mouse coordinates. There is no drag leave callback.

> **Example:** See [[examples/drag-and-drop|DragAndDrop]].

---

## Contextual Menu

### onCreateCommands
```cpp
virtual void onCreateCommands();
```
Add entries using `sdkAddCommand()` and `sdkAddCommandSeparator()`. A command calls `onCallBack` with the callback id given to `sdkAddCommand`: reuse the id of the matching button so both share the same code.

```cpp
void DrawBox::onCreateCommands()
{
	sdkAddCommandSeparator("cmd_edit");
	sdkAddCommand("cmd_reset", RESET_BTN_CBID);
	sdkAddCommand("cmd_randomize", RANDOMIZE_BTN_CBID);
}
```

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

### onMidiSendOut / onMidiSendOutArray / onMidiSysexSendOut
```cpp
virtual void onMidiSendOut(int DeviceID, TUsineMidiCode Code);
virtual void onMidiSendOutArray(int DeviceID, TUsineMidiCode** arrayCode, int arraySize);
virtual void onMidiSysexSendOut(int DeviceID, char** Sysex, int sysexSize);
```

### onDMXSendOut
```cpp
virtual void onDMXSendOut(int deviceId, char* ByteArray, int len, int universeNum = 1);
```

### onLaserSendOut
```cpp
virtual void onLaserSendOut(int DeviceID, TUsineILDAPoint** arrayPoint,
                            int arraySize, int speedPPS);
```
`speedPPS` is the speed in points per second.

---

## Errors, Warnings and Infos

Defined in `sdk/UserError.h`. A module registers its messages once, then raises them:

```cpp
inline const int ERROR_INVALID_CHUNK USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "invalid saved data, it was not restored");

sdkReport(ERROR_INVALID_CHUNK);                    // the registered message
sdkReport(WARNING_CHUNK_DESTROYED, CHUNK_NAME);    // with an extra text appended
sdkReport(WARNING_WRITE, true);                    // also written in the Usine log file
sdkSetVerbosity(UReportVerbosity::Warn);           // hides the infos of this instance
```

`sdkReport`, `sdkSetVerbosity` and `sdkGetVerbosity` are members: each instance filters its messages with its own `verbosity level` combo box, added by the SDK (hidden by default, in the `Report` section, saved with the patch). From a static function, call them through the instance: `module->sdkReport(...)`.

Use it instead of the raw `sdkTrace*` functions. See [[examples/error-handling|ErrorHandling]].

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
| `getModuleType()` | The `ModuleType` set in `onGetModuleInfo` |

`param` is the index of the parameter: its rank in the creation order of `onCreateParameters`.

---

## Members

| Member | Access | Type | Description |
|--------|--------|------|-------------|
| `panelWidth` | protected | `float` | Current canvas width |
| `panelHeight` | protected | `float` | Current canvas height |
| `m_moduleInfo` | protected | `TModuleInfo*` | Pointer to module info |
| `m_masterInfo` | public | `static TMasterInfo*` | Pointer to master info (shared) |

---

## Related Pages

- [[sdk/module-conventions|Module Conventions]] — Files, naming and style of a module
- [[sdk/usine-event-class|UsineEventClass]] — Event data manipulation
- [[sdk/data-types|Data Types & Constants]] — Type definitions and enumerations
- [[sdk/sdk-functions|SDK Functions]] — Global utility functions
- [[sdk/module-architecture|Module Architecture]] — How modules are structured
- [[sdk/utility-functions|Utility Functions]] — Color, geometry, math helpers
- [[getting-started|Getting Started]] — Getting started with the SDK
