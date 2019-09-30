//------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Helpers
// Filename    : public.sdk/source/vst/vst2wrapper/vst2wrapper.h
// Created by  : Steinberg, 01/2009
// Description : VST 3 -> VST 2 Wrapper
//
//-----------------------------------------------------------------------------
// LICENSE
// (c) 2018, Steinberg Media Technologies GmbH, All Rights Reserved
//-----------------------------------------------------------------------------
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
// 
//   * Redistributions of source code must retain the above copyright notice, 
//     this list of conditions and the following disclaimer.
//   * Redistributions in binary form must reproduce the above copyright notice,
//     this list of conditions and the following disclaimer in the documentation 
//     and/or other materials provided with the distribution.
//   * Neither the name of the Steinberg Media Technologies nor the names of its
//     contributors may be used to endorse or promote products derived from this 
//     software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
// ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
// WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
// IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
// INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
// BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#pragma once

#include "docvst2.h"

/// \cond ignore

#include "pluginterfaces/base/ftypes.h"
#include "pluginterfaces/vst/ivstaudioprocessor.h"
#include "pluginterfaces/vst/ivsteditcontroller.h"
#include "pluginterfaces/vst/ivsthostapplication.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"
#include "pluginterfaces/vst/ivstunits.h"

#include "public.sdk/source/common/memorystream.h"
#include "public.sdk/source/vst/hosting/eventlist.h"
#include "public.sdk/source/vst/hosting/parameterchanges.h"
#include "public.sdk/source/vst/hosting/processdata.h"
#include "public.sdk/source/vst2.x/audioeffectx.h"

#include "base/source/fstring.h"
#include "base/source/timer.h"

#include <map>
#include <vector>

