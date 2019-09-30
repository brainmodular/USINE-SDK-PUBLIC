//-----------------------------------------------------------------------------
//@file  
//	JuceLogWindow.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
// A simple log window system for Juce.
//
//HISTORIC 
//	2015/10/6
//    
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
#ifndef __JUCE_LOG_WINDOW_H__
#define __JUCE_LOG_WINDOW_H__

#include "JuceHeader.h"

class JuceLogWindow   : public DocumentWindow, 
					private KeyListener
{
public:
    JuceLogWindow(const String& name,
        const Colour& backgroundColour,
        int requiredButtons,
        bool addToDesktop = true);

    ~JuceLogWindow();

    void closeButtonPressed();
    bool    keyPressed (const KeyPress &key, Component *originatingComponent);

    juce_UseDebuggingNewOperator

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JuceLogWindow)
    
};

class JuceLogComponent : public Component,
					 public TableListBoxModel, 
					 public ButtonListener
{
public :
	JuceLogComponent();
	~JuceLogComponent();

    void		resized();
    void paint (Graphics & g);
    int			getNumRows();
    void		paintRowBackground (Graphics &g, int rowNumber, int width, int height, bool rowIsSelected);
    void		paintCell (Graphics &g, int rowNumber, int columnId, int width, int height, bool rowIsSelected);

    void buttonClicked (Button* buttonThatWasClicked);
	
    void AddInLogArray(const String & strMessage);

private:
    TableListBox *  m_pLogTableListBox;
    TextButton  *   m_pSaveTextButton;
    TextButton *    m_pClearTextButton;
    StringArray     m_pLogStringArray;
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JuceLogComponent)
    
};

class AppLogger : public Logger
{

public :
	AppLogger(JuceLogComponent * _logComponent);
    ~AppLogger();

    void logMessage(const String& message);

    juce_UseDebuggingNewOperator

private:
	JuceLogComponent * m_pLogComponentOwner;

private:
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (AppLogger)

};

#endif  // __JUCE_LOG_WINDOW_H__
