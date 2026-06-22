# Release Notes
#changelog #versions #history

SDK version history and changelog.

---

## SDK 702002 — Hollyhock 7 (Current)

- Background job system ([[examples/background-job|BackgroundJob]]) with lifecycle hooks (`startJob()`, `onJobBegin`, `onJobProcess`, `onJobEnd`)
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
