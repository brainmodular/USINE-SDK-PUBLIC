//-----------------------------------------------------------------------------
//@file  
//	PluginLister.cpp
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
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
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
#include "PluginLister.h"
#include <iomanip>
#include <csignal>
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    //-----------------------------------------------------------------------------
#if (defined (USINE_OSX32) || defined (USINE_OSX64))
    //-----------------------------------------------------------------------------
    //initExceptionHandler();
    //-----------------------------------------------------------------------------
#endif
    //-----------------------------------------------------------------------------
    
//	initialiseJuce_GUI ();
	pModule = new PluginLister (optionalString);
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((PluginLister*)pModule);
    
//    shutdownJuce_GUI ();
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "test plug-ins";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "test plug-ins lister user module";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = MODULE_USER_VERSION;

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void PluginLister::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtPluginLister;
	pModuleInfo->BackColor          = sdkGetUsineColor(clPluginModuleColor);
	pModuleInfo->NumberOfParams     = numOfPluginsPath + 2;
	pModuleInfo->QueryString		= "";
	pModuleInfo->QueryListValues	= "";
	pModuleInfo->QueryDefaultIdx	= 1;
	pModuleInfo->Version			= MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------

// query system not used

//-----------------------------------------------------------------------------
// initialisation
void PluginLister::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	// directory where plug-ins desc are stored
	pluginsBasePath << sdkGetUsinePlugInsPath ();
	filePluginsDirectory = pluginsBasePath;
		
	// the usine temp folder where is stored the crc+userpath file
	fileTempDirectory = String (sdkGetUsineTempPath());

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
    
	crcLoadSettings();
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void PluginLister::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{		
	// m_folderPluginsPath
	if (ParamIndex < numOfPluginsPath)
	{
		int pathIndex = ParamIndex ;
		listPluginsPath.add (String(sdkGetTrans("lister_folder")) + String(" ") + String(pathIndex + 1));
		caPluginsPath = listPluginsPath[pathIndex].toUTF8 ();

		pParamInfo->ParamType			= ptChooseFolder;
		pParamInfo->Caption				= const_cast <char*> (caPluginsPath);
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->DontSave			= TRUE;
		if (ParamIndex == 0)
		{
            strSepVstFolders = std::string(sdkGetTrans("lister_plug_folders"));

			pParamInfo->IsSeparator			= TRUE;
			pParamInfo->SeparatorCaption	= (AnsiCharPtr)strSepVstFolders.c_str();
		}
	}
	// m_btnRescanOnlyChange
	else if (ParamIndex == numOfPluginsPath)
	{
		pParamInfo->ParamType			= ptButton;
		pParamInfo->Caption				= "rescan changes";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->Translate		    = TRUE;
		pParamInfo->IsSeparator			= TRUE;
	}   
	// m_btnRescanAll
	else if (ParamIndex == numOfPluginsPath + 1)
	{
		pParamInfo->ParamType			= ptButton;
		pParamInfo->Caption				= "rescan all";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= FALSE;
		pParamInfo->Translate		    = TRUE;
		pParamInfo->IsSeparator			= TRUE;
	} 
	else {}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void PluginLister::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
	// m_folderPluginsPath
	if (ParamIndex < numOfPluginsPath)
		m_folderPluginsPath.set (ParamIndex, pEvent);
	// m_btnRescanOnlyChange
	else if (ParamIndex == numOfPluginsPath)
		m_btnRescanOnlyChange = pEvent;
	// m_btnRescanAll
	else if (ParamIndex == numOfPluginsPath + 1)
		m_btnRescanAll = pEvent;
}

