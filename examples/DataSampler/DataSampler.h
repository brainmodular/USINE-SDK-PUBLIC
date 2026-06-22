// include once, no more
#ifndef __EXAMPLE_DATA_SAMPLER_H__
#define __EXAMPLE_DATA_SAMPLER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>
#include <vector>
#include <memory>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

// Panel Height
const int DEFAULT_MODULEHEIGHT = 134;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class DataSampler : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

	// constructor
	DataSampler() {};

	// destructor
	~DataSampler() {};

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
	// parameters and process
	void onGetParamInfo(int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack(TUsineMessage* Message);
	void onProcess();

	//-----------------------------------------------------------------------------
	// layout
	void onCreateSettings();
	void onSettingsHasChanged();
	//void onCreateCommands();
	void onPaint();

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:

	//-------------------------------------------------------------------------
	// parameters events
	UsineEventClass dataIn;
	UsineEventClass toggleRec;
	UsineEventClass colorControl;     // color to display the value
	UsineEventClass envOut;

	const int numOfParams = 5;

	//-------------------------------------------------------------------------
	// Other

	// settings
	TUsineColor colorBox;

	LongBool isRec = false;

	int buffer_idx = 0;
	int buffer_length = 0;

	static const int buffer_size = 2000;
	std::vector<TPrecision>	bufA;
	std::vector<TPrecision>	bufB;
	std::vector<TPrecision>	bufRecord;

	std::vector<TPrecision>* currentBuf;
	std::vector<TPrecision>* previousBuf;

	//-------------------------------------------------------------------------
	// private methods
	//-------------------------------------------------------------------------
private:
	void initBuffers();
	void resetBuffers();
	void swapBuffers();
	void aggBuffers();
	void aggBuffersB();

	void triggerBufferCallback() const;

}; // class DataSampler

#endif // __EXAMPLE_DATA_SAMPLER_H__
