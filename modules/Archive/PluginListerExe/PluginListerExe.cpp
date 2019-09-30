//-----------------------------------------------------------------------------
//@file  
//	PluginListerExe.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the PluginLister class.
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
#include "PluginListerExe.h"
#include <iomanip>
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------
PluginListerExe::PluginListerExe()
	: numOfPluginsPath(NUM_OF_USER_FOLDERS)
	//		, isRescanAuto (true)
	, crcPlugsFolder(0)
	, isScanning(false)
{
	pluginFormatManager = new AudioPluginFormatManager();
	pluginFormatManager->addDefaultFormats();

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

	int numFormats = pluginFormatManager->getNumFormats();

	for (int i = 0; i < numFormats; i++)
	{
		AudioPluginFormat* current = pluginFormatManager->getFormat(i);
		defaultPluginsLocation.addPath(current->getDefaultLocationsToSearch());
	}
};

//
void PluginListerExe::initialise(const String& commandLine)
{
	// parse the commandLine

	StringArray commands;
	commands.addTokens(commandLine, true);

	if (commands.size() != 9)
	{
		// problem with commands

	}
    
    operationMode = commands[0].getIntValue();;
    
    portCommandUsine = commands[1].getIntValue();    
    portCommandLister = commands[2].getIntValue();
    
    socketCommand = new JuceUdpServer(this, &ServerListener::parseCommands, portCommandLister, portCommandUsine);
    
    // folder 1
	File folder1 = commands[3];
    if (folder1.isDirectory() && folder1.exists())
    {
        filelistUserPluginsLocations.remove(0);
        filelistUserPluginsLocations.add(folder1, 0);
    }
    else
    {
        // problem, folder path not OK
    }

    // folder 2
	File folder2 = commands[4].unquoted();
    if (folder2.isDirectory() && folder2.exists())
    {
        filelistUserPluginsLocations.remove(1);
        filelistUserPluginsLocations.add(folder2, 1);
    }
    else
    {
        // problem, folder path not OK
    }
    
    // folder 3
	File folder3 = commands[5].unquoted();
    if (folder3.isDirectory() && folder3.exists())
    {
        filelistUserPluginsLocations.remove(2);
        filelistUserPluginsLocations.add(folder3, 2);
    }
    else
    {
        // problem, folder path not OK
    }
    
    // folder 4
	File folder4 = commands[6].unquoted();
    if (folder4.isDirectory() && folder4.exists())
    {
        filelistUserPluginsLocations.remove(3);
        filelistUserPluginsLocations.add(folder4, 3);
    }
    else
    {
        // problem, folder path not OK
    }

	// usine plugin path
	File folderPlugin = commands[7].unquoted();
	if (folderPlugin.isDirectory() && folderPlugin.exists())
	{
		// directory where plug-ins desc are stored
		pluginsBasePath << folderPlugin.getFullPathName();
		filePluginsDirectory = folderPlugin;

		// file to store corrupted plug-ins
		String pathCorruptedFiles(pluginsBasePath);
		pathCorruptedFiles << CORRPUT_PLUG_INS_FILE;
		fileCorruptedPlugs = pathCorruptedFiles;

		// known plugs stuff
		String pathKnownPlugs(pluginsBasePath);
		pathKnownPlugs << KNOWN_PLUG_INS_FILE;
		fileKnownPlugs = pathKnownPlugs;
		fileKnownPlugs.create();

		// error plugs stuff
		String pathErrorPlugs(pluginsBasePath);
		pathErrorPlugs << ERROR_PLUG_INS_FILE;
		fileErrorPlugs = pathErrorPlugs;
	}
	else
	{
		// problem, folder path not OK
	}

	// usine temp path
	File folderTmp = commands[8].unquoted();
	if (folderTmp.isDirectory() && folderTmp.exists())
	{
		// the usine temp folder where is stored the crc+userpath file
		fileTempDirectory = folderTmp;
	}
	else
	{
		// problem, folder path not OK
	}

	//crcSaveSettings();

	/*
	// directory where plug-ins desc are stored
	pluginsBasePath << sdkGetUsinePlugInsPath();
	filePluginsDirectory = pluginsBasePath;

	// the usine temp folder where is stored the crc+userpath file
	fileTempDirectory = String(sdkGetUsineTempPath());
	*/


	crcLoadSettings();
	

	checkOperation(operationMode);
}