//-----------------------------------------------------------------------------
// Parameters callback
void PluginLister::onCallBack (UsineMessage* Message) 
{
    if (Message->message == NOTIFY_MSG_CHECK_SCANNING)
    {
    	if (isScanning == 1)
            rescanForChanges(false);
        else if (isScanning == 2)
            rescanAll();
    }
    else if (Message->message == NOTIFY_MSG_USINE_LOADED)
    {
    	if (isScanning == 1)
            rescanForChanges();
        else if (isScanning == 2)
            rescanAll();
    }
	else if (Message->lParam == MSG_CHANGE)
	{
		// m_folderPluginsPath
		if (Message->wParam < numOfPluginsPath)
		{
			// sanity check
			if (Message->wParam < m_folderPluginsPath.size() && Message->wParam >= 0)
			{
				String pathUser(sdkGetEvtPChar(m_folderPluginsPath.getUnchecked((int)Message->wParam)));
				filelistUserPluginsLocations.remove ((int)Message->wParam);
				
				if (pathUser != String() )
					filelistUserPluginsLocations.add (File(pathUser),(int)Message->wParam);

				crcSaveSettings();
			}
		}
		// m_btnRescanOnlyChange
		else if (Message->wParam == numOfPluginsPath )
		{   
			rescanForChanges();      
		} 
		// m_btnRescanAll
		else if (Message->wParam == numOfPluginsPath + 1)
		{
			rescanAll();
		}
        
	}
}


//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
//
void PluginLister::rescanAll()
{
	try
	{
		// clear all old informations
		filePluginsDirectory.deleteRecursively();
				
		MessageManager::getInstance()->runDispatchLoopUntil (100);
		
        if (xmlKnownPluginList != nullptr)
            xmlKnownPluginList->deleteAllChildElements ();

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
		sdkTraceErrorChar(" in PluginLister::PluginLister:rescanAll");
	}
	
    if (! confirmRescanPopup (2))
    {
        isScanning = 0;
        return;
    }
	
    crcPlugsFolder = crcComputeFromCurrentSettings();
    isScanning = 1;
	crcSaveSettings();	

    // trace to log + splash screen
    sdkTraceLogChar ("begin plug-ins scan all...", FALSE);

	// actually do the rescan process
	bool succes = false;
	while (!succes)
	{
		succes = rescanPlugins();
	}
    // trace only to log
    sdkTraceLogChar ("end plug-ins scan all...", FALSE);
}

// 
void PluginLister::rescanForChanges(bool wantPopup)
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
		sdkTraceErrorChar(" in PluginLister::rescanForChanges");
	}
	
    if (wantPopup)
    {
        if (! confirmRescanPopup (1))
        {
            isScanning = 0;
            return;
        }
    }
		
    crcPlugsFolder = crcComputeFromCurrentSettings();
    isScanning = 1;
    crcSaveSettings();
    
    // trace to log + splash screen
	sdkTraceLogChar ("begin plug-ins scan for change...", FALSE);
    
	// actually do the rescan process
	bool succes = false;
	while (!succes)
	{
		succes = rescanPlugins();
	}
    // trace only to log
    sdkTraceLogChar ("end plug-ins scan for change...", FALSE);


/*
    String execCmd;
	execCmd << sdkGetGlobalApplicationPath () << PLUG_LISTER_EXEC;

    ChildProcess process;
    bool started = process.start(execCmd);
*/

}


