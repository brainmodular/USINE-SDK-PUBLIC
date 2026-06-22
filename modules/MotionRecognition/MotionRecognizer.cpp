//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "MotionRecognizer.h"

#include <fstream>
#include <sstream>
#include <string>


constexpr AnsiCharPtr MOUVEMENTS_COMMATEXT = "movement 1,movement 2,movement 3,movement 4,movement 5,movement 6,movement 7,movement 8";

//----------------------------------------------------------------------------
// callback ids
//----------------------------------------------------------------------------
constexpr NativeInt SAMPLING_PERIOD_CBID  = 0x00A00;
constexpr NativeInt SELECT_MOVEMENT_CBID  = 0x00A01;
constexpr NativeInt RECORD_CBID           = 0x00A02;
constexpr NativeInt UNDO_CBID             = 0x00A03;
constexpr NativeInt CLEAR_CBID            = 0x00A04;

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MotionRecognizer();
}

// destroy
void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<MotionRecognizer*>(pModule);
}

// module constants for browser info and module info
constexpr AnsiCharPtr UserModuleBase::MODULE_NAME ="ML Motion Recognizer";
constexpr AnsiCharPtr UserModuleBase::MODULE_DESC = "Motion Recognizer in Usine";
constexpr AnsiCharPtr UserModuleBase::MODULE_VERSION = "0.1";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
MotionRecognizer::MotionRecognizer()
= default;

// destructor
MotionRecognizer::~MotionRecognizer()
{
	handler_.reset();
}

void MotionRecognizer::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = MODULE_NAME;
	pModuleInfo->Description = MODULE_DESC;
	pModuleInfo->ModuleType = mtSimple;
	pModuleInfo->BackColor = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->Version = MODULE_VERSION;
	
	// FIXME
	//pModuleInfo->NumberOfParams = 12;

	// query for flexible number of params
	if (pMasterInfo != nullptr)
	{
		pModuleInfo->QueryLabel1 = "number of inputs";
		pModuleInfo->QueryDefaultValue1 = 3;
		pModuleInfo->QueryMinValue1 = 1;
		pModuleInfo->QueryMaxValue1 = 128;

	}
	pModuleInfo->CanBeSavedInPreset = FALSE;

}

//-----------------------------------------------------------------------------
// query system and init methods
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Get total parameters number of the module
int MotionRecognizer::onGetNumberOfParams(int queryNumOfIns, int queryNumOfOuts)
{
	this->queryNumOfIns_ = queryNumOfIns;

	// Inputs are represented one extra time as sample outputs (debug)
	return 2 * queryNumOfIns + numOfExtraParam_;
}

//-----------------------------------------------------------------------------
// Called after the query popup
void MotionRecognizer::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2)
{
	handler_ = std::make_unique<process::Handler>(BATCH_SIZE, queryNumOfIns_, this);
    // init normalization buffers for each axis
    normalizationBuffer_.assign(queryNumOfIns_, std::vector<TPrecision>(normalizationWindow_, 0.0f));
    normHead_.assign(queryNumOfIns_, 0);
    newPointBuffer_.resize(queryNumOfIns_);
    recordedInputSamples_.assign(MAX_RECORDING_SAMPLES, std::vector<TPrecision>(queryNumOfIns_, 0.0f));
    recordedSamplesHead_ = 0;
}


//-----------------------------------------------------------------------------
// initialisation
void MotionRecognizer::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {
}