//
void PluginListerExe::parseCommands(MemoryBlock* dataBlock, int dataIndex, bool loopMode)
{
    
    if (dataBlock->getSize() <= dataIndex)
    {
        return;
    }
    
    TCommandPacket* packet = (TCommandPacket*)(((char*)dataBlock->getData()) + dataIndex);
    
    //
    if (packet->cmd == PW_CMD_PING)
    {
        socketCommand->sendCommandSimple(PW_CMD_PONG);
    }
    
    //
    if (loopMode)
    {
        dataIndex += sizeof(TCommandPacket);
        parseCommands(dataBlock, dataIndex, loopMode);
        
    }
}

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//
void PluginListerExe::checkOperation(int operationMode)
{
	if (operationMode == rsmVerify)
	{
		socketCommand->startCommandBloc();
		socketCommand->commandBlock.cmd = US_CMD_TRACE_PROGRESSFORM;
		socketCommand->addTextValue("verify plug-ins done");
		socketCommand->addCommandBloc();
		socketCommand->commandBlock.cmd = PL_CMD_RESCAN_PLUGINS_DONE;
		socketCommand->addCommandBloc();
		socketCommand->flushCommandBloc();
	}
	else if (operationMode == rsmAll)
	{
		rescanAll();
		socketCommand->sendCommandSimple(PL_CMD_RESCAN_PLUGINS_DONE);
	}
	else if (operationMode == rsmChanges)
	{
		rescanForChanges();
		socketCommand->sendCommandSimple(PL_CMD_RESCAN_PLUGINS_DONE);
	}
}

//
void PluginListerExe::rescanAll()
{
	try
	{
		// clear all old informations
		filePluginsDirectory.deleteRecursively();

		MessageManager::getInstance()->runDispatchLoopUntil(100);

		if (xmlKnownPluginList != nullptr)
			xmlKnownPluginList->deleteAllChildElements();

		fileCorruptedPlugs.deleteFile();
		fileCorruptedPlugs.create();

		listcorruptedPlugs.clear();

		knownPluginList.clear();
		fileKnownPlugs.deleteFile();
		fileKnownPlugs.create();

		fileErrorPlugs.deleteFile();
		fileErrorPlugs.create();

		listErrorPlugs.clear();
	}
	catch (...) // std::exception& e
	{
		//sdkTraceErrorChar(" in PluginLister::PluginLister:rescanAll");
		Logger::writeToLog("ERROR: in PluginLister::PluginLister:rescanAll");
	}

	if (!confirmRescanPopup(2))
	{
		isScanning = 0;
		return;
	}

	crcPlugsFolder = crcComputeFromCurrentSettings();
	isScanning = 1;
	crcSaveSettings();

	// trace to log + splash screen
	//sdkTraceLogChar("begin plug-ins scan all...", FALSE);
	Logger::writeToLog("begin plug-ins scan all...");

	// actually do the rescan process
	bool succes = false;
	while (!succes)
	{
		succes = rescanPlugins();
	}
	// trace only to log
	//sdkTraceLogChar("end plug-ins scan all...", FALSE);
	Logger::writeToLog("end plug-ins scan all...");
}

