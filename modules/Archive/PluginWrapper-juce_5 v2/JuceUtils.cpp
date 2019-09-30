//-----------------------------------------------------------------------------
//@file  
//	JuceAudio.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//  Utils defs and methodes to glue juce and usine types.
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
#include "JuceUtils.h"

//----------------------------------------------------------------------------
// AudioSampleBufferFifo implementation
int AudioSampleBufferFifo::addToFifo (const float** data, int numSamples)
{
    prepareToWrite (numSamples, start1, size1, start2, size2);

	if (size1 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			buffer.copyFrom (i, start1, data[i], size1);
	}

	if (size2 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			buffer.copyFrom (i, start2, data[i] + size1, size2);
	}

    finishedWrite (size1 + size2);
    return size1 + size2;
}

// return the num of samples effectivly read from the buffer
int AudioSampleBufferFifo::delFromFifo ( float** data,int startSample, int numSamples)
{
	prepareToRead (numSamples, start1, size1, start2, size2);

	AudioSampleBuffer outputsBuffer(data, numChannels, startSample, numSamples);

	return read (outputsBuffer, start1, size1, start2, size2);
}

// return the num of samples effectivly read from the buffer
int AudioSampleBufferFifo::delFromFifo ( AudioSampleBuffer &outputsBuffer, int numSamples)
{
	prepareToRead (numSamples, start1, size1, start2, size2);

	return read (outputsBuffer, start1, size1, start2, size2);
}

int AudioSampleBufferFifo::read ( AudioSampleBuffer &outputsBuffer, int start1, int size1, int start2, int size2)
{
	if (size1 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			outputsBuffer.copyFrom (i, 0, buffer, i, start1, size1);
	}

	if (size2 > 0)
	{
		for (int i = numChannels; --i >= 0;)
			outputsBuffer.copyFrom (i, size1, buffer, i, start2, size2);
	}

	finishedRead (size1 + size2);
	return size1 + size2;
}


//==============================================================================
namespace UsineLookHelpers
{
    static void createRoundedPath (Path& p,
                                   const float x, const float y,
                                   const float w, const float h,
                                   const float cs,
                                   const bool curveTopLeft, const bool curveTopRight,
                                   const bool curveBottomLeft, const bool curveBottomRight) noexcept
    {
        const float cs2 = 2.0f * cs;

        if (curveTopLeft)
        {
            p.startNewSubPath (x, y + cs);
            p.addArc (x, y, cs2, cs2, float_Pi * 1.5f, float_Pi * 2.0f);
        }
        else
        {
            p.startNewSubPath (x, y);
        }

        if (curveTopRight)
        {
            p.lineTo (x + w - cs, y);
            p.addArc (x + w - cs2, y, cs2, cs2, 0.0f, float_Pi * 0.5f);
        }
        else
        {
            p.lineTo (x + w, y);
        }

        if (curveBottomRight)
        {
            p.lineTo (x + w, y + h - cs);
            p.addArc (x + w - cs2, y + h - cs2, cs2, cs2, float_Pi * 0.5f, float_Pi);
        }
        else
        {
            p.lineTo (x + w, y + h);
        }

        if (curveBottomLeft)
        {
            p.lineTo (x + cs, y + h);
            p.addArc (x, y + h - cs2, cs2, cs2, float_Pi, float_Pi * 1.5f);
        }
        else
        {
            p.lineTo (x, y + h);
        }

        p.closeSubPath();
    }

    static Colour createBaseColour (const Colour& buttonColour,
                                    const bool hasKeyboardFocus,
                                    const bool isMouseOverButton,
                                    const bool isButtonDown) noexcept
    {
        const float sat = hasKeyboardFocus ? 1.3f : 0.9f;
        const Colour baseColour (buttonColour.withMultipliedSaturation (sat));

        if (isButtonDown)
            return baseColour.contrasting (0.2f);
        else if (isMouseOverButton)
            return baseColour.contrasting (0.1f);

        return baseColour;
    }

    static TextLayout layoutTooltipText (const String& text, const Colour& colour) noexcept
    {
        const float tooltipFontSize = 13.0f;
        const int maxToolTipWidth = 400;

        AttributedString s;
        s.setJustification (Justification::centred);
        s.append (text, Font (tooltipFontSize, Font::bold), colour);

        TextLayout tl;
        tl.createLayoutWithBalancedLineLengths (s, (float) maxToolTipWidth);
        return tl;
    }
}

//==============================================================================
UsineLook::UsineLook()
{
}


UsineLook::~UsineLook()
{
}

