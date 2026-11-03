# BrainModular SDK Release Notes

Release notes of the C++ SDK of Usine Hollyhock.

- The SDK code is under the MIT licence, each dependency under its own licence.
- Documentation: the [wiki](wiki/README.md) of the repository.
- To discuss about the SDK, visit the [forum](https://forum.brainmodular.com/). To report a bug, send a mail to support@brainmodular.com.

The version of the SDK is the `SDK_VERSION` constant of `sdk/UsineDefinitions.h`.

Olivier Sens & the BrainModular Team

---

## SDK 703003 — 2026/11/03

Changes since 702002, the previous published version. The intermediate versions 703001 and 703002 were not published.

To update a module written for 702002, follow the [Latest changes](README.md#latest-changes) of the README.

### Breaking changes

- **C++20**: the SDK uses designated initializers (in `UserModule.cpp`, and in the `TParamInfo` of the examples). Compile your module in C++20: *ISO C++20 Standard (/std:c++20)* in Visual Studio, *C++20* in Xcode.
- **`GetBrowserInfo` is implemented by the SDK**, in `UserModule.cpp`, from `MODULE_NAME`, `MODULE_DESC` and `MODULE_VERSION`. Remove it from your module: defined twice, the link fails.
- **`UserDefinitions.h` includes the new `UserError.h`**, which declares `Level`, `Verbosity`, `Error`, `errorList`, `RegisterError`, `traceType`, `sdkRaiseError`, `setVerbosityLevel` and `current_verbosity_level` in the global namespace. Rename a name of your module that clashes with them, or put it in a namespace.

### Parameters created by the module

- New callback `onCreateParameters`, called after `onAfterQuery`: the module creates its parameters with `sdkCreateParam(const TParamInfo&, UsineEventClass&)`, which also binds the parameter to its event. The parameters appear in the order of the calls. Usine calls it through the new `CreateParameters` export, with the new `TMasterInfo::CreateParam`.
- Every `TParamInfo` field has a default value: `MaxValue` 1, `CallBackId` -1, `IsVisibleByDefault` `TRUE`, the other fields 0, `FALSE` or `nullptr`. A parameter is described by an `inline constexpr TParamInfo` written with designated initializers, which lists only the fields that differ, in their declaration order. `CallBackType` defaults to `ctNormal`: a parameter without callback sets `ctNone`.
- `onGetParamInfo` is no longer pure virtual. With `onGetNumberOfParams`, `NumberOfParams` and `setEventClass`, it stays supported as the legacy way to declare the parameters.
- `sdkCreateParam` takes references. It took pointers in the internal versions 703001 and 703002.

### Module description

- The SDK sets `Name`, `Description` and `Version` of `TModuleInfo` from the `MODULE_*` constants before calling `onGetModuleInfo`: the module doesn't set them anymore.
- `TModuleInfo::DefaultWidth` and `DefaultHeight` default to 100 pixels instead of 0.
- New `getModuleType()`, the `ModuleType` set in `onGetModuleInfo`.

### Errors, warnings and infos

- New `sdk/UserError.h`. A module registers each message once with `RegisterError(Level::Error, "message")` (or `Level::Warn`, `Level::Info`) and raises it with `sdkRaiseError(index)`, `sdkRaiseError(index, extraText)` to append a text, or `sdkRaiseError(index, true)` to also write it in the Usine log file. `setVerbosityLevel(Verbosity)` filters the messages of the module.

### Background jobs

- New module type `mtBackgroundJob`: the SDK adds two outputs after the parameters of the module, `processing`, on while the job runs, and `processing time`, the duration of the last job in ms.
- `startJob()` notifies Usine without delay, and `onJobEnd` is called one audio block after the end of the job. Both used a fixed 25 ms delay.
- A job whose `onJobBegin` or `onJobProcess` throws no longer notifies its end twice.

### Drag and drop

- New callbacks `onAcceptDragOver` and `onDragDrop`, through the new `AcceptDragOver` and `DragDrop` exports: the module accepts the files dragged over its panel, and receives the dropped ones as a comma-text.

### Chunks

- New `sdkSetChunkFromFloatPointer`, `sdkGetChunkToFloatPointer` and `sdkGetChunkItemSize`: a float buffer written in a chunk item, or read from it, in one call.
- New `sdk/UsineChunks.h`: `UsineChunk` reads typed values from a named chunk without creating it. `UserDefinitions.h` doesn't include it.

### Audio, events and types

- New `sdkGetBlockDuration()`, the duration of an audio block in ms. `sdkGetBlocSize()` reads the new `TMasterInfo::GetBlockSize`, and `TMasterInfo::BlocSize` is deprecated.
- `UsineEventClass`: new `setEvent`, `createEvent(capacity)` and `createEvent(name, capacity)`, and `mult(e1, e2)` which sets the event to `e1 * e2`.
- New parameter type `ptDataFieldBounded`, a data field bounded to `MinValue` and `MaxValue`.

### Examples and documentation

- Every example is rewritten in C++20 with the same [conventions](wiki/sdk/module-conventions.md):
  - a `ModuleDefinition.h` holds the module constants, the messages, the callback ids and the `TParamInfo` of the parameters
  - the parameters are named `<name><Type><In|Out>` (`gainFdrIn`, `resultFdrOut`), their callback ids `<NAME>_<TYPE>_CBID` (`GAIN_FDR_CBID`)
  - the errors are raised with `sdkRaiseError`
- New examples: [ErrorHandling](wiki/examples/error-handling.md) and [DragAndDrop](wiki/examples/drag-and-drop.md). MyModule and TemplateModuleSimple are removed: start from [TemplateModule](wiki/examples/template-module.md), or from the [Getting Started](wiki/getting-started.md) of the wiki.
- [UsineChunks](wiki/examples/usine-chunks.md) adds a bulk float transfer, with the `push to chunk` and `read from chunk` buttons.
- Fixed examples:
  - the contextual menu commands of DrawBox, DrawTrajectoryBox and SimplePad used old parameter indexes instead of callback ids, they did nothing
  - AudioVolume mute had no effect (it muted the dB value instead of the gain coefficient)
  - UsinePointers never copied the pointer when it changed (the input had no callback)
  - the color input of DataSampler was a data field instead of a color
  - Granulator leaked its grains, used half of them with two channels, and read the wrong part of the delay line for grains pitched up near the start of the buffer
  - DrawTrajectoryBox: the Lissajous curves were distorted, the third line of `coords in` was wrong, and an invalid saved trajectory could overflow the module
  - GlobalArray wrote the same string in every cell of `GA-STRING`
  - RingMod and RingModMultithread didn't destroy their work buffers
- The Xcode projects use C++20. BackgroundJob, DataSampler and ErrorHandling have no Xcode project yet.
- The wiki is updated, with the new pages Module Conventions, ErrorHandling and DragAndDrop.

### Compatibility

- The new functions of `TMasterInfo` are appended at the end of the structure: the modules built with an older SDK keep working.
- A module built with SDK 703003 uses functions that older versions of Usine don't provide: use it with an up-to-date Usine Hollyhock 7.

---

## SDK 702002 — Hollyhock 7

- Background jobs managed by the SDK: `startJob()`, `onJobBegin`, `onJobProcess` and `onJobEnd`
- `sdkNotifyUsine` chooses the thread of the notification (`TNotifyUsineMode`), the former signature is `sdkNotifyUsineOld`
- FFT functions: `sdkFFTCreate`, `sdkFFTDestroy`, `sdkFFTForward` and `sdkFFTInverse`
- `sdkChunkExist` and `sdkChunkItemExist` check a chunk or a chunk item without creating it
- Constants renamed: `USINE_AUDIO_CHANNEL_MODULE_MAX` (192) replaces `USINE_MULTIPHONY_MAX` (144), which stays as a deprecated alias; `USINE_MAX_SOUNDCARD_AUDIO_INPUTS` and `USINE_MAX_SOUNDCARD_AUDIO_OUTPUTS` replace `USINE_MAX_AUDIO_INPUTS` and `USINE_MAX_AUDIO_OUTPUTS`
- Deprecated: `sdkGetUsineVersionType` (always 0), and `sdkGetUsineMaxSoundCardAudioInputs`, `sdkGetUsineMaxSoundCardAudioOutputs`, `sdkGetUsineMaxMidiDevices` and `sdkGetUsineMaxPolyphony`, replaced by the constants
- `TVstTimeInfo`: its last integer fields are `int32_t`, the structure was misaligned on 64-bit macOS and Linux
- Wiki documentation of the SDK and of the examples

## HH6-241014 — 2024/10/14

- `createEvent()` sets a NULL pointer
- Examples compiled with VC++22

## HH6-240901 — 2024/08/01

- Added `sdkGlobalArrayGetSize(const UINT64 hash, const AnsiCharPtr errorMsg)`
- Examples compiled with C++17

## HH6-240731 — 2024/07/31

- Added `UsineEventClass` instead of the old `UsineEventPtr`
- Marked many older functions as deprecated
- Reformatted the SDK to have a more standard structure
- Cleaned up the SDK
- Added attributes to many functions to silence some potential compiler warnings

## HH6-240115 — 2024/01/28

- Massive refactoring, potentially some code update needed
- Removed unnecessary functions
- Added the global array functions
- Added the Usine object functions
- New set of examples

## HH5-220211 — 2022/02/11

- Improved Convolution examples
- Added `sdkGetSampleArrayAudioFile()`
- Added `sdkLockPatch()`
- Added `sdkUnLockPatch()`
- Added `sdkResampleAudioFile()`
- Added `sdkDenormalizeAudioEvt()`
- Added `sdkPatchJustActivated()`

## HH5-210706 — 2021/07/06

- Compatibility with HH5
- Added Convolution examples
