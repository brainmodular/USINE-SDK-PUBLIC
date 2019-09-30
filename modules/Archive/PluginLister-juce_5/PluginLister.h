//-----------------------------------------------------------------------------
//@file  
//	PluginLister.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the PluginLister class.
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

// include once, no more
#ifndef __PLUGIN_LISTER_H__
#define __PLUGIN_LISTER_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <iostream>
#include <cmath>
#include <limits>
#include <algorithm>
#include <JuceHeader.h>
#include "JuceUtils.h"

//-----------------------------------------------------------------------------
// watchdog for juce shutdown
//static Array<void*> activePlugins;

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define PLUG_INS_PATH String("/Plug-Ins")
#define PLUG_INS_EXT String(".plugin")
#define CORRPUT_PLUG_INS_FILE String("/CorruptedPlugins.init")
#define KNOWN_PLUG_INS_FILE String("/KnownPlugins.init")
#define ERROR_PLUG_INS_FILE String("/ErrorPlugins.init")
#define CRC_FILE String("CrcPlugins.init")

#define PLUG_LISTER_EXEC String("\\Bin\\PluginListerExec.exe")

#define NUM_OF_USER_FOLDERS 6
// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_PL01[] = { 'P', 'L', '0', '1' };

static const NativeInt NOTIFY_MSG_CHECK_SCANNING	        = 0xFAC000;
//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class PluginLister  : public UserModuleBase
{

    //-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	PluginLister(AnsiCharPtr optionalString) 
		: numOfPluginsPath (NUM_OF_USER_FOLDERS)
//		, isRescanAuto (true)
		, crcPlugsFolder (0)
        , isScanning (false)
	{
		pluginFormatManager = new AudioPluginFormatManager();
		pluginFormatManager->addDefaultFormats();
		//if (AudioPluginFormatManager::getInstance()->getNumFormats () == 0)
		//	AudioPluginFormatManager::getInstance()->addDefaultFormats();

//-----------------------------------------------------------------------------
#if defined (USINE_WIN32)
//-----------------------------------------------------------------------------
        crcArchSalt = 32;
//-----------------------------------------------------------------------------
#elif defined (USINE_WIN64)
//----------------------------------------------------------------------------
        crcArchSalt = 64;
//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
//-----------------------------------------------------------------------------
        crcArchSalt = 132;
//-----------------------------------------------------------------------------
#else
    #error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------

		int numFormats = pluginFormatManager->getNumFormats ();
        
        for (int i = 0; i < numFormats; i++) 
        {
            AudioPluginFormat* current = pluginFormatManager->getFormat (i);

			defaultPluginsLocation.addPath(current->getDefaultLocationsToSearch());

			//defaultPluginsLocation.removeRedundantPaths();
		}
	};

    // destructor
	~PluginLister()
	{
		crcSaveSettings();
	};

	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	//int  onGetNumberOfParams( int queryIndex);
	//void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	//void onProcess ();

	//-----------------------------------------------------------------------------
	// midi out callbacks
	//void onMidiSendOut (int DeviceID, UsineMidiCode Code);
	//void onMidiSysexSendOut (int DeviceID, char* Sysex);

	//-----------------------------------------------------------------------------
	// chunk system
	//int  onGetChunkLen (LongBool Preset);
	//void onGetChunk (void* chunk, LongBool Preset);
	//void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);

	//-----------------------------------------------------------------------------
	// layout
	//void onCreateSettings();
	//void onSettingsHasChanged();
    //void onCreateCommands();
	//void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	//void onMouseMove(TShiftState Shift, float X, float Y);
	//void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	//void onOpenEditor();
	//void onBringToFront();

	//-----------------------------------------------------------------------------
	// audio setup update
	//void onBlocSizeChange (int BlocSize);
	//void onSampleRateChange (double SampleRate);
	
	//-----------------------------------------------------------------------------
	// recording 
	// void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);

	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:	
	//-------------------------------------------------------------------------
	// parameters events
	Array <UsineEventPtr> m_folderPluginsPath; 
	
	//UsineEventPtr m_switchRescanAotuAtStartup;
	UsineEventPtr m_btnRescanAll; 
	UsineEventPtr m_btnRescanOnlyChange;

    // 0 = idle
    // 1 = scan for change
    // 2 = scan all
	int isScanning; 

	ScopedPointer <AudioPluginFormatManager> pluginFormatManager;
	// default plugins folders
	FileSearchPath defaultPluginsLocation;
	String  captionDefaultPluginsPath[NUM_OF_USER_FOLDERS];
	const char* caDefaultPluginsPath[NUM_OF_USER_FOLDERS];
	
	// user plugins folders
	int numOfPluginsPath;
	FileSearchPath filelistUserPluginsLocations;
	StringArray listPluginsPath;
	const char* caPluginsPath;

	String pluginsBasePath;
	File filePluginsDirectory;
	File fileTempDirectory;
	File fileCorruptedPlugs;
	File fileKnownPlugs;
	File fileErrorPlugs;

	//bool isRescanAuto;

	StringArray listcorruptedPlugs;
    KnownPluginList knownPluginList;
	ScopedPointer <XmlElement> xmlKnownPluginList;
	
    PluginDescription* desc;

	//bool needRescan;
    StringArray listErrorPlugs;
    
	unsigned short int crcArchSalt;
	// used to test the integrity of the plug-ins folder
	unsigned short int crcPlugsFolder;

    // strings for setup properties tab separators 
    std::string strSepVstFolders;

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	// 
	void rescanAll();
	// 
	void rescanForChanges(bool wantPopup = true);

	//
	bool confirmRescanPopup (int option);

	//
	bool rescanPlugins();

	//-------------------------------------------------------------------------
	// 
	//void initPaths();
	
	//-------------------------------------------------------------------------
	// manage a plugins folder crc to detect if something change in the plugs setup
	//
	void crcLoadSettings();
	void crcSaveSettings();
	unsigned short int crcComputeFromCurrentSettings();

	//
	char* getDefaultPluginsLocation(int Index);
	void saveListErrors();

}; // class PluginLister

#endif //__PLUGIN_LISTER_H__