//------------------------------------------------------------------------
namespace Steinberg {
namespace Vst {

class Vst2MidiEventQueue;

//-------------------------------------------------------------------------------------------------------
class Vst2Wrapper : public ::AudioEffectX,
                    public IHostApplication,
                    public IComponentHandler,
                    public IUnitHandler,
                    public ITimerCallback,
                    public IVst3ToVst2Wrapper
{
public:
	//--- -------------------------------------------------------------------------------------------------
	// static creation method
	static AudioEffect* create (IPluginFactory* factory, const TUID vst3ComponentID,
	                            VstInt32 vst2ID, audioMasterCallback audioMaster);

	Vst2Wrapper (IAudioProcessor* processor, IEditController* controller,
	             audioMasterCallback audioMaster, const TUID vst3ComponentID, VstInt32 vst2ID,
	             IPluginFactory* factory = 0);
	~Vst2Wrapper ();

	bool init ();

	bool isActive () const { return mActive; }

	// AudioEffectX overrides -----------------------------------------------
	void suspend () SMTG_OVERRIDE; // Called when Plug-in is switched to off
	void resume () SMTG_OVERRIDE; // Called when Plug-in is switched to on
	VstInt32 startProcess () SMTG_OVERRIDE;
	VstInt32 stopProcess () SMTG_OVERRIDE;

	 // Called when the sample rate changes (always in a suspend state)
	void setSampleRate (float newSamplerate) SMTG_OVERRIDE;

	// Called when the maximum block size changes
	// (always in a suspend state). Note that the
	// sampleFrames in Process Calls could be
	// smaller than this block size, but NOT bigger.
	void setBlockSize (VstInt32 newBlockSize) SMTG_OVERRIDE;

	float getParameter (VstInt32 index) SMTG_OVERRIDE;
	void setParameter (VstInt32 index, float value) SMTG_OVERRIDE;

	void setProgram (VstInt32 program) SMTG_OVERRIDE;
	void setProgramName (char* name) SMTG_OVERRIDE;
	void getProgramName (char* name) SMTG_OVERRIDE;
	bool getProgramNameIndexed (VstInt32 category, VstInt32 index, char* text) SMTG_OVERRIDE;

	void getParameterLabel (VstInt32 index, char* label) SMTG_OVERRIDE;
	void getParameterDisplay (VstInt32 index, char* text) SMTG_OVERRIDE;
	void getParameterName (VstInt32 index, char* text) SMTG_OVERRIDE;
	bool canParameterBeAutomated (VstInt32 index) SMTG_OVERRIDE;
	bool string2parameter (VstInt32 index, char* text) SMTG_OVERRIDE;
	bool getParameterProperties (VstInt32 index, VstParameterProperties* p) SMTG_OVERRIDE;

	VstInt32 getChunk (void** data, bool isPreset = false) SMTG_OVERRIDE;
	VstInt32 setChunk (void* data, VstInt32 byteSize, bool isPreset = false) SMTG_OVERRIDE;

	bool getInputProperties (VstInt32 index, VstPinProperties* properties) SMTG_OVERRIDE;
	bool getOutputProperties (VstInt32 index, VstPinProperties* properties) SMTG_OVERRIDE;
	bool setSpeakerArrangement (VstSpeakerArrangement* pluginInput,
	                            VstSpeakerArrangement* pluginOutput) SMTG_OVERRIDE;
	bool getSpeakerArrangement (VstSpeakerArrangement** pluginInput,
	                            VstSpeakerArrangement** pluginOutput) SMTG_OVERRIDE;
	bool setBypass (bool onOff) SMTG_OVERRIDE;

	bool setProcessPrecision (VstInt32 precision) SMTG_OVERRIDE;
	VstInt32 getNumMidiInputChannels () SMTG_OVERRIDE;
	VstInt32 getNumMidiOutputChannels () SMTG_OVERRIDE;
	VstInt32 getGetTailSize () SMTG_OVERRIDE;
	bool getEffectName (char* name) SMTG_OVERRIDE;
	bool getVendorString (char* text) SMTG_OVERRIDE;
	VstInt32 getVendorVersion () SMTG_OVERRIDE;
	VstIntPtr vendorSpecific (VstInt32 lArg, VstIntPtr lArg2, void* ptrArg,
	                                  float floatArg) SMTG_OVERRIDE;
	VstPlugCategory getPlugCategory () SMTG_OVERRIDE;
	VstInt32 canDo (char* text) SMTG_OVERRIDE;

	VstInt32 getMidiProgramName (VstInt32 channel, MidiProgramName* midiProgramName) SMTG_OVERRIDE;
	VstInt32 getCurrentMidiProgram (VstInt32 channel,
	                                MidiProgramName* currentProgram) SMTG_OVERRIDE;
	VstInt32 getMidiProgramCategory (VstInt32 channel, MidiProgramCategory* category) SMTG_OVERRIDE;
	bool hasMidiProgramsChanged (VstInt32 channel) SMTG_OVERRIDE;
	bool getMidiKeyName (VstInt32 channel, MidiKeyName* keyName) SMTG_OVERRIDE;

	// finally process...
	void processReplacing (float** inputs, float** outputs, VstInt32 sampleFrames) SMTG_OVERRIDE;
	void processDoubleReplacing (double** inputs, double** outputs,
	                             VstInt32 sampleFrames) SMTG_OVERRIDE;
	VstInt32 processEvents (VstEvents* events) SMTG_OVERRIDE;

	// VST 3 Interfaces  ------------------------------------------------------
	// FUnknown
	tresult PLUGIN_API queryInterface (const char* iid, void** obj) SMTG_OVERRIDE;
	uint32 PLUGIN_API addRef () SMTG_OVERRIDE { return 1; }
	uint32 PLUGIN_API release () SMTG_OVERRIDE { return 1; }

	// IHostApplication
	tresult PLUGIN_API getName (String128 name) SMTG_OVERRIDE;
	tresult PLUGIN_API createInstance (TUID cid, TUID iid, void** obj) SMTG_OVERRIDE;

	// IComponentHandler
	tresult PLUGIN_API beginEdit (ParamID tag) SMTG_OVERRIDE;
	tresult PLUGIN_API performEdit (ParamID tag, ParamValue valueNormalized) SMTG_OVERRIDE;
	tresult PLUGIN_API endEdit (ParamID tag) SMTG_OVERRIDE;
	tresult PLUGIN_API restartComponent (int32 flags) SMTG_OVERRIDE;

	// IUnitHandler
	tresult PLUGIN_API notifyUnitSelection (UnitID unitId) SMTG_OVERRIDE;
	tresult PLUGIN_API notifyProgramListChange (ProgramListID listId,
	                                                    int32 programIndex) SMTG_OVERRIDE;

	void setVendorName (char* name);
	void setEffectName (char* name);
	void setEffectVersion (char* version);
	void setSubCategories (char* string);

	// ITimer
	void onTimer (Timer* timer) SMTG_OVERRIDE;

//-------------------------------------------------------------------------------------------------------
protected:
	using AudioEffectX::beginEdit;
	using AudioEffectX::endEdit;

	void setupBuses ();
	void setupParameters ();
	void initMidiCtrlerAssignment ();
	void getUnitPath (UnitID unitID, String& path);

	int32 countMainBusChannels (BusDirection dir, uint64& mainBusBitset);

	template <class T>
	void setProcessingBuffers (T** inputs, T** outputs);
	void setupProcessTimeInfo ();
	void doProcess (VstInt32 sampleFrames);
	void setEventPPQPositions ();
	void processOutputEvents ();
	void processMidiEvent (VstMidiEvent* midiEvent, int32 bus);
	
	virtual void processOutputParametersChanges () {}

	/**	Returns the last param change from guiTransfer queue. */
	bool getLastParamChange (ParamID id, ParamValue& value);

	bool setupProcessing (int32 processModeOverwrite = -1);
	void addParameterChange (ParamID id, ParamValue value, int32 sampleOffset);

	bool getProgramListAndUnit (int32 midiChannel, UnitID& unitId, ProgramListID& programListId);
	bool getProgramListInfoByProgramListID (ProgramListID programListId, ProgramListInfo& info);
	int32 lookupProgramCategory (int32 midiChannel, String128 instrumentAttribute);
	bool setupMidiProgram (int32 midiChannel, ProgramListID programListId,
	                       MidiProgramName& midiProgramName);

	bool getPinProperties (BusDirection dir, VstInt32 pinIndex, VstPinProperties* properties);
	bool pinIndexToBusChannel (BusDirection dir, VstInt32 pinIndex, int32& busIndex,
	                           int32& busChannel);
	static VstInt32 vst3ToVst2SpeakerArr (SpeakerArrangement vst3Arr);
	static SpeakerArrangement vst2ToVst3SpeakerArr (VstInt32 vst2Arr);
	static VstInt32 vst3ToVst2Speaker (Speaker vst3Speaker);
	static void setupVst2Arrangement (VstSpeakerArrangement*& vst2arr,
	                                  Vst::SpeakerArrangement vst3Arrangement);

	struct ProgramCategory
	{
		MidiProgramCategory vst2Category;
		String128 vst3InstrumentAttribute;
	};
	std::vector<std::vector<ProgramCategory>> mProgramCategories;
	void setupProgramCategories ();
	static uint32 makeCategoriesRecursive (std::vector<ProgramCategory>& channelCategories,
	                                       String128 vst3Category);

	static const int32 kMaxProgramChangeParameters = 16;
	ParamID mProgramChangeParameterIDs[kMaxProgramChangeParameters]; // for each midi channel
	int32 mProgramChangeParameterIdxs[kMaxProgramChangeParameters]; // for each midi channel

	VstSpeakerArrangement* mVst2InputArrangement = nullptr;
	VstSpeakerArrangement* mVst2OutputArrangement = nullptr;

	FUID mVst3EffectClassID;

	// vst3 data
	IAudioProcessor* mProcessor = nullptr;
	IComponent* mComponent = nullptr;
	IEditController* mController = nullptr;
	IUnitInfo* mUnitInfo = nullptr;
	IMidiMapping* mMidiMapping = nullptr;

	bool componentInitialized = false;
	bool controllerInitialized = false;
	bool componentsConnected = false;
	
	bool mActive = false;
	bool mProcessing = false;
	bool hasEventInputBuses = false;
	bool hasEventOutputBuses = false;

	int32 mVst3SampleSize = kSample32;
	int32 mVst3processMode = kRealtime;

	char mName[PClassInfo::kNameSize];
	char mVendor[PFactoryInfo::kNameSize];
	char mSubCategories[PClassInfo2::kSubCategoriesSize];
	int32 mVersion = 0;

	struct ParamMapEntry
	{
		ParamID vst3ID;
		int32 vst3Index;
	};

	std::vector<ParamMapEntry> mParameterMap;
	std::map<ParamID, int32> mParamIndexMap;
	ParamID mBypassParameterID = kNoParamId;
	ParamID mProgramParameterID = kNoParamId;
	int32 mProgramParameterIdx = -1;

	HostProcessData mProcessData;
	ProcessContext mProcessContext;
	ParameterChanges mInputChanges;
	ParameterChanges mOutputChanges;
	EventList* mInputEvents = nullptr;
	EventList* mOutputEvents = nullptr;
	Vst2MidiEventQueue* mVst2OutputEvents = nullptr;
	uint64 mMainAudioInputBuses = 0;
	uint64 mMainAudioOutputBuses = 0;

	ParameterChangeTransfer mInputTransfer;
	ParameterChangeTransfer mOutputTransfer;
	ParameterChangeTransfer mGuiTransfer;

	MemoryStream mChunk;

	Timer* mTimer = nullptr;
	IPluginFactory* mFactory = nullptr;

	enum
	{
		kMaxMidiMappingBusses = 4
	};
	int32* mMidiCCMapping[kMaxMidiMappingBusses][16];
};

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg

/** Must be implemented externally. */
extern ::AudioEffect* createEffectInstance (audioMasterCallback audioMaster);

/// \endcond
