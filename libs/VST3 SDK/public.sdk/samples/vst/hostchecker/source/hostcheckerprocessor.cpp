//-----------------------------------------------------------------------------
// Project     : VST SDK
//
// Category    : Examples
// Filename    : public.sdk/samples/vst/hostchecker/source/hostcheckerprocessor.cpp
// Created by  : Steinberg, 04/2012
// Description :
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
// OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
// OF THE POSSIBILITY OF SUCH DAMAGE.
//-----------------------------------------------------------------------------

#include "hostcheckerprocessor.h"
#include "hostcheckercontroller.h"

#include "pluginterfaces/base/ustring.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"
#include "base/source/fstreamer.h"

namespace Steinberg {
namespace Vst {

#define kMaxLatency 8192

//-----------------------------------------------------------------------------
FUID HostCheckerProcessor::cid (0x23FC190E, 0x02DD4499, 0xA8D2230E, 0x50617DA3);

//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
HostCheckerProcessor::HostCheckerProcessor ()
{
	mCurrentState = State::kUninitialized;
	
	mLatency = 256;

	setControllerClass (HostCheckerController::cid);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::initialize (FUnknown* context)
{
	tresult result = AudioEffect::initialize (context);
	if (result == kResultOk)
	{
		if (mCurrentState != kUninitialized)
			addLogEvent (kLogIdInvalidStateInitializedMissing);

		mCurrentState = State::kInitialized;

		addAudioInput (USTRING ("Audio Input"), SpeakerArr::kStereo);
		addAudioOutput (USTRING ("Audio Output"), SpeakerArr::kStereo);
		addEventInput (USTRING ("Event Input"), 1);

		mHostCheck.setComponent (this);
	}
	return result;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::terminate () 
{
	mCurrentState = kUninitialized;

	return AudioEffect::terminate ();
}

//------------------------------------------------------------------------
void HostCheckerProcessor::addLogEvent (Steinberg::int32 logId)
{
	mHostCheck.getEventLogger ().addLogEvent (logId);
}

//-----------------------------------------------------------------------------
void HostCheckerProcessor::sendLogEventMessage (const LogEvent& logEvent)
{
	IMessage* message = allocateMessage ();
	FReleaser msgReleaser (message);
	if (message)
	{
		message->setMessageID ("LogEvent");
		IAttributeList* attributes = message->getAttributes ();
		if (attributes)
		{
			SMTG_ASSERT (logEvent.id >= 0);
			attributes->setInt ("ID", logEvent.id);
			attributes->setInt ("Count", logEvent.count);
			sendMessage (message);
		}
	}
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::setAudioPresentationLatencySamples (
    BusDirection dir, int32 busIndex, uint32 latencyInSamples)
{
	addLogEvent (kLogIdAudioPresentationLatencySamplesSupported);
	return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::getPrefetchableSupport (
    PrefetchableSupport& prefetchable /*out*/)
{
	addLogEvent (kLogIdIPrefetchableSupportSupported);
	prefetchable = kIsYetPrefetchable;
	return kResultTrue;
}

//-----------------------------------------------------------------------------
void HostCheckerProcessor::informLatencyChanged ()
{
	IMessage* newMsg = allocateMessage ();
	if (newMsg)
	{
		newMsg->setMessageID ("Latency");
		Steinberg::Vst::IAttributeList* attr = newMsg->getAttributes ();
		if (attr)
		{
			attr->setFloat ("Value", mWantedLatency);
		}
		sendMessage (newMsg);
	}
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::process (ProcessData& data)
{
	mHostCheck.validate (data);
	
	if (mCurrentState != State::kProcessing)
	{
		addLogEvent (kLogIdInvalidStateProcessingMissing);
	}

	Algo::foreach (data.inputParameterChanges, [&] (IParamValueQueue& paramQueue) {
		Algo::foreachLast (paramQueue, [&] (ParamID id, int32 sampleOffset, ParamValue value) {
			if (id == kBypassTag)
			{
				mBypassProcessor.setActive (value > 0);
			}
			else if (id == kLatencyTag)
			{
				mWantedLatency = value * kMaxLatency;
				informLatencyChanged ();
			}
			else if (id == kParam1Tag)
			{
			}
			else if (id == kGeneratePeaksTag)
			{
				mGeneratePeaks = value;
			}
		});
	});

	if (mBypassProcessor.isActive ())
	{
		mBypassProcessor.process (data);
	}
	else if (data.numSamples && data.numOutputs)
	{
		Algo::clear32 (data.outputs, data.numSamples);

		if (mGeneratePeaks > 0)
		{
			float coef = mGeneratePeaks * mLastBlockMarkerValue;
			for (int32 i = 0; i < data.outputs[0].numChannels; i++)
			{
				data.outputs[0].channelBuffers32[i][0] = coef;
			}
			mLastBlockMarkerValue = -mLastBlockMarkerValue;

			data.outputs[0].silenceFlags = 0x0;

			const float kMaxNotesToDisplay = 5.f;

			// check event
			Algo::foreach (data.inputEvents, [&] (Event& event) {
				switch (event.type)
				{
					case Event::kNoteOnEvent:
						mNumNoteOns++;
						data.outputs[0].channelBuffers32[0][event.sampleOffset] =
						    mNumNoteOns / kMaxNotesToDisplay;
						break;
					case Event::kNoteOffEvent:
						data.outputs[0].channelBuffers32[1][event.sampleOffset] =
						    -mNumNoteOns / kMaxNotesToDisplay;
						mNumNoteOns--;
						break;
					default: break;
				}
			});
		}
		else
			data.outputs[0].silenceFlags = 0x3;
	}

	const EventLogger::Codes& errors = mHostCheck.getEventLogs ();
	EventLogger::Codes::const_iterator iter = errors.begin ();
	while (iter != errors.end ())
	{
		if ((*iter).fromProcessor)
			sendLogEventMessage ((*iter));
		++iter;
	}
	return kResultOk;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::setupProcessing (ProcessSetup& setup)
{
	if (mCurrentState != State::kInitialized)
	{
		addLogEvent (kLogIdInvalidStateInitializedMissing);
	}
	
	mCurrentState = State::kSetupDone;

	mHostCheck.setProcessSetup (setup);
	return AudioEffect::setupProcessing (setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::setActive (TBool state)
{
	if (!state)
	{
		mCurrentState = State::kSetupDone;
		mBypassProcessor.reset ();
	}
	else
	{
		if (mCurrentState != State::kSetupDone)
		{
			addLogEvent (kLogIdInvalidStateSetupMissing);
		}

		mCurrentState = State::kActivated;
		mLatency = mWantedLatency;

		// prepare bypass
		mBypassProcessor.setup (*this, processSetup, mLatency);
	}
	mLastBlockMarkerValue = 0.5f;
	mNumNoteOns = 0;

	return AudioEffect::setActive (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::notify (IMessage* message)
{
	if (!message)
		return kInvalidArgument;

	if (FIDStringsEqual (message->getMessageID (), "Parameter"))
	{
		int64 paramId = -1;
		if (message->getAttributes ()->getInt ("ID", paramId) == kResultOk)
		{
			mHostCheck.addParameter (paramId);
		}
	}

	return kResultOk;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::canProcessSampleSize (int32 symbolicSampleSize)
{
	if (symbolicSampleSize == kSample32)
		addLogEvent (kLogIdCanProcessSampleSize32);
	else if (symbolicSampleSize == kSample64)
		addLogEvent (kLogIdCanProcessSampleSize64);

	return AudioEffect::canProcessSampleSize (symbolicSampleSize);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::setProcessing (TBool state)
{
	if (state)
		mCurrentState = State::kProcessing;
	else
		mCurrentState = State::kActivated;

	return AudioEffect::setProcessing (state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::setState (IBStream* state)
{
	FUnknownPtr<IStreamAttributes> stream (state);
	if (stream)
	{
		IAttributeList* list = stream->getAttributes ();
		if (list)
		{
			addLogEvent (kLogIdIAttributeListInSetStateSupported);
		}
	}

	IBStreamer streamer (state, kLittleEndian);

	float saved = 0.f;
	if (streamer.readFloat (saved) == false)
		return kResultFalse;
	if (saved != 12345.67f)
	{
		SMTG_ASSERT (false)
	}

	uint32 latency = mLatency;
	if (streamer.readInt32u (latency) == false)
		return kResultFalse;

	uint32 bypass;
	if (streamer.readInt32u (bypass) == false)
		return kResultFalse;
	mBypassProcessor.setActive (bypass > 0);

	if (latency != mLatency)
	{
		mLatency = latency;
		informLatencyChanged ();
	}

	return kResultOk;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API HostCheckerProcessor::getState (IBStream* state)
{
	if (!state)
		return kResultFalse;

	IBStreamer streamer (state, kLittleEndian);

	float toSave = 12345.67f;
	streamer.writeFloat (toSave);
	streamer.writeInt32u (mLatency);
	streamer.writeInt32u (mBypassProcessor.isActive () ? 1 : 0);

	return kResultOk;
}

//------------------------------------------------------------------------
} // namespace Vst
} // namespace Steinberg
