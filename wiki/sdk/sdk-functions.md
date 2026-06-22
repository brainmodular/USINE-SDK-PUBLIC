# SDK Functions Reference
#sdk #reference #functions #audio #threading #files #fft #trace #dialog #math

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
void onGetModuleInfo(TMasterInfo*, TModuleInfo* pModuleInfo) override
{
    pModuleInfo->QueryListString = sdkGetAudioQueryTitle();
    pModuleInfo->QueryListValues = sdkGetAudioQueryChannelList();
}

int onGetNumberOfParams(int QIdx1, int QIdx2) override
{
    numChannels = sdkGetAudioQueryToNbChannels(QIdx1);
    return numChannels * 2 + controlParams;
}
```

> **Example:** See [[examples/audio-volume|AudioVolume]] for audio channel query usage.

---

## Global Array Functions

Shared data arrays accessible across all modules in a workspace.

| Function | Description |
|----------|-------------|
| `sdkGlobalArrayGetHash(AnsiCharPtr name)` | Get hash for named global array. Returns `UINT64` |
| `sdkGlobalArraySetValueFloat(UINT64 hash, int index, TPrecision value)` | Set float value at index |
| `sdkGlobalArrayGetValueFloat(UINT64 hash, int index)` | Get float value at index. Returns `TPrecision` |
| `sdkGlobalArraySetValueColor(UINT64 hash, int index, TUsineColor value)` | Set color value at index |
| `sdkGlobalArrayGetValueColor(UINT64 hash, int index)` | Get color value at index. Returns `TUsineColor` |
| `sdkGlobalArraySetValueAnsiChar(UINT64 hash, int index, AnsiCharPtr value)` | Set string value at index |
| `sdkGlobalArrayGetValueAnsiChar(UINT64 hash, int index)` | Get string value at index. Returns `AnsiCharPtr` |
| `sdkGlobalArraySetValueEvt(UINT64 hash, UsineEventPtr value)` | Set entire event |
| `sdkGlobalArrayGetValueEvt(UINT64 hash, UsineEventPtr value)` | Get entire event |
| `sdkGlobalArrayGetSize(UINT64 hash)` | Get array size. Returns `int` |
| `sdkGlobalArrayGetAllNames()` | Get all names as comma-text. Returns `AnsiCharPtr` |

### Usage
```cpp
UINT64 hash = sdkGlobalArrayGetHash("my-shared-data");
sdkGlobalArraySetValueFloat(hash, 0, 42.0f);
float val = sdkGlobalArrayGetValueFloat(hash, 0);  // 42.0
```

> **Example:** See [[examples/global-array|GlobalArray]] for global array usage.

---

## Usine Core Information

| Function | Returns | Description |
|----------|---------|-------------|
| `sdkGetBlocSize()` | `int` | Current audio block size |
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

## Trace & Logging

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
