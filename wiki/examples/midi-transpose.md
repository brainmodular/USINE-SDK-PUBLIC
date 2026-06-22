# MidiTranspose
#example #midi #simple

**MIDI note transposition utility.**

| | |
|---|---|
| **Module Name** | `midi transpose` |
| **Type** | `mtSimple` |
| **Color** | `clMIDIModuleColor` |
| **Source** | `examples/MidiTranspose/` |

## Description

Transposes incoming MIDI note messages by a specified number of semitones. Demonstrates MIDI event processing using `TUsineMidiCode` and array-based MIDI access.

## Parameters

| # | Name | Type | I/O | Range | Details |
|---|------|------|-----|-------|---------|
| 0 | `in` | `ptMidi` | Input | — | MIDI input stream |
| 1 | `out` | `ptMidi` | Output | — | Transposed MIDI output |
| 2 | `pitch` | `ptDataFader` | Input | -127 – +127 ht | Transposition amount (IsStoredInPreset) |

## How It Works

In `onProcess`:
1. Copies MIDI input to output
2. Iterates through each MIDI message in the event
3. For NOTEON and NOTEOFF messages, adds the pitch offset to Data1 (note number)
4. Clamps the result to 0–127

```cpp
for (int i = 0; i < size; i++)
{
    TUsineMidiCode code = midiOut.getArrayMidi(i);
    if (code.Msg == MIDI_NOTEON || code.Msg == MIDI_NOTEOFF)
    {
        int note = code.Data1 + (int)pitch;
        code.Data1 = std::clamp(note, 0, 127);
        midiOut.setArrayMidi(i, code);
    }
}
```

## Key Concepts Demonstrated

- **`ptMidi`** parameter type for MIDI streams
- **`TUsineMidiCode`** structure: Channel, Msg, Data1, Data2
- **`getArrayMidi` / `setArrayMidi`** for MIDI event access
- **MIDI constants**: `MIDI_NOTEON`, `MIDI_NOTEOFF`
- **Process-driven** module (no callbacks needed for the main logic)
- **`IsStoredInPreset`** for pitch offset persistence

## SDK Callbacks Used

`onGetModuleInfo` · `onGetParamInfo` · `onProcess`

## Related Examples

- [[examples/data-multiply|DataMultiply]] — Simple data processing
- [[examples/audio-volume|AudioVolume]] — Audio stream processing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