// 
void PluginListerExe::rescanForChanges(bool wantPopup)
{

	try
	{
		if (fileCorruptedPlugs.existsAsFile() && fileKnownPlugs.existsAsFile())
		{
			listcorruptedPlugs.clear();
			fileCorruptedPlugs.readLines(listcorruptedPlugs);

			xmlKnownPluginList = XmlDocument::parse(fileKnownPlugs);
			if (xmlKnownPluginList != nullptr)
				knownPluginList.recreateFromXml(*xmlKnownPluginList);

		}
		if (fileErrorPlugs.existsAsFile())
		{
			listErrorPlugs.clear();
			fileErrorPlugs.readLines(listErrorPlugs);
		}

		//knownPluginList.clear();
		//fileKnownPlugs.deleteFile();
		//fileKnownPlugs.create();
	}
	catch (...) // std::exception& e
	{
		//sdkTraceErrorChar(" in PluginLister::rescanForChanges");
		Logger::writeToLog("ERROR: in PluginLister::rescanForChanges");
	}

	if (wantPopup)
	{
		if (!confirmRescanPopup(1))
		{
			isScanning = 0;
			return;
		}
	}

	crcPlugsFolder = crcComputeFromCurrentSettings();
	isScanning = 1;
	crcSaveSettings();

	// trace to log + splash screen
	//sdkTraceLogChar("begin plug-ins scan for change...", FALSE);
	Logger::writeToLog("begin plug-ins scan for change...");

	// actually do the rescan process
	bool succes = false;
	while (!succes)
	{
		succes = rescanPlugins();
	}
	// trace only to log
	//sdkTraceLogChar("end plug-ins scan for change...", FALSE);
	Logger::writeToLog("end plug-ins scan for change...");


	/*
	String execCmd;
	execCmd << sdkGetGlobalApplicationPath () << PLUG_LISTER_EXEC;

	ChildProcess process;
	bool started = process.start(execCmd);
	*/

}

//
bool PluginListerExe::confirmRescanPopup(int option)
{
	bool result = false;
	String popupMessage;
/*
	if (isScanning > 0)
	{
		popupMessage += sdkGetTrans("lister_last_scan_not_complete");
		popupMessage += newLine;
		popupMessage += sdkGetTrans("lister_continue_scan_now");
		popupMessage += newLine;
	}
	else if (option == 1)
	{
		popupMessage += sdkGetTrans("lister_rescan_plugins_change_now");
		popupMessage += newLine;
	}
	else if (option == 2)
	{
		popupMessage += sdkGetTrans("lister_rescan_all_plugins_now");
		popupMessage += newLine;
	}

	popupMessage += String(CharPointer_UTF8(sdkGetTrans("lister_restart_if_crash")));
	popupMessage += newLine + newLine;


	const char* mess = popupMessage.toUTF8();
	int dialogResult = sdkDialogConfirmationOKCancel(const_cast <char*> (mess));

	if (dialogResult == idOk) // 1 = ok, 2 = cancel
		result = true;
*/
	return true;
}