//==============================================================================
void UsineLook::drawButtonBackground (Graphics& g,
                                        Button& button,
                                        const Colour& backgroundColour,
                                        bool isMouseOverButton,
                                        bool isButtonDown)
{
    const int width = button.getWidth();
    const int height = button.getHeight();

    const float outlineThickness = button.isEnabled() ? ((isButtonDown || isMouseOverButton) ? 1.2f : 0.7f) : 0.4f;
    const float halfThickness = outlineThickness * 0.5f;

    const float indentL = button.isConnectedOnLeft()   ? 0.1f : halfThickness;
    const float indentR = button.isConnectedOnRight()  ? 0.1f : halfThickness;
    const float indentT = button.isConnectedOnTop()    ? 0.1f : halfThickness;
    const float indentB = button.isConnectedOnBottom() ? 0.1f : halfThickness;

    //const Colour baseColour (UsineLookHelpers::createBaseColour (backgroundColour,
    //                                                               button.hasKeyboardFocus (true),
    //                                                               isMouseOverButton, isButtonDown)
    //                           .withMultipliedAlpha (button.isEnabled() ? 1.0f : 0.5f));

    drawButtonLozenge (g,
                      indentL,
                      indentT,
                      width - indentL - indentR,
                      height - indentT - indentB,
                      backgroundColour, outlineThickness, 3.0f,
                      true,
                      true,
                      true,
                      true);
}

//==============================================================================
void UsineLook::drawButtonLozenge (Graphics& g,
                                    const float x, const float y,
                                    const float width, const float height,
                                    const Colour& colour,
                                    const float outlineThickness,
                                    const float cornerSize,
                                    const bool flatOnLeft,
                                    const bool flatOnRight,
                                    const bool flatOnTop,
                                    const bool flatOnBottom) noexcept
{
    if (width <= outlineThickness || height <= outlineThickness)
        return;

    const int intX = (int) x;
    const int intY = (int) y;
    const int intW = (int) width;
    const int intH = (int) height;

    const float cs = cornerSize < 0 ? jmin (width * 0.5f, height * 0.5f) : cornerSize;
    const float edgeBlurRadius = height * 0.75f + (height - cs * 2.0f);
    const int intEdge = (int) edgeBlurRadius;

    Path outline;
    UsineLookHelpers::createRoundedPath (outline, x, y, width, height, cs,
                                            ! (flatOnLeft || flatOnTop),
                                            ! (flatOnRight || flatOnTop),
                                            ! (flatOnLeft || flatOnBottom),
                                            ! (flatOnRight || flatOnBottom));

    {
        //ColourGradient cg (colour.darker (0.2f), 0, y,
        //                   colour.darker (0.2f), 0, y + height, false);

        //cg.addColour (0.03, colour.withMultipliedAlpha (0.3f));
        //cg.addColour (0.4, colour);
        //cg.addColour (0.97, colour.withMultipliedAlpha (0.3f));

        //g.setGradientFill (cg);
		g.setColour (colour);
        g.fillPath (outline);
    }

    //ColourGradient cg (Colours::transparentBlack, x + edgeBlurRadius, y + height * 0.5f,
    //                   colour.darker (0.2f), x, y + height * 0.5f, true);

    //cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.5f) / edgeBlurRadius), Colours::transparentBlack);
    //cg.addColour (jlimit (0.0, 1.0, 1.0 - (cs * 0.25f) / edgeBlurRadius), colour.darker (0.2f).withMultipliedAlpha (0.3f));

    if (! (flatOnLeft || flatOnTop || flatOnBottom))
    {
        g.saveState();
        //g.setGradientFill (cg);
		g.setColour (colour);
        g.reduceClipRegion (intX, intY, intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

    if (! (flatOnRight || flatOnTop || flatOnBottom))
    {
        //cg.point1.setX (x + width - edgeBlurRadius);
        //cg.point2.setX (x + width);

        g.saveState();
        //g.setGradientFill (cg);
		g.setColour (colour);
        g.reduceClipRegion (intX + intW - intEdge, intY, 2 + intEdge, intH);
        g.fillPath (outline);
        g.restoreState();
    }

    {
        const float leftIndent = flatOnTop || flatOnLeft ? 0.0f : cs * 0.4f;
        const float rightIndent = flatOnTop || flatOnRight ? 0.0f : cs * 0.4f;

        Path highlight;
        UsineLookHelpers::createRoundedPath (highlight,
                                               x + leftIndent,
                                               y + cs * 0.1f,
                                               width - (leftIndent + rightIndent),
                                               height * 0.4f, cs * 0.4f,
                                               ! (flatOnLeft || flatOnTop),
                                               ! (flatOnRight || flatOnTop),
                                               ! (flatOnLeft || flatOnBottom),
                                               ! (flatOnRight || flatOnBottom));

        //g.setGradientFill (ColourGradient (colour.brighter (10.0f), 0, y + height * 0.06f,
        //                                   Colours::transparentWhite, 0, y + height * 0.4f, false));
        g.setColour (colour);
		g.fillPath (highlight);
    }

    //g.setColour (colour.darker().withMultipliedAlpha (1.5f));
    //g.setColour (Colours::black);
    //g.strokePath (outline, PathStrokeType (outlineThickness));
}

    