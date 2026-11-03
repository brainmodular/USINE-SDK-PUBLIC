# MidiTranspose
#example #midi #simple

**MIDI note transposition.**

| | |
|---|---|
| **Module Name** | `midi transpose` |
| **Type** | `mtSimple` |
| **Color** | `clMIDIModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/MidiTranspose/` |

## Description

Transposes the incoming MIDI notes by a number of semitones. Demonstrates MIDI processing with `TUsineMidiCode` and the MIDI array access of `UsineEventClass`.

## Parameters

| # | Name | Member | Type | I/O | Details |
|---|------|--------|------|-----|---------|
| 0 | `in` | `midiIn` | `ptMidi` | Input | MIDI input |
| 1 | `out` | `midiOut` | `ptMidi` | Output | Transposed MIDI |
| 2 | `pitch` | `pitchFdrIn` | `ptDataFader` | Input | -127 – +127 halftones, IsStoredInPreset, no callback |

## How It Works

In `onProcess`:
1. Copies the MIDI input to the output
2. Goes through each MIDI message of the output
3. For `MIDI_NOTEON` and `MIDI_NOTEOFF` messages, adds the pitch to `Data1` (the note number), clamped to 0 – 127

```cpp
const int pitch = static_cast<int>(std::round(pitchFdrIn.getData()));
for (int i = 0; i < sizeMidiIn; i++)
{
	TUsineMidiCode code = midiOut.getArrayMidi(i);
	if (code.Msg == MIDI_NOTEON || code.Msg == MIDI_NOTEOFF)
	{
		code.Data1 = std::min(127, std::max(0, static_cast<int>(code.Data1) + pitch));
		midiOut.setArrayMidi(i, code);
	}
}
```

## Key Concepts Demonstrated

- **`ptMidi`** parameters for MIDI streams
- **`TUsineMidiCode`**: Channel, Msg, Data1, Data2
- **`getArrayMidi` / `setArrayMidi`** to access the messages
- **A parameter without callback**: `pitch` is read directly in `onProcess`
- **`IsStoredInPreset`** to save the pitch in presets

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onProcess`

## Related Examples

- [[examples/data-multiply|DataMultiply]] — Simple data processing
- [[examples/audio-volume|AudioVolume]] — Audio stream processing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
