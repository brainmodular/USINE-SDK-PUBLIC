///-----------------------------------------------------------------------------
//@file  
//	JucePluginWindow.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the JucePluginWindow class.
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
#include "JucePluginWindow.h"
#include "PluginWrapper.h"

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
#include <Windows.h>  
#undef min
#undef max

//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
//-----------------------------------------------------------------------------
                                
//-----------------------------------------------------------------------------
#else
  #error "condidionnal compilation error!"
#endif
//---------------------------------------------------------------------------
static Array <PluginWindow*> activePluginWindows;

//-----------------------------------------------------------------------------
PluginWindow::PluginWindow (PluginWrapper* module_, AudioPluginInstance* owner_)
	: minWidth (50)
	, minHeight (50)
	, offsetXButtons (0)
	, offsetYButtons (0)
{
	module = module_;
	owner = owner_;
    

	setOpaque (true);
	//setTopLeftPosition (module->lastEditorPosX, module->lastEditorPosY); 
}

PluginWindow::~PluginWindow()
{
	module->lastEditorPosX = getPosition().getX();
	module->lastEditorPosY = getPosition().getY();
    
    removeKeyListener (module);

    removeFromDesktop();

    activePluginWindows.removeFirstMatchingValue (this);
	
	//delete ui;
	//removeAllChildren ();

    //uiComp.deleteAndZero();
    uiComp->toFront (false);
    uiComp->setVisible(false);
    delete uiComp;
    
	owner = nullptr;
}

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
void PluginWindow::focusGained (FocusChangeType cause)
{
    setAlwaysOnTop(false);
}

void PluginWindow::focusLost (FocusChangeType cause)
{
    setAlwaysOnTop(false);
}
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------

PluginWindow* PluginWindow::getWindowFor (PluginWrapper* module, AudioPluginInstance* plugin, void *nativeWindowToAttachTo)
{
    //AudioProcessorEditor* uiComp = nullptr;
    PluginWindow* result = nullptr;
    

    result =  new PluginWindow (module, plugin);
	
	if (plugin != nullptr && plugin->hasEditor())
    {
  //      sdkTraceLogChar("call result->initWindow");
		result->initWindow (plugin->createEditor(), nativeWindowToAttachTo);
  //      sdkTraceLogChar("call result->initWindow ok");

        return result;
    }

    return result;
}

void PluginWindow::closeEditor(PluginWindow* plugEditor)
{
    
    int index = activePluginWindows.indexOf(plugEditor);
    if( index >= 0)
    {
        //plugEditor->removeFromDesktop();
        delete activePluginWindows.getUnchecked (index);
    }
    
    
}

void PluginWindow::initWindow (AudioProcessorEditor* const plugEditor, void *nativeWindowToAttachTo)
{
    
    uiComp = plugEditor;
    
    this->setName (uiComp->getName());
	
    //-----------------------------------------------------------------------------
//#if (defined (USINE_WIN32) || defined (USINE_WIN64))
    //-----------------------------------------------------------------------------
	
	constructButtonsBar ();
    
	//uiComp->setOpaque (true);
	uiComp->setTopLeftPosition (0, BTN_HEIGHT);
	setVisible(true);
	addChildAndSetID (uiComp, "editor");
 	uiComp->toFront (true);
    
    addToDesktop (ComponentPeer::windowHasDropShadow | ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowIsResizable | ComponentPeer::windowRepaintedExplictly, nativeWindowToAttachTo);
    
    updateName();
	//setVisible(true);
    toFront(true);

	//-----------------------------------------------------------------------------
#if defined (USINE_OSX32)
	//-----------------------------------------------------------------------------
    setAlwaysOnTop(true);
	//-----------------------------------------------------------------------------
#endif
	//-----------------------------------------------------------------------------
	
    uiComp->setWantsKeyboardFocus(true);
	uiComp->addKeyListener (module);
	setWantsKeyboardFocus(true);
	grabKeyboardFocus();
    addKeyListener(module);
    setSize(std::max(minWidth, uiComp->getWidth()), offsetYButtons + uiComp->getHeight());
	setTopLeftPosition(module->lastEditorPosX, module->lastEditorPosY);
    
    setVisible(true);
	
    activePluginWindows.add (this);
}

void PluginWindow::closeWindow()
{
    module->setVisible (FALSE);
}

