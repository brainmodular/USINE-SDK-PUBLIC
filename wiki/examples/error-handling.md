# ErrorHandling
#example #errors #trace #callback #simple

**Registered error, warning and info messages, filtered by the verbosity level of each instance.**

| | |
|---|---|
| **Module Name** | `error handling` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.261125 |
| **Source** | `examples/ErrorHandling/` |

## Description

Shows how to report problems to the user with the report system of `UserError.h` (included by `UserDefinitions.h`). The module registers one message per level and raises them when a button is clicked. Change its `verbosity level` to see which messages reach the Usine trace and which ones are filtered out.

Every example of the SDK reports its errors this way: modules never call the raw `sdkTrace*` functions.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `error btn` | `errorBtnIn` | `ptButton` | Input | `ctImmediate` (`ERROR_BTN_CBID`) |
| 1 | `warning btn` | `warningBtnIn` | `ptButton` | Input | `ctImmediate` (`WARNING_BTN_CBID`) |
| 2 | `info btn` | `infoBtnIn` | `ptButton` | Input | `ctImmediate` (`INFO_BTN_CBID`) |
| 3 | `verbosity level` | *created by the SDK* | `ptComboBox` | — | none |

The `verbosity level` combo box is added by the SDK to every module that registers at least one report. It is hidden by default: show all the parameters of the module to find it, in the `Report` section. Its value is saved with the patch.

## How It Works

1. **`ModuleDefinition.h`**: registers the messages with `registerReport()` at static initialization, and keeps the returned indices in `inline const int` constants.
2. **`onCallBack`**: each button raises its message with a different overload of `sdkReport()`:
   - `error btn` → `sdkReport(ERROR_WRITE, " And this is an additional msg")`: appends an extra text to the registered message.
   - `warning btn` → `sdkReport(WARNING_WRITE, true)`: `forceLog = true` also writes the message in the Usine log file.
   - `info btn` → `sdkReport(INFO_WRITE)`: the plain form.
3. **`verbosity level`**: `none` traces nothing, `error` only the errors, `warn` the errors and warnings, `all` every message. Each instance has its own level: two ErrorHandling modules in a patch can filter differently.
4. **`onProcess`**: not used (`DontProcess = TRUE`).

## Report API

```cpp
enum class UReportLevel     { Error = 1, Warn, Info };
enum class UReportVerbosity { None, Error, Warn, All };

// UserError.h, module-wide
int  registerReport(UReportLevel level, const char* message);

// UserModuleBase members, filtered by the verbosity level of the instance
void sdkReport(int index, bool forceLog = false) const;
void sdkReport(int index, const char* extraMsg,        bool forceLog = false) const;
void sdkReport(int index, const std::string& extraMsg, bool forceLog = false) const;
void             sdkSetVerbosity(UReportVerbosity v);  // moves the combo box of the instance
UReportVerbosity sdkGetVerbosity() const;              // All before the combo box is created
```

| Level | Sent to | Shown when verbosity is | `forceLog` |
|---|---|---|---|
| `UReportLevel::Error` | error trace (`sdkTraceErrorChar`) | `Error`, `Warn`, `All` | no effect |
| `UReportLevel::Warn` | warning trace (`sdkTraceWarningChar`) | `Warn`, `All` | also written to the log |
| `UReportLevel::Info` | plain trace (`sdkTraceChar`) | `All` | also written to the log |

With `forceLog = true`, Warn and Info messages are written to the log file even when the verbosity level hides them from the trace. An invalid index is ignored silently.

`sdkReport` is a member of `UserModuleBase`: from a static function, such as a thread proc, call it through the instance (`module->sdkReport(...)`, see [[examples/multithreading|MultiThreading]]). Code that has no instance at all, such as a global library callback, calls `reportWithVerbosity(verbosity, index, extraMsg, forceLog)` with a verbosity of its own.

## Code Pattern: Register and Raise

```cpp
// ModuleDefinition.h
inline const int ERROR_WRITE   USINE_MODULE_LOCAL = registerReport(UReportLevel::Error, "this is an error print");
inline const int WARNING_WRITE USINE_MODULE_LOCAL = registerReport(UReportLevel::Warn, "this is a warn print");
inline const int INFO_WRITE    USINE_MODULE_LOCAL = registerReport(UReportLevel::Info, "this is an info print");

// ErrorHandling.cpp
void ErrorHandling::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case ERROR_BTN_CBID:
			sdkReport(ERROR_WRITE, " And this is an additional msg");
			break;
		case WARNING_BTN_CBID:
			// forceLog: also writes the message in the Usine log file
			sdkReport(WARNING_WRITE, true);
			break;
		case INFO_BTN_CBID:
			sdkReport(INFO_WRITE);
			break;
		default:
			break;
	}
}
```

## Guidelines

- **Register the messages in `ModuleDefinition.h`** with `inline const int`, named `ERROR_`, `WARNING_` or `INFO_` followed by what happened.
- **Don't repeat the module name** in the messages: the trace already starts with `[MODULE_NAME]`.
- **Use `extraMsg` for runtime details** such as a file name or a value, instead of registering a new message for each case. End the registered message with `": "` when it expects one.
- **Raise once, not at every bloc**: when a problem lasts, keep a flag and raise when it appears (see `grainOverlapWarned` in [[examples/granulator|Granulator]]).
- **The verbosity level belongs to the instance**: the registry of messages is shared by every instance of the module binary, but each instance filters them with its own combo box.
- **Don't use CallBackIds from `CALLBACK_SDK_LIMIT` (`0xFFFF00`)**: they are reserved by the SDK, and `sdkCreateParam` refuses such a parameter.
- **Name clash**: `UserError.h` declares `UReportLevel`, `UReportVerbosity`, `UReport`, `uReportList`, `registerReport` and `reportWithVerbosity` in the global namespace. If your module defines one of these names, rename yours or put it in a namespace.

## Key Concepts Demonstrated

- **`registerReport`**: register a message once and get its index back
- **`sdkReport`**: the plain, `extraMsg` and `forceLog` variants
- **`verbosity level`**: the combo box added by the SDK, which filters the messages of each instance
- **`onCreateParameters` + `constexpr TParamInfo`**: parameters defined in `ModuleDefinition.h`
- **`DontProcess = TRUE`**: callback-only module

## SDK Callbacks Used

`onGetModuleInfo` · `onCreateParameters` · `onCallBack`

## Related Examples

Messages raised by the other examples:

- [[examples/draw-trajectory-box|DrawTrajectoryBox]], [[examples/simple-pad|SimplePad]] — Errors when saved data is invalid
- [[examples/granulator|Granulator]] — Errors when automation recording fails, a warning raised once when grains overlap
- [[examples/usine-pointers|UsinePointers]] — Error when a pointer has the wrong type
- [[examples/usine-chunks|UsineChunks]] — Warning and info on chunk notifications
- [[examples/data-sampler|DataSampler]], [[examples/multithreading|MultiThreading]] — Info messages
- [[examples/global-array|GlobalArray]], [[examples/usine-objects|UsineObjects]] — Values traced as infos with an extra text

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
- [[sdk/module-conventions|Module Conventions]]
- [[sdk/sdk-functions|SDK Functions]] — `sdkTrace*` functions used under the hood
