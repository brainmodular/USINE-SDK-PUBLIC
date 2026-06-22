#pragma once

#include <UserDefinitions.h>

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class DTWDistance : public UserModuleBase {


	//-----------------------------------------------------------------------------
	// Constructor & destructor
	//-----------------------------------------------------------------------------
public:

	//Constructor
	DTWDistance() = default;

	//Destructor
	virtual ~DTWDistance() = default;

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;

	//-----------------------------------------------------------------------------
	//  init
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) override;

	void onCallBack(TUsineMessage* Message) override;

	void onProcess() override;

	//-----------------------------------------------------------------------------
	// Private members
	//-----------------------------------------------------------------------------

private:
	UsineEventClass arrDataA_;
	UsineEventClass arrDataB_;
	UsineEventClass dtDistance_;
	UsineEventClass btnCalculate_;
	UsineEventClass dtMin_;
	UsineEventClass dtMax_;
};