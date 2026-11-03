# Module Conventions
#sdk #conventions #style #parameters #errors #reference

How a user module is organized and written. Every example of the SDK follows these rules, use them as a model for your own modules.

## Files of a Module

A module named `MyModule` is made of three files:

| File | Contains |
|------|----------|
| `MyModule.h` | The class, derived from `UserModuleBase`: overridden callbacks, parameter events, state |
| `MyModule.cpp` | `CreateModule`, `DestroyModule` and the callbacks |
| `ModuleDefinition.h` | Everything that describes the module: constants, messages, callback ids and parameters |

`GetBrowserInfo` is not in the module: the SDK implements it in `UserModule.cpp` from the `MODULE_*` constants of `ModuleDefinition.h`.

Helper classes go in their own files (see `Grain.h` in [[examples/granulator|Granulator]] or `comb.hpp` in [[examples/reverb|Reverb]]).

## ModuleDefinition.h

The file is organized in this order:

1. the module constants `MODULE_NAME`, `MODULE_DESC` and `MODULE_VERSION`
2. the messages raised with `sdkRaiseError`
3. the callback ids
4. the `TParamInfo` of each parameter

```cpp
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "my module";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "converts a frequency to a period (sdk example)";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

inline const int WARNING_NEGATIVE_FREQUENCY = RegisterError(Level::Warn, "the frequency must be positive, got: ");

constexpr NativeInt FREQUENCY_FDR_CBID = 0x001900F0;

inline constexpr TParamInfo frequencyFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "freq",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.MinValue     = 0.1f,
	.MaxValue     = 20000.0f,
	.DefaultValue = 50.0f,
	.CallBackType = ctNormal,
	.CallBackId   = FREQUENCY_FDR_CBID
};
```

> **Important:** `ModuleDefinition.h` defines the `MODULE_*` constants, so it must be included by **one** `.cpp` only, the one that holds `CreateModule`. Include it after the module header.

A module without parameters keeps only the module constants in `ModuleDefinition.h` (see [[examples/drag-and-drop|DragAndDrop]]). When a `TParamInfo` needs a constant of the class, `ModuleDefinition.h` includes the module header (`MAXPOINTS` in [[examples/draw-trajectory-box|DrawTrajectoryBox]]).

### Writing a TParamInfo

`TParamInfo` uses designated initializers: the fields must be written in the order of their declaration in `UsineDefinitions.h`, and the module must be compiled in C++20.

Every field has a default value, so a `TParamInfo` only lists what matters:

| Field | Default |
|-------|---------|
| `MaxValue` | `1` |
| `CallBackType` | `ctNormal` (the first value of the enum) |
| `CallBackId` | `-1` |
| `IsVisibleByDefault` | `TRUE` |
| every other field | `0`, `FALSE` or `nullptr` |

The examples always write `ParamType`, `IsInput`, `IsOutput` and `CallBackType`, the `Caption` unless it is set at creation (the audio channels), then `CallBackId` when the parameter has a callback. A parameter without callback says it with `.CallBackType = ctNone`: left out, the type would be `ctNormal`.

## Naming

### Parameter events

A `UsineEventClass` member bound to a parameter is named `<what><Type><In|Out>` in camelCase: what the parameter is, the type of the parameter, and its direction. `mixFdrIn`, `resetBtnIn`, `resultFdrOut`. No `m_` prefix.