//
bool PluginLister::confirmRescanPopup (int option)
{       
    bool result = false;
    String popupMessage;
   

    if (isScanning > 0)
    {
        StringArray listLastErrorPlugs;
        String pathErrorPlugs(pluginsBasePath);
        pathErrorPlugs << ERROR_PLUG_INS_FILE;
        fileErrorPlugs = pathErrorPlugs;
        fileErrorPlugs.readLines(listLastErrorPlugs);
        
        popupMessage += sdkGetTrans("lister_last_scan_not_complete");
        popupMessage += newLine;
        if (listLastErrorPlugs.size()>1)
        {
        popupMessage += sdkGetTrans("lister_last_error");
        popupMessage += ":";
        popupMessage += listLastErrorPlugs[listLastErrorPlugs.size()-2];
        popupMessage += newLine;
        }
        
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
        result =  true;

    return result;
}

void PluginLister::saveListErrors()
{
	fileErrorPlugs.deleteFile();
	fileErrorPlugs.create();
	for (int j = 0; j < listErrorPlugs.size(); j++)
	{
		fileErrorPlugs.appendText(listErrorPlugs[j]);
		fileErrorPlugs.appendText(newLine);
	}
}

//------------------------------------------------
//void on_abort(int sign)
//{
//    // do whatever you think is appropriate
//    throw(false);
//
//}

//------------------------------------------------
bool PluginLister::rescanPlugins () noexcept
{     
    
     try
	{
		int numFormats = pluginFormatManager->getNumFormats ();
		//int lastNumPlugs = 0;

        FileSearchPath searchLocations (filelistUserPluginsLocations);

        // notification to fire a callback after the scan to check if the scann is complete or cr   shed in middle
        //sdkNotifyUsine (NOTIFY_TARGET_USER_MODULE, NOTIFY_MSG_CHECK_SCANNING);

        // iterate each plugins formats (VST, AU)
		for (int i = 0; i < numFormats; i++) 
		{
			AudioPluginFormat* currentFormat = pluginFormatManager->getFormat (i);

			PluginDirectoryScanner scanner(knownPluginList, *currentFormat, searchLocations, true, fileCorruptedPlugs, true);
		
			String nextPlug;
			String currentPlug;

			bool scan = true;
            int numScanned = 0;
            // Directory scan
			while (scan)
			{
				nextPlug = scanner.getNextPluginFileThatWillBeScanned ();

				if (nextPlug == String())
				{
					scan = false;
					break;
				}

				listcorruptedPlugs.clear();
				fileCorruptedPlugs.readLines(listcorruptedPlugs);
				++numScanned;
				
				if (   knownPluginList.getTypeForFile(nextPlug) != nullptr	
					|| listcorruptedPlugs.contains (nextPlug, true) 
					|| listErrorPlugs.contains (nextPlug, true) 
					)
				{
                    String message;
					message << nextPlug << " " << "skipped";

                    // trace to log
					sdkTraceLogChar ((AnsiCharPtr)message.toUTF8().getAddress(), FALSE);
					
					scan = scanner.skipNextFile();
				}
				else
				{
                    desc = knownPluginList.getTypeForFile(nextPlug);
                    
                    String scanningMessage = String(sdkGetTrans("lister_scanning"));
                    
                    if (desc != nullptr)
                    {
                        // trace to log + splash screen
                        scanningMessage << " " << currentFormat->getName();
                        String plugName = desc->name;
                        scanningMessage << " ("  << (int)(scanner.getProgress()*100)  << "%) : " << plugName;
                        sdkTraceLogChar ((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), TRUE);
                    }
                    else
                    {
                        // trace to log + splash screen
                        scanningMessage << " " << currentFormat->getName();
                        if (nextPlug != String())
                        {
                            scanningMessage << " ("  << (int)(scanner.getProgress()*100)  << "%) : " << nextPlug;
                            
                            sdkTraceLogChar ((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), TRUE);
                            // trace to log
                            scanningMessage = String();
                            scanningMessage << nextPlug;
                            sdkTraceLogChar ((AnsiCharPtr)scanningMessage.toUTF8().getAddress(), FALSE);
                        }
                    }
                    
                    // ajouter le next dans la liste des erreurs potentielles
					listErrorPlugs.add(nextPlug);
					listErrorPlugs.removeDuplicates(true);
					saveListErrors();
                    
                    
					try
					{
                       //signal(SIGABRT, on_abort);
                       scan = scanner.scanNextFile(true, currentPlug);
                        
					}
					catch ( std::exception& e)
					{
						sdkTraceLogChar("in PluginLister::PluginLister:rescanPlugins::scanNextFile", FALSE);
						continue;
					}
                    
					// scan ok on l'efface de la liste des erreurs
					listErrorPlugs.removeString(nextPlug,true);
					saveListErrors();

			        const StringArray failedFiles = scanner.getFailedFiles();
                    
                    listErrorPlugs.addArray(failedFiles);
					listErrorPlugs.removeDuplicates(true);
                    
                    
		
                    // update plugs with error
		            StringArray listLastErrorPlugs;
		            fileErrorPlugs.readLines(listLastErrorPlugs);
				
					if (listLastErrorPlugs != listErrorPlugs)
					{
						saveListErrors();
					}


					knownPluginList.clearBlacklistedFiles();
					xmlKnownPluginList = knownPluginList.createXml ();
					xmlKnownPluginList->writeToFile (fileKnownPlugs, String());
					
                    

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
				fileErrorPlugs.appendText (listErrorPlugs[j]);
				fileErrorPlugs.appendText (newLine);
			}
		}

		DirectoryIterator iter (filePluginsDirectory, true, "*" + PLUG_INS_EXT);

		// purge of the plugins folder
		while (iter.next())
		{
			File filePlugText (iter.getFile());
			StringArray listPlugTextInfo;
			filePlugText.readLines(listPlugTextInfo);

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
			File plug(listPlugTextInfo[1]);
                
			if ((! plug.existsAsFile()) && (searchLocations.isFileInPath(plug, true)))
				filePlugText.deleteFile();

            // TODO : when rescan for change, some old plug file can still exist and be valid 
            // without the corresponding user vst folder 
            // need to take care of parent folder if any
            // need to remove it from knownPluginList if needed
			//if ((! searchLocations.isFileInPath(plug, true)))
			//	filePlugText.deleteFile();
//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
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
		for (int j = knownPluginList.getNumTypes()-1; j >=0; j--)
		{
			PluginDescription* desc = knownPluginList.getType(j);
			if (! pluginFormatManager->doesPluginStillExist(*desc))
			{
				knownPluginList.removeType(j);
				needRecreate = true;
			}
		}
		//if (needRecreate)
		{
			knownPluginList.clearBlacklistedFiles();
			xmlKnownPluginList = knownPluginList.createXml ();
			xmlKnownPluginList->writeToFile (fileKnownPlugs, String());
		}

        String message;
        String plugName;
        // create .plugin files
		for (int j = knownPluginList.getNumTypes()-1; j >=0; j--)
		{
			PluginDescription* desc = knownPluginList.getType(j);
			if (pluginFormatManager->doesPluginStillExist(*desc))
			{
					    String pathPlugDescText = pluginsBasePath;
                    
						String plugFileName = desc->name.replaceCharacters ("\"#@,;:<>*^|?\\/", "--------------");
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
								    subPath = String();
							    else
								    subPath = subPath.upToLastOccurrenceOf("\\", true, true);

							    pathPlugDescText << "/" << desc->pluginFormatName << "/" << subPath << plugFileName << PLUG_INS_EXT;
						    }
					    }
                    
    //-----------------------------------------------------------------------------
    #elif (defined (USINE_OSX32) || defined (USINE_OSX64))
    //-----------------------------------------------------------------------------
                        if (desc != nullptr)
                        {
							String plugFormat("-");
							
							if (desc->pluginFormatName == "AudioUnit")
								plugFormat << "au";
							else if (desc->pluginFormatName == "VST")
								plugFormat << "vst";
							else if (desc->pluginFormatName == "VST3")
								plugFormat << "vst3";

									
							if (desc->manufacturerName!= String())
                                pathPlugDescText << "/" << desc->pluginFormatName << "/" + desc->manufacturerName.replaceCharacters ("\"#@,;:<>*^|?\\/", "--------------") << "/" + plugFileName << plugFormat << PLUG_INS_EXT;
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

                        message = String();
					    message << "create .plugin file: " << plugName;
						
					    // trace to log
					    sdkTraceLogChar ((AnsiCharPtr)message.toUTF8().getAddress(), FALSE);
			}
		}

	}
	catch (...) // std::exception& e
	{
        sdkTraceLogChar ("in PluginLister::PluginLister:rescanPlugins", FALSE);
		return false;
	}

	crcPlugsFolder = crcComputeFromCurrentSettings();
    isScanning = 0;
	crcSaveSettings();

    int totalPlugScanned = knownPluginList.getNumTypes () + listcorruptedPlugs.size () + listErrorPlugs.size ();
	String mess(knownPluginList.getNumTypes ());
	mess << "/" << totalPlugScanned << " " << sdkGetTrans("lister_plugins_successfully_scanned");

	sdkTraceLogChar ((AnsiCharPtr)mess.toUTF8().getAddress(), TRUE);
    sdkTraceChar ((AnsiCharPtr)mess.toUTF8().getAddress());

	sdkNotifyUsine (NOTIFY_TARGET_SETUP, NOTIFY_MSG_RESCAN_PLUGINS_DONE);  

	return true;
}

//-------------------------------------------------------------------------
// manage a plugins folder crc to detect if something change in the plugs setup
//
void PluginLister::crcLoadSettings()
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
			crcPlugsFolder = listCrcTextInfo[0].getIntValue ();
				
			// read the scan state
			if (listCrcTextInfo[1] == "1")
			{
                // rescan for change
				isScanning = 1;
			}
			else if(listCrcTextInfo[1] == "2")
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
				String pathUserPlugLoc(listCrcTextInfo[i+2]);
					
				if (pathUserPlugLoc != String())
				{					
					if (File::isAbsolutePath(pathUserPlugLoc))
					{
						File fileUserPlugLoc(pathUserPlugLoc);
						if (fileUserPlugLoc.isDirectory())
						{
							filelistUserPluginsLocations.remove (i);
							filelistUserPluginsLocations.add (pathUserPlugLoc, i);
							juce::CharPointer_UTF8 pcharUser =  pathUserPlugLoc.toUTF8 ();
							sdkSetEvtPChar(m_folderPluginsPath.getUnchecked(i), pcharUser.getAddress());
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
				    filelistUserPluginsLocations.remove (i);
                    filelistUserPluginsLocations.add (pathDefPlugLoc, i);
				    juce::CharPointer_UTF8 pcharUser =  pathDefPlugLoc.toUTF8 ();
				    sdkSetEvtPChar(m_folderPluginsPath.getUnchecked(i), pcharUser.getAddress());
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
void PluginLister::crcSaveSettings()
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
		filelistUserPluginsLocations.remove(i);

		String pathUser(sdkGetEvtPChar(m_folderPluginsPath.getUnchecked(i)));
		if (pathUser != String())
		{
			filelistUserPluginsLocations.add(File(pathUser), i);
		}

		crcFile.appendText(newLine);
		crcFile.appendText(pathUser);
	}
}

unsigned short int PluginLister::crcComputeFromCurrentSettings()
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
		    DirectoryIterator iterUserFolder (filelistUserPluginsLocations[i], true, "*.dll");
//-----------------------------------------------------------------------------
#elif (defined (USINE_OSX32) || defined (USINE_OSX64))
//-----------------------------------------------------------------------------
		    DirectoryIterator iterUserFolder (filelistUserPluginsLocations[i], true, "*");
//-----------------------------------------------------------------------------
#else
  #error "condidionnal compilation error!"
#endif
//----------------------------------------------------------------------------	

		    while (iterUserFolder.next())
		    {
			    File filePlug (iterUserFolder.getFile());

			    // the creation time of the file
			    String creationTime(filePlug.getCreationTime().toString (true, true, true, true));
			    juce::CharPointer_UTF8 pCreationTime =  creationTime.toUTF8 ();
			    CRC_ProcessByte(&crcNew, pCreationTime.getAddress(), pCreationTime.sizeInBytes ());

			    // the name of the file
			    String fileName(filePlug.getFileName());
			    juce::CharPointer_UTF8 pFileName =  fileName.toUTF8 ();
			    CRC_ProcessByte(&crcNew, pFileName.getAddress(), pFileName.sizeInBytes ());

		    }
        }
	}

    DirectoryIterator iterPlugs (filePluginsDirectory, true, "*.plugin");
    
	// for each desc text file in the usine plugins folder
    while (iterPlugs.next())
    {
        File theFileItFound (iterPlugs.getFile());
           
		// the name of the file
		String fileName(theFileItFound.getFileName());
		juce::CharPointer_UTF8 pFileName =  fileName.toUTF8 ();
		CRC_ProcessByte(&crcNew, pFileName.getAddress(), pFileName.sizeInBytes ());
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
char* PluginLister::getDefaultPluginsLocation(int Index)
{
	captionDefaultPluginsPath[Index] = String();
	caDefaultPluginsPath[Index] = "";
	int numOfPaths = defaultPluginsLocation.getNumPaths();

	if ( (numOfPaths > 0) && (Index < numOfPluginsPath))
	{
		captionDefaultPluginsPath[Index] = defaultPluginsLocation[Index].getFullPathName();
		caDefaultPluginsPath[Index] = captionDefaultPluginsPath[Index].toUTF8 ();
	}
	return const_cast <char*> (caDefaultPluginsPath[Index]);
}

