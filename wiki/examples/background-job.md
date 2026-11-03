# BackgroundJob
#example #threading #advanced

**Long computation in a background thread with the job API.**

| | |
|---|---|
| **Module Name** | `background job` |
| **Type** | `mtBackgroundJob` |
| **Color** | `clInterfaceDesignModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/BackgroundJob/` |

## Description

Demonstrates the background job system of the SDK, to run a heavy computation without blocking Usine. The module overrides the three job callbacks and starts the job from a button.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `data1` | `oneFdrIn` | `ptDataField` | Input, 0 – 1 | `ctImmediate` (`ONE_FDR_CBID`) |
| 1 | `data2` | `twoFdrIn` | `ptDataField` | Input, 0 – 1 | `ctImmediate` (`TWO_FDR_CBID`) |
| 2 | `start job` | `startBtnIn` | `ptButton` | Input | `ctImmediate` (`START_BTN_CBID`) |
| 3 | `out` | `outFdrOut` | `ptDataField` | Output (ReadOnly, DontSave) | None |
| 4 | `processing` | — | `ptRightLed` | Output (ReadOnly, DontSave), created by the SDK | None |
| 5 | `processing time` | — | `ptDataField` (ms) | Output (ReadOnly, DontSave), created by the SDK | None |

Parameters 4 and 5 are added by the SDK because the module type is `mtBackgroundJob`, see [[sdk/user-module-base|UserModuleBase: automatic job parameters]].

## How It Works

1. `onCallBack` stores `data1` and `data2`, and calls `startJob()` when `start job` is clicked.
2. The SDK switches the `processing` led on and starts the job thread.
3. **`onJobBegin`** (job thread): nothing to set up here.
4. **`onJobProcess`** (job thread): runs the computation, 100 million iterations of nested `std::sin`.
5. **`onJobEnd`** (called back by Usine once the job is done): writes the result.
6. The SDK writes the duration in `processing time` and switches the led off.

## Job Lifecycle

```
User clicks "start job"
    └── onCallBack() → startJob()        ← notifies Usine
        ├── onJobBegin()                  ← job thread
        ├── onJobProcess()                ← job thread: heavy work
        └── onJobEnd()                    ← callback dispatch, after the job thread
```

`startJob()` only asks Usine to start the job, the SDK starts the job thread when the notification comes back. A start requested while a job is running waits for the end of that job.

## Key Concepts Demonstrated

- **`startJob()`** and the **`onJobBegin` / `onJobProcess` / `onJobEnd`** callbacks
- **`mtBackgroundJob`**: `processing` led and `processing time` output managed by the SDK
- **`DontProcess = TRUE`**: no audio processing needed
- **`CanBeRandomized = TRUE`**: randomize support
- Values copied in members before the job, read by the job thread

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onCallBack` · `onJobBegin` · `onJobProcess` · `onJobEnd`

## Related Examples

- [[examples/multithreading|MultiThreading]] — Manual thread management
- [[examples/ring-mod-multithread|RingModMultithread]] — Real-time multi-threading

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/module-architecture|Module Architecture]] — Background jobs