| Type | Parameter types | Example |
|------|-----------------|---------|
| `Fdr` | `ptDataFader`, `ptGainFader`, `ptMidiNoteFader`, `ptDataField`, `ptDataFieldBounded` | `gainFdrIn`, `outFdrOut` |
| `Sw` | `ptSwitch`, `ptLeftLed`, `ptRightLed`, `ptTriggerLed` | `muteSwIn`, `grainStartSwOut` |
| `Btn` | `ptButton` | `startBtnIn` |
| `Lbox` | `ptListBox` | `grainModeLboxIn` |
| `Arr` | `ptArray` | `delayArrIn`, `posXArrOut` |
| `Txt` | `ptText` | `verbosityTxtIn` |
| `Col` | `ptColor` | `boxColIn` |
| `Audio` | `ptAudio` | `modAudioIn` |
| `Midi` | `ptMidi` | `midiIn` |
| `Ptr` | `ptPointer` | `ptrIn` |
| `Bmp` | `ptPointerBitMap` | |
| `File` / `Dir` | `ptFileName` / `ptChooseFolder` | |

- The direction is `In` for a parameter with an input port, even when it also has an output port (`exampleTxtIn` in [[examples/template-module|TemplateModule]]), `Out` for an output.
- When the type is enough, the name starts with it: `arrIn`, `arrOut`, `midiOut`, `ptrOut`.
- The arrays of audio channels are named `audioInputs` and `audioOutputs`.

### Derived names

| Element | Rule | Example |
|---------|------|---------|
| `TParamInfo` | member name + `ParamInfo` | `mixFdrInParamInfo` |
| Callback id | what + type in UPPER_SNAKE_CASE + `_CBID`, without the direction | `MIX_FDR_CBID` |
| Message | `ERROR_`, `WARNING_` or `INFO_` + what happened | `ERROR_INVALID_CHUNK` |
| Constant | UPPER_SNAKE_CASE | `MAX_DELAY` |
| Class | the name of the folder and of the files | `RingMod` in `RingMod.h` |

A callback id keeps the direction when it is needed: to tell an input from an output of the same name (`COORDS_ARR_IN_CBID` and `COORDS_ARR_OUT_CBID` in [[examples/draw-trajectory-box|DrawTrajectoryBox]]), or when the name is the type alone (`PTR_IN_CBID` for `ptrIn`).

A callback id shared by several parameters, or that is not a parameter (a chunk subscription, a notification the module sends to itself), is named after what it triggers and documented with a comment: `INPUT_FDR_CBID` in [[examples/data-multiply|DataMultiply]], `AGG_BUFFER_CBID` in [[examples/data-sampler|DataSampler]]. A contextual menu command reuses the callback id of the matching button (`RESET_BTN_CBID`).

### Module name and description

`MODULE_NAME` is the folder name in lower case words (`ring mod`, `usine pointers`), without `example` or `test`. `MODULE_DESC` says what the module does, followed by `(sdk example)` for the examples.

## Parameters

Parameters are created in `onCreateParameters` with `sdkCreateParam`, in the order they appear in the module:

```cpp
void MyModule::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(frequencyFdrInParamInfo, frequencyFdrIn);
	sdkCreateParam(periodFdrOutParamInfo, periodFdrOut);
}
```

`onGetModuleInfo` only sets what describes the module: its type, its color, the query, the options. It doesn't set:

- `Name`, `Description` and `Version`: the SDK fills them from the `MODULE_*` constants before calling `onGetModuleInfo`
- `NumberOfParams`: it keeps its default 0, it is only used by the legacy `onGetParamInfo` system

When a parameter depends on the query (audio channels), copy its `TParamInfo` and change the copy:

```cpp
for (int num = 0; num < numOfAudioInsOuts; num++)
{
	TParamInfo audioIn = audioInParamInfo;
	audioIn.Caption    = sdkGetAudioQueryChannelNames("in ", num + 1, queryResult);
	sdkCreateParam(audioIn, audioInputs[num]);
}
```

## Callbacks

`onCallBack` filters the message first, then dispatches on the callback id:

```cpp
void MyModule::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case FREQUENCY_FDR_CBID:
			computePeriod();
			break;
		default:
			break;
	}
}
```

A contextual menu command created with `sdkAddCommand` calls `onCallBack` with its callback id: give it the id of the matching button rather than a hard-coded number.

## Errors, Warnings and Infos