void PluginWindow::updateName()
{
	if (owner != nullptr)
    {
		String presetName = owner->getProgramName (owner->getCurrentProgram());
        
        if (presetName.isNotEmpty())
            this->setName (module->getPluginName () + "-" + presetName);
        else
            this->setName (module->getPluginName ());
    }
}

void* PluginWindow::getNativeHandle()
{
     void* result = nullptr;

     if (getPeer() != nullptr)
         result = getPeer()->getNativeHandle();

     return result;
};

//-----------------------------------------------------------------------------
void PluginWindow::constructButtonsBar ()
{
	int currentWidth = 200;
	Component* editor = findChildWithID("editor");

	if (editor != nullptr)
		currentWidth = editor->getWidth();

	offsetXButtons = 0;

	btnLoadBank   = new TextButton(module->sdkGetTrans("wrapper_load_bank"));
	//btnLoadBank->setOpaque (true);
	btnLoadBank->changeWidthToFitText(BTN_HEIGHT - 2);
	btnLoadBank->setTopLeftPosition(0, 1);
	btnLoadBank->addListener (this);
	addAndMakeVisible (btnLoadBank);
	offsetXButtons += btnLoadBank->getWidth () + 1;

	btnSaveBank = new TextButton(module->sdkGetTrans("wrapper_save_bank"));
	btnSaveBank->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnSaveBank->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnSaveBank->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnSaveBank->addListener (this);
	addAndMakeVisible (btnSaveBank);
	offsetXButtons += btnSaveBank->getWidth () + 4;

	btnLoadPreset = new TextButton(module->sdkGetTrans("wrapper_load_preset"));
	btnLoadPreset->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnLoadPreset->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnLoadPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnLoadPreset->addListener (this);
	addAndMakeVisible (btnLoadPreset);
	offsetXButtons += btnLoadPreset->getWidth () + 1;

	btnSavePreset = new TextButton(module->sdkGetTrans("wrapper_save_preset"));
	btnSavePreset->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnSavePreset->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnSavePreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnSavePreset->addListener (this);
	addAndMakeVisible (btnSavePreset);
	offsetXButtons += btnSavePreset->getWidth () + 4;

	btnPrevPreset = new TextButton(module->sdkGetTrans("wrapper_prev_preset"));
	btnPrevPreset->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnPrevPreset->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnPrevPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnPrevPreset->addListener (this);
	addAndMakeVisible (btnPrevPreset);
	offsetXButtons += btnPrevPreset->getWidth () + 1;

	btnNextPreset = new TextButton(module->sdkGetTrans("wrapper_next_preset"));
	btnNextPreset->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnNextPreset->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnNextPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnNextPreset->addListener (this);
	addAndMakeVisible (btnNextPreset);
	offsetXButtons += btnNextPreset->getWidth () + 4;

	btnShowAllParams = new TextButton(module->sdkGetTrans("wrapper_show_all_params"));
	btnShowAllParams->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnShowAllParams->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnShowAllParams->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnShowAllParams->addListener (this);
	addAndMakeVisible (btnShowAllParams);
	offsetXButtons += btnShowAllParams->getWidth () + 1;

	btnHideAllParams = new TextButton(module->sdkGetTrans("wrapper_hide_all_params"));
	btnHideAllParams->changeWidthToFitText(BTN_HEIGHT - 2);
	if (offsetXButtons + btnHideAllParams->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}
	btnHideAllParams->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	btnHideAllParams->addListener (this);
	addAndMakeVisible(btnHideAllParams);
	offsetYButtons += BTN_HEIGHT;
	
}


void PluginWindow::paint (Graphics& g)
{
	g.fillAll (Colours::black);
}


