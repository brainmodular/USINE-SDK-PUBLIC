# MultiThreading
#example #threading #errors #advanced

**Two Usine threads sharing a critical section.**

| | |
|---|---|
| **Module Name** | `multithreading` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/MultiThreading/` |

## Description

Demonstrates how to create threads with the SDK and protect shared code with a critical section.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `output` | `arrOut` | `ptArray` | Output | None |

## How It Works

1. **`onInitModule`**: creates a critical section (`sdkCriticalSectionCreate`) and two threads (`sdkThreadCreate`) with a 100 ms period.
2. **Thread functions**: each thread locks the critical section, raises its info message, then unlocks it. The thread functions are static, the module comes back through their `pModule` argument.
3. **Destructor**: destroys the threads (`sdkThreadDestroy`) and the critical section (`sdkCriticalSectionDestroy`).

```cpp
void MultiThreading::processThread1(void* pModule, TThreadPtr pThread)
{
	MultiThreading* module = static_cast<MultiThreading*>(pModule);
	module->sdkCriticalSectionLock(module->criticalSection);
	sdkRaiseError(INFO_THREAD_1);
	module->sdkCriticalSectionUnLock(module->criticalSection);
}
```

## Messages

| Message | Level | Raised when |
|---------|-------|-------------|
| `INFO_THREAD_1` | Info | Thread 1 runs |
| `INFO_THREAD_2` | Info | Thread 2 runs |

## Key Concepts Demonstrated

- **`sdkThreadCreate`** with a period
- **`sdkCriticalSectionCreate/Lock/UnLock/Destroy`** to protect shared code
- **Static thread function** with the module passed as context
- **Releasing the SDK resources** in the destructor
- **`DontProcess = TRUE`**: no audio processing

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onCreateParameters`

## Related Examples

- [[examples/ring-mod-multithread|RingModMultithread]] — Multi-threaded audio processing
- [[examples/background-job|BackgroundJob]] — Background task execution

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/sdk-functions|SDK Functions - Synchronization]]