//
bool PluginListerExe::rescanPlugins()
{
	//int totalPlugScanned = 0;

	try
	{
		int numFormats = pluginFormatManager->getNumFormats();
		//int lastNumPlugs = 0;

		FileSearchPath searchLocations(filelistUserPluginsLocations);

		// notification to fire a callback after the scan to check if the scann is complete or cr   shed in middle
		//sdkNotifyUsine (NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_CHECK_SCANNING);

		// iterate each plugins formats (VST, AU)
		for (int i = 0; i < numFormats; i++)
		{
			AudioPluginFormat* currentFormat = pluginFormatManager->getFormat(i);

			PluginDirectoryScanner scanner(knownPluginList, *currentFormat, searchLocations, true, fileCorruptedPlugs);

			String nextPlug;
			String currentPlug;

			bool scan = true;
			int numScanned = 0;
			// Directory scan
			while (scan)
			{
				nextPlug = scanner.getNextPluginFileThatWillBeScanned();

				if (nextPlug == String::empty)
				{
					scan = false;
					break;
				}

				listcorruptedPlugs.clear();
				fileCorruptedPlugs.readLines(listcorruptedPlugs);
				++numScanned;

				if (knownPluginList.getTypeForFile(nextPlug) != nullptr
					|| listcorruptedPlugs.contains(nextPlug, true)
					|| listErrorPlugs.contains(nextPlug, true)
					)
				{
					String message;
					message << nextPlug << " " << "skipped";

					// trace to log
					//sdkTraceLogChar((AnsiCharPtr)message.toUTF8().getAddress(), FALSE);
					Logger::writeToLog(message);

					scan = scanner.skipNextFile();
				}
				else
				{
					scan = scanner.scanNextFile(true, currentPlug);


					const StringArray failedFiles = scanner.getFailedFiles();

					listErrorPlugs.addArray(failedFiles);
					listErrorPlugs.removeDuplicates(true);

					// update plugs with error
					StringArray listLastErrorPlugs;
					fileErrorPlugs.readLines(listLastErrorPlugs);

					if (listLastErrorPlugs != listErrorPlugs)
					{
						fileErrorPlugs.deleteFile();
						fileErrorPlugs.create();
						for (int j = 0; j < listErrorPlugs.size(); j++)
						{
							fileErrorPlugs.appendText(listErrorPlugs[j]);
							fileErrorPlugs.appendText(newLine);
						}
					}


					knownPluginList.clearBlacklistedFiles();
					xmlKnownPluginList = knownPluginList.createXml();
					xmlKnownPluginList->writeToFile(fileKnownPlugs, String::empty);

					desc = knownPluginList.getTypeForFile(nextPlug);

                    //String scanningMessage = String(sdkGetTrans("lister_scanning"));
                    String scanningMessage = "lister_scanning";

					if (desc != nullptr)
					{
						// trace to log + splash screen
						//String message("scanning " + currentFormat->getName());
						scanningMessage << " " << currentFormat->getName();
						String plugName = desc->name;
						//message << " ("  << (int)(scanner.getProgress()*100)  << "%) : " << plugName;
						scanningMessage << " (" << (int)(scanner.getProgress() * 100) << "%) : " << plugName;
						
						//sdkTraceLogChar((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), TRUE);
						Logger::writeToLog(scanningMessage);
						
						/*
						String pathPlugDescText = pluginsBasePath;

						String plugFileName = desc->name.replaceCharacters ("\"#@,;:<>*^|?\\/", "--------------");
						//-----------------------------------------------------------------------------
						#if (defined (USINE_WIN32) || defined (USINE_WIN64))
						//-----------------------------------------------------------------------------

						File filePlug(desc->fileOrIdentifier);
						for (int i = 0; i < searchLocations.getNumPaths(); i++)
						{
						if (filePlug.isAChildOf(searchLocations[i]))
						{
						String subPath = filePlug.getRelativePathFrom(searchLocations[i]);

						if (subPath == filePlug.getFileName())
						subPath = String::empty;
						else
						subPath = subPath.upToLastOccurrenceOf("\\", true, true);

						pathPlugDescText << "/" << desc->pluginFormatName << "/" << subPath << plugFileName << PLUG_INS_EXT;
						}
						}

						//-----------------------------------------------------------------------------
						#elif defined (USINE_OSX32)
						//-----------------------------------------------------------------------------
						if (desc != nullptr)
						{
						String plugFormat("-");

						if (desc->pluginFormatName == "AudioUnit")
						plugFormat << "au";
						else if (desc->pluginFormatName == "VST")
						plugFormat << "vst";


						if (desc->category != String::empty)
						pathPlugDescText << "/" << desc->pluginFormatName << "/" + desc->category << "/" + plugFileName << plugFormat << PLUG_INS_EXT;
						else
						pathPlugDescText << "/" << desc->pluginFormatName << "/" + plugFileName << plugFormat << PLUG_INS_EXT;
						}

						//-----------------------------------------------------------------------------
						#else
						#error "condidionnal compilation error!"
						#endif
						//-----------------------------------------------------------------------------

						File filePlugDescText(pathPlugDescText);

						filePlugDescText.deleteFile();
						filePlugDescText.create();

						// content of the .plugin file used by usine and the browser
						String content;
						content << desc->name << newLine << desc->fileOrIdentifier << newLine << String::toHexString (desc->uid) << newLine << desc->pluginFormatName;
						filePlugDescText.appendText(content, true, true);

						message = String::empty;
						message << plugName << " OK";

						// trace to log
						sdkTraceLogChar ((AnsiCharPtr)message.toUTF8().getAddress(), FALSE);
						*/
					}
					else
					{
						// trace to log + splash screen
						//String message("scanning " + currentFormat->getName());
						scanningMessage << " " << currentFormat->getName();
						File plug(nextPlug);

						//message << " ("  << (int)(scanner.getProgress()*100)  << "%) : " << plug.getFileNameWithoutExtension();
						scanningMessage << " (" << (int)(scanner.getProgress() * 100) << "%) : " << plug.getFileNameWithoutExtension();

						//sdkTraceLogChar((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), TRUE);
						Logger::writeToLog(scanningMessage);
						
						// trace to log
						scanningMessage = String::empty;
						scanningMessage << plug.getFileNameWithoutExtension() << " KO (failed)";
						//sdkTraceLogChar((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), FALSE);
						Logger::writeToLog(scanningMessage);
					}

				}
			}
		}
		// update plugs with error
		StringArray listLastErrorPlugs;
		fileErrorPlugs.readLines(listLastErrorPlugs);

		if (listLastErrorPlugs != listErrorPlugs)
		{
			fileErrorPlugs.deleteFile();
			fileErrorPlugs.create();
			for (int j = 0; j < listErrorPlugs.size(); j++)
			{
				fileErrorPlugs.appendText(listErrorPlugs[j]);
				fileErrorPlugs.appendText(newLine);
			}
		}

		DirectoryIterator iter(filePluginsDirectory, true, "*" + PLUG_INS_EXT);

		// purge of the plugins folder
		while (iter.next())
		{
			File filePlugText(iter.getFile());
			StringArray listPlugTextInfo;
			filePlugText.readLines(listPlugTextInfo);

			//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
			//-----------------------------------------------------------------------------
			File plug(listPlugTextInfo[1]);

			if ((!plug.existsAsFile()) && (searchLocations.isFileInPath(plug, true)))
				filePlugText.deleteFile();

			// TODO : when rescan for change, some old plug file can still exist and be valid 
			// without the corresponding user vst folder 
			// need to take care of parent folder if any
			// need to remove it from knownPluginList if needed
			//if ((! searchLocations.isFileInPath(plug, true)))
			//	filePlugText.deleteFile();
			//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
			//-----------------------------------------------------------------------------

			//if (! AudioPluginFormatManager::getInstance()->doesPluginStillExist (listPlugTextInfo[1]))
			//	filePlugText.deleteFile();
			//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
			//-----------------------------------------------------------------------------
		}

		// purge of the knownPlugList
		bool needRecreate = false;
		for (int j = knownPluginList.getNumTypes() - 1; j >= 0; j--)
		{
			PluginDescription* desc = knownPluginList.getType(j);
			if (!pluginFormatManager->doesPluginStillExist(*desc))
			{
				knownPluginList.removeType(j);
				needRecreate = true;
			}
		}
		//if (needRecreate)
		{
			knownPluginList.clearBlacklistedFiles();
			xmlKnownPluginList = knownPluginList.createXml();
			xmlKnownPluginList->writeToFile(fileKnownPlugs, String::empty);
		}

		String message;
		String plugName;
		// create .plugin files
		for (int j = knownPluginList.getNumTypes() - 1; j >= 0; j--)
		{
			PluginDescription* desc = knownPluginList.getType(j);
			if (pluginFormatManager->doesPluginStillExist(*desc))
			{
				String pathPlugDescText = pluginsBasePath;

				String plugFileName = desc->name.replaceCharacters("\"#@,;:<>*^|?\\/", "--------------");
				plugName = desc->name;
				//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
				//-----------------------------------------------------------------------------

				File filePlug(desc->fileOrIdentifier);
				for (int i = 0; i < searchLocations.getNumPaths(); i++)
				{
					if (filePlug.isAChildOf(searchLocations[i]))
					{
						String subPath = filePlug.getRelativePathFrom(searchLocations[i]);

						if (subPath == filePlug.getFileName())
							subPath = String::empty;
						else
							subPath = subPath.upToLastOccurrenceOf("\\", true, true);

						pathPlugDescText << "/" << desc->pluginFormatName << "/" << subPath << plugFileName << PLUG_INS_EXT;
					}
				}

				//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
				//-----------------------------------------------------------------------------
				if (desc != nullptr)
				{
					String plugFormat("-");

					if (desc->pluginFormatName == "AudioUnit")
						plugFormat << "au";
					else if (desc->pluginFormatName == "VST")
						plugFormat << "vst";


					if (desc->category != String::empty)
						pathPlugDescText << "/" << desc->pluginFormatName << "/" + desc->category << "/" + plugFileName << plugFormat << PLUG_INS_EXT;
					else
						pathPlugDescText << "/" << desc->pluginFormatName << "/" + plugFileName << plugFormat << PLUG_INS_EXT;
				}

				//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
				//-----------------------------------------------------------------------------

				File filePlugDescText(pathPlugDescText);

				filePlugDescText.deleteFile();
				filePlugDescText.create();

				// content of the .plugin file used by usine and the browser
				String content;
				content << desc->name << newLine << desc->fileOrIdentifier << newLine << String::toHexString(desc->uid) << newLine << desc->pluginFormatName;
				filePlugDescText.appendText(content, true, true);

				message = String::empty;
				message << "create .plugin file: " << plugName;

				// trace to log
				//sdkTraceLogChar((AnsiCharPtr)message.toUTF8().getAddress(), FALSE);
				Logger::writeToLog(message);
			}
		}

	}
	catch (...) // std::exception& e
	{
		//sdkTraceLogChar("plug-in crashed during init", FALSE);
		Logger::writeToLog("plug-in crashed during init");
		return false;
	}

	crcPlugsFolder = crcComputeFromCurrentSettings();
	isScanning = 0;
	crcSaveSettings();

	int totalPlugScanned = knownPluginList.getNumTypes() + listcorruptedPlugs.size() + listErrorPlugs.size();
	String mess(knownPluginList.getNumTypes());
	mess << "/" << totalPlugScanned << " " << /*sdkGetTrans*/("lister_plugins_successfully_scanned");

	//sdkTraceLogChar((AnsiCharPtr)mess.toUTF8().getAddress(), TRUE);
	//sdkTraceChar((AnsiCharPtr)mess.toUTF8().getAddress());
	Logger::writeToLog(mess);

	//sdkNotifyUsine(NOTIFY_TARGET_SETUP, NOTIFY_MSG_RESCAN_PLUGINS_DONE);

	return true;
}


