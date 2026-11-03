# ErrorHandling
#example #errors #trace #callback #simple

**Registered error, warning and info messages with a runtime verbosity level.**

| | |
|---|---|
| **Module Name** | `error handling` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Version** | 1.0 |
| **Source** | `examples/ErrorHandling/` |

## Description

Shows how to report problems to the user with the error system of `UserError.h` (included by `UserDefinitions.h`). The module registers one message per level and raises them when a button is clicked. A text input changes the verbosity level, so you can see which messages reach the Usine trace and which ones are filtered out.

Every example of the SDK reports its errors this way: modules never call the raw `sdkTrace*` functions.

## Parameters

| # | Name | Member | Type | I/O | Callback |
|---|------|--------|------|-----|----------|
| 0 | `error btn` | `errorBtnIn` | `ptButton` | Input | `ctImmediate` (`ERROR_BTN_CBID`) |
| 1 | `warning btn` | `warningBtnIn` | `ptButton` | Input | `ctImmediate` (`WARNING_BTN_CBID`) |
| 2 | `info btn` | `infoBtnIn` | `ptButton` | Input | `ctImmediate` (`INFO_BTN_CBID`) |
| 3 | `verbosity level` | `verbosityTxtIn` | `ptText` | Input | `ctImmediate` (`VERBOSITY_TXT_CBID`) |

## How It Works

1. **`ModuleDefinition.h`**: registers the messages with `RegisterError()` at static initialization, and keeps the returned indices in `inline const int` constants.
2. **`onCallBack`**: each button raises its message with a different overload of `sdkRaiseError()`:
   - `error btn` → `sdkRaiseError(ERROR_WRITE, " And this is an additional msg")`: appends an extra text to the registered message.
   - `warning btn` → `sdkRaiseError(WARNING_WRITE, true)`: `force_log = true` also writes the message in the Usine log file.
   - `info btn` → `sdkRaiseError(INFO_WRITE)`: the plain form.
3. **`verbosity level`**: typing `None`, `Error`, `Warn`, `Info` or `All` calls `setVerbosityLevel()`. An empty text is ignored, any other text raises `WARNING_UNKNOWN_VERBOSITY` with the text appended, and keeps the current level.
4. **`onProcess`**: not used (`DontProcess = TRUE`).

## Error API

```cpp
enum class Level     { Error, Warn, Info };
enum class Verbosity { None = -1, Error, Warn, Info, All = Info };

int  RegisterError(Level level, const char* message);
void sdkRaiseError(int index, bool force_log = false);
void sdkRaiseError(int index, const char* extraMsg,        bool force_log = false);
void sdkRaiseError(int index, const std::string& extraMsg, bool force_log = false);
void setVerbosityLevel(Verbosity v);   // default: Verbosity::All
```

| Level | Sent to | Shown when verbosity is | `force_log` |
|---|---|---|---|
| `Level::Error` | error trace (`sdkTraceErrorChar`) | `Error`, `Warn`, `Info`/`All` | no effect |
| `Level::Warn` | warning trace (`sdkTraceWarningChar`) | `Warn`, `Info`/`All` | also written to the log |
| `Level::Info` | plain trace (`sdkTraceChar`) | `Info`/`All` | also written to the log |

With `force_log = true`, Warn and Info messages are written to the log file even when the verbosity level hides them from the trace. An invalid index is ignored silently.

## Code Pattern: Register and Raise

```cpp
// ModuleDefinition.h
inline const int ERROR_WRITE               = RegisterError(Level::Error, "this is an error print");
inline const int WARNING_WRITE             = RegisterError(Level::Warn, "this is a warn print");
inline const int INFO_WRITE                = RegisterError(Level::Info, "this is an info print");
inline const int WARNING_UNKNOWN_VERBOSITY = RegisterError(Level::Warn, "unknown verbosity level, expected None, Error, Warn, Info or All, got: ");

// ErrorHandling.cpp
void ErrorHandling::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case ERROR_BTN_CBID:
			sdkRaiseError(ERROR_WRITE, " And this is an additional msg");
			break;
		case WARNING_BTN_CBID:
			// force_log: also writes the message in the Usine log file
			sdkRaiseError(WARNING_WRITE, true);
			break;
		case INFO_BTN_CBID:
			sdkRaiseError(INFO_WRITE);
			break;
		case VERBOSITY_TXT_CBID:
			changeVerbosityLevel();
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
- **The verbosity level is global to the module binary**: changing it in one instance changes it for every instance of that module.
- **Name clash**: `UserError.h` declares `Level`, `Verbosity`, `Error`, `errorList`, `RegisterError`, `setVerbosityLevel` and `current_verbosity_level` in the global namespace. If your module defines one of these names, rename yours or put it in a namespace.

## Key Concepts Demonstrated

- **`RegisterError`**: register a message once and get its index back
- **`sdkRaiseError`**: the plain, `extraMsg` and `force_log` variants
- **`setVerbosityLevel`**: filter messages by level at runtime
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
