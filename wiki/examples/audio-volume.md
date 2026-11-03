# AudioVolume
#example #audio #query #simple

**Multi-channel audio gain control with mute.**

| | |
|---|---|
| **Module Name** | `audio volume` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Version** | 2.0 |
| **Source** | `examples/AudioVolume/` |

## Description

A multi-channel audio volume module with a gain fader and a mute switch. Demonstrates the audio query system, which lets the user choose the number of channels when the module is added to a patch.

## Parameters

Created in `onCreateParameters`, N is the number of channels chosen in the query popup:

| # | Name | Member | Type | I/O | Details |
|---|------|--------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `audioInputs[]` | `ptAudio` | Input | Separator "audio in" |
| N..2N-1 | `out 1`, `out 2`, ... | `audioOutputs[]` | `ptAudio` | Output | Separator "audio out" |
| 2N | `gain` | `gainFdrIn` | `ptGainFader` | Input | In dB, separator "volume", `ctImmediate` (`GAIN_FDR_CBID`) |
| 2N+1 | `mute` | `muteSwIn` | `ptLeftLed` | Input | `ctNormal` (`MUTE_SW_CBID`) |

## How It Works

1. **Audio query**: `onGetModuleInfo` sets `QueryListString` and `QueryListValues` with `sdkGetAudioQueryTitle()` and `sdkGetAudioQueryChannelList()`, Usine shows the popup.
2. **`onAfterQuery`**: Converts the answer to a number of channels with `sdkGetAudioQueryToNbChannels()`, clamped to `USINE_AUDIO_CHANNEL_MODULE_MAX`.
3. **`onCreateParameters`**: Creates one input and one output per channel, named with `sdkGetAudioQueryChannelNames()`, then the gain and mute.
4. **`onCallBack`**: Converts the gain to a linear coefficient with `sdkDbToCoeff()`, then applies the mute: `coeffGain = sdkDbToCoeff(gain) * (1 - mute)`. The mute must zero the coefficient, not the dB value: 0 dB is a gain of 1.
5. **`onProcess`**: Copies each input to its output and multiplies it by the coefficient.

## Key Concepts Demonstrated

- **Audio query system** for a dynamic number of channels
- **Dynamic parameters**: a `TParamInfo` copied and modified in a loop
- **`ptGainFader`** with dB to linear conversion
- **`ptLeftLed`** as a mute switch
- **`CanBeSavedInPreset = FALSE`**: module excluded from presets

## SDK Callbacks Used

`onGetModuleInfo` · `onAfterQuery` · `onCreateParameters` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod|RingMod]] — Another multi-channel audio module
- [[examples/reverb|Reverb]] — Complex multi-channel audio processing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