//----------------------------------------------------------------------------
// parameters and process
//---------------------------------O-------------------------------------------
void MotionRecognizer::onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo) {
	if (ParamIndex < queryNumOfIns_) {
		pParamInfo->ParamType = ptDataField;
		std::string strCaption("in ");
		strCaption.append(std::to_string(ParamIndex + 1));
		strcpy(caption, strCaption.c_str());
		pParamInfo->Caption = caption;
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(dtDataIn[ParamIndex]);

		if (ParamIndex == 0)
		{
			pParamInfo->IsSeparator = TRUE;
			pParamInfo->SeparatorCaption = "data in";
		}
	}

	else if (ParamIndex == queryNumOfIns_) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "detection out";
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(dtDataOut);
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "data out";
	}

	else if (ParamIndex == queryNumOfIns_ + 1) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "sampling period";
		pParamInfo->MinValue = 1;
		pParamInfo->MaxValue = 10000;
		pParamInfo->DefaultValue = 30;
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = SAMPLING_PERIOD_CBID;
		pParamInfo->setEventClass(dtSamplePeriod);
	}

	else if (ParamIndex == queryNumOfIns_ + 2) {
		pParamInfo->ParamType = ptDataField;
		pParamInfo->Caption = "threshold";
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 1;
		pParamInfo->DefaultValue = 0.5;
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->setEventClass(dtTreshold);
	}

	else if (ParamIndex == queryNumOfIns_ + 3) {
		pParamInfo->ParamType = ptListBox;
		pParamInfo->Caption = "select movement";

		pParamInfo->ListBoxStrings = MOUVEMENTS_COMMATEXT;
		pParamInfo->DefaultValue = 0;
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = SELECT_MOVEMENT_CBID;
		pParamInfo->setEventClass(lbSelectedSample);
		pParamInfo->IsSeparator = TRUE;
		pParamInfo->SeparatorCaption = "movement handling";
	}

	else if (ParamIndex == queryNumOfIns_ + 4) {
		pParamInfo->ParamType = ptSwitch;
		pParamInfo->Caption = "record";
		pParamInfo->MinValue = 0;
		pParamInfo->MaxValue = 1;
		pParamInfo->DefaultValue = 0.0f;
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = RECORD_CBID;
		pParamInfo->setEventClass(switchRecord);
	}

	else if (ParamIndex == queryNumOfIns_ + 5) {
		pParamInfo->ParamType = ptButton;
		pParamInfo->Caption = "undo";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = UNDO_CBID;
		pParamInfo->setEventClass(btnUndo);
	}

	else if (ParamIndex == queryNumOfIns_ + 6) {
		pParamInfo->ParamType = ptButton;
		pParamInfo->Caption = "clear";
		pParamInfo->IsInput = TRUE;
		pParamInfo->IsOutput = FALSE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->CallBackType = ctImmediate;
		pParamInfo->CallBackId = CLEAR_CBID;
		pParamInfo->setEventClass(btnClear);
	}
	else if ((ParamIndex >= queryNumOfIns_ + 7) && 
		(ParamIndex < 2 * queryNumOfIns_ + 7)) {
		pParamInfo->ParamType = ptArray;
		
		std::string strCaption("sample out ");
		strCaption += std::to_string(ParamIndex - queryNumOfIns_ - 6);
		strcpy(caption, strCaption.c_str());
		pParamInfo->Caption = caption;
		pParamInfo->IsInput = FALSE;
		pParamInfo->IsOutput = TRUE;
		pParamInfo->ReadOnly = FALSE;
		pParamInfo->setEventClass(arrSamplesOut[ParamIndex - queryNumOfIns_ - 7]);
	}
    else if (ParamIndex == 2 * queryNumOfIns_ + 7) {
        pParamInfo->ParamType = ptDataField;
        pParamInfo->IsVisibleByDefault = FALSE;
        pParamInfo->Caption = "latency";
        pParamInfo->IsInput = FALSE;
        pParamInfo->IsOutput = TRUE;
        pParamInfo->ReadOnly = TRUE;
        pParamInfo->setEventClass(dtLatency);
    }
	else {}
}

