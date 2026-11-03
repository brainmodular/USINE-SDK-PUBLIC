# SDK Functions Reference
#sdk #reference #functions #audio #threading #files #fft #trace #dialog #math #chunks

Global utility functions available to all modules. These are inline wrappers around `TMasterInfo` function pointers.

**Defined in:** `sdk/UsineFunctions.h`

---

## Synchronization Objects

| Function | Description |
|----------|-------------|
| `sdkSyncObjectCreate()` | Create a sync object. Returns `TSyncObjectPtr` |
| `sdkSyncObjectDestroy(TSyncObjectPtr)` | Destroy a sync object |
| `sdkSyncObjectSet(TSyncObjectPtr)` | Signal the sync object |
| `sdkSyncObjectReset(TSyncObjectPtr)` | Reset the sync object |
| `sdkSyncObjectWait(TSyncObjectPtr, UINT32 timeout)` | Wait for signal with timeout (ms) |

> **Example:** See [[examples/ring-mod-multithread|RingModMultithread]] for synchronization object usage.

## Audio Channel Query

These functions support the multi-channel audio query system:

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetAudioQueryTitle()` | `AnsiCharPtr` | Get the query popup title |
| `sdkGetAudioQueryChannelList()` | `AnsiCharPtr` | Get comma-separated channel options |
| `sdkGetAudioQueryToNbChannels(int queryIdx)` | `int` | Convert query index to channel count |
| `sdkGetAudioQueryChannelNames(prefix, idx, queryIdx)` | `AnsiCharPtr` | Get channel name for display |

### Usage
```cpp
void AudioVolume::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->QueryListString     = sdkGetAudioQueryTitle();
	pModuleInfo->QueryListValues     = sdkGetAudioQueryChannelList();
	pModuleInfo->QueryListDefaultIdx = 1;
}

void AudioVolume::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	queryResult       = queryResult1;
	numOfAudioInsOuts = std::min(sdkGetAudioQueryToNbChannels(queryResult1), static_cast<int>(USINE_AUDIO_CHANNEL_MODULE_MAX));
}