//-------------------------------------------------------------------------
// manage a plugins folder crc to detect if something change in the plugs setup
//
void PluginListerExe::crcLoadSettings()
{
	try
	{
		bool isCrcFileCorrupted = false;
		FileSearchPath filelistUserPlugsLoc;

		File crcFile(filePluginsDirectory.getChildFile(CRC_FILE).getFullPathName());

		if (crcFile.existsAsFile())
		{
			StringArray listCrcTextInfo;

			// read the crc code
			crcFile.readLines(listCrcTextInfo);
			crcPlugsFolder = listCrcTextInfo[0].getIntValue();

			// read the scan state
			if (listCrcTextInfo[1] == "1")
			{
				// rescan for change
				isScanning = 1;
			}
			else if (listCrcTextInfo[1] == "2")
			{
				// rescan all
				isScanning = 2;
			}
			else
			{
				// idle
				isScanning = 0;
			}

			// read user plugs path
			for (int i = 0; i < numOfPluginsPath; i++)
			{
				String pathUserPlugLoc(listCrcTextInfo[i + 2]);

				if (pathUserPlugLoc != String::empty)
				{
					if (File::isAbsolutePath(pathUserPlugLoc))
					{
						File fileUserPlugLoc(pathUserPlugLoc);
						if (fileUserPlugLoc.isDirectory())
						{
							filelistUserPluginsLocations.remove(i);
							filelistUserPluginsLocations.add(pathUserPlugLoc, i);
							//juce::CharPointer_UTF8 pcharUser = pathUserPlugLoc.toUTF8();
							//sdkSetEvtPChar(m_folderPluginsPath.getUnchecked(i), pcharUser.getAddress());
						}
						else
						{
							isCrcFileCorrupted = true;
						}
					}
					else
					{
						isCrcFileCorrupted = true;
					}
				}
			}
		}
		else
		{
			// fill folders with system default folders for vst
			for (int i = 0; i < numOfPluginsPath; i++)
			{
				if (defaultPluginsLocation[i].exists())
				{
					String pathDefPlugLoc(defaultPluginsLocation[i].getFullPathName());
					filelistUserPluginsLocations.remove(i);
					filelistUserPluginsLocations.add(pathDefPlugLoc, i);
					//juce::CharPointer_UTF8 pcharUser = pathDefPlugLoc.toUTF8();
					//sdkSetEvtPChar(m_folderPluginsPath.getUnchecked(i), pcharUser.getAddress());
				}
			}

			isCrcFileCorrupted = true;
		}

	}
	catch (...)
	{
		// error in crcload, will rescan all with the default plugs location
	}

	if (isScanning == 1)
		rescanForChanges();
	else if (isScanning == 2)
		rescanAll();

}

