# Release Notes
#changelog #versions #history

SDK version history and changelog.

---

## SDK 703003 — Hollyhock 7 (Current)

- `sdkCreateParam` takes references instead of pointers: `sdkCreateParam(gainFdrInParamInfo, gainFdrIn)`. A module written for 703002 removes the `&` of both arguments

## SDK 703002 — Hollyhock 7

- Parameters are created in `onCreateParameters` with `sdkCreateParam` and `constexpr TParamInfo` descriptions, `onGetParamInfo` / `onGetNumberOfParams` become legacy. Every `TParamInfo` field has a default value, a description only lists the fields that differ
- `GetBrowserInfo` is implemented by the SDK from `MODULE_NAME`, `MODULE_DESC` and `MODULE_VERSION`, which also fill `Name`, `Description` and `Version` before `onGetModuleInfo`. A module removes its own `GetBrowserInfo` (it would be defined twice) and no longer sets these fields, nor `NumberOfParams`
- `TModuleInfo::DefaultWidth` and `DefaultHeight` default to 100 pixels instead of 0
- Error system ([[examples/error-handling|ErrorHandling]]): `RegisterError`, `sdkRaiseError` and a verbosity level, instead of raw `sdkTrace*` calls
- New module type `mtBackgroundJob`: the SDK creates the `processing` and `processing time` outputs after the parameters of the module. `getModuleType()` returns the type of the module
- Job notifications: `startJob()` notifies Usine without delay, and `onJobEnd` is called one audio block after the end of the job (`sdkGetBlockDuration()`), both used a fixed 25 ms delay
- `TMasterInfo::BlocSize` is deprecated, use `sdkGetBlocSize()`. New `sdkGetBlockDuration()`
- Chunks: `sdkSetChunkFromFloatPointer`, `sdkGetChunkToFloatPointer` and `sdkGetChunkItemSize` write and read a float buffer in one call; `UsineChunk` (`UsineChunks.h`) reads typed values from a chunk without creating it
- `ptDataFieldBounded` parameter type
- Drag and drop of files on the panel: `onAcceptDragOver` and `onDragDrop` ([[examples/drag-and-drop|DragAndDrop]])
- All examples rewritten with the same conventions ([[sdk/module-conventions|Module Conventions]]): `ModuleDefinition.h` holds the module constants, messages, callback ids and parameters; parameter events named `<what><Type><In|Out>` (`mixFdrIn`, `resetBtnIn`, `resultFdrOut`) and callback ids `<WHAT>_<TYPE>_CBID` (`MIX_FDR_CBID`); `override`, C++ casts, `#pragma once`, short MIT headers
- [[examples/usine-chunks|UsineChunks]] moved to `onCreateParameters`, with a bulk float transfer (`push to chunk`, `read from chunk`)
- Examples written in C++20 (`std::array`, `std::span`, `enum class`, `std::clamp`, `std::numbers`...), Xcode projects moved from C++14 to C++20. BackgroundJob, DataSampler and ErrorHandling have no Xcode project yet
- Examples fixes:
  - contextual menu commands of DrawBox, DrawTrajectoryBox and SimplePad used old parameter indexes instead of callback ids, they did nothing
  - AudioVolume mute had no effect (it muted the dB value instead of the gain coefficient)
  - UsinePointers never copied the pointer when it changed (the input had no callback)
  - DataSampler color input was a data field instead of a color
  - Granulator leaked its grains, used half of them with two channels, and read the wrong part of the delay line for grains pitched up near the start of the buffer
  - DrawTrajectoryBox: Lissajous curves were distorted, the third line of coords in was wrong, invalid saved trajectories could overflow the module
  - GlobalArray wrote the same string in every cell of `GA-STRING`
  - RingMod and RingModMultithread didn't destroy their work buffers
- MyModule and TemplateModuleSimple removed: start from [[examples/template-module|TemplateModule]], or from scratch with [[getting-started|Getting Started]]

## SDK 702002 — Hollyhock 7

- Background job system ([[examples/background-job|BackgroundJob]]) with lifecycle hooks (`startJob()`, `onJobBegin`, `onJobProcess`, `onJobEnd`)
- Added `sdkChunkExist` and `sdkChunkItemExist` — check whether a chunk or a chunk item exists without creating it (fields appended at the end of `TMasterInfo`, older modules stay ABI-compatible)
- Updated examples with modern C++ practices
- Job management handled by SDK for the user

## HH6-241014 — 2024/10/14

- `createEvent()` now sets event to NULL on creation
- Compiled with VC++22 (latest)

## HH6-240901 — 2024/08/01

- Added `sdkGlobalArrayGetSize` function
- C++17 standard adopted

## HH6-240731 — 2024/07/31

- **Major:** Added `UsineEventClass` wrapper class ([[sdk/usine-event-class|UsineEventClass]]) — modern C++ interface for event manipulation
- Deprecated all `sdk*Evt*` functions in favor of `UsineEventClass` methods
- Reformatted and cleaned up all SDK header files
- Updated all examples to use `UsineEventClass`

## HH6-240115 — 2024/01/28

- **Major refactoring** of SDK architecture
- Added Global Array functions ([[examples/global-array|GlobalArray]]) (`sdkGlobalArrayGetHash`, `sdkGlobalArraySetValue*`, `sdkGlobalArrayGetValue*`)
- Added Usine Object functions (`sdkUsineObjectSet*`, `sdkUsineObjectGet*`, `sdkUsineObjectFind`, etc.)
- Added Chunk system functions (`sdkGetChunk`, `sdkChunkOperation`, `sdkChunkSubscribe`)
- New examples: GlobalArray, UsineObjects, UsineChunks, UsinePointers, BackgroundJob
- New trace functions: `sdkTraceErrorChar`, `sdkTraceWarningChar`, `sdkTraceSplashChar`, `sdkTraceLogChar`
- Added sync object functions for thread synchronization
- Parameter system improvements: `CallBackId`, `setEventClass()`, `IsStoredInPreset`

## HH5-220211 — 2022/02/11

- Added convolution reverb example
- Added `sdkGetSampleArrayAudioFile` for batch sample reading
- Added `sdkLockPatch` / `sdkUnLockPatch` functions
- Patch state query functions: `sdkPatchIsRunning`, `sdkPatchLoading`, `sdkPatchJustActivated`

## HH5-210706 — 2021/07/06

- Hollyhock 5 compatibility
- Convolution examples added
- Various bug fixes and improvements

---

## Related Pages

- [[getting-started|Getting Started]]
- [[sdk/module-architecture|Module Architecture]]