void PluginWindow::resized()
{
	offsetXButtons = 0;
	offsetYButtons = 0;

	int currentWidth = getWidth();

	// btnLoadBank
	offsetXButtons += btnLoadBank->getWidth() + 1;

	// btnSaveBank
	if (offsetXButtons + btnSaveBank->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnSaveBank->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnSaveBank->getWidth() + 4;

	// btnLoadPreset
	if (offsetXButtons + btnLoadPreset->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnLoadPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnLoadPreset->getWidth() + 1;

	// btnSavePreset
	if (offsetXButtons + btnSavePreset->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnSavePreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnSavePreset->getWidth() + 4;

	// btnPrevPreset
	if (offsetXButtons + btnPrevPreset->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnPrevPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnPrevPreset->getWidth() + 1;

	// btnNextPreset
	if (offsetXButtons + btnNextPreset->getWidth() + 4 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnNextPreset->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnNextPreset->getWidth() + 4;

	// btnShowAllParams
	if (offsetXButtons + btnShowAllParams->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnShowAllParams->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnShowAllParams->getWidth() + 1;

	// btnHideAllParams
	if (offsetXButtons + btnHideAllParams->getWidth() + 1 > currentWidth)
	{
		offsetXButtons = 0;
		offsetYButtons += BTN_HEIGHT;
	}

	btnHideAllParams->setTopLeftPosition(offsetXButtons, offsetYButtons + 1);
	offsetXButtons += btnHideAllParams->getWidth() + 1;

	offsetYButtons += BTN_HEIGHT;

	Component* editor = findChildWithID("editor");

	if (editor != nullptr)
	{
		editor->setBounds(0, offsetYButtons, std::max(minWidth, getWidth()), getHeight() - offsetYButtons);
	}
}

void PluginWindow::moved ()
{
	module->lastEditorPosX = getPosition().getX();
	module->lastEditorPosY = getPosition().getY();
}

void PluginWindow::userTriedToCloseWindow()
{
    closeWindow();
}


//-----------------------------------------------------------------------------
void PluginWindow::buttonClicked (Button *button)
{
	if (button == btnLoadBank)
	{
        FileChooser bankChooser ("Please select the bank you want to load...",
                               File::nonexistent,
                               "*.fxb");

        if (bankChooser.browseForFileToOpen())
        {
            File bankFile (bankChooser.getResult());
			   
			MemoryBlock chunk;
			bankFile.loadFileAsData(chunk);
			owner->setStateInformation (chunk.getData (), chunk.getSize());
        }	
	}
	else if (button == btnSaveBank)
	{
        FileChooser bankChooser ("Please select the bank you want to save...",
                               File::nonexistent,
                               "*.fxb");

        if (bankChooser.browseForFileToSave(true))
        {
            File bankFile (bankChooser.getResult());
			bankFile = bankFile.withFileExtension("fxb");
			   
			MemoryBlock chunk;
			owner->getStateInformation (chunk);
			bankFile.replaceWithData(chunk.getData (), chunk.getSize());
        }	
	}
	else if (button == btnLoadPreset)
	{
        FileChooser presetChooser ("Please select the preset you want to load...",
                               File::nonexistent,
                               "*.fxp");

        if (presetChooser.browseForFileToOpen())
        {
            File presetFile (presetChooser.getResult());
			   
			MemoryBlock chunk;
			presetFile.loadFileAsData(chunk);
			owner->setCurrentProgramStateInformation (chunk.getData (), chunk.getSize());
        }	
	}
	else if (button == btnSavePreset)
	{
        FileChooser presetChooser ("Please select the preset you want to save...",
                               File::nonexistent,
                               "*.fxp");

        if (presetChooser.browseForFileToSave(true))
        {
            File presetFile (presetChooser.getResult());
			presetFile = presetFile.withFileExtension("fxp");

			MemoryBlock chunk;
			owner->getCurrentProgramStateInformation (chunk);
			presetFile.replaceWithData(chunk.getData (), chunk.getSize());
        }	
	}
	else if (button == btnPrevPreset)
	{
		int numPresets = owner->getNumPrograms ();
        if (numPresets > 1)
        {
            int preset = owner->getCurrentProgram ();
            preset = (numPresets + preset - 1) % numPresets;
            owner->setCurrentProgram (preset);
            updateName();
            module->updateModuleParams ();
        }
	}
	else if (button == btnNextPreset)
	{
		int numPresets = owner->getNumPrograms ();
        if (numPresets > 1)
        {
            int preset = owner-> getCurrentProgram ();
            preset = (++preset) % numPresets;
            owner->setCurrentProgram (preset);
            updateName();
            module->updateModuleParams ();
        }
	}	
    else if (button == btnShowAllParams)
	{
        module->showAllModuleParams ();
	}
	else if (button == btnHideAllParams)
	{
        module->hideAllModuleParams ();
	}
}