void AudioVolume::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	for (int num = 0; num < numOfAudioInsOuts; num++)
	{
		TParamInfo audioIn = audioInParamInfo;
		audioIn.Caption    = sdkGetAudioQueryChannelNames("in ", num + 1, queryResult);
		sdkCreateParam(audioIn, audioInputs[num]);
	}
}
```

> **Example:** See [[examples/audio-volume|AudioVolume]] for audio channel query usage.

---

## Global Array Functions

Shared data arrays accessible across all modules in a workspace.

| Function | Description |
|----------|-------------|
| `sdkGlobalArrayGetHash(AnsiCharPtr name)` | Get hash for named global array. Returns `UINT64` |
| `sdkGlobalArraySetValueFloat(UINT64 hash, int index, TPrecision value, AnsiCharPtr errorMsg)` | Set float value at index |
| `sdkGlobalArrayGetValueFloat(UINT64 hash, int index, AnsiCharPtr errorMsg)` | Get float value at index. Returns `TPrecision` |
| `sdkGlobalArraySetValueColor(UINT64 hash, int index, TUsineColor value, AnsiCharPtr errorMsg)` | Set color value at index |
| `sdkGlobalArrayGetValueColor(UINT64 hash, int index, AnsiCharPtr errorMsg)` | Get color value at index. Returns `TUsineColor` |
| `sdkGlobalArraySetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr value, AnsiCharPtr errorMsg)` | Set string value at index |
| `sdkGlobalArrayGetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr errorMsg)` | Get string value at index. Returns `AnsiCharPtr` |
| `sdkGlobalArraySetValueEvt(UINT64 hash, UsineEventPtr value, AnsiCharPtr errorMsg)` | Set entire event |
| `sdkGlobalArrayGetValueEvt(UINT64 hash, AnsiCharPtr errorMsg, UsineEventPtr result)` | Get entire event |
| `sdkGlobalArrayGetSize(UINT64 hash, AnsiCharPtr errorMsg)` | Get array size. Returns `TPrecision` |
| `sdkGlobalArrayGetAllNames()` | Get all names as comma-text. Returns `AnsiCharPtr`. Can be slow |

`errorMsg` is the text Usine reports if the operation fails: the examples give the name of the array.

### Usage
```cpp
UINT64 hash = sdkGlobalArrayGetHash("my-shared-data");
sdkGlobalArraySetValueFloat(hash, 0, 42.0f, "my-shared-data");
float val = sdkGlobalArrayGetValueFloat(hash, 0, "my-shared-data");  // 42.0
```

> **Example:** See [[examples/global-array|GlobalArray]] for global array usage.

---

## Chunks

Named key-value stores shared between modules, with change notifications. See [[examples/usine-chunks|UsineChunks]].

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetChunk(AnsiCharPtr chunkName)` | `TUsineChunkPtr` | Get a chunk by name. Creates it if it doesn't exist |
| `sdkChunkExist(AnsiCharPtr chunkName)` | `LongBool` | Check if a chunk exists. Never creates it |
| `sdkChunkItemExist(AnsiCharPtr chunkName, AnsiCharPtr itemName)` | `LongBool` | Check if an item exists within a chunk. Never creates anything |
| `sdkGetChunkItemCount(TUsineChunkPtr chunk)` | `int` | Number of items in the chunk |
| `sdkChunkOperation(chunk, itemName, flowType, payload, operation, index = -1)` | `void` | Read/write/clear/delete chunk items. See `TChunkOperation` in [[sdk/data-types|Data Types]] |
| `sdkChunkSubscribe(chunk, itemName, callbackID, immediate)` | `void` | Receive notifications in `onCallBack` when the chunk or item changes |
| `sdkChunkUnSubscribe(chunk, itemName)` | `void` | Stop notifications for a chunk or item |
| `sdkChunkUnSubscribeAll()` | `void` | Cancel every chunk subscription of the module |
| `sdkSetChunkFromFloatPointer(chunk, itemName, float* payload, UINT32 size)` | `LongBool` | Write `size` floats in an item in one call, without an event |
| `sdkGetChunkItemSize(chunk, itemName)` | `UINT32` | Number of values of an item |
| `sdkGetChunkToFloatPointer(chunk, itemName, float* payload)` | `UINT32` | Copy a float item into `payload`, which must hold `sdkGetChunkItemSize` values |

These are methods of `UserModuleBase`, call them from the module.

### Usage
```cpp
// check existence without creating anything
if (sdkChunkItemExist("mychunk", "item1"))
{
    TUsineChunkPtr chunk = sdkGetChunk("mychunk");
    sdkChunkOperation(chunk, "item1", ftDataFloat, outputEvent, coGetItem, -1);
}

// subscribe to changes (fires callbackID in onCallBack)
sdkChunkSubscribe(sdkGetChunk("mychunk"), "item1", MY_CALLBACK_ID, TRUE);

// a whole float buffer, written and read back in one call
TUsineChunkPtr chunk = sdkGetChunk("mychunk");
sdkSetChunkFromFloatPointer(chunk, "data_bulk", buffer.data(), static_cast<UINT32>(buffer.size()));
std::vector<float> data(sdkGetChunkItemSize(chunk, "data_bulk"));
sdkGetChunkToFloatPointer(chunk, "data_bulk", data.data());

// cleanup in the destructor
sdkChunkUnSubscribeAll();
```

### UsineChunk reader

`sdk/UsineChunks.h` holds `UsineChunk`, a helper that reads typed values from a named chunk in one call. Reading never creates the chunk nor the item: the getters return `false`, or the fallback value, when one is missing. The header is not included by `UserDefinitions.h`, include it yourself.

```cpp
#include "../../sdk/UsineChunks.h"

UsineChunk reader(this, "mychunk");

std::vector<float> values;
if (reader.getFloats("item1", values))
{
    // ...
}
int         count = reader.getInt("count", 0);
std::string names;
reader.getText("commatxt", names);
```

