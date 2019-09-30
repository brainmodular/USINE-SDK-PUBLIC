//-----------------------------------------------------------------------------
//@file  
//	PluginWrapper.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the PluginWrapper class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//  
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "PluginWrapper.h"
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent) 
{
    pMasterInfo->TraceLogChar ("PluginWrapper : start CreateModule", FALSE);
    pMasterInfo->TraceLogChar ("PluginWrapper : start init juce", FALSE);
	initialiseJuce_GUI ();
    pMasterInfo->TraceLogChar ("PluginWrapper : end init juce", FALSE);
    pMasterInfo->TraceLogChar ("PluginWrapper : start create module", FALSE);
	pModule = new PluginWrapper(optionalString, Flag, pMasterInfo, optionalContent);
    pMasterInfo->TraceLogChar ("PluginWrapper : end create module", FALSE);
    pMasterInfo->TraceLogChar ("PluginWrapper : end CreateModule", FALSE);
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    
    if (   ((PluginWrapper*)pModule)->getPluginManufacturer().compare("Arturia") == 0
        && ((PluginWrapper*)pModule)->getPluginFormat().compare("VST") == 0
		&& (juce::SystemStats::getOperatingSystemType() & juce::SystemStats::MacOSX) != 0)
    {
        ((PluginWrapper*)pModule)->deleteEditor ();
    }
    else
    {
        // cast is important to call the good destructor
        delete ((PluginWrapper*)pModule);
    }
    
	// in case other plug are open
	if (activePlugins.size() == 0 && Desktop::getInstance().getNumComponents() == 0)
		shutdownJuce_GUI ();
}

