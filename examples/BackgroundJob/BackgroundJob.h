// include once, no more
#ifndef __BACKGROUND_JOB_H__
#define __BACKGROUND_JOB_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class BackgroundJob : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

	// constructor
	BackgroundJob() {};

	// destructor
	~BackgroundJob() {};

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack(TUsineMessage* Message);

	//---------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:

	//-------------------------------------------------------------------------
	// parameters events
	UsineEventClass dataOne;
	UsineEventClass dataTwo;
	UsineEventClass dataOut;
	UsineEventClass start;
	UsineEventClass processing;

	static constexpr int numOfParams = 5;

	TPrecision valueOne = .0f;
	TPrecision valueTwo = .0f;
	TPrecision retValue = .0f;

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------
private:
	
	void onJobBegin();
	void onJobProcess();
	void onJobEnd();



}; // class BackgroundJob

#endif // __BACKGROUND_JOB_H__