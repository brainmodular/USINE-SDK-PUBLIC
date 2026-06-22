# MultiThreading
#example #threading #advanced

**Thread creation and synchronization with critical sections.**

| | |
|---|---|
| **Module Name** | `multithreading` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/MultiThreading/` |

## Description

Demonstrates how to create threads and protect shared data with critical sections. Includes a RAII-style `CriticalSectionGuard` helper for safe locking.

## Parameters

| # | Name | Type | I/O | Callback |
|---|------|------|-----|----------|
| 0 | `output` | `ptArray` | Output | None |

## How It Works

1. **`onInitModule`**: Creates a critical section (`sdkCriticalSectionCreate`) and two threads (`sdkThreadCreate`) that run every 100ms
2. **Thread functions**: Each thread locks the critical section, traces a message, then unlocks
3. **Destructor**: Destroys threads (`sdkThreadDestroy`) and critical section (`sdkCriticalSectionDestroy`)

## RAII Critical Section Pattern

```cpp
class CriticalSectionGuard
{
    TCriticalSectionPtr cs;
public:
    CriticalSectionGuard(TCriticalSectionPtr cs) : cs(cs)
    {
        sdkCriticalSectionLock(cs, 1000);  // 1s timeout
    }
    ~CriticalSectionGuard()
    {
        sdkCriticalSectionUnLock(cs);
    }
};

// Usage:
void processThread(void* pModule, TThreadPtr thread)
{
    auto* module = static_cast<MultiThreading*>(pModule);
    CriticalSectionGuard guard(module->criticalSection);
    // ... safe access to shared data ...
}
```

## Key Concepts Demonstrated

- **`sdkThreadCreate`** with periodic interval (100ms)
- **`sdkCriticalSectionCreate/Lock/UnLock/Destroy`** for mutex-like protection
- **RAII pattern** for exception-safe critical section management
- **Static thread callback** with module pointer passed as context
- **`DontProcess = TRUE`** — no audio processing

## SDK Callbacks Used

`onGetModuleInfo` · `onGetNumberOfParams` · `onAfterQuery` · `onInitModule` · `onGetParamInfo` · `onCallBack`

## Related Examples

- [[examples/ring-mod-multithread|RingModMultithread]] — Multi-threaded audio processing
- [[examples/background-job|BackgroundJob]] — Background task execution

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/sdk-functions|SDK Functions - Synchronization]]
