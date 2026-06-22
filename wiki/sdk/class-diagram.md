# SDK Class Diagram

#sdk #architecture #reference #class #types #diagram

Complete class diagram of the Usine Hollyhock SDK showing all classes, structures, enums, and their relationships.

## 1. Core Classes & Relationships

The two main classes and the structures they interact with directly.

```mermaid
classDiagram
    direction TB

    class UserModuleBase {
        <<abstract>>
        #float panelWidth
        #float panelHeight
        #TModuleInfo* m_moduleInfo
        #TMasterInfo* m_masterInfo$
        +onGetModuleInfo()* 🔴
        +onGetParamInfo()* 🔴
        +onCallBack() 🟡
        +onProcess() 🔵
        +onProcessVideo() 🔵
        +onInitModule()
        +onGetNumberOfParams()
        +onAfterQuery()
        +startJob()
        +onJobBegin()
        +onJobProcess()
        +onJobEnd()
        +onGetChunkLen()
        +onGetChunk()
        +onSetChunk()
        +onCreateSettings()
        +onPaint() 🟢
        +onMouseDown() 🟢
        +onMouseMove() 🟢
        +onMouseUp() 🟢
        +onRandomize()
        +onReset()
        +sdkDrawPoint()
        +sdkFillRect()
        +sdkFillText()
        +sdkRepaintPanel()
        +sdkThreadCreate()
        +sdkNotifyUsine()
    }

    class UsineEventClass {
        -UsineEventPtr p_event_
        +createEvent(int)
        +destroyEvent()
        +setSize() / getSize()
        +setData() / getData()
        +setArrayData() / getArrayData()
        +setPChar() / getPChar()
        +setColor() / getColor()
        +setArrayMidi() / getArrayMidi()
        +getPointerData()
        +copyfrom() / copyto()
        +add() / sub() / mult() / div()
        +multAudio()
        +fadeIn() / fadeOut()
        +clearAudio()
        +smooth()
    }

    class TMasterInfo {
        <<struct — provided by Usine>>
        ~150 function pointers
        Events · Audio · Drawing
        MIDI · Settings · Threads
        Sync · GlobalArrays
        Objects · Chunks
    }

    class TUsineMessage {
        <<struct>>
        +NativeInt message
        +NativeInt wParam ← CallBackId
        +NativeInt lParam ← MSG_CHANGE etc
        +NativeInt result
    }

    UserModuleBase --> TMasterInfo : uses static ref
    UserModuleBase --> UsineEventClass : owns N members
    UserModuleBase --> TUsineMessage : receives in onCallBack
    UsineEventClass --> TMasterInfo : calls function ptrs

    note for UserModuleBase "🔴 = pure virtual (mandatory) 
    🟡 = callback 
    🔵 = real-time thread 
    🟢 = graphics/mouse
    "
```

---

## 2. Module Description Structs

Structures used to describe a module and its parameters to Usine.