| Method | Returns | Description |
|--------|---------|-------------|
| `exists()` | `bool` | The chunk exists |
| `has(item)` | `bool` | The chunk and the item exist |
| `getFloats(item, std::vector<float>&)` / `getInts(item, std::vector<int>&)` | `bool` | Read a float or integer array |
| `getText(item, std::string&)` | `bool` | Read a text (a comma text is returned as is) |
| `getFloat(item, fallback)` / `getInt(item, fallback)` | `float` / `int` | First value of the item, or `fallback` |

A `UsineChunk` is not thread-safe: it reuses one event for every read.

> **Example:** See [[examples/usine-chunks|UsineChunks]] for chunk system usage.

---

## Usine Core Information

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetBlocSize()` | `int` | Current audio block size, in samples |
| `sdkGetBlockDuration()` | `double` | Duration of an audio block, in ms |
| `sdkGetSampleRate()` | `double` | Current sample rate |
| `sdkGetUsineBlockSizeList()` | `AnsiCharPtr` | Available block sizes |
| `sdkGetUsineDefaultBlockSizeIndex()` | `int` | Default block size index |
| `sdkGetUsineColor(UINT32 colorIndex)` | `TUsineColor` | Get Usine theme color |
| `sdkGetTrans(AnsiCharPtr stringID)` | `AnsiCharPtr` | Get translated string |
| `sdkGetUsineLanguage()` | `AnsiCharPtr` | Current UI language |
| `sdkGetUsineSaveVersion()` | `int` | File format version |
| `sdkGetUsineMaxSoundCardAudioInputs()` | `int` | Max audio inputs |
| `sdkGetUsineMaxSoundCardAudioOutputs()` | `int` | Max audio outputs |
| `sdkGetUsineMaxMidiDevices()` | `int` | Max MIDI devices |
| `sdkGetUsineMaxPolyphony()` | `int` | Max polyphony |

### Platform-Specific

| Function | Platform | Returns | Description |
|----------|----------|---------|-------------|
| `sdkGetUsineMainWindow()` | Windows | `NativeInt` | Main window handle (HWND) |
| `sdkGetUsineNSView()` | macOS | `NativeInt` | NSView pointer |
| `sdkGetUsineNSApplication()` | macOS | `NativeInt` | NSApplication pointer |

---

## Usine Paths

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetUsineSoundPath()` | `AnsiCharPtr` | Default sound files directory |
| `sdkGetUserLibPath()` | `AnsiCharPtr` | User library path |
| `sdkGetUsineRecordPath()` | `AnsiCharPtr` | Recording output directory |
| `sdkGetGlobalApplicationPath()` | `AnsiCharPtr` | Usine application path |
| `sdkGetCurrentWorkspacePath()` | `AnsiCharPtr` | Current workspace path |
| `sdkGetCurrentPatchPath()` | `AnsiCharPtr` | Current patch path |
| `sdkGetUsineTempPath()` | `AnsiCharPtr` | Temporary files path |

---

## File Utilities

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkProcessOpenDialog(filename, initialDir, filter)` | `LongBool` | Show file open dialog |
| `sdkProcessSaveDialog(filename, initialDir, filter)` | `LongBool` | Show file save dialog |
| `sdkFindFile(filename, initialDir)` | `LongBool` | Search for a file |

---

## Audio File Manipulation

### Lifecycle
| Function | Description |
|----------|-------------|
| `sdkCreateAudioFile()` | Create audio file handle. Returns `TAudioFilePtr` |
| `sdkDestroyAudioFile(TAudioFilePtr)` | Destroy audio file handle |
| `sdkLoadInMemoryAudioFile(ptr, filename, optimized)` | Load file entirely into memory |
| `sdkLoadStayOnDiskAudioFile(ptr, filename, optimized)` | Load file with disk streaming |
| `sdkSaveToDiskAudioFile(ptr, filename)` | Save audio file to disk |
| `sdkClearAudioFile(ptr)` | Clear audio file contents |

### Properties
| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetSizeAudioFile(ptr)` | `int` | Total number of samples |
| `sdkGetChannelAudioFile(ptr)` | `int` | Number of channels |
| `sdkGetSampleRateAudioFile(ptr)` | `double` | Sample rate |
| `sdkGetBitPerSampleAudioFile(ptr)` | `int` | Bits per sample |

