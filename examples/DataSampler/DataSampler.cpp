//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "DataSampler.h"

//----------------------------------------------------------------------------
// setup a callback_id constant for all params that specify a callback type
//----------------------------------------------------------------------------
constexpr NativeInt TOGGLE_REC_CBID         = 0x001400F0;
constexpr NativeInt CLEAR_REC_CBID          = 0x001400F1;
constexpr NativeInt COLOR_CONTROL_CBID      = 0x001400F2;
constexpr NativeInt AGG_BUFFER_CONTENT_CBID = 0x001400FF;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new DataSampler();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule(void* pModule)
{
    // cast is important to call the right destructor
    delete ((DataSampler*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "data sampler test";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "data sampler sdk module test";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";
constexpr float EMPTY_COORD = -999.9f;


// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void DataSampler::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = MODULE_NAME;
    pModuleInfo->Description = MODULE_DESC;
    pModuleInfo->ModuleType = mtControl;
    pModuleInfo->BackColor = sdkGetUsineColor(clInterfaceDesignModuleColor);
    pModuleInfo->NumberOfParams = numOfParams;
    pModuleInfo->DefaultWidth = DEFAULT_MODULEHEIGHT;
    pModuleInfo->DefaultHeight = DEFAULT_MODULEHEIGHT;
    pModuleInfo->CanBeRandomized = TRUE;
    pModuleInfo->DontProcess = FALSE;
    pModuleInfo->Version = MODULE_VERSION;
    pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void DataSampler::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    colorBox = sdkGetUsineColor(clCursor);
    initBuffers();
    resetBuffers();
}

//-----------------------------------------------------------------------------
// Parameters description
void DataSampler::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
    switch (ParamIndex)
    {
    case 0:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "data";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->DefaultValue = 0.f;
        pParamInfo->MinValue = .0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(dataIn);
        break;
    case 1:
        pParamInfo->ParamType = ptSwitch;
        pParamInfo->Caption = "toggle Rec";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = .0f;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->CallBackId = TOGGLE_REC_CBID;
        pParamInfo->setEventClass(toggleRec);
        break;
    case 2:
        pParamInfo->ParamType = ptButton;
        pParamInfo->Caption = "clear Rec";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = .0f;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->CallBackId = CLEAR_REC_CBID;
        break;
    case 3:
        pParamInfo->ParamType = ptColor;
        pParamInfo->Caption = "color";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = TRUE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->setEventClass(colorControl);
        pParamInfo->CallBackId = COLOR_CONTROL_CBID;
        break;
    case 4:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "out";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->ReadOnly = TRUE;
        pParamInfo->DontSave = TRUE;
        pParamInfo->DefaultValue = .0f;
        pParamInfo->CallBackType = ctNone;
        pParamInfo->setEventClass(envOut);
        break;
    default:
        // do nothing
        break;
    }
}

//-----------------------------------------------------------------------------
// Parameters callback
void DataSampler::onCallBack(TUsineMessage* Message)
{
    if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
        return;
    switch (Message->wParam)
    {
    case TOGGLE_REC_CBID:
        isRec = (LongBool)toggleRec.getData();
        if (isRec) {
            resetBuffers();
            envOut.setData(0);
        }
        else {
            aggBuffersB();
            buffer_length = (int)bufRecord.size();
        }
        break;
    case CLEAR_REC_CBID:
        isRec = FALSE;
        resetBuffers();
        envOut.setData(0);
        break;
    case COLOR_CONTROL_CBID:
        colorBox = colorControl.getColor();
        break;
    case AGG_BUFFER_CONTENT_CBID:
        aggBuffers();
        break;
    default:
        // do nothing
        break;
    }
}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create user defined layout options in the 'edit layout' panel
void DataSampler::onCreateSettings()
{
    // we had our user section in the edit layout panel
    sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
    sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorBox, "box color");
}

//-----------------------------------------------------------------------------
// Callback which indicate that the user has changed someting in the layout
// You'll probably need to redraw
void DataSampler::onSettingsHasChanged()
{
    // Ask to repaint the module
    sdkRepaintPanel();
}


//-----------------------------------------------------------------------------
// paint the module panel
void DataSampler::onPaint()
{
    if (buffer_length == 0) return;
    // Drawing the path
    sdkDrawPathStart();
    TPointF previous;
    TPointF current;
    previous.x = previous.y = current.x = current.y = EMPTY_COORD;

    TPrecision delta = 1.0f / buffer_length;

    for (int i = 0; i < buffer_length; i++) {
        int current_idx = (buffer_idx + i) % buffer_length;
        TPrecision y = bufRecord[current_idx];
        current.x = i * delta;
        current.y = 1 - y;

        if (i == 0) {
            previous = current;
            continue;
        }
        if (current.x > previous.x) {
            sdkDrawPathMoveTo(previous);
            sdkDrawPathLineTo(current);
        }
        previous = current;
    }
    sdkDrawPathClose();
    sdkDrawPathDraw(colorBox, 0.5);
}

//-------------------------------------------------------------------------
// private methods implementation
//-----------------------------------------------------------------------
//-------------------------------------------------------------------------

void DataSampler::onProcess()
{
    if (isRec) {
        if (currentBuf->size() + 10 > buffer_size) {
            swapBuffers();
            triggerBufferCallback();
        }
        currentBuf->push_back(dataIn.getData());
        // Data Passthrough
        envOut.setData(dataIn.getData());
    }
    else if (bufRecord.size() > 0) {
        if ((size_t)buffer_idx == bufRecord.size())
            buffer_idx = 0;
        envOut.setData(bufRecord.at(buffer_idx));
        buffer_idx++;
    }
    if (sdkRefreshSpeedClock()) {
        sdkRepaintPanel();
    }
}

void DataSampler::initBuffers()
{
    bufA.reserve(buffer_size);
    bufB.reserve(buffer_size);
    currentBuf = &bufA;
    previousBuf = &bufB;
}

void DataSampler::resetBuffers()
{
    bufA.clear();
    bufB.clear();
    bufRecord.clear();
    buffer_idx = 0;
    buffer_length = 0;
}

void DataSampler::swapBuffers()
{
    auto* tmp = currentBuf;
    currentBuf = previousBuf;
    previousBuf = tmp;
}

void DataSampler::aggBuffers()
{
    bufRecord.insert(bufRecord.end(), previousBuf->begin(), previousBuf->end());
    previousBuf->clear();
}

void DataSampler::aggBuffersB()
{
    bufRecord.insert(bufRecord.end(), currentBuf->begin(), currentBuf->end());
    currentBuf->clear();
}

void DataSampler::triggerBufferCallback() const
{
    sdkNotifyUsine(NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, AGG_BUFFER_CONTENT_CBID, MSG_CHANGE);
}