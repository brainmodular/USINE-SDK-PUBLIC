## SDK for brainmodular Usine HH7

The power of Usine come from the generic way you can customize 
your data and your interface to just make the sound you want the 
way you want. The core of this power is the module concept, which 
let you patch and organize your data flow in any direction with a 
generic approach.

To optimize or extend your patch, you can use the script 
language to make your own module, it's more CPU friendly than patch 
and easy because completely integrated in Usine.

The Brainmodular SDK is the ultimate tool to extend Usine. You can construct 
your own user module, you code in C++ to create any new module you can imagine.

Note: The SDK is Open source (MIT License). 

Some simple examples of User Modules are provided to quickly learn and 
start making your own.

More than 20 complex modules made with this SDK are distributed with Usine, 
like Matrix and devices. Sources are provided and are good 
examples of the integration.

### Latest changes

SDK 703003 changes the way a module describes itself and creates its parameters.
A module written for the previous version (702002) builds again after the steps 1 to 3.
The steps 4 to 6 move it to the new way of writing a module, followed by every example.
The [release notes](RELEASENOTES.md) list all the changes.

**1. Compile in C++20.** The SDK now uses designated initializers.
Set *C++ Language Standard* to *ISO C++20 Standard (/std:c++20)* in Visual Studio,
*C++ Language Dialect* to *C++20* in Xcode.

**2. Remove your `GetBrowserInfo`.** The SDK implements it in `UserModule.cpp` from your
`MODULE_NAME`, `MODULE_DESC` and `MODULE_VERSION` constants: if your module still defines it,
the link fails. The SDK also sets `Name`, `Description` and `Version` before calling
`onGetModuleInfo`, remove these lines from your `onGetModuleInfo` too.

**3. Check the new global names.** `UserDefinitions.h` now includes `UserError.h`, which declares
`Level`, `Verbosity`, `Error`, `errorList`, `RegisterError`, `traceType`, `sdkRaiseError`,
`setVerbosityLevel` and `current_verbosity_level` in the global namespace.
Rename what clashes in your module.

**4. Describe the module in `ModuleDefinition.h`, and create the parameters in `onCreateParameters`.**
Each parameter is an `inline constexpr TParamInfo`, created and bound to its event by `sdkCreateParam`.

Before, with SDK 702002:

```cpp
// MyModule.cpp
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "my module";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "what my module does";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt GAIN_CBID = 0x00100000;

void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name        = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version     = UserModuleBase::MODULE_VERSION;
}

void MyModule::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name           = MODULE_NAME;
    pModuleInfo->Description    = MODULE_DESC;
    pModuleInfo->Version        = MODULE_VERSION;
    pModuleInfo->ModuleType     = mtSimple;
    pModuleInfo->NumberOfParams = 1;
}

void MyModule::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
    if (ParamIndex == 0)
    {
        pParamInfo->ParamType    = ptGainFader;
        pParamInfo->Caption      = "gain";
        pParamInfo->IsInput      = TRUE;
        pParamInfo->IsOutput     = FALSE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->CallBackId   = GAIN_CBID;
        pParamInfo->setEventClass(fdrGain);
    }
}
```

Now, with SDK 703003:

```cpp
// ModuleDefinition.h, included by MyModule.cpp only: it defines the MODULE_* constants
#pragma once

#include "../../sdk/UserDefinitions.h"

constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "my module";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "what my module does";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

constexpr NativeInt GAIN_FDR_CBID = 0x00100000;

inline constexpr TParamInfo gainFdrInParamInfo {
    .ParamType    = ptGainFader,
    .Caption      = "gain",
    .IsInput      = TRUE,
    .IsOutput     = FALSE,
    .CallBackType = ctImmediate,
    .CallBackId   = GAIN_FDR_CBID
};

// MyModule.cpp
#include "MyModule.h"
#include "ModuleDefinition.h"

void MyModule::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->ModuleType = mtSimple;
}

void MyModule::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    sdkCreateParam(gainFdrInParamInfo, gainFdrIn);
}
```

- Write the `TParamInfo` fields in their declaration order. The others keep their default value:
  `MaxValue` 1, `CallBackType` `ctNormal`, `CallBackId` -1, the rest 0, `FALSE` or `nullptr`.
  A parameter without callback sets `.CallBackType = ctNone`.
- The parameters appear in the order of the `sdkCreateParam` calls. `NumberOfParams` and
  `onGetNumberOfParams` are no longer needed: keep the query results in `onAfterQuery`,
  called before `onCreateParameters`.
- When a parameter depends on the query (one audio input per channel), copy its `TParamInfo`,
  change the copy (`Caption`...) and create it in a loop: see the AudioVolume example.
- `onGetParamInfo` still works as the legacy system. Use one system or the other in a module.

**5. Report errors with `sdkRaiseError`.** Register each message once in `ModuleDefinition.h`,
then raise it where the problem happens. The user filters the messages with a verbosity level.

```cpp
// ModuleDefinition.h
inline const int ERROR_INVALID_CHUNK = RegisterError(Level::Error, "invalid saved data, it was not restored");

// MyModule.cpp
sdkRaiseError(ERROR_INVALID_CHUNK);
```

**6. Follow the naming of the examples.** A parameter is named `<name><Type><In|Out>`
(`gainFdrIn`, `muteSwIn`, `resultFdrOut`), its `TParamInfo` adds `ParamInfo` (`gainFdrInParamInfo`)
and its callback id is `<NAME>_<TYPE>_CBID` (`GAIN_FDR_CBID`).

Also new in SDK 703003:
  * `mtBackgroundJob`: the SDK adds the `processing` and `processing time` outputs of the job
  * drag and drop of files on the panel: `onAcceptDragOver` and `onDragDrop`
  * chunks: a float buffer written or read in one call (`sdkSetChunkFromFloatPointer`,
    `sdkGetChunkToFloatPointer`), and the `UsineChunk` reader of `sdk/UsineChunks.h`
  * `sdkGetBlockDuration()`; `TMasterInfo::BlocSize` is deprecated, use `sdkGetBlocSize()`

A module built with SDK 703003 needs an up-to-date Usine Hollyhock 7. The modules built with an
older SDK keep working.

To start a new module, copy the TemplateModule example or follow the
[Getting Started](wiki/getting-started.md) of the wiki, and read the
[Module Conventions](wiki/sdk/module-conventions.md).

#### resources
  * sources files on [sources](https://github.com/brainmodular/USINE-SDK-PUBLIC)
  * wiki and manuals on [wiki](https://github.com/brainmodular/USINE-SDK-PUBLIC/wiki)
  * forum on [forum](https://forum.brainmodular.com/)
  * YouTube channel on [youtube](https://www.youtube.com/channel/UC1g0k3J6x5Z8z5F7G9Q9V1A)
  * download Usine on [usine](https://www.brainmodular.com/usine.php)
