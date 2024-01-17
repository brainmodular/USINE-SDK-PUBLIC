///-----------------------------------------------------------------------------
///@file UserUtils.h
///
///@author
///	BrainModular team
///
///@brief  
///	Utilities global functions
///
///@HISTORIC 
///	2013/05/15
///    first release for Hollyhock CPP SDK 6.00.226 
///
///@IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///  Please, report bugs and patch to Usine forum :
///  support@brainmodular.com 
///
/// All dependencies are under there own licence.
///
///@LICENSE
/// Copyright (C) 2023 BrainModular, BeSpline, Adamson
///
///-----------------------------------------------------------------------------

// include once, no more
#ifndef _USERUTILS_H_
#define _USERUTILS_H_

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "UsineDefinitions.h"
#include <string>
#include <cmath>

// TODO : std::atanf is used on PC
const float PI = std::atan(1.0f)*4;
const float COEFF_TO_DB_MIN = -200;
const float GAIN_MIN = -80; // db
const float LN10S20 = 0.115129254f;

const int MID_NOTE_ON   = 144;
const int MID_VOL_RANGE = 127;

/// @addtogroup Globals
/// @{

//-----------------------------------------------------------------------------
// global color utilities functions
//-----------------------------------------------------------------------------
/// Various helpers around colors.

/// Calculates HSL from RGB.
/// @param Hue is in degrees,
/// @param Lightness is between 0 and 1,
/// @param Saturation is between 0 and 1.
TColorAhsl sdkColorArgbToColorAhsl (TColorArgb c1);

/// Calculate RGB from HSL, reverse of RGB2HSL()
/// @param Hue is in degrees
/// @param Lightness is between 0 and 1
/// @param Saturation is between 0 and 1
TColorArgb sdkColorAhslToColorArgb (TColorAhsl c1);

/// RGB color to Usine color format
TUsineColor sdkColorArgbToAlphaColor (TColorArgb c1);

/// Usine color format to RGB color
TColorArgb sdkAlphaColorToColorArgb (TUsineColor c1);

/// HSL color to Usine color format
TUsineColor sdkColorAhslToAlphaColor (TColorAhsl c1);

/// Usine color format to HSL color
TColorAhsl sdkAlphaColorToColorAhsl (TUsineColor c1);

/// RGB color constructor
TColorArgb sdkColorAgrb (float r, float g, float b, float a=1.0);

/// HSL color constructor
/// @param h : float between 0 and 360
/// @param s : float between 0 and 1
/// @param l : float between 0 and 1
/// @return TColorHSL : the new created color
TColorAhsl sdkColorAhsl (float h, float s, float l, float a=1.0);

/// Usine color constructor
/// @param r : float between 0 and 1
/// @param g : float between 0 and 1
/// @param b : float between 0 and 1
/// TUsineColor : the new created color
TUsineColor sdkAlphaColor (float r, float g, float b, float a=1.0 );

/// Usine color constructor
/// @param r : int between 0 and 255
/// @param g : float between 0 and 255
/// @param b : float between 0 and 255
/// TUsineColor : the new created color
TUsineColor sdkAlphaColor (int r, int g, int b, int a=255 );

//-----------------------------------------------------------------------------
// global geometric utilities functions
//-----------------------------------------------------------------------------

/// Construct a TPointF.
/// @param x x coordinates
/// @param y y coordinates
/// @return The new point.
TPointF sdkPointF(float x, float y);

/// Test if coordinate is in a rect bound
/// @param X x coordinates to test
/// @param Y y coordinates to test
/// @param LeftTop Left top coordinates of the rect bound
/// @param RightBottom Right bottom coordinates of the rect bound.
/// @return true if is in rect
bool sdkPointInRect (int X, int Y, TPointI LeftTop, TPointI RightBottom);

/// Test if coordinate is in a rect bound
/// @param Coords : coordinates to test
/// @param LeftTop : left top coordinates of the rect bound
/// @param RightBottom : right bottom coordinates of the rect bound
bool sdkPointInRect (TPointI Coords, TPointI LeftTop, TPointI RightBottom);

bool sdkPointInRect (float X, float Y, TRectF rect);

/// Four control point Bezier interpolation
/// @param mu ranges from 0 to 1, start to end of curve
TPointF sdkBezier4 (TPointF p1, TPointF p2, TPointF p3, TPointF p4, float mu);

/// Four control float Bezier interpolation
/// @param mu ranges from 0 to 1, start to end of curve
float sdkBezier4 (float p1, float p2, float p3, float p4, float mu);

//-----------------------------------------------------------------------------
// global numeric utilities functions
//-----------------------------------------------------------------------------
float sdkCoeffToDb (const float Coeff);
float sdkDbToCoeff (const float Gain);
inline float sqr(float x)
{
    return x*x;
}



std::string sdkFloatToString (const float Number, const int Prec=0, const int Width=0);
std::string sdkIntToString (const int Number);

/// @}
    
//-----------------------------------------------------------------------------
#if (defined (USINE_OSX32) || defined (USINE_OSX64))
//-----------------------------------------------------------------------------
void initExceptionHandler();
//-----------------------------------------------------------------------------
#endif
//-----------------------------------------------------------------------------

#endif // _USERUTILS_H_
