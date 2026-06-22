#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------

#include <vector>
#include <UserDefinitions.h>
#include <UsineEventClass.h>
#include "process/Handler.h"

//-----------------------------------------------------------------------------
// definitions
//-----------------------------------------------------------------------------
constexpr int DATA_INS_OUTS_MAX = 128;

// change that to an user defined variable
constexpr int BATCH_SIZE = 32;
constexpr TPrecision MAX_SENSIVITY = 4.0f;
constexpr int MAX_RECORDING_SAMPLES = 2000;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class MotionRecognizer : public UserModuleBase
{
	//-----------------------------------------------------------------------------
	// Constructor & destructor
	//-----------------------------------------------------------------------------
public:

	//Constructor
	MotionRecognizer();

	//Destructor
	~MotionRecognizer() override;

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

	int onGetNumberOfParams(int queryNumOfIns, int queryNumOfOuts) override;

	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;

	//-----------------------------------------------------------------------------
	//  init
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) override;

	void onCallBack(TUsineMessage* Message) override;

	void onProcess() override;

	//----------------------------------------------------------------------------
	// chunk system
	int onGetStateChunkLen(LongBool Preset) override;

	void onGetStateChunk(void* chunk, LongBool Preset) override;

	void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) override;


	//-----------------------------------------------------------------------------
	// Private members
	//-----------------------------------------------------------------------------
private:
	void drawSample();
	char caption[32];

	UsineEventClass dtDataIn[DATA_INS_OUTS_MAX];

	UsineEventClass dtDataOut;
	
	UsineEventClass dtSamplePeriod;
	UsineEventClass dtTreshold;

	UsineEventClass lbSelectedSample;
	UsineEventClass switchRecord;


	UsineEventClass arrSamplesOut[DATA_INS_OUTS_MAX];

	UsineEventClass btnUndo;
	UsineEventClass btnClear;

    UsineEventClass dtLatency;

	std::vector<std::vector<TPrecision>> recordedInputSamples_;
	int recordedSamplesHead_ = 0;
	int selectedSample_ = 0;

    std::vector<std::vector<TPrecision>> normalizationBuffer_;
    std::vector<int> normHead_;
    int normalizationWindow_ = 100; // window size for statistics
	
	std::vector<TPrecision> newPointBuffer_;

	//-------------------------------------------------------------------------
	static const int numOfExtraParam_ = 8;

	float timeSinceLastPoint_ = 0;

	//bool inTraining = FALSE;

	int queryNumOfIns_ = 3;
	std::unique_ptr<process::Handler> handler_;
};
