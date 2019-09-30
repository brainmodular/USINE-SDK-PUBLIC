//-----------------------------------------------------------------------------
//@file  
//	JucePluginWindow.h
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

// include once, no more
#ifndef __JUCEPLUGINWWINDOW_H__
#define __JUCEPLUGINWWINDOW_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"
#include "JuceHeader.h"

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
#define BTN_HEIGHT 20

//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
//-----------------------------------------------------------------------------
#define HEIGHT_CORRECTION 30
//-----------------------------------------------------------------------------
#elif defined (USINE_OSX32)
//-----------------------------------------------------------------------------
#define HEIGHT_CORRECTION 0
//-----------------------------------------------------------------------------
#else
#error "condidionnal compilation error!"
#endif
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------

class PluginWrapper;

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
/** A desktop window containing a plugin's UI.
*/
class PluginWindow  : public juce::Component, Button::Listener
{
    PluginWindow (PluginWrapper* module_, AudioPluginInstance* owner_);

	void initWindow (AudioProcessorEditor* const plugEditor, void *nativeWindowToAttachTo);
	
public:
    static PluginWindow* getWindowFor (PluginWrapper* module, AudioPluginInstance* plugin, void *nativeWindowToAttachTo=nullptr);
    static void closeEditor(PluginWindow* plugEditor);

    ~PluginWindow ();
	
	void closeWindow ();
    void updateName();

    void* getNativeHandle();

    // trick needed on PC only to let usines windows go in front of the plug window
    // on MAC, it work without that
	//-----------------------------------------------------------------------------
#if (defined (USINE_WIN32) || defined (USINE_WIN64))
	//-----------------------------------------------------------------------------
    void focusGained (FocusChangeType cause);
    void focusLost (FocusChangeType cause);
	//-----------------------------------------------------------------------------
#endif
	//-----------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
private:
	void constructButtonsBar ();

	// reimpl from component
	void paint (Graphics& g);
	void moved ();
    void resized ();
    void userTriedToCloseWindow();
		
	// reimpl from Button::Listener
	void buttonClicked (Button *button);

	//-----------------------------------------------------------------------------
private:
	PluginWrapper* module;
    AudioPluginInstance* owner;
    
    

	int minWidth;
	int minHeight;

	int offsetXButtons;
	int offsetYButtons;
    
    
	ScopedPointer <TextButton> btnLoadBank;
	ScopedPointer <TextButton> btnSaveBank;
	ScopedPointer <TextButton> btnLoadPreset;
	ScopedPointer <TextButton> btnSavePreset;
	ScopedPointer <TextButton> btnPrevPreset;
	ScopedPointer <TextButton> btnNextPreset;
	ScopedPointer <TextButton> btnShowAllParams;
	ScopedPointer <TextButton> btnHideAllParams;

    AudioProcessorEditor* uiComp;
    AudioProcessorEditor* plugEditor;
    //Component::SafePointer<AudioProcessorEditor> uiComp;
    
   JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (PluginWindow);
};


#endif //__JUCEPLUGINWWINDOW_H__