//--------------------------------------------------------------------------
void MotionRecognizer::onCallBack(TUsineMessage* Message){
	try
	{
		// filter only message specific to this user module and type MS_CHANGE
		if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
		{
			// WParam contains the Param Number
			// Select sample list box
			if (Message->wParam == SELECT_MOVEMENT_CBID) {
				selectedSample_ = static_cast<int>(lbSelectedSample.getData());
				drawSample();
			}

			// Record button
			if (Message->wParam == RECORD_CBID) {
				if (switchRecord.getData() == 0.0f) {
					const std::vector<std::vector<TPrecision>> recorded(
						recordedInputSamples_.begin(),
						recordedInputSamples_.begin() + recordedSamplesHead_);
					handler_->addSample(recorded, selectedSample_);
					recordedSamplesHead_ = 0;
				}
				drawSample();
			}

			// Undo
			else if (Message->wParam == UNDO_CBID) {
				if (!handler_->undo())
					sdkTraceWarningChar("Nothing to undo.");
				drawSample();
			}

			// Clear
			else if (Message->wParam == CLEAR_CBID) {
				handler_->clearSamples();
				drawSample();
			}
		}
	}
	catch (...)
	{
		sdkTraceErrorChar("error");
	}
}


//--------------------------------------------------------------------------
void MotionRecognizer::onProcess() {
    // timing check
    if (timeSinceLastPoint_ < dtSamplePeriod.getData()) {
        timeSinceLastPoint_ += static_cast<float>(1000 * sdkGetBlocSize() / sdkGetSampleRate());
        return;
    }
    timeSinceLastPoint_ = 0;

    for (int i = 0; i < queryNumOfIns_; ++i) {
        TPrecision rawVal = dtDataIn[i].getData();

        // --- update buffer for this axis ---
        auto& buf = normalizationBuffer_[i];
        buf[normHead_[i]] = rawVal;
        normHead_[i] = (normHead_[i] + 1) % normalizationWindow_;

        // --- compute mean/std for this axis ---
        TPrecision mean = 0.0f;
        for (TPrecision v : buf) mean += v;
        mean /= (TPrecision)normalizationWindow_;

        TPrecision variance = 0.0f;
        for (TPrecision v : buf) variance += (v - mean) * (v - mean);
        variance /= (TPrecision)normalizationWindow_;
        TPrecision stddev = std::sqrt(variance + 1e-6f); // add epsilon

        // --- normalized value ---
        TPrecision normVal = (rawVal - mean) / stddev;
        newPointBuffer_[i] = normVal;
    }

    if (switchRecord.getData() != 0.0f) {
        if (recordedSamplesHead_ < MAX_RECORDING_SAMPLES)
            recordedInputSamples_[recordedSamplesHead_++] = newPointBuffer_;
    }
    else {
        dtDataOut.setSize(0); // reset
        const TPrecision threshold = MAX_SENSIVITY * dtTreshold.getData();
        const auto processResult = handler_->process(newPointBuffer_, threshold);

        for (int j = 0; j < processResult.detectedCount_; ++j)
            dtDataOut.pushArrayData(processResult.detected_[j]);
        
        dtLatency.setData(static_cast<TPrecision>(processResult.latency_.count()));

        if (processResult.error_) {
            switch (processResult.error_) {
                case 1: break; // async running
                case 2: break; // training
                default: sdkTraceErrorChar("Unknown error during process."); break;
            }
            return;
        }
    }
}


//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// compute the lentgh of the chunk string 
int MotionRecognizer::onGetStateChunkLen(LongBool Preset) {
	return META_DATA_SIZE * sizeof(TPrecision) + handler_->getByteSize();
}

//-----------------------------------------------------------------------------
// store module datas in the chunk string
void MotionRecognizer::onGetStateChunk(void* chunk, LongBool Preset) {
	TPrecision* castedChunk = static_cast<TPrecision*>(chunk);

	// version number
	*(castedChunk) = 2;

	// number of movements
	*(castedChunk + 1) = N_MVT_MAX;

	// number of samples
	*(castedChunk + 2) = static_cast<float>(handler_->getSampleNumber());

	auto& test = *(castedChunk + 1);
	// number of inputs
	*(castedChunk + 3) = static_cast<float>(queryNumOfIns_);

	// instantiate the write head skipping the first metadata
	size_t writeHead = META_DATA_SIZE;

	for (int mvtNumber = 0; mvtNumber < N_MVT_MAX; ++mvtNumber) {
		// get the current movement
		auto movement = handler_->getMovement(mvtNumber);

		for (const auto& sample : movement) {

			// movement label
			*(castedChunk + writeHead) = static_cast<float>(mvtNumber);

			// movement size
			*(castedChunk + writeHead + 1) = static_cast<float>(sample.size());

			//increment the write head
			writeHead += META_DATA_SIZE;

			// iterate over the data points of the movement,
			// the inner vectors are to be stored contiguously
			for (auto& point : sample) {
				memcpy(castedChunk + writeHead, point.data(), queryNumOfIns_ * sizeof(TPrecision));
				writeHead += queryNumOfIns_;
			}
		}
	}

}