//-----------------------------------------------------------------------------
// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "plugin wrapper";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "plugin wrapper user module";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "7.0_01";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo)
{
	pModuleInfo->Name = UserModuleBase::MODULE_NAME;
	pModuleInfo->Description = UserModuleBase::MODULE_DESC;
	pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructor/destructor
//-------------------------------------------------------------------------
// constructor
PluginWrapper::PluginWrapper (AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent) 
	: pluginInstance (nullptr)
	, pluginWindow (nullptr)
	, offsetVisibleIn (1)
	, offsetBypassIn (2)
	, offsetAudioIn (3)
	, numOfAudioIn (0)
	, offsetAudioOut (0)
	, numOfAudioOut (0)
	, offsetMidiIn (0)
	, numOfMidiIn (1)
	, offsetMidiOut (0)
	, numOfMidiOut (1)
	, offsetProgIn (0)
	, offsetLatencyOut (0)
	, offsetParamsArrayInOut (0)
	, offsetParamsNamesOut (0)
	, offsetParamsInOut (0)
	, numOfParamsInOut (0)
	, caStringToUTF8Buffer (nullptr)
	, lastEditorPosX (50)
	, lastEditorPosY (50)
	, currentSampleRate (0)
	, currentUsineBlockSize (0)
	, isBypass (false)
	, isInstrument (false)
	, isPlugMissing (false)
	, isProducingMidi(false)
    , timeLastParameterChanged (0.0)
    , lastParamChangeIndex (-2)
{
	pluginFormatManager = new AudioPluginFormatManager ();
	pluginFormatManager->addDefaultFormats ();
        
    // needed for createPlugin to retrieve the knownPluginsFile
	UserModuleBase::InitInfosStructures (pMasterInfo, nullptr);

	createPlugin (String (optionalString), Flag, String (optionalContent));
    
    
    
	activePlugins.add (this);
};

// destructor
PluginWrapper::~PluginWrapper ()
{
	deleteEditor ();
    
    //sdkNotifyUsine (NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_USINE_CALLBACK, MSG_ASK_FOR_SHUTDOWN, MSG_CHANGE);
    //sendActionMessage("ShutDown");
    
    //Component dummyModalComp;
    //dummyModalComp.enterModalState();
    //MessageManager::getInstance()->runDispatchLoopUntil (1000);
    
    
	if (pluginInstance != nullptr)
    {
		pluginInstance->removeListener (this);
        
        //delete pluginInstance;
        //pluginInstance = nullptr;
    }
    
    jassert (activePlugins.contains (this));
    activePlugins.removeFirstMatchingValue (this);
    
};

String PluginWrapper::getPluginManufacturer ()
{
    String result;
    
    if (pluginInstance != nullptr)
        result = pluginInstance->getPluginDescription().manufacturerName;
    
    return result;
}

String PluginWrapper::getPluginFormat ()
{
    String result;
    
    if (pluginInstance != nullptr)
        result = pluginInstance->getPluginDescription().pluginFormatName;
    
    return result;
}

//-----------------------------------------------------------------------------
// module description
void PluginWrapper::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
		const char* caPluginName("empty"); 
		TModuleType pluginType = mtPluginWrapper;
		TColorUsine pluginColor = sdkGetUsineColor(clPluginModuleColor);

		if (isPlugMissing)
		{
			tryToLocatePlug();
		}

		// pluginInstance is allready created in constructor to populate ModuleInfo
		if (pluginInstance != nullptr)
		{
			//pluginName = pluginInstance->getName ();
			caPluginName = pluginName.toUTF8 ();

			if ( pluginInstance->hasEditor())
			{
				LookAndFeel::setDefaultLookAndFeel (usineLook = new UsineLook());

				TColorArgb font = sdkAlphaColorToColorArgb(sdkGetUsineColor(clFontLight));
				TColorArgb buttonClick = sdkAlphaColorToColorArgb(sdkGetUsineColor(clButton));
				TColorArgb button = sdkAlphaColorToColorArgb(sdkGetUsineColor(clButton));

				usineLook->setColour (juce::TextButton::textColourOffId,   Colour((uint8)(font.r*255), (uint8)(font.g*255), (uint8)(font.b*255), font.a ));
				usineLook->setColour (juce::TextButton::textColourOnId, Colour((uint8)(font.r*255), (uint8)(font.g*255), (uint8)(font.b*255), font.a ));
				usineLook->setColour (juce::TextButton::buttonColourId,   Colour((uint8)(buttonClick.r*255), (uint8)(buttonClick.g*255), (uint8)(buttonClick.b*255), buttonClick.a ));
				usineLook->setColour (juce::TextButton::buttonOnColourId, Colour((uint8)(button.r*255), (uint8)(button.g*255), (uint8)(button.b*255), button.a ));
			}
			else
			{
				offsetVisibleIn = -1;
				offsetBypassIn = 0;
				offsetAudioIn = 1;
			}

			numOfAudioIn = pluginInstance->getNumInputChannels ();
			numOfAudioOut = pluginInstance->getNumOutputChannels ();
			offsetAudioOut = offsetAudioIn + numOfAudioIn;

			// prepare in/out events
			m_audioIns.resize (numOfAudioIn);
			m_audioOuts.resize (numOfAudioOut);
			
			offsetMidiIn = offsetAudioOut + numOfAudioOut;

			if (pluginInstance->producesMidi ())
				isProducingMidi = true;

			offsetMidiOut = offsetMidiIn + numOfMidiIn;
			
            populateProgramsNameList ();
 
			// latency
			offsetLatencyOut = offsetProgIn + 1;

			// plugin in/out params
			offsetParamsArrayInOut = offsetLatencyOut + 1;
			offsetParamsNamesOut = offsetParamsArrayInOut + 1;
			numOfParamsInOut = std::min ( PLUG_MAX_PARAMS, pluginInstance->getNumParameters ());
			offsetParamsInOut = offsetParamsNamesOut + 1;
			
			// prepare in/out events
			m_fdrParamsInsOuts.resize (numOfParamsInOut);
		}
		else
		{
			// plugin not found, we make a fake module
			//File tmp(missingPlugDesc);
			//pluginName = tmp.getFileNameWithoutExtension();
			caPluginName = pluginName.toUTF8 ();
			//caPluginName = tmp.getFileNameWithoutExtension().toUTF8 ();

			offsetParamsInOut = 0;
			numOfParamsInOut = 32;

			pluginColor = sdkGetUsineColor(clURed);
		}

		pModuleInfo->Name				= const_cast <char*> (caPluginName);
		pModuleInfo->Description		= MODULE_DESC;
		pModuleInfo->ModuleType         = pluginType;
		pModuleInfo->BackColor          = pluginColor;
		pModuleInfo->NumberOfParams     = offsetParamsInOut + numOfParamsInOut + 1;
		pModuleInfo->QueryString		= "";
		pModuleInfo->QueryListValues	= "";
		pModuleInfo->QueryDefaultIdx	= 1;
		pModuleInfo->Version			= MODULE_VERSION;
		pModuleInfo->CanBeSavedInPreset = TRUE;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------

// query system not used

//-----------------------------------------------------------------------------
// initialisation
void PluginWrapper::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	updateModuleParamsName ();
	preparePlugin();
    initBuffers ();
    
    
    //pluginWindow = getPluginWindow();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void PluginWrapper::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{		
		// fake module with no plugin
		if (pluginInstance == nullptr)
		{
			pParamInfo->ParamType		= ptDataFader;
			pParamInfo->Caption			= "invalid";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= TRUE;

			return;
		}

		// show editor in
		if (ParamIndex < offsetVisibleIn)
		{
			pParamInfo->ParamType		= ptButton;
			pParamInfo->Caption			= "show editor";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->ReadOnly		= FALSE;
		}    
		// visible in out
		else if (ParamIndex >= offsetVisibleIn && ParamIndex < offsetBypassIn)
		{
			pParamInfo->ParamType		= ptSwitch;
			pParamInfo->Caption			= "visible";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= TRUE;
		} 
		// bypass in
		else if (ParamIndex >= offsetBypassIn && ParamIndex < offsetAudioIn)
		{
			pParamInfo->ParamType		= ptSwitch;
			pParamInfo->Caption			= "bypass";
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->IsSeparator		= TRUE;
		}
		// audio ins
		else if ( ParamIndex >= offsetAudioIn && ParamIndex < offsetAudioOut)
		{
			//construct the name
			int numInput = ParamIndex - offsetAudioIn;
			listAudioInNames.add (String("in ") + String(numInput + 1));
			caStringToUTF8Buffer = listAudioInNames[numInput].toUTF8 ();

			pParamInfo->ParamType		= ptAudio;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->DefaultValue	= 0;
		}	
		//  audio outs
		else if ( ParamIndex >= offsetAudioOut && ParamIndex < offsetMidiIn )
		{
			int numOutput = ParamIndex - offsetAudioOut;
			listAudioOutNames.add (String("out ") + String(numOutput + 1));
			caStringToUTF8Buffer = listAudioOutNames[numOutput].toUTF8 ();

			pParamInfo->ParamType		= ptAudio;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->DefaultValue	= 0;
		}
		//  midi in
		else if ( ParamIndex >= offsetMidiIn && ParamIndex < offsetMidiOut )
		{
			midiInName = "midi in";
			caStringToUTF8Buffer = midiInName.toUTF8 ();

			pParamInfo->ParamType		= ptMidi;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->DefaultValue	= 0;
			pParamInfo->CallBackType	= ctImmediate;
		}
		//  midi out
		else if ( ParamIndex >= offsetMidiOut && ParamIndex < offsetProgIn )
		{
			midiOutName = "midi out";
			caStringToUTF8Buffer = midiOutName.toUTF8 ();

			pParamInfo->ParamType		= ptMidi;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= FALSE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->DefaultValue	= 0;
		}
		//  program in
		else if ( ParamIndex >= offsetProgIn && ParamIndex < offsetLatencyOut )
		{
			progInName = "program";
			caStringToUTF8Buffer = progInName.toUTF8 ();

			pParamInfo->ParamType		= ptListBox;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= FALSE;
			pParamInfo->DefaultValue	= 0;
			pParamInfo->ReadOnly		= FALSE;
			
			caStringToUTF8Buffer = qsProgInName.toUTF8 ();
			pParamInfo->ListBoxStrings	= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->DontSave		= TRUE;
			pParamInfo->IsSeparator		= TRUE;
		}
		//  latency out
		else if ( ParamIndex >= offsetLatencyOut && ParamIndex < offsetParamsArrayInOut )
		{
			latencyOutName = "latency";
			caStringToUTF8Buffer = latencyOutName.toUTF8 ();

			pParamInfo->ParamType				= ptDataField;
			pParamInfo->Caption					= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput					= FALSE;
			pParamInfo->IsOutput				= TRUE;
			pParamInfo->ReadOnly				= TRUE;
			pParamInfo->MinValue				= 0;
			pParamInfo->MaxValue				= 92000.0;
			pParamInfo->DefaultValue			= (float)pluginInstance->getLatencySamples ();
			pParamInfo->Symbol					= " samp";
			pParamInfo->Format					= "%g";
//			pParamInfo->IsStoredInConductor		= FALSE;
			pParamInfo->IsStoredInPreset		= FALSE;
		}
		//  param array in out
		else if ( ParamIndex >= offsetParamsArrayInOut && ParamIndex < offsetParamsNamesOut )
		{
			paramArrayInOutName = "params";
			caStringToUTF8Buffer = paramArrayInOutName.toUTF8 ();

			pParamInfo->ParamType		= ptArray;
			pParamInfo->Caption			= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput			= TRUE;
			pParamInfo->IsOutput		= TRUE;
			pParamInfo->MinValue		= 0;
			pParamInfo->MaxValue		= 1;
			pParamInfo->DontSave		= TRUE;
			pParamInfo->IsSeparator		= TRUE;
			pParamInfo->CallBackType	= ctImmediate;
		}
		//  param names out
		else if ( ParamIndex >= offsetParamsNamesOut && ParamIndex < offsetParamsInOut )
		{
			paramNamesOutName = "params names";
			caStringToUTF8Buffer = paramNamesOutName.toUTF8 ();

			pParamInfo->ParamType			= ptTextField;
			pParamInfo->Caption				= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput				= FALSE;
			pParamInfo->IsOutput			= TRUE;
			pParamInfo->ReadOnly			= TRUE;
//			pParamInfo->IsStoredInConductor	= FALSE;
			pParamInfo->IsStoredInPreset	= FALSE;
			pParamInfo->DontSave			= TRUE;
			
			caStringToUTF8Buffer = qsParamNamesOut.toUTF8 ();
			pParamInfo->TextValue		= const_cast <char*> (caStringToUTF8Buffer);
		}
		//  params in out
		else if ( ParamIndex >= offsetParamsInOut && ParamIndex < offsetParamsInOut + numOfParamsInOut )
		{
			//construct the name
			caStringToUTF8Buffer = listParamInOutNames[listParamInOutIndex[ParamIndex - offsetParamsInOut]].toUTF8 ();
			LongBool isInvisible = FALSE;
			try
			{
				if (pluginInstance->hasEditor() && (pluginInstance->isParameterAutomatable (ParamIndex - offsetParamsInOut) || String(caStringToUTF8Buffer).isEmpty()))
				{
					isInvisible = TRUE;
				}
				else
				{
					listVisiblePlugParamsIndex.add(ParamIndex - offsetParamsInOut);
				}
			}
			catch (...)
			{

			}

			pParamInfo->ParamType			= ptDataFader;
			pParamInfo->Caption				= const_cast <char*> (caStringToUTF8Buffer);
			pParamInfo->IsInput				= TRUE;
			pParamInfo->IsOutput			= TRUE;
			pParamInfo->IsInvisible			= isInvisible;
			pParamInfo->MinValue			= 0;
			pParamInfo->MaxValue			= 1.0;
			pParamInfo->DefaultValue		= 0;
			pParamInfo->Symbol				= "";
			pParamInfo->Format				= "%.5f";
			pParamInfo->DontSave			= TRUE;
		}
        // m_lledKeysToUsine
		else if ( ParamIndex >= offsetParamsInOut + numOfParamsInOut )
		{			
            pParamInfo->ParamType		 = ptLeftLed;
			pParamInfo->Caption			 = "send to usine";
			pParamInfo->DefaultValue	 = 1.0;
			pParamInfo->IsInput			 = FALSE;
			pParamInfo->IsOutput		 = FALSE;	
			pParamInfo->IsSeparator		 = TRUE;
			pParamInfo->SeparatorCaption = "shorcuts";
        }
		else {}

		caStringToUTF8Buffer = nullptr;
}

