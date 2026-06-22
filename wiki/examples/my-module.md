# MyModule
#example #template #simple #data #callback

**Simple frequency-to-period converter (legacy callback style).**

| | |
|---|---|
| **Module Name** | `template simple` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/MyModule/` |

## Description

A very simple module that converts a frequency (Hz) to its corresponding period (ms). Functionally identical to [[examples/template-module-simple|TemplateModuleSimple]] but uses an older callback pattern (parameter index instead of callback IDs).

## Parameters

| # | Name | Type | I/O | Range | Scale | Default |
|---|------|------|-----|-------|-------|---------|
| 0 | `freq` | `ptDataFader` | Input | 0.1 – 20000 Hz | `scLog` | 50 |
| 1 | `period` | `ptDataFader` | Output (ReadOnly) | 0.05 – 10000 ms | `scLog` | — |

## How It Works

- **`onCallBack`**: Checks `Message->wParam == 0` (parameter index) to detect freq changes, then calls `computePeriod()`.
- **`computePeriod()`**: Sets `period = 1000.0 / freq`.

## Legacy Pattern Note

This example uses the **old-style callback pattern** where `wParam` is the parameter index:

```cpp
if (Message->wParam == 0)  // parameter index — legacy pattern
```

The modern recommended pattern uses named callback IDs (see [[examples/template-module-simple|TemplateModuleSimple]]).

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/template-module-simple|TemplateModuleSimple]] — Same logic with modern callback IDs
- [[examples/data-multiply|DataMultiply]] — Another simple data module

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