### Sample Access
| Function | Description |
|----------|-------------|
| `sdkGetSampleAudioFile(ptr, channel, index)` | Get single sample. Returns `TPrecision` |
| `sdkGetSampleArrayAudioFile(ptr, channel, index, dest, size)` | Get sample array |
| `sdkGetBlocSampleAudioFile(ptr, channel, index, dest)` | Get block-sized samples |
| `sdkSetSampleAudioFile(ptr, channel, index, value)` | Set single sample |
| `sdkGetPeaksAudioFile(ptr, channel, index, length)` | Get peak value. Returns `TPrecision` |

### Modification
| Function | Description |
|----------|-------------|
| `sdkSetChannelsAudioFile(ptr, channels)` | Set number of channels |
| `sdkSetSizeAudioFile(ptr, size)` | Set total sample count |
| `sdkResampleAudioFile(ptr, factor)` | Resample audio file |

---

## Math Utilities

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkLinearInterpolation(f, a, b)` | `TPrecision` | Linear interpolation between a and b |
| `sdkCubicInterpolation(fr, a, b, c, d)` | `TPrecision` | Cubic interpolation |
| `sdkSplineInterpolation(fr, a, b, c, d)` | `TPrecision` | Spline interpolation |

---

## FFT Functions

| Function | Description |
|----------|-------------|
| `sdkFFTCreate(UINT32 size)` | Create FFT object. Returns `TUsineFFTPtr` |
| `sdkFFTDestroy(TUsineFFTPtr)` | Destroy FFT object |
| `sdkFFTForward(TUsineFFTPtr, float* audio, float* re, float* im)` | Forward FFT (time to frequency) |
| `sdkFFTInverse(TUsineFFTPtr, float* audio, float* re, float* im)` | Inverse FFT (frequency to time) |

### Usage
```cpp
TUsineFFTPtr fft = sdkFFTCreate(1024);

float audio[1024], re[513], im[513];
sdkFFTForward(fft, audio, re, im);
// ... process frequency domain ...
sdkFFTInverse(fft, audio, re, im);

sdkFFTDestroy(fft);
```

---

## Errors, Warnings and Infos

Modules report what happens with the error system of `sdk/UserError.h` rather than with the raw trace functions below: each message is registered once, and the user can filter them with a verbosity level.

| Function | Description |
|----------|-------------|
| `RegisterError(Level level, const char* message)` | Registers a message (`Level::Error`, `Level::Warn` or `Level::Info`), returns its index |
| `sdkRaiseError(int index, bool force_log = false)` | Traces the message, `force_log` also writes a warning or an info in the Usine log file |
| `sdkRaiseError(int index, const char* extraMsg, bool force_log = false)` | Same, with `extraMsg` appended to the message |
| `sdkRaiseError(int index, const std::string& extraMsg, bool force_log = false)` | Same, with a `std::string` |
| `setVerbosityLevel(Verbosity v)` | `Verbosity::None`, `Error`, `Warn`, `Info` or `All`: the messages above this level are not traced |

```cpp
// ModuleDefinition.h
inline const int ERROR_POINTER_NOT_FLOAT = RegisterError(Level::Error, "the pointer is not type float");

// UsinePointers.cpp
if (ptrIn.getPointerDataType() != usinePtrFloat)
	sdkRaiseError(ERROR_POINTER_NOT_FLOAT);
