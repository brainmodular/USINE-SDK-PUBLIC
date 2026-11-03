# RingModMultithread
#example #audio #threading #query #advanced

**Ring modulator processing its channels in two threads.**

| | |
|---|---|
| **Module Name** | `ring mod multithread` |
| **Type** | `mtSimple` |
| **Color** | `clAudioModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/RingModMultithread/` |

## Description

Same ring modulator as [[examples/ring-mod|RingMod]], but the channels are processed in parallel by two threads. Demonstrates real-time multi-threading with a sync object and a critical section.

## Parameters

Same as [[examples/ring-mod|RingMod]]: N audio inputs (`audioInputs[]`) and outputs (`audioOutputs[]`), `mod` (`modAudioIn`) and `mix` (`mixFdrIn`, `MIX_FDR_CBID`).

## How It Works

1. **`onInitModule`**: creates two threads that only run when restarted (`UINT32_MAX` period), a sync object, a critical section and one work event per channel.
2. **`onProcess`**: sets the counter to 2, resets the sync object, restarts both threads and waits on the sync object (500 ms timeout).
3. **Threads**: thread 1 processes the even channels, thread 2 the odd ones. The last thread to finish sets the sync object.
4. **Destructor**: destroys the threads, the sync object, the critical section and the work events.

## Threading Architecture

```
onProcess()
    ├── counter = 2, reset sync object
    ├── restart thread1 → channels 0, 2, 4, ...
    ├── restart thread2 → channels 1, 3, 5, ...
    └── wait on sync object

Each thread:
    ├── process its channels
    ├── lock critical section, --counter, unlock
    └── if counter reached 0: set sync object
```

## Key Concepts Demonstrated

- **`sdkThreadCreate`** with a `UINT32_MAX` period and **`sdkThreadRestart`**
- **`sdkSyncObjectCreate/Reset/Set/Wait`** to wait for the threads
- **`sdkCriticalSectionCreate/Lock/UnLock`** to protect the shared counter
- **Channel interleaving** to share the load
- **Releasing the SDK resources** in the destructor

## SDK Callbacks Used

`onGetModuleInfo` · `onAfterQuery` · `onInitModule` · `onCreateParameters` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/ring-mod|RingMod]] — Same effect, single-threaded
- [[examples/multithreading|MultiThreading]] — Thread and critical section basics

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/usine-event-class|UsineEventClass]]
- [[sdk/module-architecture|Module Architecture]]
- [[sdk/sdk-functions|SDK Functions - Synchronization]]