Modules never call `sdkTraceChar`, `sdkTraceErrorChar` or `sdkTraceWarningChar` directly. Each message is registered once in `ModuleDefinition.h`, and raised with `sdkRaiseError`:

```cpp
// ModuleDefinition.h
inline const int ERROR_INVALID_CHUNK = RegisterError(Level::Error, "invalid saved data, it was not restored");

// MyModule.cpp
if (sizeInBytes < static_cast<int>(sizeof(data)))
{
	sdkRaiseError(ERROR_INVALID_CHUNK);
	return;
}
```

The user can then filter the messages of the module with its verbosity level. The trace already starts with the module name, so the messages don't repeat it. A message ending with `": "` is meant to receive an extra text: `sdkRaiseError(WARNING_UNKNOWN_VERBOSITY, verbosityLevel)`.

Raise a message where something goes wrong and the module can't do what the user asked: invalid data from a file, a value out of range, a failed SDK call. Avoid raising on every bloc: raise once when the problem appears (see `grainOverlapWarned` in [[examples/granulator|Granulator]]).

See [[examples/error-handling|ErrorHandling]] for the complete API.

## Modern C++

The examples are written in C++20, on Windows and on macOS.

- `#pragma once` in every header.
- `override` on every overridden callback, the compiler then checks the signatures.
- Members are initialized where they are declared: `int queryResult = 0;`. No empty constructor or destructor.
- C++ casts only: `static_cast<int>(value)`, never `(int)value`.
- `nullptr`, never `NULL` or `0` for pointers.
- `std::array` instead of C arrays, `std::vector` and `std::unique_ptr` instead of `new[]` / `delete[]`. A class with an array of channels declares `using AudioEvents = std::array<UsineEventClass, USINE_AUDIO_CHANNEL_MODULE_MAX>;`.
- `std::span` instead of a pointer and a size: `void setbuffer(std::span<float> buf);`.
- `enum class` instead of `int` constants, for example for the items of a list box. Clamp the value read from the parameter before converting it.
- Designated initializers for the SDK structures too: `TPointF { .x = 0.0f, .y = 1.0f }`, `TRectF { .left = 0.25f, .top = 0.5f, .right = 0.75f, .bottom = 0.75f }`.
- The standard library rather than hand-written code: `std::clamp`, `std::erase_if`, `std::ranges::fill`, `std::lerp`, `std::hypot`, `std::numbers::pi_v<float>`.
- `[[nodiscard]]` on const getters and on functions whose result must be used.
- Code that needs Usine (`sdkGetUsineColor`, `sdkGetBlocSize`, `createEvent`...) goes in `onInitModule`, not in the constructor.
- Every event created with `createEvent` is destroyed with `destroyEvent` in the destructor.

The SDK API itself keeps its C types: `LongBool` with `TRUE` / `FALSE`, `AnsiCharPtr`, `CreateModule` returning a raw pointer.

## Comments

A comment explains **why**, or what happens behind the scenes. It never repeats the code:

```cpp
// cast is important to call the good destructor
delete static_cast<MyModule*>(pModule);
```

No section banners, no commented-out code, no empty callbacks: remove what the module doesn't use.

## File Header

Every source file starts with the same header:

```cpp
//-----------------------------------------------------------------------------
// @file    MyModule.cpp
// @author  BrainModular team
// @brief   Converts a frequency to a period.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
```

Code adapted from a third party keeps its author and its original notice, see `Grain.h` in [[examples/granulator|Granulator]] or `comb.hpp` in [[examples/reverb|Reverb]].

## Related Pages

- [[getting-started|Getting Started]] — A first module, written with these conventions
- [[sdk/module-architecture|Module Architecture]] — Lifecycle and data flow
- [[sdk/user-module-base|UserModuleBase Class]] — Callback reference
- [[examples/template-module|TemplateModule]] — Template with every callback
- [[examples/error-handling|ErrorHandling]] — The error system
