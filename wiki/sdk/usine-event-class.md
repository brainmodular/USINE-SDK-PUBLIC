# UsineEventClass Reference
#sdk #reference #class #events #audio #midi #data #color

`UsineEventClass` is the main data container class in the Usine SDK. It wraps a `UsineEventPtr` and provides methods for manipulating audio, data, MIDI, color, and text values.

**Defined in:** `sdk/UsineEventClass.h`

## Overview

Events are the universal data type in Usine. Every parameter is backed by an event, and all data flows through events. An event is essentially a dynamically-sized array of `float` (`TPrecision`) values with specialized accessors for different data interpretations.

## Construction & Lifecycle

```cpp
UsineEventClass();                          // Default constructor (null event)
explicit UsineEventClass(UsineEventPtr e);  // Wrap existing event pointer
```

> **Note:** `UsineEventClass` is **non-copyable** (copy assignment is deleted). Events are typically bound to parameters via `TParamInfo::setEventClass()` and managed by Usine.

### Manual Event Management

```cpp
void createEvent(int size);   // Allocate a new event with given size
void destroyEvent();          // Destroy the event and set to nullptr
```

Use `createEvent`/`destroyEvent` for temporary events (e.g., intermediate buffers in audio processing). Events bound to parameters are managed automatically.

## Core Access

```cpp
UsineEventPtr& event();             // Get raw event pointer
operator UsineEventPtr();           // Implicit cast to UsineEventPtr
explicit operator UsineEventPtr*(); // Explicit cast to pointer-to-pointer
```

## Size Management

```cpp
void setSize(int size);    // Set number of elements
int  getSize();            // Get number of elements
void setMaxSize(int size); // Set maximum capacity (pre-allocate)
```

## Data Access — Single Values

```cpp
void       setData(TPrecision value);   // Set the first element
TPrecision getData();                    // Get the first element

void         setColor(TUsineColor color); // Set as color value
TUsineColor  getColor();                   // Get as color value

void setPChar(AnsiCharPtr s);  // Set text value
AnsiCharPtr getPChar();        // Get text value
```

## Data Access — Arrays

```cpp
void       setArrayData(int index, TPrecision value);  // Set element at index
TPrecision getArrayData(int index);                     // Get element at index

void pushArrayData(TPrecision value);  // Append value (auto-resizes by PUSH_GRANULARITY=128)

void        setArrayColor(int index, TUsineColor color);  // Set color at index
TUsineColor getArrayColor(int index);                       // Get color at index

void           setArrayMidi(int index, TUsineMidiCode midi);  // Set MIDI code at index
TUsineMidiCode getArrayMidi(int index);                        // Get MIDI code at index

T3DPointF get3DPoint(int pointIndex);  // Get 3D point (event size must be multiple of 3)

TPrecision* getDataAddr();  // Get raw pointer to first element
```

## Pointer Data

For `ptPointer` type parameters:

```cpp
void* getPointerData();                // Get raw pointer data
int   getPointerDataSize();            // Get pointer data size in bytes
void  setPointerDataSize(int size);    // Set pointer data size in bytes
int   getPointerDataType();            // Get pointer data type
void  setPointerDataChanged();         // Notify Usine that pointer data changed
```

## Event Operations

### Copy & Compare

```cpp
LongBool compare(UsineEventClass& e);   // Compare two events for equality
void     copyfrom(UsineEventClass& src); // Copy data from source
void     copyto(UsineEventClass& dest);  // Copy data to destination
void     concat(UsineEventClass& e);     // Append another event's data
void     shift(int n);                   // Shift values right (n>0) or left (n<0)
```

### Arithmetic

```cpp
void add(UsineEventClass& e);          // Element-wise addition
void sub(UsineEventClass& e);          // Element-wise subtraction
void mult(TPrecision factor);          // Multiply all elements by scalar
void mult(UsineEventClass& e);         // Element-wise multiplication
void div(UsineEventClass& e);          // Element-wise division
void mod(UsineEventClass& e);          // Element-wise modulo
void power(UsineEventClass& e);        // Element-wise power
```

### Math Functions

```cpp
void exp();           // Exponential of all elements (in-place)
void sqrt();          // Square root of all elements (in-place)
TPrecision max();     // Get maximum value
TPrecision min();     // Get minimum value
void tresh(TPrecision min, TPrecision max);  // Clamp values to range
```

### Audio-Specific

```cpp
void multAudio(UsineEventClass& e);   // Multiply audio events (optimized)
void fadeIn();                         // Apply fade-in envelope
void fadeOut();                        // Apply fade-out envelope
void clearAudio();                     // Zero audio with denormalization
void denormalizeAudio();               // Add tiny value to prevent denormals
```

### MIDI

```cpp
void mixMidi(UsineEventClass& e);  // Mix MIDI events together
```

### Smoothing

```cpp
void smooth(TPrecision& oldValue, TPrecision factor);
// Smooth a single value: oldValue += (getData() - oldValue) * factor

void smooth(TPrecision& oldValue, TPrecision target, TPrecision factor);
// Smooth towards target: oldValue += (target - oldValue) * factor
```

## Smoothing Factor Constants

| Constant | Value | Description |
|----------|-------|-------------|
| `SMOOTH_VERY_VERY_SLOW` | `0.00001f` | Extremely slow smoothing |
| `SMOOTH_VERY_SLOW` | `0.0001f` | Very slow smoothing |
| `SMOOTH_SLOW` | `0.001f` | Slow smoothing |
| `SMOOTH_NORMAL` | `0.005f` | Normal smoothing |
| `SMOOTH_FAST` | `0.01f` | Fast smoothing |
| `SMOOTH_VERY_FAST` | `0.05f` | Very fast smoothing |
| `SMOOTH_VERY_VERY_FAST` | `0.1f` | Extremely fast smoothing |
| `SMOOTH_NO_SMOOTH` | `1.0f` | No smoothing (instant) |

## Usage Examples

### Basic Data Processing
See [[examples/data-multiply|DataMultiply]] for a complete example.
```cpp
UsineEventClass input, output;

// In onProcess:
float value = input.getData();
output.setData(value * 2.0f);
```

### Audio Processing
See [[examples/audio-volume|AudioVolume]] for a complete example.
```cpp
UsineEventClass audioIn, audioOut, gain;

// In onProcess:
audioOut.copyfrom(audioIn);
audioOut.multAudio(gain);
```

### Array Manipulation
```cpp
UsineEventClass arrayEvt;

arrayEvt.createEvent(10);
for (int i = 0; i < 10; i++)
    arrayEvt.setArrayData(i, (float)i);

float val = arrayEvt.getArrayData(5);  // 5.0
arrayEvt.pushArrayData(99.0f);         // appends, now size=11
```

### MIDI Processing
See [[examples/midi-transpose|MidiTranspose]] for a complete example.
```cpp
UsineEventClass midiIn, midiOut;

// In onProcess:
int size = midiIn.getSize();
midiOut.copyfrom(midiIn);
for (int i = 0; i < size; i++)
{
    TUsineMidiCode code = midiOut.getArrayMidi(i);
    if (code.Msg == MIDI_NOTEON)
        code.Data1 += 12;  // transpose up one octave
    midiOut.setArrayMidi(i, code);
}
```

## Related Pages

- [[sdk/data-types|Data Types & Constants]] — `TPrecision`, `TUsineColor`, `TUsineMidiCode`, etc.
- [[sdk/module-architecture|Module Architecture]] — How events flow through modules
- [[sdk/user-module-base|UserModuleBase Class]] — Module base class and callbacks
- [[sdk/sdk-functions|SDK Functions]] — Global utility functions