//-----------------------------------------------------------------------------
// set the parameters events address
void PluginWrapper::onSetEventAddress (int numParam, UsineEventPtr pEvent) 
{
	// show editor in
	if (numParam < offsetVisibleIn)
		m_showEditor = pEvent;
	// visible in out
	else if (numParam >= offsetVisibleIn && numParam < offsetBypassIn)
		m_visibleEditor = pEvent;
	// where dummy in
	//else if (numParam >= offsetWhereIn && numParam < offsetBypassIn)
	//	m_whereDummy = pEvent;
	// bypass in
	else if (numParam >= offsetBypassIn && numParam < offsetAudioIn)
		m_bypassPlug = pEvent;
	// audio ins
	else if ( numParam >= offsetAudioIn && numParam < offsetAudioOut)
		m_audioIns.set (numParam - offsetAudioIn, pEvent);
	//  audio outs
	else if ( numParam >= offsetAudioOut && numParam < offsetMidiIn )
		m_audioOuts.set (numParam - offsetAudioOut, pEvent);
	//  midi in
	else if ( numParam >= offsetMidiIn && numParam < offsetMidiOut )
		m_midiIn = pEvent;
	//  midi out
	else if ( numParam >= offsetMidiOut && numParam < offsetProgIn )
		m_midiOut = pEvent;
	//  program in
	else if ( numParam >= offsetProgIn && numParam < offsetLatencyOut )
		m_lboxProgramIn = pEvent;
	//  latency out
	else if ( numParam >= offsetLatencyOut && numParam < offsetParamsArrayInOut )
		m_dtfLatencyOut = pEvent;
	//  param array in out
	else if ( numParam >= offsetParamsArrayInOut && numParam < offsetParamsNamesOut )
		m_arrParamsInOut = pEvent;
	//  param names out
	else if ( numParam >= offsetParamsNamesOut && numParam < offsetParamsInOut )
		m_txtParamsNamesOut = pEvent;
	//  params in out
	else if ( numParam >= offsetParamsInOut && numParam < offsetParamsInOut + numOfParamsInOut )
		m_fdrParamsInsOuts.set (numParam - offsetParamsInOut, pEvent);
    // m_lledKeysToUsine
	else if ( numParam >= offsetParamsInOut + numOfParamsInOut )
		m_lledKeysToUsine = pEvent;

}

//-----------------------------------------------------------------------------
// Parameters callback

void PluginWrapper::onCallBack (UsineMessage *Message)
{
 
    if (pluginInstance.get () == nullptr)
        return;
    
    if (   (Message->message == NOTIFY_MSG_USINE_CALLBACK)
        && (Message->wParam == MSG_ASK_FOR_SHUTDOWN)
        && (Message->lParam == MSG_CHANGE) )
    {
        deleteEditor();
    }
        
	if (Message->lParam == MSG_CHANGE)
    {
		// show editor in
		if (Message->wParam < offsetVisibleIn)
		{
			int  visible =  (int)sdkGetEvtData (m_visibleEditor);
			if (visible != TRUE)
			{
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
	            void* nativeWindowHandle = (void*)sdkGetUsineMainWindow();
                if (pluginWindow != nullptr && (pluginWindow->getNativeHandle() != nativeWindowHandle))
                    pluginWindow = nullptr;
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
                if (pluginWindow != nullptr)
                    pluginWindow->setVisible (true);
                else
                {
				    pluginWindow = getPluginWindow();
                    //addActionListener(pluginWindow);
                    //MessageManager::getInstance()->registerBroadcastListener(pluginWindow);
                }
			}
			else
			{
                deleteEditor ();
				//pluginWindow->closeWindow();
			}
			sdkSetEvtData (m_visibleEditor, 1.0f - visible);
        }
		// visible in out
		else if (Message->wParam >= offsetVisibleIn && Message->wParam < offsetBypassIn)
		{              
			int  visible =  sdkGetEvtData (m_visibleEditor);
				
			if ( visible != FALSE)
			{
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
	            void* nativeWindowHandle = (void*)sdkGetUsineMainWindow();
                if (pluginWindow != nullptr && (pluginWindow->getNativeHandle() != nativeWindowHandle))
                    pluginWindow = nullptr;
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------
                if (pluginWindow != nullptr)
                    pluginWindow->setVisible (true);
                else
                {
                    pluginWindow = getPluginWindow();
                    //addActionListener(pluginWindow);
                    //MessageManager::getInstance()->registerBroadcastListener(pluginWindow);
                }
			}
			else if (pluginWindow != nullptr)
            {
                deleteEditor ();
				//pluginWindow->closeWindow();
			}
		}
        
		// bypass in
		else if (Message->wParam >= offsetBypassIn && Message->wParam < offsetAudioIn)
		{
			isBypass = (bool) sdkGetEvtData (m_bypassPlug);
			if (isBypass)
			{
				for (int i = 0; i < numOfAudioOut; ++i)
					memset( sdkGetEvtDataAddr (m_audioOuts[i]), 0, sizeof(TPrecision)*sdkGetBlocSize() );

				bufferMidiMessages->clear ();

				pluginInstance->reset();
			}
		}
		// midi in
		else if ( Message->wParam >= offsetMidiIn && Message->wParam < offsetMidiOut )
		{
			if (   ! isBypass
				//&& isInstrument
				&& pluginInstance->acceptsMidi ()
				)
			{
				// FROM JUCE : If the filter is receiving a midi input, 
				// then the midiMessages array will be filled with the midi messages for this block. 
				// Each message's timestamp will indicate the message's time
				// , as a number of samples from the start of the block.

				usineEventsToMidiBuffer(m_midiIn);
			}
		}
		//  program in
		else if ( Message->wParam >= offsetProgIn && Message->wParam < offsetLatencyOut )
		{
			int index = (int)sdkGetEvtData (m_lboxProgramIn);
				
			// the check is made in pluginInstance->setCurrentProgram()
			//if (index < pluginInstance->getNumPrograms () && index != pluginInstance->getCurrentProgram ())
				pluginInstance->setCurrentProgram (index);
				updateModuleParams ();

                if (pluginWindow != nullptr)
                    pluginWindow->updateName();
		}
		//  param array in out
		else if ( Message->wParam >= offsetParamsArrayInOut && Message->wParam < offsetParamsNamesOut )
		{
			// sanity check in case array size != numOfParams InsOuts
			int numOfArrayParams = std::min (numOfParamsInOut, (int) sdkGetEvtSize (m_arrParamsInOut));

			// update all params
			for (int i = 0; i < numOfArrayParams; ++i)
			{
				float oldValue = sdkGetEvtData (m_fdrParamsInsOuts[i]);
				float newValue = sdkGetEvtArrayData (m_arrParamsInOut, i);
				if (newValue != oldValue)
                {
                    pluginInstance->setParameter(i, newValue);
                    setModuleParamValue (i, newValue);
                }
			}
		}
		//  params in out
		else if ( Message->wParam >= offsetParamsInOut && Message->wParam < offsetParamsInOut + numOfParamsInOut )
		{
			int num = Message->wParam - offsetParamsInOut;
			float value = sdkGetEvtData (m_fdrParamsInsOuts[num]);

            // TODO : use sdkNotify with this module as target to make the param visible from another callback than this one 
            showModuleParam (num);
			pluginInstance->setParameter (num, value);
            setModuleParamValue (num, value);
         }
     
     }
  
}

