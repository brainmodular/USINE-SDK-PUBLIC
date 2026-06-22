# BackgroundJob
#example #threading #advanced

**Asynchronous background task execution.**

| | |
|---|---|
| **Module Name** | `big task async process test` |
| **Type** | `mtSimple` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Source** | `examples/BackgroundJob/` |

## Description

Demonstrates the SDK's background job system for executing heavy computations without blocking the audio thread. Shows the complete job lifecycle: begin, process, and end hooks.

## Parameters

| # | Name | Type | I/O | Range | Callback |
|---|------|------|-----|-------|----------|
| 0 | `data1` | `ptDataField` | Input | 0–1 | `ctImmediate` |
| 1 | `data2` | `ptDataField` | Input | 0–1 | `ctImmediate` |
| 2 | `start job` | `ptButton` | Input | — | `ctImmediate` |
| 3 | `processing` | `ptRightLed` | Output (ReadOnly, DontSave) | — | None |
| 4 | `out` | `ptDataField` | Output (ReadOnly, DontSave) | — | None |

## How It Works

1. User sets `data1` and `data2` input values
2. Clicking `start job` calls `startJob()` (SDK method)
3. **`onJobBegin`** (background thread): Sets processing LED on
4. **`onJobProcess`** (background thread): Runs heavy computation (100M iterations of nested sin calculations)
5. **`onJobEnd`** (main thread): Sets processing LED off, outputs the result

## Job Lifecycle

```
User clicks "start job"
    └── startJob()
        ├── onJobBegin()      ← background thread: setup
        ├── onJobProcess()    ← background thread: heavy work
        └── onJobEnd()        ← main thread: collect results
```

> **Important:** `onJobBegin` and `onJobProcess` run on a background thread. `onJobEnd` runs on the main thread, making it safe to update outputs and UI.

## Key Concepts Demonstrated

- **`startJob()`** — launches the background job thread
- **`onJobBegin/onJobProcess/onJobEnd`** lifecycle hooks
- **`DontProcess = TRUE`** — no audio processing needed
- **`CanBeRandomized = TRUE`** — randomize support
- **`ptRightLed`** as a processing indicator
- Thread-safe result passing via class member variables

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess` · `onJobBegin` · `onJobProcess` · `onJobEnd`

## Related Examples

- [[examples/multithreading|MultiThreading]] — Manual thread management
- [[examples/ring-mod-multithread|RingModMultithread]] — Real-time multi-threading

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/sdk-functions|SDK Functions - Synchronization]]
