# Getting Started with the Usine SDK
#getting-started #tutorial #setup #first-module

## What is the Usine SDK?

The Usine SDK allows you to extend the [Usine Hollyhock](https://www.brainmodular.com) modular environment by writing custom **user modules** in C++. User modules are dynamic libraries (`.dll` on Windows, `.dylib` on macOS) that Usine loads at runtime.

Modules can process audio, MIDI, data, video, and provide custom graphical interfaces — essentially anything you can do with Usine's built-in modules.

## SDK Contents

```
USINE-SDK-PRIVATE/
├── sdk/                    # Core SDK header files (include these in your project)
│   ├── UserDefinitions.h   # Master include file
│   ├── UserModule.h        # Base class for all modules
│   ├── UserModule.cpp      # Base class implementation
│   ├── UsineDefinitions.h  # Data types, constants, enums
│   ├── UsineEventClass.h   # Event data container class
│   ├── UsineFunctions.h    # SDK utility functions
│   └── UserUtils.h         # Color, geometry, math helpers
├── examples/               # 23 example modules
├── docs/                   # Documentation and Doxygen setup
├── bin/                    # Compiled binaries
└── libs/                   # Internal library dependencies
```

## Prerequisites

- **Windows:** Visual Studio 2022 (Community edition or higher), C++17
- **macOS:** Xcode 14+, deployment target 10.13+
- **Usine Hollyhock 7** installed on your system

## Your First Module in 5 Minutes

### 1. Include the SDK

In your module's header file, include the single master header:

```cpp
#include "UserDefinitions.h"
```

This pulls in all SDK types, classes, and functions.

### 2. Create Your Module Class

Inherit from `UserModuleBase` and implement the mandatory callbacks:

```cpp
class MyModule : public UserModuleBase
{
public:
    MyModule() {}
    ~MyModule() {}

    void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
    void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) override;
    void onCallBack(TUsineMessage* Message) override;
};
```

### 3. Implement the Three Global Functions

Every module DLL must export these three functions:

```cpp
void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag,
                  TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new MyModule();
}

void DestroyModule(void* pModule)
{
    delete static_cast<MyModule*>(pModule);
}

void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = "my module";
    pModuleInfo->Description = "My first module";
}
```

### 4. Describe Your Module

```cpp
void MyModule::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = "my module";
    pModuleInfo->Description = "My first user module";
    pModuleInfo->ModuleType = mtSimple;
    pModuleInfo->BackColor = clDataModuleColor;
    pModuleInfo->NumberOfParams = 2;  // one input, one output
    pModuleInfo->Version = "1.0";
    pModuleInfo->DontProcess = TRUE;
}
```

### 5. Define Parameters

```cpp
void MyModule::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
    switch (ParamIndex)
    {
    case 0:
        pParamInfo->ParamType = ptDataFader;
        pParamInfo->Caption = "input";
        pParamInfo->IsInput = TRUE;
        pParamInfo->MinValue = 0.0f;
        pParamInfo->MaxValue = 100.0f;
        pParamInfo->DefaultValue = 50.0f;
        pParamInfo->CallBackType = ctNormal;
        pParamInfo->CallBackId = 1;
        pParamInfo->setEventClass(inputEvent);
        break;
    case 1:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "output";
        pParamInfo->IsOutput = TRUE;
        pParamInfo->ReadOnly = TRUE;
        pParamInfo->setEventClass(outputEvent);
        break;
    }
}
```

### 6. Handle Callbacks

```cpp
void MyModule::onCallBack(TUsineMessage* Message)
{
    if (Message->message == MSG_CHANGE && Message->wParam == 1)
    {
        float value = inputEvent.getData();
        outputEvent.setData(value * 2.0f);  // double the input
    }
}
```

### 7. Build and Deploy

Build your project as a dynamic library with the appropriate extension:
- **Windows:** `.usr-win64`
- **macOS Universal:** `.usr-macos64uni`
- **macOS Intel:** `.usr-osx64`
- **macOS ARM64:** `.usr-osxarm64`

Copy the built file to your Usine user modules folder, restart Usine, and find your module in the browser.

## Next Steps

- Read the platform-specific setup guides: [[docs/writing-user-modules-windows|Windows]] | [[docs/writing-user-modules-macos|macOS]]
- Explore the [[sdk/module-architecture|Module Architecture]] to understand the full lifecycle
- Browse the [[README|Examples]] for working code demonstrations
- Check the [[sdk/user-module-base|UserModuleBase reference]] for all available callbacks