```mermaid
classDiagram
    direction TB

    class TModuleInfo {
        <<struct>>
        +AnsiCharPtr Name
        +AnsiCharPtr Description
        +TModuleType ModuleType
        +TUsineColor BackColor
        +int NumberOfParams
        +float DefaultWidth
        +float DefaultHeight
        +LongBool DontProcess
        +LongBool CanRecord
        +LongBool CanBeRandomized
        +LongBool CanBeReset
        +LongBool CanBeSavedInPreset
        +AnsiCharPtr Version
        +AnsiCharPtr QueryListString
        +AnsiCharPtr QueryListValues
        +int QueryListDefaultIdx
        +int NumberOfVideoInputs
        +int NumberOfVideoOutputs
    }

    class TParamInfo {
        <<struct>>
        +TParamType ParamType
        +AnsiCharPtr Caption
        +LongBool IsInput / IsOutput
        +TScale Scale
        +TUsineColor Color / OffColor
        +TPrecision MinValue / MaxValue
        +TPrecision DefaultValue
        +AnsiCharPtr Symbol / Format
        +AnsiCharPtr ListBoxStrings
        +LongBool ReadOnly
        +TFastCallBackType CallBackType
        +NativeInt CallBackId
        +LongBool DontSave
        +LongBool IsSeparator
        +LongBool IsStoredInPreset
        +UINT32 PointerDataType
        +setEventClass(UsineEventClass&)
    }

    class TModuleType {
        <<enum int32_t>>
        mtSimple
        mtControl
        mtVideo
        mtDeviceAudio
        mtDeviceMidi
        mtDeviceDMX
        mtDeviceLaser
        mtDeviceOther
        mtPluginWrapper
        mtOther
    }

    class TParamType {
        <<enum int32_t>>
        ptDataFader · ptDataField
        ptAudio · ptMidi
        ptGainFader · ptColor
        ptButton · ptSwitch
        ptListBox · ptArray
        ptText · ptPointer
        ptRightLed · ptLeftLed
        ptTriggerLed
        ptFileName · ptBitwise
    }

    class TFastCallBackType {
        <<enum int32_t>>
        ctNormal
        ctImmediate
        ctAsynchronous
        cttVideo
        cbtRealTime
        cbtGraphic
        cbtNetwork
        ctNone
    }

    class TScale {
        <<enum int32_t>>
        scLinear
        scLog
        scExp
    }

    TModuleInfo --> TModuleType : ModuleType
    TParamInfo --> TParamType : ParamType
    TParamInfo --> TScale : Scale
    TParamInfo --> TFastCallBackType : CallBackType
```

---

## 3. Data Structures — Geometry & Color

```mermaid
classDiagram
    direction TB

    class TPointF {
        <<struct>>
        +float x
        +float y
    }

    class T3DPointF {
        <<struct>>
        +float x
        +float y
        +float z
    }

    class TRectF {
        <<struct>>
        +float left
        +float top
        +float right
        +float bottom
    }

    class TColorArgb {
        <<struct>>
        +float a
        +float r
        +float g
        +float b
    }

    class TColorAhsl {
        <<struct>>
        +float a
        +float h
        +float s
        +float l
    }

    TPointF <|-- T3DPointF : extends with z
    TColorArgb <.. TColorAhsl : converts to/from
```

## 4. Data Structures — MIDI, Video & Time

```mermaid
classDiagram
    direction TB

    class TUsineMidiCode {
        <<struct>>
        +unsigned char Channel
        +unsigned char Msg
        +unsigned char Data1
        +unsigned char Data2
    }

    class TUsinePixel {
        <<struct>>
        +BYTE B
        +BYTE G
        +BYTE R
        +BYTE A
    }

    class TUsineFrame {
        <<struct>>
        +int Width
        +int Height
        +TUsinePixelPtr Pixels
        +LongBool Valid
    }

    class TUsineILDAPoint {
        <<struct>>
        +float x
        +float y
        +float z
        +TUsineColor Color
    }

    class TVstTimeInfo {
        <<struct>>
        +double samplePos
        +double sampleRate
        +double tempo
        +double PpqPos
        +double barStartPos
        +long timeSigNumerator
        +long timeSigDenominator
    }

    TUsineFrame *-- TUsinePixel : Pixels array
    TUsineMidiCode .. TVstTimeInfo : both used in onProcess
```

---

## 5. Enums — Threading & Notifications

```mermaid
classDiagram
    direction TB

    class TMouseButton {
        <<enum>>
        mbLeft
        mbRight
        mbMiddle
    }

    class TThreadPriority {
        <<enum int32_t>>
        tpIDLE
        tpLow
        tpMedium
        tpRealTime
        tpTimeCritical
    }

    class TNotifyUsineMode {
        <<enum int32_t>>
        numMainThread
        numAsync
        numVideo
        numRealTime
        numGraphic
        numNetwork
    }

    class TTextAlign {
        <<enum>>
        taCenter
        taLeading
        taTrailing
    }

    TThreadPriority -- TNotifyUsineMode : thread context
    TMouseButton -- TTextAlign : UI enums
```

## 6. Enums — Chunks & Settings

