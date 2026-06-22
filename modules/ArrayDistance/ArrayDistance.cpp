#include "ArrayDistance.h"
#include <vector>
#include "DTW.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

constexpr NativeInt CALCULATE_CBID = 0x003200F0;

//-----------------------------------------------------------------------------
// Create
void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DTWDistance();
}

// destroy
void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete ((DTWDistance*)pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME    = "Array Distance";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC    = "Calculates the DTW distance between two arrays.\nThe closer the two arrays are, the lower the difference will be.";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "0.1";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

void DTWDistance::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = MODULE_NAME;
	pModuleInfo->Description = MODULE_DESC;
	pModuleInfo->ModuleType = mtSimple;
	pModuleInfo->BackColor = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->Version = MODULE_VERSION;
	pModuleInfo->NumberOfParams = 6;
	pModuleInfo->CanBeSavedInPreset = FALSE;
	pModuleInfo->DontProcess = true;
}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Called after the query popup
void DTWDistance::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
}


//-----------------------------------------------------------------------------
// initialisation
void DTWDistance::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
}

//----------------------------------------------------------------------------
// parameters and process
//-----------------------------------------------------------------------------
void DTWDistance::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo)
{
	if (ParamIndex == 0) {
		pParamInfo->ParamType = ptArray;
		pParamInfo->Caption = "array 1";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "data in";
		pParamInfo->setEventClass(arrDataA_);
		pParamInfo->MaxValue = 16384;
		pParamInfo->MinValue = -16384;
		pParamInfo->CallBackType = ctNone;
	}
	else if (ParamIndex == 1) {
		pParamInfo->ParamType = ptArray;
		pParamInfo->Caption = "array 2";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(arrDataB_);
		pParamInfo->MaxValue = 16384;
		pParamInfo->MinValue = -16384;
		pParamInfo->CallBackType = ctNone;
	}
	else if (ParamIndex == 2) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "distance";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(dtDistance_);
		pParamInfo->CallBackType = ctNone;
	}
	else if (ParamIndex == 3) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "min";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(dtMin_);
		pParamInfo->DefaultValue = 0.0f;
		pParamInfo->CallBackType = ctNone;
	}
	else if (ParamIndex == 4) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "max";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(dtMax_);
		pParamInfo->DefaultValue = 1.0f;
		pParamInfo->CallBackType = ctNone;
	}
	else if (ParamIndex == 5) {
		pParamInfo->ParamType = ptButton;
		pParamInfo->Caption = "calculate";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(btnCalculate_);
		pParamInfo->CallBackType = ctNormal;
		pParamInfo->CallBackId = CALCULATE_CBID;
	}
}

// onCallback called with a ctNormal callback type will run on the message thread
void DTWDistance::onCallBack(TUsineMessage* Message) {
	// filter only message specific to this user module and type MS_CHANGE
	if (Message->message == NOTIFY_MSG_USINE_CALLBACK &&
		Message->lParam == MSG_CHANGE &&
		Message->wParam == CALCULATE_CBID)
	{
		std::vector<TPrecision> a;
		std::vector<TPrecision> b;
		a.reserve(arrDataA_.getSize());
		b.reserve(arrDataB_.getSize());

		for (int i = 0; i <arrDataA_.getSize(); ++i)
			a.push_back(arrDataA_.getArrayData(i));

		for (int i = 0; i < arrDataB_.getSize(); ++i)
			b.push_back(arrDataB_.getArrayData(i));

		float normalizationFactor = maxDTW<TPrecision>(a, b, dtMin_.getData(), dtMax_.getData());
		dtDistance_.setData(DTW<TPrecision>(a, b) / normalizationFactor);
	}
}

void DTWDistance::onProcess() {}