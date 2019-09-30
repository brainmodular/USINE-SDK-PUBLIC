//@file  
//	JuceLogWindow.cpp
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
#include "JuceLogWindow.h"

JuceLogWindow::JuceLogWindow(const String& strWindowName,
                             const Colour& clrWindow,
                             int nReqBtns,
                             bool bAddTodsktop )
                             : DocumentWindow (strWindowName,
                             clrWindow,
                             nReqBtns,
                             bAddTodsktop )
{
    setTitleBarTextCentred( false );
	addKeyListener(this);
    setSize(800,400);

}

JuceLogWindow::~JuceLogWindow()
{

}

void JuceLogWindow::closeButtonPressed()
{
    setVisible( false );
}

bool JuceLogWindow::keyPressed (const KeyPress & key, Component * /*originatingComponent*/)
{
	if(key == KeyPress::escapeKey)
    {
		setVisible(false);
	}
	return true;
}

// ------------------------------------------------------------

JuceLogComponent::JuceLogComponent()
{
    m_pLogTableListBox = new TableListBox("Logger", this);
	m_pLogTableListBox->setColour(ListBox::outlineColourId, Colours::white);
    m_pLogTableListBox->setOutlineThickness (1);
    m_pLogTableListBox->addMouseListener(this, true);
    m_pLogTableListBox->setMultipleSelectionEnabled(true);
    addAndMakeVisible( m_pLogTableListBox );
	m_pLogTableListBox->setColour(TableListBox::backgroundColourId, Colours::black);
	m_pLogTableListBox->setColour(TableListBox::textColourId, Colours::white);
    m_pLogTableListBox->getHeader().addColumn( "Time", 1, 70, 70, 70, TableHeaderComponent::visible | TableHeaderComponent::notResizable);
    m_pLogTableListBox->getHeader().addColumn( "Log", 2, 325, 300, 1000, TableHeaderComponent::visible | TableHeaderComponent::resizable);
	m_pLogTableListBox->getHeader().setStretchToFitActive(true);
    addAndMakeVisible (m_pSaveTextButton = new TextButton("Save") );
    m_pSaveTextButton->addListener(this);

    addAndMakeVisible (m_pClearTextButton = new TextButton("Clear") );
    m_pClearTextButton->addListener(this);
}

JuceLogComponent::~JuceLogComponent()
{
    deleteAllChildren();
}

void JuceLogComponent::resized()
{
    m_pLogTableListBox->setBounds(0, 0, getWidth(), getHeight() - 25);
    m_pSaveTextButton->setBounds(getWidth()/2 - 100, getHeight()-25, 50, 24);
    m_pClearTextButton->setBounds(getWidth()/2 , getHeight()-25, 50, 24);
}

void JuceLogComponent::paint (Graphics & g)
{
    // backGround Filling
	g.fillAll(Colours::black);
}

int JuceLogComponent::getNumRows()
{
    return m_pLogStringArray.size();
}

void JuceLogComponent::paintRowBackground (Graphics & g, int /*rowNumber*/, int /*width*/, int /*height*/, bool rowIsSelected)
{
    if(rowIsSelected)
		g.fillAll(Colours::lightblue);
}

void JuceLogComponent::paintCell (Graphics &g, int rowNumber, int columnId, int width, int height, bool /*rowIsSelected*/)
{
	g.setColour(Colours::white);
	// to show Error in log
	if(m_pLogStringArray[rowNumber].containsIgnoreCase("Error"))
    {
        g.setColour(Colours::red);
    }
    if(columnId == 1)
    {
        g.drawText(m_pLogStringArray[rowNumber].upToFirstOccurrenceOf("+", false, false ), 0 , 0, width, height, Justification::left, false);
    }
    else if(columnId == 2)
    {
        g.drawText(m_pLogStringArray[rowNumber].fromFirstOccurrenceOf("+", false, false ), 0 , 0, width, height, Justification::left, false);
    }
}

void JuceLogComponent::AddInLogArray(const String & strMessage)
{
    m_pLogStringArray.add(strMessage);

	const MessageManagerLock mmLock;
    m_pLogTableListBox->updateContent();
}

void JuceLogComponent::buttonClicked (Button* buttonThatWasClicked)
{
    if(buttonThatWasClicked == m_pSaveTextButton)
    {
		AlertWindow w ("Enter Log File Name: ","", AlertWindow::QuestionIcon);

        w.addTextEditor ("Name", Time::getCurrentTime().toString(true,false, true) );
        w.addButton ("Ok",     1, KeyPress (KeyPress::returnKey, 0, 0));
        w.addButton ("Cancel", 0, KeyPress (KeyPress::escapeKey, 0, 0));

        if (w.runModalLoop() != 0) // is they picked 'ok'
        {
			File logFile(File::getSpecialLocation (File::userApplicationDataDirectory).getFullPathName() + File::separatorString + "AuViD" + File::separatorString + "Route" + File::separatorString  + w.getTextEditorContents ("Name") + "_log.txt" );
			for(int i=0; i < m_pLogStringArray.size(); i ++)
			{
				logFile.appendText(m_pLogStringArray[i]);
				if(i != (m_pLogStringArray.size()-1))
				{
					logFile.appendText("\n");
				}
			}
		}
    }
    else if (buttonThatWasClicked == m_pClearTextButton)
    {
        m_pLogStringArray.clear();
        m_pLogTableListBox->updateContent();
    }
}



// ------------------------------------------------------------

AppLogger::AppLogger(JuceLogComponent * _logComponent) : m_pLogComponentOwner(_logComponent)
{

}

AppLogger::~AppLogger()
{

}

void AppLogger::logMessage(const String & message)
{
    String dataMessage = Time::getCurrentTime().toString(false, true, true, true) + "+" + message;
    m_pLogComponentOwner->AddInLogArray(dataMessage);
}
