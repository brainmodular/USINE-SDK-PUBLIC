# AudioVolume
#example #audio #query #threading #simple

**Multi-channel audio gain control with mute.**

| | |
|---|---|
| **Module Name** | `audio volume` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Source** | `examples/AudioVolume/` |

## Description

A multi-channel audio volume module with a gain fader and mute switch. Demonstrates the audio query system for dynamic channel counts and thread-safe parameter handling with `std::atomic`.

## Parameters

Dynamic parameter count based on audio query (N = number of channels):

| # | Name | Type | I/O | Details |
|---|------|------|-----|---------|
| 0..N-1 | `in 1`, `in 2`, ... | `ptAudio` | Input | Audio inputs (separator: "audio in") |
| N..2N-1 | `out 1`, `out 2`, ... | `ptAudio` | Output (ReadOnly) | Audio outputs (separator: "audio out") |
| 2N | `gain` | `ptGainFader` | Input | Volume fader in dB (separator: "Volume") |
| 2N+1 | `mute` | `ptLeftLed` | Input | Mute toggle LED |

## How It Works

1. **Audio Query**: On creation, Usine shows a popup asking the user how many channels. Uses `sdkGetAudioQueryTitle()` and `sdkGetAudioQueryChannelList()`.
2. **`onCallBack`**: Computes `coeffGain = dBtoCoeff(gain) * (1 - mute)` and stores it in a `std::atomic<float>` for thread safety.
3. **`onProcess`**: For each channel, copies input to output and multiplies by the gain coefficient.

## Key Concepts Demonstrated

- **Audio query system** for dynamic multi-channel support
- **`std::atomic<float>`** for thread-safe parameter sharing between `onCallBack` and `onProcess`
- **`ptGainFader`** parameter type with dB scaling
- **`ptLeftLed`** as a mute toggle
- **`CanBeSavedInPreset = FALSE`** — excluding module from preset system
- Separator labels for parameter grouping

## Code Pattern: Audio Query

```cpp
void onGetModuleInfo(TMasterInfo*, TModuleInfo* pModuleInfo) override
{
    pModuleInfo->QueryListString = sdkGetAudioQueryTitle();
    pModuleInfo->QueryListValues = sdkGetAudioQueryChannelList();
    pModuleInfo->QueryListDefaultIdx = 1;
}

int onGetNumberOfParams(int QIdx1, int QIdx2) override
{
    numOfAudioInsOuts = sdkGetAudioQueryToNbChannels(QIdx1);
    return numOfAudioInsOuts * 2 + 2;  // ins + outs + gain + mute
}
```

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod|RingMod]] — Another multi-channel audio module
- [[examples/reverb|Reverb]] — Complex multi-channel audio processing

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