//-----------------------------------------------------------------------------
// Main process
void PluginWrapper::onProcess () 
{
	if (pluginInstance == nullptr)
		return;

    try 
	{   // zeroing the midi event size in case we have no more activity
        sdkSetEvtSize (m_midiOut, 0);
		
		if (!isBypass)
		{
			
			if (   (*m_masterInfo->NeedAllNoteOff) != FALSE
				//&& isInstrument
				&& pluginInstance->acceptsMidi ()
				)
			{
				usineEventsToMidiBuffer(m_masterInfo->AllNotOffEvent);
			}

			// populate buffers from audio and midi ins
			for (int i = 0; i < numOfAudioIn; ++i)
				bufferAudioInOut->copyFrom (i, 0, sdkGetEvtDataAddr(m_audioIns[i]), currentUsineBlockSize);
            
            collectorMidiMessages->removeNextBlockOfMessages(*bufferMidiMessages, currentUsineBlockSize);
			// process buffers by the plug
			pluginInstance->processBlock (*bufferAudioInOut, *bufferMidiMessages);


			// update audio  outs
			//*/
			if (numOfAudioOut > 0)
			{
				for (int i = 0; i < numOfAudioOut; ++i)
				{
					sdkSetEvtSize(m_audioOuts[i], currentUsineBlockSize);
					memcpy( sdkGetEvtDataAddr (m_audioOuts[i]), bufferAudioInOut->getWritePointer (i), sizeof(TPrecision)*currentUsineBlockSize );
							
				}
			}
				
			// update midi outs
			if (isProducingMidi)
			{
				int samplePos;
				int i = 0;
				itorMidiOutMessages->setNextSamplePosition(0);
				while (itorMidiOutMessages->getNextEvent (juceMidiOutTmp, samplePos))
				{
					if ( juceMidiOutTmp.getRawDataSize () <= 3)
					{
						int rawDataSize = juceMidiOutTmp.getRawDataSize ();	
						const uint8* rawData = juceMidiOutTmp.getRawData ();

						usineMidiOutTmp.Channel = (unsigned char) juceMidiOutTmp.getChannel ();
						usineMidiOutTmp.Data2 = 0;
						usineMidiOutTmp.Data1 = 0;

						switch (rawDataSize)
						{
						case 3:
							usineMidiOutTmp.Data2 = rawData[2];
						case 2:
							usineMidiOutTmp.Data1 = rawData[1];
						case 1:
							usineMidiOutTmp.Msg = rawData[0];
							break;
						default:
							// should never arrive
							break;
						}
					}
					sdkSetEvtArrayMidi( m_midiOut, i, usineMidiOutTmp);
					i++;
				}
				if (i > 0)
				{
					sdkRepaintParam(offsetMidiOut);
					bufferMidiMessages->clear ();
				}
			}
		}
		else
		{
			// passthru if corresponding input else set size to 0
			for (int i = 0; i < numOfAudioOut; ++i)
            {
                if (m_audioIns[i] != nullptr)
				    sdkCopyEvt (m_audioIns[i], m_audioOuts[i]);
                else
                    sdkSetEvtSize (m_audioOuts[i], 0);
            }
		}
            
    }
    catch (...) 
    {
		sdkTraceErrorChar(" in Process");
    }
}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// compute the lentgh of the chunk
int PluginWrapper::onGetChunkLen (LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;	
	int charInt = sizeof(int);
	//int charFloat = sizeof(float);

	int ChunkLen = 0;

	// 
	if (pluginInstance == nullptr)
	{
		ChunkLen = std::max (4, (int)totalChunk.getSize());
	}
	else
	{
		try
		{
			pluginInstance->getStateInformation (currentPlugStateInfo);

			ChunkLen = ( charHeader								// the version header
				+ charInt										// plug pos X
				+ charInt										// plug pos Y
				+ charInt 										// plug current state size
				+ charInt										// params visibility list size
                + charInt * listVisiblePlugParamsIndex.size()	// params visibility list datas
                + charInt										// plug current state size
				+ currentPlugStateInfo.getSize()				// plug current state datas
				);
		}
		catch (...)
		{
			sdkTraceErrorChar(" in getChunkLen");
		}
	}
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// store module datas in the chunk
void PluginWrapper::onGetChunk (void* chunk, LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;	
	int charInt = sizeof(int);
	//int charFloat = sizeof(float);

	int chunkOffset = 0;
			
	// fake module with no plugin
	if (pluginInstance == nullptr)
	{
		// copy the ui x pos into the chunk
		memcpy( (void*)((char*)chunk)
			, (void*)totalChunk.getData()
			, totalChunk.getSize() 
			);
			
		return;
	}

	try
	{
		// -----------------------------------------------------------------------
		// HEADER section
		// copy the header code into the chunk
		memcpy( chunk
			, (void*)CHUNCK_PG01
			, charHeader 
			);
		chunkOffset += charHeader;

		// reserve space to copy the total size of data stored into the chunk (see end of this methode)
		chunkOffset += charInt;

		// -----------------------------------------------------------------------
		// Values section
		// copy the ui x pos into the chunk
		memcpy( (void*)((char*)chunk + chunkOffset)
			, (void*)&lastEditorPosX
			, charInt 
			);
		chunkOffset += charInt;

		// copy the ui y pos into the chunk
		memcpy( (void*)((char*)chunk + chunkOffset)
			, (void*)&lastEditorPosY
			, charInt 
			);
		chunkOffset += charInt;
						
		// -----------------------------------------------------------------------		
		// copy the params visibility size into the chunk
		int sizeVisibilityList = listVisiblePlugParamsIndex.size();
		memcpy( (void*)((char*)chunk + chunkOffset)
			, (void*)&sizeVisibilityList
			, charInt
			);
		chunkOffset += charInt;
				
		// copy the params visibility datas into the chunk
		memcpy( (void*)((char*)chunk + chunkOffset)
			, listVisiblePlugParamsIndex.getRawDataPointer()
			, charInt * sizeVisibilityList
			);

		chunkOffset += charInt * sizeVisibilityList;
        
        //-----------------------------------------------------------------------------
		// copy the plug current state size  into the chunk
		int sizePlugChunk = currentPlugStateInfo.getSize();
		memcpy( (void*)((char*)chunk + chunkOffset)
               , (void*)&sizePlugChunk
               , charInt
               );
		chunkOffset += charInt;
        
		// copy the plug current state datas into the chunk
		memcpy( (void*)((char*)chunk + chunkOffset)
               , currentPlugStateInfo.getData()
               , sizePlugChunk
               );
		//chunkOffset += sizePlugChunk;
	}
	catch (...)
	{
		sdkTraceErrorChar(" in getChunk");
	}
}

//-----------------------------------------------------------------------------
// restore module datas from the chunk
void PluginWrapper::onSetChunk (const void* chunk, int sizeInBytes, LongBool isPreset)
{
	int charHeader = sizeof(char) * 4;	
	int charInt = sizeof(int);
	//int charFloat = sizeof(float);
		
	char headerChunk[4];

	int chunkOffset = 0;

	// fake module with no plugin
	if (pluginInstance == nullptr)
	{
		int sizeUsineChunk;
		// extract the total size of data stored
		memcpy( (void*)&sizeUsineChunk
			, (void*)((char*)chunk + charHeader)
			, charInt 
			);
			
		totalChunk.replaceWith(chunk, sizeUsineChunk);

		return;
	}

	try
	{
		// -----------------------------------------------------------------------
		// HEADER section
		// extract the header code from the chunk
		memcpy( (void*)headerChunk
			, (void*)((char*)chunk)
			, charHeader 
			);
		chunkOffset += charHeader;

		// reserved space for the total size of data stored
		chunkOffset += charInt;

		//chunk was saved with version 0.1
		if ( std::string(CHUNCK_PG01, 4) == std::string(headerChunk, 4) )
		{
            Point <int> pos;
			// -----------------------------------------------------------------------
			// Values section
			// extract the the ui x pos from the chunk
			memcpy( (void*)&pos.x
				, (void*)((char*)chunk + chunkOffset)
				, charInt 
				);
			chunkOffset += charInt;
			
			// extract the the ui y pos from the chunk
			memcpy( (void*)&pos.y
				, (void*)((char*)chunk + chunkOffset)
				, charInt 
				);
			chunkOffset += charInt;

            // use saved coordinates are only if they are in a visible area
            RectangleList<int> displays =  Desktop::getInstance().getDisplays().getRectangleList (true);
            juce::Rectangle <int> rect;
            for (int i = 0; i <  displays.getNumRectangles (); i++)
            {
                rect =  displays.getRectangle (i);
                if (rect.contains (pos))
                {
                    lastEditorPosX = pos.x;
                    lastEditorPosY = pos.y;
                }
            }

			// tel the new pos to the plug window
			if (pluginWindow != nullptr)
			{
				pluginWindow->setTopLeftPosition(lastEditorPosX, lastEditorPosY);
			}
							
			// -----------------------------------------------------------------------
			// extract the params visibility size from the chunk
			int sizeVisibilityList;
			memcpy( (void*)&sizeVisibilityList
				, (void*)((char*)chunk + chunkOffset)
				, charInt 
				);
			chunkOffset += charInt;
				
			MemoryBlock visibilityDatas(charInt * sizeVisibilityList);

			// extract the params visibility datas from the chunk
			memcpy( visibilityDatas.getData()
				, (void*)((char*)chunk + chunkOffset)
				, charInt * sizeVisibilityList
				);

			chunkOffset += charInt * sizeVisibilityList;
				
			Array <int> arrayVisibilityDatas((int*)visibilityDatas.getData(), std::min (sizeVisibilityList, numOfParamsInOut));

            listVisiblePlugParamsIndex.clear ();
			listVisiblePlugParamsIndex.addArray(arrayVisibilityDatas);
			
            // visibility is update at the end, after reloaded the state of the plug
            
            //----------------------------------------------------------------------------- 
			// extract the plug current state size from the chunk
			int sizePlugChunk;
			memcpy( (void*)&sizePlugChunk
                   , (void*)((char*)chunk + chunkOffset)
                   , charInt
                   );
			chunkOffset += charInt;
            
			currentPlugStateInfo.setSize(sizePlugChunk);
            
			// extract the plug current state datas from the chunk
			memcpy( currentPlugStateInfo.getData()
                   , (void*)((char*)chunk + chunkOffset)
                   , sizePlugChunk
                   );
            
			pluginInstance->setStateInformation(currentPlugStateInfo.getData(), sizePlugChunk);

            updateModuleParamsName ();
            populateParamsNameList ();

			for (int i = 0; i < listVisiblePlugParamsIndex.size(); i++)
			{
				sdkSetParamVisible (offsetParamsInOut + listParamInOutIndex[listVisiblePlugParamsIndex[i]], TRUE);
			}
		}
		else
		{
			// error, bad version, data not recognized
		}

	}
	catch (...)
	{
		sdkTraceErrorChar(" in setChunk.");
	}
}

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------
// mouse callbacks not used

void PluginWrapper::onOpenEditor()
{
    if(pluginWindow == nullptr)
    {
    sdkTraceLogChar("call getPluginWindow");
    pluginWindow = getPluginWindow();
    sdkTraceLogChar("call getPluginWindow ok");
        //addActionListener(pluginWindow);
    }
	else
	{
		deleteEditor();
	}
}

void PluginWrapper::onBringToFront()
{
	try
	{
		if (pluginInstance->hasEditor() && pluginWindow != nullptr)
        {
            sdkTraceLogChar("call pluginWindow->toFront");
            pluginWindow->toFront(true);
            sdkTraceLogChar("call pluginWindow->toFront ok");
            
        }
	}
	catch (...)
	{
		sdkTraceErrorChar(" in BringToFront");
	}
}

void PluginWrapper::onCloseEditor()
{
	deleteEditor ();
}

//-----------------------------------------------------------------------------
// audio setup update
//----------------------------------------------------------------------------
void PluginWrapper::onBlocSizeChange (int BlocSize)
{
	currentUsineBlockSize = BlocSize;
	preparePlugin();
	initBuffers ();
}

void PluginWrapper::onSampleRateChange (double SampleRate)
{
	currentSampleRate = (int)SampleRate;
	preparePlugin();
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//

void PluginWrapper::showModuleParam (int paramIndex)
{ 
	if (   listVisiblePlugParamsIndex.size() < PLUG_MAX_PARAMS_VISIBLE
        && ! listVisiblePlugParamsIndex.contains(paramIndex)
        && pluginInstance->hasEditor ()
        && pluginWindow != nullptr)
	{
		sdkSetParamVisible (offsetParamsInOut + paramIndex, TRUE);
		listVisiblePlugParamsIndex.addIfNotAlreadyThere(paramIndex);
	}
}

void PluginWrapper::hideModuleParam (int paramIndex)
{    
	if (   listVisiblePlugParamsIndex.contains(paramIndex)
        && pluginInstance->hasEditor ()
        && pluginWindow != nullptr)
	{
		sdkSetParamVisible (offsetParamsInOut + paramIndex, FALSE);
		listVisiblePlugParamsIndex.remove (paramIndex);
	}
}

void PluginWrapper::showAllModuleParams ()
{  
    try
    {
        int visibleParams = std::min ( PLUG_MAX_PARAMS_VISIBLE, numOfParamsInOut);
        for (int i = 0; i < visibleParams; i++)
        {
            sdkSetParamVisible (offsetParamsInOut + listParamInOutIndex[i], TRUE);
            listVisiblePlugParamsIndex.addIfNotAlreadyThere(i);
        }
	}
	catch (...)
	{
	}
}

void PluginWrapper::hideAllModuleParams ()
{    
    for (int i = 0; i < numOfParamsInOut; i++)
    {
        sdkSetParamVisible (offsetParamsInOut + i, FALSE);
    }
    listVisiblePlugParamsIndex.clear ();
}

void PluginWrapper::updateModuleParams ()
{
	sdkSetEvtData (m_lboxProgramIn, (float)pluginInstance->getCurrentProgram());
	// program changed so update all params
	for (int i = 0; i < numOfParamsInOut; ++i)
	{
		float value = pluginInstance->getParameter (i);
        setModuleParamValue (i, value);
	}
}

void PluginWrapper::setModuleParamValue (int index, TPrecision newValue)
{
	sdkSetEvtData (m_fdrParamsInsOuts[index], newValue);

    paramTmpValueText = pluginInstance->getParameterText(index);
    if (paramTmpValueText.isNotEmpty())
    {
    const char* caValueText = paramTmpValueText.toUTF8 ();
	sdkSetParamValueText (offsetParamsInOut + index, const_cast <char*> (caValueText));
    sdkRepaintParam (offsetParamsInOut + index);
    }
    else
    {
        sdkSetParamValueText (offsetParamsInOut + index, nullptr);
    }
	sdkSetEvtArrayData (m_arrParamsInOut, index, newValue);
}

void PluginWrapper::deleteEditor ()
{
    sdkNotifyUsine (NOTIFY_TARGET_USINE, NOTIFY_MSG_FOCUS_LOST);
	if (pluginWindow != nullptr)
	{
		
		sdkSetEvtData (m_visibleEditor, FALSE);
		sdkRepaintParam(offsetVisibleIn);
        
        lastEditorPosX = pluginWindow->getPosition().getX();
        lastEditorPosY = pluginWindow->getPosition().getY();
        pluginWindow->closeEditor(pluginWindow);
        pluginWindow = nullptr;
	}
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//
void PluginWrapper::preparePlugin()
{
	if (currentSampleRate == 0)
		currentSampleRate = (int)sdkGetSampleRate();
		
	if (currentUsineBlockSize == 0)
		currentUsineBlockSize = sdkGetBlocSize();
    
    
	bufferAudioInOut = new AudioSampleBuffer ( std::max(1, std::max(numOfAudioIn, numOfAudioOut)), currentUsineBlockSize);

	if (pluginInstance != nullptr)
	{
		// store the state of the plugin
		juce::MemoryBlock plugState;
		pluginInstance->getStateInformation (plugState);

		plugAudioPlayHead = new UsineAudioPlayHead(m_masterInfo, m_moduleInfo);
		pluginInstance->setPlayHead(plugAudioPlayHead);
		pluginInstance->prepareToPlay (currentSampleRate, currentUsineBlockSize);

		pluginInstance->setStateInformation (plugState.getData(), plugState.getSize());
	}
        
}

//
void PluginWrapper::initBuffers ()
{
	try
	{
		if (pluginInstance != nullptr)
		{
			pluginInstance->suspendProcessing (true);

			// audio buffer
			bufferAudioInOut = new AudioSampleBuffer ( std::max(1, std::max(numOfAudioIn, numOfAudioOut)), currentUsineBlockSize);
		
			if (numOfAudioOut > 0)
			{
				for (int i = 0; i < numOfAudioOut; ++i)
					sdkSetEvtSize(m_audioOuts[i], currentUsineBlockSize);
		
				sdkSetEvtSize(m_arrParamsInOut, numOfParamsInOut);
			}

			// midi buffer
			bufferMidiMessages = new MidiBuffer();
			itorMidiOutMessages = new MidiBuffer::Iterator(*bufferMidiMessages);
            collectorMidiMessages = new MidiMessageCollector();
            collectorMidiMessages->reset(currentSampleRate);
			// to force updating all modules params
			//pluginInstance->setCurrentProgram (1);

			pluginInstance->addListener (this);
			pluginInstance->suspendProcessing (false);
		}
		else if (lastError != String::empty)
		{
			//pluginName
			sdkTraceErrorChar ((AnsiCharPtr)lastError.toUTF8().getAddress());
		}
	}
	catch (...)
	{
			sdkTraceErrorChar ("in initBuffers");
	}
}

void PluginWrapper::setVisible(LongBool isVisible)
{
	try
	{
		if (pluginInstance->hasEditor())
		{
            // make visible
            if (isVisible != FALSE)
            {
                if (pluginWindow != nullptr)
                {
                    sdkTraceLogChar("call pluginWindow->setVisible");
                    pluginWindow->setVisible (true);
                    sdkTraceLogChar("call pluginWindow->setVisible ok");
                }
                else
                {
                    sdkTraceLogChar("call getPluginWindow");
				    pluginWindow = getPluginWindow();
                    sdkTraceLogChar("call getPluginWindow ok");
                }
                sdkSetEvtData (m_visibleEditor, 1.0);
                pluginWindow->toFront(true);
                sdkRepaintParam(offsetVisibleIn);
            }
            // make invisible
            else
            {
                sdkNotifyUsine (NOTIFY_TARGET_USINE, NOTIFY_MSG_FOCUS_LOST);
	            if (pluginWindow != nullptr)
	            {
		            pluginWindow->setVisible (false);
		
		            sdkSetEvtData (m_visibleEditor, FALSE);
		            sdkRepaintParam(offsetVisibleIn);
	            }
            }
		}
	}
	catch (...)
	{
	}
}

//populate the params name list
void PluginWrapper::populateProgramsNameList ()
{
    listProgInNames.clear ();
	for (int i = 0; i < pluginInstance->getNumPrograms (); ++i)
	{
		String name(pluginInstance->getProgramName (i));
		listProgInNames.add (name);
	}
	offsetProgIn = offsetMidiOut + numOfMidiOut;
	StringArrayToCommaString (qsProgInName, listProgInNames, false);
			
}
//populate the params name list
void PluginWrapper::populateParamsNameList ()
{
    if (pluginInstance != nullptr)
    {
	    listParamInOutNames.clear();
        listParamInOutIndex.clear();
	    for (int i = 0; i < numOfParamsInOut; ++i)
	    {
		    String name(pluginInstance->getParameterName (i));
				
		    //if (name == String::empty || name == " ")
		    //	name = String("p-") + String(i);
                String nameReaktorNotUsed("P");
                nameReaktorNotUsed << String (i+1).paddedLeft ('0', 4);

                if (   name.isNotEmpty () 
                    && (name != nameReaktorNotUsed)
                    && ! name.containsAnyOf ("{}#")
                    )
                {
		            listParamInOutNames.add (name);
                    listParamInOutIndex.add (i);
                }
	    }
        sdkSetEvtSize(m_arrParamsInOut, listParamInOutNames.size());
    }
}
		
void PluginWrapper::updateModuleParamsName ()
{
    if (pluginInstance != nullptr)
    {
	    populateParamsNameList ();
	    for (int i = 0; i < listParamInOutNames.size(); ++i)
	    {
		    //construct the name
		    caStringToUTF8Buffer = listParamInOutNames[i].toUTF8 ();
		    sdkSetParamCaption(offsetParamsInOut + listParamInOutIndex[i], const_cast <char*> (caStringToUTF8Buffer));
	    }
	    StringArrayToCommaString (qsParamNamesOut, listParamInOutNames, false);
        caStringToUTF8Buffer = qsParamNamesOut.toUTF8 ();
        sdkSetEvtPChar (m_txtParamsNamesOut, const_cast <char*> (caStringToUTF8Buffer));
    }
};


void PluginWrapper::createPlugin (String optionalString, LongBool Flag, String optionalContent )
{
	try
	{
        if (optionalContent.isNotEmpty())
		{
            StringArray pluginInfos;
            pluginInfos.addTokens (optionalContent, ",", "\"");

            if (pluginInfos.size () == 4)
            {
                pluginName = pluginInfos[0];
				String identifierString = pluginInfos[2];
                String plugFormatString = pluginInfos[3];

				//sdkTraceChar((AnsiCharPtr)identifierString.toUTF8().getAddress());
				
                // known plugs stuff
                String pathKnownPlugs(sdkGetUsinePlugInsPath());
                pathKnownPlugs << KNOWN_PLUG_INS_FILE;
                File fileKnownPlugs = pathKnownPlugs;
				
				//sdkTraceChar((AnsiCharPtr)plugFormatString.toUTF8().getAddress());
				
                if (fileKnownPlugs.existsAsFile())
                {
                    KnownPluginList knownPluginList;
                    ScopedPointer<XmlElement> xmlKnownPluginList (XmlDocument::parse(fileKnownPlugs));
                    if (xmlKnownPluginList != nullptr)
                        knownPluginList.recreateFromXml(*xmlKnownPluginList);

                    PluginDescription* plugDesc = getDescriptionFromIdentifierString (knownPluginList, identifierString, plugFormatString);

                    if (plugDesc != nullptr)
					{
                        //pluginInstance = pluginFormatManager->createPluginInstance (*plugDesc, lastError);
						pluginInstance = createPluginInstanceFromDesc (*plugDesc);
					}
                }
            }
        }
		if ((pluginInstance == nullptr) && (optionalString.isNotEmpty()))
		{
			// in the absolute path provided by usine, only the relative part from the "Plug-ins" folder is relevent
			// because usine upgrade or patch/wkp from another os have different absolute path
			// so we reconstruct a valid path
			
			String validPlugPathName (sdkGetUsinePlugInsPath());
			
			// isolate VST or AudioUnit part and extract the name
			String plugTypeName = optionalString.fromFirstOccurrenceOf(PLUG_INS_PATH, false, true);
			plugTypeName = plugTypeName.replaceCharacter ('\\', '/');
			String plugFileName (plugTypeName.fromLastOccurrenceOf("/", false, true));
			plugTypeName = plugTypeName.trimCharactersAtStart("/");

			plugTypeName = plugTypeName.upToFirstOccurrenceOf("/", false, true);

			if (plugTypeName != String::empty)
			{
				
				//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
				//-----------------------------------------------------------------------------
				// only VST on PC
				validPlugPathName << "/";
				//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
				//-----------------------------------------------------------------------------
				validPlugPathName << "/" << plugTypeName;
				//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
				//------------------------------------------------------------------------------
				
				File plug (validPlugPathName);
				//plug.getChildFile(plugTypeName);
				
				int whatToLookFor = juce::File::findFiles + juce::File::ignoreHiddenFiles;
				Array< juce::File > resultsDescTextFiles;
				plug.findChildFiles (resultsDescTextFiles, whatToLookFor, true, plugFileName);
				
				// we found it
				if (resultsDescTextFiles.size() > 0)
				{
					plug = resultsDescTextFiles[0].getFullPathName();
				
				String toto = plug.getFullPathName();
				sdkTraceLogChar ((AnsiCharPtr)toto.toUTF8().getAddress(), FALSE);
				
				if ((plug.existsAsFile()) && (plug.getFileExtension() == PLUG_INS_EXT))	
				{
					StringArray plugInfos;
					plug.readLines(plugInfos);
					pluginDescription.fileOrIdentifier = plugInfos[1];
					
					pluginInstance = createPluginInstanceFromDesc (pluginDescription);
				}
				}
				
			}

			if (pluginInstance == nullptr)
			{
				isPlugMissing = true;
				missingPlugDesc = optionalString;
			}
		}
        if (Flag == TRUE)
        {
            // compatibility mode = show all params
            for (int i = 0; i < numOfParamsInOut; i++)
            {
                sdkSetParamVisible (offsetParamsInOut + i, TRUE);
                listVisiblePlugParamsIndex.addIfNotAlreadyThere(i);
            }
        }
	}
	catch (...)
	{
		sdkTraceErrorChar("in createPlugin");
	}
}

AudioPluginInstance* PluginWrapper::createPluginInstanceFromDesc (PluginDescription& plugDesc)
{
	AudioPluginInstance* plugInstance = pluginFormatManager->createPluginInstance (plugDesc, sdkGetSampleRate(), sdkGetBlocSize(), lastError);
	if (plugInstance != nullptr)
	{
	    pluginName = plugInstance->getName ();
		plugInstance->fillInPluginDescription (plugDesc);
		isInstrument = plugDesc.isInstrument;
	}
	else 
	{
		isInstrument = false;
	}

	
	return plugInstance;
}

PluginWindow* PluginWrapper::getPluginWindow()
{
	void* nativeWindowHandle = nullptr;

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
	nativeWindowHandle = (void*)sdkGetUsineMainWindow();
//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
//-----------------------------------------------------------------------------
	//nativeWindowHandle = (void*)sdkGetUsineNSView();
//-----------------------------------------------------------------------------
#else
  #error "condidionnal compilation error!"
#endif
//------------------------------------------------------------------------------
    sdkTraceLogChar("call PluginWindow::getWindowFor");
	return PluginWindow::getWindowFor (this, pluginInstance, nativeWindowHandle);
    sdkTraceLogChar("call PluginWindow::getWindowFor ok");
    
}

// convert usine midi event in juce message and add them to bufferMidiMessages
void PluginWrapper::usineEventsToMidiBuffer(UsineEventPtr eventSource)
{
	UsineMidiCode usineMidiTmp;
	//MidiMessage juceMidiTmp;
	for (int i = 0; i < sdkGetEvtSize(eventSource); ++i)
	{
		usineMidiTmp = sdkGetEvtArrayMidi( eventSource, i );
		if (usineMidiTmp.Msg != 0)
		{
            //VstTimeInfo* tmp = sdkGetVstTimeInfo ();
			//MidiMessage juceMidiTmp ( usineMidiTmp.Msg, usineMidiTmp.Data1, usineMidiTmp.Data2, sdkGetVstTimeInfo ()->samplePos);
            //if (usineMidiTmp.Data2 == 0)
            //    usineMidiTmp.Data2 = 100;
			MidiMessage juceMidiTmp ( usineMidiTmp.Msg, usineMidiTmp.Data1, usineMidiTmp.Data2, Time::getMillisecondCounterHiRes() * 0.001);
            juceMidiTmp.setChannel(usineMidiTmp.Channel);
            collectorMidiMessages->addMessageToQueue(juceMidiTmp);
		}
	}
}

//-------------------------------------------------------------------------
// the plug isn't at the location saved in the patch/wrokspace
// try to locate it in default location
void PluginWrapper::tryToLocatePlug()
{

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
		
	// plugin isn't in the place indicated by the desc file
	// try to locate it in alternative places

	juce::File filePlugDescText(missingPlugDesc);				
	String missingPlugName = filePlugDescText.getFileNameWithoutExtension() + ".dll";
	String result = missingPlugName + " : " + String(" missing at stored location");

	sdkTraceLogChar ((AnsiCharPtr)result.toUTF8().getAddress());

	Array< juce::File > resultsDescTextFiles;

	int whatToLookFor = juce::File::findFiles + juce::File::ignoreHiddenFiles;
			
	// if pat/wkp come from another computer, we need to search in this computer usine plug folder for the desc text file
	if (! filePlugDescText.isAChildOf( File(sdkGetUsinePlugInsPath())))
	{
		Array< juce::File > resultsDescTextFiles;
		String missingPlugFile = filePlugDescText.getFileNameWithoutExtension() + PLUG_INS_EXT;
		int whatToLookFor = juce::File::findFiles + juce::File::ignoreHiddenFiles;
		pluginsBasePath = sdkGetUsinePlugInsPath();
		filePluginsDirectory = pluginsBasePath;
		filePluginsDirectory.findChildFiles (resultsDescTextFiles, whatToLookFor, true, missingPlugFile);
					
		// we found it
		if (resultsDescTextFiles.size() > 0)
		{
			filePlugDescText = resultsDescTextFiles[0].getFullPathName();
		}
			
		StringArray plugInfos;
		filePlugDescText.readLines(plugInfos);
		pluginDescription.fileOrIdentifier = plugInfos[1];

		//pluginInstance = pluginFormatManager->createPluginInstance (pluginDescription, lastError);
		pluginInstance = createPluginInstanceFromDesc (pluginDescription);
		
		if (pluginInstance != nullptr)
		{
			result = missingPlugName + " : " + String("found in") + " " + pluginsBasePath;
			isPlugMissing = false;
			missingPlugDesc = String::empty;
		}
	}

	// search in userlib folder
	pluginsBasePath = sdkGetUserLibPath ();
	if (isPlugMissing && (pluginsBasePath.isNotEmpty()))
	{
		pluginsBasePath = sdkGetUserLibPath ();
		filePluginsDirectory = pluginsBasePath;

		filePluginsDirectory.findChildFiles (resultsDescTextFiles, whatToLookFor, true, missingPlugName);
		
		// we found it
		if (resultsDescTextFiles.size() > 0)
		{
			String newPath = resultsDescTextFiles[0].getFullPathName();
			pluginDescription.fileOrIdentifier = newPath;
			
			//pluginInstance = pluginFormatManager->createPluginInstance (pluginDescription, lastError);
			pluginInstance = createPluginInstanceFromDesc (pluginDescription);
			
			if (pluginInstance != nullptr)
			{
				result = missingPlugName + " : " + String("found in") + " " + pluginsBasePath;
				isPlugMissing = false;
				missingPlugDesc = String::empty;
			}
		}	
	}
		
	// search in current workspace folder
	pluginsBasePath = sdkGetCurrentWorkspacePath ();
	if (isPlugMissing && (pluginsBasePath.isNotEmpty()))
	{
		pluginsBasePath = sdkGetCurrentWorkspacePath ();
		filePluginsDirectory = pluginsBasePath;

		filePluginsDirectory.findChildFiles (resultsDescTextFiles, whatToLookFor, true, missingPlugName);
		
		// we found it
		if (resultsDescTextFiles.size() > 0)
		{
			String newPath = resultsDescTextFiles[0].getFullPathName();
			pluginDescription.fileOrIdentifier = newPath;
			
			//pluginInstance = pluginFormatManager->createPluginInstance (pluginDescription, lastError);
			pluginInstance = createPluginInstanceFromDesc (pluginDescription);

			if (pluginInstance != nullptr)
			{
				result = missingPlugName + " : " + String("found in ") + " " + pluginsBasePath;
				isPlugMissing = false;
				missingPlugDesc = String::empty;
			}
		}
	}
			
	// search in current patch folder	
	pluginsBasePath = sdkGetCurrentPatchPath ();
	if (isPlugMissing && (pluginsBasePath.isNotEmpty()))
	{
		filePluginsDirectory = pluginsBasePath;

		if (filePluginsDirectory.isDirectory())
		{
			filePluginsDirectory.findChildFiles (resultsDescTextFiles, whatToLookFor, true, missingPlugName);
		
			// we found it
			if (resultsDescTextFiles.size() > 0)
			{
				String newPath = resultsDescTextFiles[0].getFullPathName();
				pluginDescription.fileOrIdentifier = newPath;
				
				//pluginInstance = pluginFormatManager->createPluginInstance (pluginDescription, lastError);
				pluginInstance = createPluginInstanceFromDesc (pluginDescription);

				if (pluginInstance != nullptr)
				{
					result = missingPlugName + " : " + String("found in ") + " " + pluginsBasePath;
					isPlugMissing = false;
					missingPlugDesc = String::empty;
				}
			}
		}
	}


	if (! isPlugMissing)
		sdkTraceLogChar ((AnsiCharPtr)result.toUTF8().getAddress());
	
    
//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
//----------------------------------------------------------------------------              
	
	String result = pluginName + String(", plugin is missing at stored location");
	sdkTraceErrorChar((AnsiCharPtr)result.toUTF8().getAddress());
//-----------------------------------------------------------------------------
#else
  #error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------

}

PluginDescription* PluginWrapper::getDescriptionFromIdentifierString (const KnownPluginList& knownPluginList, const String& identifierString, const String& plugFormatName) const
{  
    
    int fallbackUid = -1;
    for (int i = 0; i < knownPluginList.getNumTypes (); ++i)
    {
        String uid = String::toHexString (knownPluginList.getType(i)->uid);
        if ( uid == identifierString)
        {
            if (knownPluginList.getType(i)->pluginFormatName == plugFormatName)
                return knownPluginList.getType(i);
            else
                fallbackUid = i;
        }
    }

    if (fallbackUid >= 0)
        return knownPluginList.getType(fallbackUid);

    return nullptr;
}

//-------------------------------------------------------------------------
// from AudioProcessorListener interface
void PluginWrapper::audioProcessorParameterChanged (AudioProcessor *processor, int parameterIndex, float newValue)
{
    double newTime = sdkGetTimeMs ();
     
    // make param visible if beginning a 'gesture' on a ui param of the plug
    if ( newTime > timeLastParameterChanged + PARAM_CHANGE_TIME_MS)
    {
        showModuleParam (parameterIndex);
        timeLastParameterChanged = newTime;
    }
    
    // some plugins (like Mcompressor) use change param when we change preset
    // workaround to detect if the plug is applying a preset
    // by checking if the two last params are changed successively
    if (   parameterIndex == pluginInstance->getNumParameters() - 1
        && lastParamChangeIndex == parameterIndex - 1
        ) 
    {   
        //std::ostringstream os;
        //os << "delta time: " << (newTime - timeLastParameterChanged) << ", param index: " << parameterIndex;
        //stringTrace =  os.str();
        //sdkTraceLogChar ((AnsiCharPtr) stringTrace.c_str());

        timeLastParameterChanged = newTime;
        audioProcessorChanged (pluginInstance);
    }

    // update value
    setModuleParamValue (parameterIndex, newValue);

    lastParamChangeIndex = parameterIndex;
}
	
// from AudioProcessorListener interface
void PluginWrapper::audioProcessorChanged (AudioProcessor *processor)
{
    sdkTraceLogChar ("audioProcessorChanged");
    populateProgramsNameList ();
    const char* caProgInName = qsProgInName.toUTF8 ();
    sdkSetListBoxCommatext (offsetProgIn, const_cast <char*> (caProgInName));
    //  TODO : reconstruir la list de programme et les noms des params
	if (processor->getCurrentProgram () != (int)sdkGetEvtData (m_lboxProgramIn))
	{
		sdkSetEvtData (m_lboxProgramIn, (float)processor->getCurrentProgram ());
		if (pluginWindow != nullptr)
		{
			String presetName = processor->getProgramName (processor->getCurrentProgram());
			pluginWindow->setName (processor->getName() + "-" + presetName);
		}
	}
		
    updateModuleParams ();
	updateModuleParamsName ();
}

//
bool PluginWrapper::keyPressed (const KeyPress &key, Component *originatingComponent)
{
    if (1 == (int)sdkGetEvtData (m_lledKeysToUsine))
    {
        kboardLastKeyPress = key.getKeyCode ();
        kboardLastKeyModifiers = key.getModifiers ().getRawFlags ();
		
        sdkNotifyUsine (NOTIFY_TARGET_USINE, NOTIFY_MSG_KEY_DOWN, kboardLastKeyPress, kboardLastKeyModifiers);
    }
    return true;
}

//  
bool PluginWrapper::keyStateChanged (bool isKeyDown, Component *originatingComponent)
{
      if (   1 == (int)sdkGetEvtData (m_lledKeysToUsine)
        && ! isKeyDown
        && kboardLastKeyPress != 0
        ) 
    {
        sdkNotifyUsine (NOTIFY_TARGET_USINE, NOTIFY_MSG_KEY_UP, kboardLastKeyPress, kboardLastKeyModifiers);
        kboardLastKeyPress = 0;
        kboardLastKeyModifiers = 0;
    }
    return true;
}

