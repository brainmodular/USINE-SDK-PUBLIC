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
#include "JucePluginWindow.h"
#include "PluginWrapper.h"
#include "../../sdk/UserModule.h"

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
PluginWindow::PluginWindow(PluginWrapper* module_, AudioPluginInstance* owner_, void *nativeWindowToAttachTo)
	: DocumentWindow(owner_->getName(),
		LookAndFeel::getDefaultLookAndFeel().findColour(ResizableWindow::backgroundColourId),
		DocumentWindow::minimiseButton | DocumentWindow::closeButton)
{

	module = module_;
	owner = owner_;
    module->sdkTraceLogChar("call setSize");
	//setSize(400, 400);
    module->sdkTraceLogChar("call setUsingNativeTitleBar");
	setUsingNativeTitleBar(true);

    module->sdkTraceLogChar("call owner_->hasEditor");
	if (owner_ != nullptr && owner_->hasEditor())
	{
        module->sdkTraceLogChar("call owner_->createEditorIfNeeded");
		if (auto* ui = owner_->createEditorIfNeeded())
		{
            module->sdkTraceLogChar("call setContentOwned");
			setContentOwned(ui, true);
            module->sdkTraceLogChar("call ui->setOpaque");
			ui->setOpaque(true);
      /*      module_->sdkTraceLogChar("call ui->addToDesktop");
			ui->addToDesktop(ComponentPeer::windowHasDropShadow | ComponentPeer::windowHasTitleBar | ComponentPeer::windowHasCloseButton | ComponentPeer::windowIsResizable | ComponentPeer::windowRepaintedExplictly, nativeWindowToAttachTo);
*/
		}
        module->sdkTraceLogChar("call owner_->getName");
		setName(owner_->getName());
        module->sdkTraceLogChar("call setVisible");
		setVisible(true);
        module->sdkTraceLogChar("call toFront");
		toFront(true);
        module->sdkTraceLogChar("call setAlwaysOnTop");
		setAlwaysOnTop(true);
        module->sdkTraceLogChar("call grabKeyboardFocus");
		grabKeyboardFocus();
        module->sdkTraceLogChar("call setWantsKeyboardFocus");
		setWantsKeyboardFocus(true);
        module->sdkTraceLogChar("call addKeyListener");
		addKeyListener(module);
		module->sdkTraceLogChar("call activePluginWindows");
		activePluginWindows.add(this);
        if ((module->lastEditorPosX!=0) && (module->lastEditorPosX!=0))
        {
            module_->sdkTraceLogChar("call setTopLeftPosition");
            setTopLeftPosition(module->lastEditorPosX, module->lastEditorPosY);
        }
        else
        {
            module->sdkTraceLogChar("call centreWithSize");
            centreWithSize(getWidth(), getHeight());
        }
	}
    module_->sdkTraceLogChar("create PluginWindow done");

}

PluginWindow::~PluginWindow()
{
	module->lastEditorPosX = getPosition().getX();
	module->lastEditorPosY = getPosition().getY();
	clearContentComponent();

    removeKeyListener (module);

    removeFromDesktop();

    activePluginWindows.removeFirstMatchingValue (this);
	    
	owner = nullptr;
}

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
void PluginWindow::focusGained (FocusChangeType cause)
{
//    setAlwaysOnTop(false);
}

void PluginWindow::focusLost (FocusChangeType cause)
{
  //  setAlwaysOnTop(false);
}

//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------


void PluginWindow::closeEditor(PluginWindow* plugEditor)
{
    
    int index = activePluginWindows.indexOf(plugEditor);
    if( index >= 0)
    {
        delete activePluginWindows.getUnchecked (index);
    }
}


void PluginWindow::closeWindow()
{
    module->lastEditorPosX = getPosition().getX();
    module->lastEditorPosY = getPosition().getY();
    module->deleteEditor();
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



void PluginWindow::paint (Graphics& g)
{
	g.fillAll (Colours::black);
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