```mermaid
classDiagram
    direction TB

    class TChunkOperation {
        <<enum>>
        coAddOrModifyItem
        coGetItem
        coClearItem
        coDeleteItem
        coClearChunk
        coDeleteChunk
    }

    class TFlowType {
        <<enum>>
        ftNone = 0
        ftDataFloat = 1
        ftDataInteger = 8
        ftTextGeneral = 2048
        ftColor = 131072
    }

    class TSettingInOutletMode {
        <<enum int32_t>>
        siomInOut
        siomIn
        siomOut
        siomNone
        sioOther
    }

    TChunkOperation -- TFlowType : used together in sdkChunkOperation
```

## 7. Type Aliases

```mermaid
classDiagram
    direction TB

    class PrimitiveTypes {
        <<typedefs>>
        TPrecision = float
        LongBool = int
        AnsiCharPtr = const char*
        NativeInt = int64_t
        NativeUInt = uint64_t
        UINT32 = uint32_t
        UINT64 = uint64_t
        BYTE = uint8_t
    }

    class UsineTypes {
        <<typedefs>>
        TUsineColor = UINT32
        TShiftState = UINT32
        UsineEventPtr = UsineEvent*
    }

    class HandleTypes {
        <<typedefs — opaque void*>>
        TAudioFilePtr
        TCriticalSectionPtr
        TThreadPtr
        TSyncObjectPtr
        TUsineFFTPtr
        TUsineChunkPtr
        TUsinePtr
    }

    PrimitiveTypes <-- UsineTypes : based on
    UsineTypes <-- HandleTypes : used alongside
```

## File Mapping

| SDK File | Contains |
|----------|----------|
| `UserDefinitions.h` | Master include — pulls in all other headers |
| `UsineDefinitions.h` | All types, structs, enums, constants, `TMasterInfo`, `TModuleInfo`, `TParamInfo` |
| `UserModule.h` | `UserModuleBase` class definition, 3 mandatory global functions |
| `UserModule.cpp` | `UserModuleBase` implementation, DLL export bridge functions |
| `UsineEventClass.h` | `UsineEventClass` wrapper class |
| `UsineFunctions.h` | Inline SDK utility functions (wrappers around `TMasterInfo` pointers) |
| `UserUtils.h` | Color conversion, geometry, numeric helper functions |

## Inheritance Pattern

```mermaid
classDiagram
    direction LR

    class UserModuleBase {
        <<abstract - sdk/UserModule.h>>
    }

    class YourModule {
        <<your code>>
        -UsineEventClass input
        -UsineEventClass output
        +onGetModuleInfo()
        +onGetParamInfo()
        +onCallBack()
        +onProcess()
    }

    UserModuleBase <|-- YourModule : inherits

    note for YourModule "Every user module inherits\nfrom UserModuleBase and\noverrides virtual callbacks"
```

## Data Flow Relationships

```mermaid
flowchart LR
    subgraph "Usine Host"
        MI[TMasterInfo]
        MOD[TModuleInfo]
    end

    subgraph "Your Module"
        UMB[UserModuleBase]
        EVT1[UsineEventClass\ninput]
        EVT2[UsineEventClass\noutput]
    end

    subgraph "Parameter System"
        PI[TParamInfo]
        MSG[TUsineMessage]
    end

    MI -->|provides function ptrs| UMB
    UMB -->|fills| MOD
    UMB -->|fills per param| PI
    PI -->|binds via setEventClass| EVT1
    PI -->|binds via setEventClass| EVT2
    MSG -->|delivered to onCallBack| UMB
    EVT1 -->|read in onProcess| UMB
    UMB -->|write in onProcess| EVT2

    style MI fill:#59d,color:#fff
    style UMB fill:#4a9,color:#fff
    style EVT1 fill:#da5,color:#fff
    style EVT2 fill:#da5,color:#fff
```

## Related Pages

- [[sdk/module-architecture|Module Architecture]] — Lifecycle and data flow
- [[sdk/user-module-base|UserModuleBase Class]] — Complete callback reference
- [[sdk/usine-event-class|UsineEventClass]] — Event manipulation API
- [[sdk/data-types|Data Types & Constants]] — All types and enums in detail
- [[sdk/sdk-functions|SDK Functions]] — Global utility functions
- [[sdk/utility-functions|Utility Functions]] — Color, geometry, math helpers
- [[getting-started|Getting Started]] — Your first module