//-----------------------------------------------------------------------------
// restore module datas from the chunk string
void MotionRecognizer::onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) {
	const TPrecision* castedChunk = static_cast<const TPrecision*>(chunk);

	// assert that the chunk version matches the current version
	int version = static_cast<int>(*castedChunk);
	
	if (version == 2) {
		
		// preemptive checks
		int savedNbMovements = static_cast<int>(*(castedChunk + 1));
		const int savedNbSamples = static_cast<int>(*(castedChunk + 2));
		const int savedNumberOfIns = static_cast<int>(*(castedChunk + 3));
		
		if (savedNbMovements > N_MVT_MAX) {
			char warnMsg[255];
			std::stringstream warnStream;
			warnStream
				<< "Saved data has too many movements. Expected: "
				<< N_MVT_MAX << " maximum but received: " << savedNbMovements << std::endl
				<< "Movements truncated.";
			strcpy(warnMsg, warnStream.str().c_str());
			sdkTraceWarningChar(warnMsg);
			
			savedNbMovements = N_MVT_MAX;
		}

		if (savedNumberOfIns != queryNumOfIns_) {
			char errorMsg[255];
			std::stringstream errorStream;
			errorStream
				<< "Saved data does not have the correct number of inputs. Expected: "
				<< queryNumOfIns_ << "but received: " << savedNumberOfIns << std::endl
				<< "Aborting chunk load.";
			strcpy(errorMsg, errorStream.str().c_str());
			sdkTraceErrorChar(errorMsg);
			return;
		}

		// instanciate the read head skipping the first metadata
		size_t readHead = META_DATA_SIZE;
		for (int sampleNumber = 0; sampleNumber < savedNbSamples; ++sampleNumber) {
			
			TPrecision label = *(castedChunk + readHead);

			int sampleSize = (int)*(castedChunk + readHead + 1);

			readHead += META_DATA_SIZE;

			// if the label is too high, we skip the point as indicated in the warning above. 
			if ((label > static_cast<TPrecision>(savedNbMovements)) || !sampleSize) {
				continue;
			}
			
			std::vector<std::vector<TPrecision>> sample;

			const size_t startPos = readHead;
			for (;  readHead < startPos + queryNumOfIns_ * sampleSize; readHead += queryNumOfIns_) {			
				
				if (readHead * sizeof(TPrecision) > sizeInBytes) {
					sdkTraceErrorChar("Read head exceeded allocated size, data may be corrupted.\nAborting chunk load.");
					return;
				}

				std::vector<TPrecision> point;

				point.reserve(queryNumOfIns_);
				for (int offset = 0; offset < queryNumOfIns_; ++offset) {
					point.push_back(*(castedChunk + readHead + offset));
				}

				sample.push_back(point);
			}

			handler_->addSample(sample, (int)label, false);
		}
		

		drawSample();
	}
	else {
		// FIXME: add ascending compatibility here 
		sdkTraceErrorChar("Module version does not match saved data, aborting chunk load.");
		return;
	}
}


//----------------------------------------------------------------------------
// utilities
//----------------------------------------------------------------------------

void MotionRecognizer::drawSample() {
	auto sample = handler_->getMovement(selectedSample_);

	for (int axis = 0; axis < queryNumOfIns_; ++axis) {
		arrSamplesOut[axis].setSize(0);

		if (!sample.empty())
			for (auto point : sample.back())
				arrSamplesOut[axis].pushArrayData(point[axis]);
	}
}
