//-----------------------------------------------------------------------------
//@file  
//	PluginListerExe.h
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
#ifndef __PLUGIN_LISTER_EXE_H__
#define __PLUGIN_LISTER_EXE_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h" 
#include <JuceHeader.h>
#include <JuceUtils.h>
#include <JuceUdpServer.h>

//-----------------------------------------------------------------------------
// watchdog for juce shutdown
//static Array<void*> activePlugins;

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

#define PLUG_INS_PATH String("/Plug-Ins")
#define CORRPUT_PLUG_INS_FILE String("/CorruptedPlugins.init")
#define KNOWN_PLUG_INS_FILE String("/KnownPlugins.init")
#define ERROR_PLUG_INS_FILE String("/ErrorPlugins.init")
#define CRC_FILE String("CrcPlugins.init")


static String const PARAM_VALUE_FORMAT("0%");
static const double PARAM_CHANGE_TIME_MS = 200.0;


#define NUM_OF_USER_FOLDERS 4 
// chunk header code to validate the matrix version how saved chunk of wkp/pat/presets
static char const CHUNCK_PE01[] = { 'P', 'E', '0', '1' };

// plateform specific defines
//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_OSX32))
//-----------------------------------------------------------------------------
#define PLUG_INS_EXT String(".plugin32")
//-----------------------------------------------------------------------------
#elif defined (USINE_WIN64)
//-----------------------------------------------------------------------------
#define PLUG_INS_EXT String(".plugin64")
//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

// structures and typedef goes here

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------

//==============================================================================
/** This is the application object that is started up when Juce starts. It handles
the initialisation and shutdown of the whole application.
*/
class PluginListerExe
	: public JUCEApplication
    , public ServerListener
{
public:
	//==============================================================================
	PluginListerExe();

	//==============================================================================
	//
	void initialise(const String& commandLine) override;


	void shutdown() override
	{
		crcSaveSettings();
	}

	//==============================================================================
	const String getApplicationName() override
	{
		return "PluginListerExe";
	}

	const String getApplicationVersion() override
	{
		// The ProjectInfo::versionString value is automatically updated by the Jucer, and
		// can be found in the JuceHeader.h file that it generates for our project.
		return ProjectInfo::versionString;
	}

	bool moreThanOneInstanceAllowed() override
	{
		return true;
	}

	void anotherInstanceStarted(const String& commandLine) override
	{
	}
    
    
    //-------------------------------------------------------------------------
    // from ServerListener interface
    void parseCommands(MemoryBlock* dataBlock, int dataIndex, bool loopMod);
    
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
    
    int operationMode;
    
    // ports for udp servers
    int portCommandUsine;
    int portCommandLister;
    
    ScopedPointer <JuceUdpServer> socketCommand;
    
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
	void checkOperation(int operationMode);
	// 
	void rescanAll();
	// 
	void rescanForChanges(bool wantPopup = true);

	//
	bool confirmRescanPopup(int option);

	//
	bool rescanPlugins();

	//-------------------------------------------------------------------------
	// manage a plugins folder crc to detect if something change in the plugs setup
	//
	void crcLoadSettings();
	void crcSaveSettings();
	unsigned short int crcComputeFromCurrentSettings();

	//
	char* getDefaultPluginsLocation(int Index);



	//-------------------------------------------------------------------------
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PluginListerExe);

}; // class PluginListerExe

#endif //__PLUGIN_LISTER_EXE_H__