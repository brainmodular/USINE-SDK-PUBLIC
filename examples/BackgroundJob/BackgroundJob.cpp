//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "BackgroundJob.h"

//----------------------------------------------------------------------------
// setup a callback_id constant for all params that specify a callback type
//----------------------------------------------------------------------------
constexpr NativeInt DATA_ONE_CBID    = 0x001800F0;
constexpr NativeInt DATA_TWO_CBID    = 0x001800F1;
constexpr NativeInt START_JOB_CBID   = 0x001800F2;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new BackgroundJob();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule(void* pModule)
{
    // cast is important to call the right destructor
    delete ((BackgroundJob*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME = "big task async process test";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "big task async process test";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";


// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void BackgroundJob::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = MODULE_NAME;
    pModuleInfo->Description = MODULE_DESC;
    pModuleInfo->ModuleType = mtSimple;
    pModuleInfo->BackColor = sdkGetUsineColor(clInterfaceDesignModuleColor);
    pModuleInfo->NumberOfParams = numOfParams;
    pModuleInfo->CanBeRandomized = TRUE;
    pModuleInfo->DontProcess = TRUE;
    pModuleInfo->Version = MODULE_VERSION;
    pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void BackgroundJob::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
}

//-----------------------------------------------------------------------------
// Parameters description
void BackgroundJob::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
    switch (ParamIndex)
    {
    case 0:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "data1";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->DefaultValue = 0.f;
        pParamInfo->MinValue = .0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->setEventClass(dataOne);
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->CallBackId = DATA_ONE_CBID;
        break;
    case 1:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "data2";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->MinValue = .0f;
        pParamInfo->MaxValue = 1.0f;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->setEventClass(dataTwo);
        pParamInfo->CallBackId = DATA_TWO_CBID;
        break;
    case 2:
        pParamInfo->ParamType = ptButton;
        pParamInfo->Caption = "start job";
        pParamInfo->IsInput = TRUE;
        pParamInfo->IsOutput = FALSE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->IsSeparator = FALSE;
        pParamInfo->CallBackType = ctImmediate;
        pParamInfo->setEventClass(start);
        pParamInfo->CallBackId = START_JOB_CBID;
        break;
    case 3:
        pParamInfo->ParamType = ptRightLed;
        pParamInfo->Caption = "processing";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->ReadOnly = TRUE;
        pParamInfo->DontSave = TRUE;
        pParamInfo->DefaultValue = 0.0f;
        pParamInfo->setEventClass(processing);
        pParamInfo->CallBackType = ctNone;
        break;
    case 4:
        pParamInfo->ParamType = ptDataField;
        pParamInfo->Caption = "out";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->ReadOnly = TRUE;
        pParamInfo->DontSave = TRUE;
        pParamInfo->DefaultValue = .0f;
        pParamInfo->setEventClass(dataOut);
        pParamInfo->CallBackType = ctNone;
        break;
    default:
        // do nothing
        break;
    }
}

//-----------------------------------------------------------------------------
// Parameters callback
void BackgroundJob::onCallBack(TUsineMessage* Message)
{
     // filter only message specific to this user module and type MS_CHANGE
    if (Message->message != NOTIFY_MSG_USINE_CALLBACK || Message->lParam != MSG_CHANGE)
        return;
    // WParam contains the CALLBACK_ID
    switch (Message->wParam)
    {
    case DATA_ONE_CBID:
        valueOne = dataOne.getData();
        break;
    case DATA_TWO_CBID:
        valueTwo = dataTwo.getData();
        break;
    case START_JOB_CBID:
        startJob();
        break;
    default:
        // do nothing
        break;
    }
}

//-------------------------------------------------------------------------
// private methods implementation
//-------------------------------------------------------------------------
//-------------------------------------------------------------------------

void BackgroundJob::onJobBegin()
{
    processing.setData(1.0f);
}

void BackgroundJob::onJobProcess()
{
    TPrecision ret = .0f;
    TPrecision vone = valueOne;
    TPrecision vtwo = valueTwo;
    for (int i = 0; i < 100000000; i++) {
        ret = sin(vone * sin(sin(vtwo) + ret));
    }
    retValue = vone * vtwo * ret;
}

void BackgroundJob::onJobEnd()
{
    processing.setData(0.0f);
    dataOut.setData(retValue);
}