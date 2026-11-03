# Getting Started with the Usine SDK
#getting-started #tutorial #setup #first-module

## What is the Usine SDK?

The Usine SDK allows you to extend the [Usine Hollyhock](https://www.brainmodular.com) modular environment by writing custom **user modules** in C++. User modules are dynamic libraries (`.dll` on Windows, `.dylib` on macOS) that Usine loads at runtime.

Modules can process audio, MIDI, data, video, and provide custom graphical interfaces — essentially anything you can do with Usine's built-in modules.

## SDK Contents

```
USINE-SDK-PRIVATE/
├── sdk/                    # Core SDK files (add them to your project)
│   ├── UserDefinitions.h   # Master include file
│   ├── UserModule.h        # Base class for all modules
│   ├── UserModule.cpp      # Base class implementation
│   ├── UserError.h         # Errors, warnings and infos (sdkRaiseError)
│   ├── UsineDefinitions.h  # Data types, constants, enums
│   ├── UsineEventClass.h   # Event data container class
│   ├── UsineFunctions.h    # SDK utility functions
│   ├── UsineChunks.h       # UsineChunk, typed reader of a Usine chunk (include it yourself)
│   ├── UserUtils.h         # Color, geometry, math helpers
│   └── UserUtils.cpp
├── examples/               # 23 example modules
├── wiki/                   # This documentation
├── bin/                    # Compiled binaries
└── libs/                   # Internal library dependencies
```

## Prerequisites

- **Windows:** Visual Studio 2022 or later, C++20 (the example projects use Visual Studio 2026, toolset v145)
- **macOS:** Xcode 14+, C++20, deployment target 10.13+
- **Usine Hollyhock 7** installed on your system

> **Note:** C++20 is needed for the designated initializers used to describe the parameters.

## Your First Module

This walkthrough builds `MyModule`, a module that converts a frequency in Hz into a period in ms. A module is made of three files, see [[sdk/module-conventions|Module Conventions]] for the details.

### 1. Describe the Module in ModuleDefinition.h

`ModuleDefinition.h` holds everything that describes the module: its name, the callback ids and the parameters.

```cpp
#pragma once

#include "../../sdk/UserDefinitions.h"

// module constants for browser info and module info
// they are defined here, so this file must be included by a single .cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "my module";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "converts a frequency to a period";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt FREQUENCY_FDR_CBID = 0x001900F0;

inline constexpr TParamInfo frequencyFdrInParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "freq",
	.IsInput      = TRUE,
	.IsOutput     = FALSE,
	.Scale        = scLog,
	.MinValue     = 0.1f,
	.MaxValue     = 20000.0f,
	.DefaultValue = 50.0f,
	.Symbol       = "Hz",
	.CallBackType = ctNormal,
	.CallBackId   = FREQUENCY_FDR_CBID
};

inline constexpr TParamInfo periodFdrOutParamInfo {
	.ParamType    = ptDataFader,
	.Caption      = "period",
	.IsInput      = FALSE,
	.IsOutput     = TRUE,
	.Scale        = scLog,
	.MinValue     = 0.05f,
	.MaxValue     = 10000.0f,
	.Symbol       = "mS",
	.ReadOnly     = TRUE,
	.CallBackType = ctNone
};
```

Each parameter with a callback has its own `CallBackId`: Usine gives it back to `onCallBack` when the parameter changes.

A `TParamInfo` only lists the fields that matter, in their declaration order: the others keep their default value (`MaxValue` 1, `CallBackType` `ctNormal`, `CallBackId` -1, `IsVisibleByDefault` `TRUE`, everything else 0, `FALSE` or `nullptr`). A parameter without callback says so with `.CallBackType = ctNone`.

The names follow the naming of the [[sdk/module-conventions|Module Conventions]]: what the parameter is, its type and its direction (`frequencyFdrIn`, `periodFdrOut`).

### 2. Declare the Module Class

Inherit from `UserModuleBase`, override the callbacks you need, and declare one `UsineEventClass` per parameter:

```cpp
#pragma once

#include "../../sdk/UserDefinitions.h"

class MyModule : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	UsineEventClass frequencyFdrIn;
	UsineEventClass periodFdrOut;

	void computePeriod();
};
```

### 3. Implement the Two Global Functions

Every module library implements these two functions, Usine calls them through the SDK:

```cpp
#include "MyModule.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MyModule();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<MyModule*>(pModule);
}
```

`GetBrowserInfo`, which gives the name of the module to the Usine browser, is implemented by the SDK in `UserModule.cpp` from the `MODULE_*` constants. Don't write it in your module: it would be defined twice and the link would fail.

### 4. Describe the Module to Usine

```cpp
void MyModule::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType  = mtSimple;
	pModuleInfo->BackColor   = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess = TRUE;
}
```

The SDK has already set `Name`, `Description` and `Version` from the `MODULE_*` constants before calling `onGetModuleInfo`, and `NumberOfParams` keeps its default 0: the parameters are created in `onCreateParameters`. Set only what differs from the `TModuleInfo` defaults listed in [[sdk/user-module-base|UserModuleBase]].

`DontProcess` is `TRUE` because this module only reacts to callbacks, it doesn't need `onProcess`.

### 5. Create the Parameters

`sdkCreateParam` creates a parameter from its `TParamInfo` and binds it to its event:

```cpp
void MyModule::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(frequencyFdrInParamInfo, frequencyFdrIn);
	sdkCreateParam(periodFdrOutParamInfo, periodFdrOut);
}
```

The parameters appear in the order of the calls.

### 6. Handle Callbacks

```cpp
void MyModule::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	if (Message->wParam == FREQUENCY_FDR_CBID)
		computePeriod();
}

void MyModule::computePeriod()
{
	const TPrecision freq = frequencyFdrIn.getData();
	if (freq <= 0.0f)
		return;

	periodFdrOut.setData(1000.0f / freq);
}
```

To report a problem to the user, register a message in `ModuleDefinition.h` and raise it with `sdkRaiseError`, see [[examples/error-handling|ErrorHandling]].

### 7. Build and Deploy

Build your project as a dynamic library with the appropriate extension:
- **Windows:** `.usr-win64`
- **macOS Universal:** `.usr-macos64uni`
- **macOS Intel:** `.usr-osx64`
- **macOS ARM64:** `.usr-osxarm64`

Copy the built file to your Usine user modules folder, restart Usine, and find your module in the browser.

## Next Steps

- Read the platform-specific setup guides: [[docs/writing-user-modules-windows|Windows]] | [[docs/writing-user-modules-macos|macOS]]
- Follow the [[sdk/module-conventions|Module Conventions]] used by all the examples
- Explore the [[sdk/module-architecture|Module Architecture]] to understand the full lifecycle
- Browse the [[README|Examples]] for working code demonstrations
- Check the [[sdk/user-module-base|UserModuleBase reference]] for all available callbacks
