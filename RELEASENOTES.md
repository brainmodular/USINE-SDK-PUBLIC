# BrainModular SDK Release Notes

Release notes of the C++ SDK of Usine Hollyhock.

- The SDK code is under the MIT licence, each dependency under its own licence.
- Documentation: the [wiki](wiki/README.md) of the repository.
- To discuss about the SDK, visit the [forum](https://forum.brainmodular.com/). To report a bug, send a mail to support@brainmodular.com.

The version of the SDK is the `SDK_VERSION` constant of `sdk/UsineDefinitions.h`.

Olivier Sens & the BrainModular Team

---

## SDK 703007 — 2026/11/25

Changes since 703006, the previous published version.

A module written for 703006 builds without change. Remove `inline` from its `TParamInfo`, build it with hidden visibility on macOS and Linux (see below) and rebuild it with 703007.

### `TParamInfo` private to each module

On macOS, dyld gives every module the weak definitions of the first module loaded that defines the same name, and an `inline constexpr TParamInfo` is one. DrawBox and SimplePad both have a `randomizeBtnInParamInfo`: when DrawBox was loaded first, SimplePad created its button with the caption, callback type and callback id of DrawBox, and its `onCallBack` never recognized the id.

- Declare the `TParamInfo` of `ModuleDefinition.h` `constexpr`, never `inline`. A `constexpr` variable at namespace scope has internal linkage: each module keeps its own copy, without `USINE_MODULE_LOCAL`. The examples, the modules of the `modules` folder, the README and the wiki do so.
- Build the modules with hidden visibility on macOS and Linux, so that they export only the `USINE_MODULE_EXPORT` functions. In Xcode, set *Symbols Hidden by Default* and *Inline Methods Hidden* to Yes (`GCC_SYMBOLS_PRIVATE_EXTERN`, `GCC_INLINES_ARE_PRIVATE_EXTERN`). With CMake, set `CMAKE_C_VISIBILITY_PRESET` and `CMAKE_CXX_VISIBILITY_PRESET` to `hidden` and `CMAKE_VISIBILITY_INLINES_HIDDEN` to `ON`.
- The Xcode projects of the examples and of the `modules` folder set both options. The `CMakeLists.txt` of the `modules` folder set hidden visibility on macOS too, not only on Linux.
- [Writing User Modules on macOS](wiki/docs/writing-user-modules-macos.md) explains how to check a build with `nm -gU`, and [Module Conventions](wiki/sdk/module-conventions.md) why a `TParamInfo` must not be `inline`.

### Background jobs

- `startJob()` and the end of the job notify Usine with `numMainThread` instead of `numRealTime`. `onJobEnd` now runs on the main thread, one at a time with the `ctNormal` callbacks of the module and never during its destruction. Before, it ran on the real-time queue thread of Usine, concurrently with both.

### Threads of the callbacks and notifications

The documentation of `TNotifyUsineMode`, `TFastCallBackType` and `sdkNotifyUsine` gives the thread of each value. It corrects the former one:

- `numRealTime`, the default of `sdkNotifyUsine`, and `cbtRealTime` are the real-time queue thread of Usine (medium priority, every 10 ms), neither the audio thread nor the main thread. The wiki gave the audio thread for `numRealTime`.
- `ctImmediate` runs in the thread that changes the value: the audio thread for a value that comes through a wire, the main thread for a change made in the interface.
- `numVideo`, `numRealTime` and `numNetwork` deliver the message concurrently with the `ctNormal` callbacks of the module, and `numRealTime` and `numNetwork` are not serialized with its destruction either. Use `numMainThread` for a callback that must not overlap them.
- `sdkNotifyUsine` only posts the message and returns at once, from any thread, the audio thread included.

### Other changes

- `Create` writes `UserModuleBase::m_masterInfo` only when it changes, and so does `sdkSetMasterInfo`. Usine gives every module the same `TMasterInfo`, and the write at each creation raced with the threads of the running instances, which read it.

### Examples and documentation

- [DataSampler](wiki/examples/data-sampler.md) is rewritten. Its double buffer was shared by the audio thread, the real-time queue thread and the main thread without synchronization, and `toggle rec` and `clear rec` reset it and reported from their immediate callback, possibly in the audio thread. It now records in chunks of 2000 values that the main thread allocates before the audio thread reaches them, shares its state through atomics, and reports its messages from the main thread. A recording holds at most 8 192 000 values, about 6 hours at 375 blocks per second.
- The wiki pages [Module Architecture](wiki/sdk/module-architecture.md), [Data Types](wiki/sdk/data-types.md) and [UserModuleBase](wiki/sdk/user-module-base.md) give the threads of the callback types, of the notification modes and of `onJobEnd`.

### Compatibility

- The structures and the exports don't change: the modules built with SDK 703006 keep working.
- Rebuild for macOS a module whose `TParamInfo` are `inline`, with hidden visibility: loaded with another module that uses the same constant names, it can create its parameters from the `TParamInfo` of the other one.

---

## SDK 703006 — 2026/11/23

Changes since 703005, the previous published version.

### Breaking changes

The error system of `UserError.h` is renamed: it reports errors, warnings and infos, not only errors. A module written for 703005 must rename these names to build.

| Before | Now |
|---|---|
| `sdkRaiseError(...)` | `sdkReport(...)` |
| `RegisterError(...)` | `registerReport(...)` |
| `setVerbosityLevel(...)` | `sdkSetVerbosity(...)` |
| `Level::Error`, `Level::Warn`, `Level::Info` | `UReportLevel::Error`, `UReportLevel::Warn`, `UReportLevel::Info` |
| `Verbosity::None`, `Error`, `Warn` | `UReportVerbosity::None`, `Error`, `Warn` |
| `Verbosity::Info`, `Verbosity::All` | `UReportVerbosity::All` |
| `Error`, `errorList()` | `UReport`, `uReportList()` |
| `current_verbosity_level` | removed: the verbosity belongs to each instance, see below |

- `UReportVerbosity` has no `Info` value any more: `All` shows every message, as `Info` did.
- The numeric values change: `UReportLevel` starts at 1, `UReportVerbosity` is `None` = 0, `Error` = 1, `Warn` = 2, `All` = 3. Use the names, not the numbers.

The verbosity is now set per instance, not per module binary:

- `sdkReport(...)` and `sdkSetVerbosity(...)` are members of `UserModuleBase`, no longer free functions. A call in a member function builds without change. A call in a static function, such as a thread proc, must go through the instance: `module->sdkReport(...)`.
- New `sdkGetVerbosity()`, the verbosity of the instance.
- New `reportWithVerbosity(verbosity, index, extraMsg, forceLog)` in `UserError.h`, for code that has no module instance: it traces a registered message with the verbosity you give it.
- `traceType` is removed.

Reserved CallBackIds:

- The CallBackIds from `CALLBACK_SDK_LIMIT` (`0xFFFF00`) are reserved by the SDK: `sdkCreateParam` traces an error and does not create a parameter with a callback that uses one. Before, the documentation allowed values up to `0xF000000`.
- `CALLBACK_WPARAM_LIMIT` is removed, use `CALLBACK_SDK_LIMIT`. `CALLBACK_JOB_START` and `CALLBACK_JOB_END` move to `0xFFFF00` and `0xFFFF01`.

### New features

- **Verbosity level combo box**: every module that registers at least one report gets a `verbosity level` combo box, created by the SDK after the parameters of the module. It is hidden by default and shown with all the parameters, in a `Report` section. Each instance filters its messages with its own level (`none`, `error`, `warn` or `all`, default `all`), saved with the patch.

### Other changes

- `ptListBox` is renamed `ptComboBox`, and the `TParamInfo::ListBoxStrings` field `ComboBoxStrings`. The old names are kept as deprecated aliases, so a module that uses them still builds.
- `sdkTraceChar` starts the line with `[MODULE_NAME]`, as `sdkTraceErrorChar` and `sdkTraceWarningChar` already did. Pass `namePrefix = FALSE` to the `AnsiCharPtr` overload to trace without it.

---

## SDK 703005 — 2026/11/16

Changes since 703003, the previous published version. The intermediate version 703004 was not published.

A module written for 703003 builds without change. Rebuild it with 703005 for macOS and Linux, and mark its error constants `USINE_MODULE_LOCAL` (see below).

### Parameter descriptions

- New field `TParamInfo::Description`, the last of the structure: what the parameter does, shown in the hint of the parameter in Usine and used by the MCP. `"\n"` starts a new line.
- `MODULE_DESC` is shown in the hint of the module, where `"\n"` starts a new line too.

### Modules loaded together on macOS and Linux

Usine loads several modules in one process, and on macOS and Linux the dynamic linker unifies the weak definitions of the whole process: a module could read the constants of another one.

- New macro `USINE_MODULE_LOCAL`: hidden visibility on macOS and Linux, empty on Windows, for a symbol that must stay private to the module.
- `MODULE_NAME`, `MODULE_DESC` and `MODULE_VERSION` are `USINE_MODULE_LOCAL`. Before, every module could report the name, description and version of the first module loaded.
- Everything in `UserError.h` is `USINE_MODULE_LOCAL`: the error registry (`errorList`), `RegisterError`, `sdkRaiseError` and `current_verbosity_level`. Each module keeps its own registry, and the verbosity set by a module no longer filters the messages of the others.
- Mark the error constants of your `ModuleDefinition.h` `USINE_MODULE_LOCAL`, as every example does: `inline const int ERROR_WRITE USINE_MODULE_LOCAL = RegisterError(Level::Error, "...");`. Otherwise two modules that use the same constant name can mix up their messages. The [Module Conventions](wiki/sdk/module-conventions.md) explain why.

### Other changes

- `sdkRaiseError(index, extraMsg)` accepts a null `extraMsg`.
- `TParamInfo::setEventClass` no longer gives a deprecation warning with clang and clang-cl.

### Examples and documentation

- Every parameter of the 23 examples has a `Description`, and the `MODULE_DESC` of the examples say more precisely what the module does, one sentence per line.
- New `examples/CMakeLists.txt`: it builds the examples for Windows x64 (clang-cl, `.usr-win64`), macOS (universal, `.usr-macos64uni`) and Linux x86_64 (clang 22, `.usr-linint64`). Its header gives the commands.
- The modules of the `modules` folder (ArrayDistance, AudioAnalysisDescriptors, DatabaseCreation, DatabasePlayer and MotionRecognition) have a `ModuleDefinition.h`, descriptions of their parameters and a `CMakeLists.txt` for Linux.
- The wiki explains `USINE_MODULE_LOCAL` in [UserModuleBase](wiki/sdk/user-module-base.md) and [Module Conventions](wiki/sdk/module-conventions.md).

### Compatibility

- `Description` is appended at the end of `TParamInfo`, and Usine reads it only from the modules built with SDK 703004 or later: the modules built with an older SDK keep working.
- As with 703003, use a module built with this SDK with an up-to-date Usine Hollyhock 7.

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
