# TemplateModuleSimple
#example #template #simple #data #callback

**Minimal starting template for new modules.**

| | |
|---|---|
| **Module Name** | `template simple` |
| **Type** | `mtSimple` |
| **Color** | `clDataModuleColor` |
| **Source** | `examples/TemplateModuleSimple/` |

## Description

The simplest possible working module. Converts a frequency input (Hz) to a period output (ms) using the formula `period = 1000 / freq`. This is the recommended starting point for new modules.

## Parameters

| # | Name | Type | I/O | Range | Scale | Default | Callback |
|---|------|------|-----|-------|-------|---------|----------|
| 0 | `freq` | `ptDataFader` | Input | 0.1 – 20000 Hz | `scLog` | 50 | `ctNormal` |
| 1 | `period` | `ptDataFader` | Output (ReadOnly) | 0.05 – 10000 ms | `scLog` | 0.05 | None |

## How It Works

- **`onCallBack`**: When the frequency input changes (identified by `CallBackId`), calls `computePeriod()` which calculates `period = 1000 / freq` (with a divide-by-zero guard).
- **`onProcess`**: Not used (`DontProcess = TRUE`).

## Key Concepts Demonstrated

- **Callback IDs**: Uses named constants (`FDR_FREQ_CBID`) — the recommended modern pattern
- **Event binding**: Binds `UsineEventClass` members to parameters via `setEventClass()`
- **Read-only outputs**: Output parameters marked `ReadOnly = TRUE`
- **DontProcess**: Skips audio processing when not needed
- **Logarithmic scale**: Uses `scLog` for perceptually linear frequency/time controls

## Code Skeleton

```cpp
class TemplateModuleSimple : public UserModuleBase
{
private:
    UsineEventClass fdrFreq;
    UsineEventClass fdrPeriod;
    void computePeriod();
public:
    void onGetModuleInfo(TMasterInfo*, TModuleInfo*) override;
    void onInitModule(TMasterInfo*, TModuleInfo*) override;
    void onGetParamInfo(int, TParamInfo*) override;
    void onCallBack(TUsineMessage*) override;
};

void TemplateModuleSimple::onCallBack(TUsineMessage* Message)
{
    if (Message->wParam == FDR_FREQ_CBID)
        computePeriod();
}

void TemplateModuleSimple::computePeriod()
{
    float freq = fdrFreq.getData();
    if (freq > 0.0f)
        fdrPeriod.setData(1000.0f / freq);
}
```

## SDK Callbacks Used

`onGetModuleInfo` · `onInitModule` · `onGetParamInfo` · `onCallBack` · `onProcess`

## Related Examples

- [[examples/template-module|TemplateModule]] — Full template with all callback stubs
- [[examples/data-multiply|DataMultiply]] — Another simple data processing example

### SDK Reference

- [[sdk/user-module-base|UserModuleBase Class]]