// 
void PluginListerExe::crcSaveSettings()
{
	File crcFile(filePluginsDirectory.getChildFile(CRC_FILE).getFullPathName());

	crcFile.create();

	// store the crc code
	crcFile.replaceWithText(String(crcPlugsFolder));

	// store the rescan auto mode
	crcFile.appendText(newLine);

	// append the scan state
	crcFile.appendText(String(isScanning));


	// store each user plug path
	for (int i = 0; i < numOfPluginsPath; i++)
	{
        /*
		filelistUserPluginsLocations.remove(i);

		String pathUser(sdkGetEvtPChar(m_folderPluginsPath.getUnchecked(i)));
		if (pathUser != String::empty)
		{
			filelistUserPluginsLocations.add(File(pathUser), i);
		}
*/
		crcFile.appendText(newLine);
		crcFile.appendText(filelistUserPluginsLocations[i].getFullPathName());
	}
}

unsigned short int PluginListerExe::crcComputeFromCurrentSettings()
{
	try
	{
		unsigned short int crcNew;

		// init the crc process
		CRC_Init(&crcNew);


		// each user folders path
		for (int i = 0; i < filelistUserPluginsLocations.getNumPaths(); i++)
		{
			//filelistUserPluginsLocations[i]
			if (filelistUserPluginsLocations[i].exists())
			{
				//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
				//-----------------------------------------------------------------------------
				DirectoryIterator iterUserFolder(filelistUserPluginsLocations[i], true, "*.dll");
				//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
				//-----------------------------------------------------------------------------
				DirectoryIterator iterUserFolder(filelistUserPluginsLocations[i], true, "*");
				//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
				//----------------------------------------------------------------------------	

				while (iterUserFolder.next())
				{
					File filePlug(iterUserFolder.getFile());

					// the creation time of the file
					String creationTime(filePlug.getCreationTime().toString(true, true, true, true));
					juce::CharPointer_UTF8 pCreationTime = creationTime.toUTF8();
					CRC_ProcessByte(&crcNew, pCreationTime.getAddress(), pCreationTime.sizeInBytes());

					// the name of the file
					String fileName(filePlug.getFileName());
					juce::CharPointer_UTF8 pFileName = fileName.toUTF8();
					CRC_ProcessByte(&crcNew, pFileName.getAddress(), pFileName.sizeInBytes());

				}
			}
		}

		DirectoryIterator iterPlugs(filePluginsDirectory, true, "*.plugin");

		// for each desc text file in the usine plugins folder
		while (iterPlugs.next())
		{
			File theFileItFound(iterPlugs.getFile());

			// the name of the file
			String fileName(theFileItFound.getFileName());
			juce::CharPointer_UTF8 pFileName = fileName.toUTF8();
			CRC_ProcessByte(&crcNew, pFileName.getAddress(), pFileName.sizeInBytes());
		}

		CRC_ProcessByte(&crcNew, (char*)&crcArchSalt, sizeof(crcArchSalt));
		// finalyse the crc process
		crcNew = CRC_Value(crcNew);

		return crcNew;

	}
	catch (...)
	{
		return 0;
	}
}

//
char* PluginListerExe::getDefaultPluginsLocation(int Index)
{
	captionDefaultPluginsPath[Index] = String::empty;
	caDefaultPluginsPath[Index] = "";
	int numOfPaths = defaultPluginsLocation.getNumPaths();

	if ((numOfPaths > 0) && (Index < numOfPluginsPath))
	{
		captionDefaultPluginsPath[Index] = defaultPluginsLocation[Index].getFullPathName();
		caDefaultPluginsPath[Index] = captionDefaultPluginsPath[Index].toUTF8();
	}
	return const_cast <char*> (caDefaultPluginsPath[Index]);
}