```

> **Example:** See [[examples/error-handling|ErrorHandling]].

---

## Trace & Logging

Low level functions, used by `sdkRaiseError` and by the SDK itself.

| Function | Description |
|----------|-------------|
| `sdkTraceChar(AnsiCharPtr msg)` | Print message to Usine trace |
| `sdkTraceChar(std::string& msg)` | Print string to Usine trace |
| `sdkTraceInt(int value)` | Print integer to trace |
| `sdkTracePrecision(TPrecision value)` | Print float to trace |
| `sdkTraceErrorChar(AnsiCharPtr msg, LongBool namePrefix)` | Print error message |
| `sdkTraceErrorChar(std::string& msg)` | Print error string |
| `sdkTraceWarningChar(AnsiCharPtr msg)` | Print warning message |
| `sdkTraceWarningChar(std::string& msg)` | Print warning string |
| `sdkTraceSplashChar(AnsiCharPtr msg, LongBool autoClose)` | Show splash message |
| `sdkTraceLogChar(AnsiCharPtr msg, LongBool showInSplash)` | Log message |
| `sdkTraceLogValueChar(std::string& msg, std::string& value, LongBool showInSplash)` | Log key-value pair |

---

## Dialog Popups

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkDialogConfirmationYesNoCancel(msg)` | `TDialogsResults` | Yes / No / Cancel dialog |
| `sdkDialogConfirmationYesNo(msg)` | `TDialogsResults` | Yes / No dialog |
| `sdkDialogInformationOk(msg)` | `TDialogsResults` | Information with OK button |
| `sdkDialogConfirmationOKCancel(msg)` | `TDialogsResults` | OK / Cancel dialog |
| `sdkDialogInputBox(caption, prompt, default)` | `AnsiCharPtr` | Text input dialog |

---

## Deprecated Event Functions

> **Note:** These functions are deprecated. Use `UsineEventClass` methods instead.

The following `sdk*Evt*` functions are still available for backward compatibility but should not be used in new code:

`sdkCompareEvt`, `sdkCopyEvt`, `sdkConcatEvt`, `sdkShiftEvt`, `sdkSetEvtSize`, `sdkGetEvtSize`, `sdkSetEvtMaxSize`, `sdkDestroyEvt`, `sdkCreateEvt`, `sdkMoveBLOC32`, `sdkAddEvt2/3`, `sdkSubEvt2/3`, `sdkDivEvt3`, `sdkMultEvt1/2/3`, `sdkMultEvt2Audio`, `sdkModEvt3`, `sdkPowerEvt3`, `sdkExpEvt1`, `sdkSqrtEvt1`, `sdkMaxEvt1/3`, `sdkMinEvt1/3`, `sdkThreshEvt1/2`, `sdkMixMidiEvt2/3`, `sdkFadeInAudioEvt`, `sdkFadeOutAudioEvt`, `sdkClearAudioEvt`, `sdkDenormalizeAudioEvt`, `sdkSmoothPrecision`, `sdkSmoothEvent`, `sdkSetEvtPChar`, `sdkGetEvtPChar`, `sdkGetEvtDataAddr`, `sdkSetEvtData`, `sdkGetEvtData`, `sdkSetEvtArrayData`, `sdkGetEvtArrayData`, `sdkPushEvtArrayData`, `sdkSetEvtPointer`, `sdkGetEvtPointer`, `sdkSetEvtColor`, `sdkGetEvtColor`, `sdkSetEvtArrayColor`, `sdkGetEvtArrayColor`, `sdkSetEvtArrayMidi`, `sdkGetEvtArrayMidi`, `sdkGetEvt3DPoint`.

See [UsineEventClass](usine-event-class.md) for the modern equivalents.

---

## Related Pages

- [[sdk/usine-event-class|UsineEventClass]] — Modern event manipulation
- [[sdk/data-types|Data Types & Constants]] — Type definitions
- [[sdk/utility-functions|Utility Functions]] — Color, geometry, math helpers
- [[sdk/user-module-base|UserModuleBase Class]] — Module base class and callbacks
- [[sdk/module-architecture|Module Architecture]] — How modules are structured
