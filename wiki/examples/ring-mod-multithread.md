# RingModMultithread
#example #audio #threading #query #advanced

**Ring modulator with parallel multi-threaded processing.**

| | |
|---|---|
| **Module Name** | `ring mod multithread` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Source** | `examples/RingModMultithread/` |

## Description

Same ring modulator as [[examples/ring-mod|RingMod]], but processes channels in parallel using two worker threads. Demonstrates real-time multi-threading with sync primitives.

## Parameters

Same as [[examples/ring-mod|RingMod]] — N audio ins/outs + mod + mix.

## How It Works

1. **`onInitModule`**: Creates a sync object, a critical section, and two worker threads (manual restart mode)
2. **`onProcess`**: Resets sync, sets counter=2, restarts both threads, waits for completion (500ms timeout)
3. **Workers**: Thread 1 processes even channels, thread 2 processes odd channels. Last thread to finish signals the sync object.

## Threading Architecture

```
onProcess()
    ├── Reset sync object, counter = 2
    ├── Restart thread1 → processes channels 0, 2, 4, ...
    ├── Restart thread2 → processes channels 1, 3, 5, ...
    └── Wait on sync object

Each thread:
    ├── Process assigned channels
    ├── Lock critical section
    ├── if (--counter == 0) signal sync
    └── Unlock critical section
```

## Key Concepts Demonstrated

- **`sdkThreadCreate`** with `UINT32_MAX` interval (manual restart)
- **`sdkSyncObjectCreate/Reset/Set/Wait`** — process synchronization
- **`sdkCriticalSectionCreate/Lock/UnLock`** — shared counter protection
- **Channel interleaving** for load balancing

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod|RingMod]] — Same effect, single-threaded
- [[examples/multithreading|MultiThreading]] — Thread and critical section basics

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
- [[sdk/sdk-functions|SDK Functions - Synchronization]]
