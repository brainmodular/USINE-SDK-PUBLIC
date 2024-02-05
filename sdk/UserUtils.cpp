///-----------------------------------------------------------------------------
///@file  
///	UserUtils.cpp
///
///@author
///	BrainModular team
///
///@brief  
///	Utilities global functions implementation
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

//----------------------------------------------------------------------------
// includes
//----------------------------------------------------------------------------
#include "UserUtils.h"
#include <math.h>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cstdint>

using namespace std;

//-----------------------------------------------------------------------------
// global color utilities functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// calculate HSL from RGB
TColorAhsl sdkColorArgbToColorAhsl(TColorArgb c1)
{
    float themin,themax,delta;
    TColorAhsl c2;

	c2.a = c1.a;

    themin = min( c1.r, min(c1.g,c1.b) );
    themax = max( c1.r, max(c1.g,c1.b) );
    delta = themax - themin;
    c2.l = (themin + themax) / 2;
    c2.s = 0;
    if (c2.l > 0 && c2.l < 1)
        c2.s = delta / (c2.l < 0.5 ? (2*c2.l) : (2-2*c2.l));
    c2.h = 0;
    if (delta > 0) {
        if (themax == c1.r && themax != c1.g)
            c2.h += (c1.g - c1.b) / delta;
        if (themax == c1.g && themax != c1.b)
            c2.h += (2 + (c1.b - c1.r) / delta);
        if (themax == c1.b && themax != c1.r)
            c2.h += (4 + (c1.r - c1.g) / delta);
        c2.h *= 60;
    }
    
    while (c2.h < 0)
        c2.h += 360;
    while (c2.h > 360)
        c2.h -= 360;

    return c2;
}

//-----------------------------------------------------------------------------
// calculate RGB from HSL, reverse of RGB2HSL()
TColorArgb sdkColorAhslToColorArgb(TColorAhsl c1)
{
    TColorArgb c2,sat,ctmp;

	c2.a = c1.a;

    while (c1.h < 0)
        c1.h += 360;
    while (c1.h > 360)
        c1.h -= 360;

    if (c1.h < 120) {
        sat.r = (120 - c1.h) / 60.0f;
        sat.g = c1.h / 60.0f;
        sat.b = 0;
    } else if (c1.h < 240) {
        sat.r = 0;
        sat.g = (240 - c1.h) / 60.0f;
        sat.b = (c1.h - 120) / 60.0f;
    } else {
        sat.r = (c1.h - 240) / 60.0f;
        sat.g = 0;
        sat.b = (360 - c1.h) / 60.0f;
    }
    sat.r = min(sat.r, 1.0f);
    sat.g = min(sat.g, 1.0f);
    sat.b = min(sat.b, 1.0f);

    ctmp.r = 2 * c1.s * sat.r + (1 - c1.s);
    ctmp.g = 2 * c1.s * sat.g + (1 - c1.s);
    ctmp.b = 2 * c1.s * sat.b + (1 - c1.s);

    if (c1.l < 0.5) {
        c2.r = c1.l * ctmp.r;
        c2.g = c1.l * ctmp.g;
        c2.b = c1.l * ctmp.b;
    } else {
        c2.r = (1 - c1.l) * ctmp.r + 2 * c1.l - 1;
        c2.g = (1 - c1.l) * ctmp.g + 2 * c1.l - 1;
        c2.b = (1 - c1.l) * ctmp.b + 2 * c1.l - 1;
    }

    return c2;
}

//-----------------------------------------------------------------------------
// RGB color to Usine color format
TUsineColor sdkColorArgbToAlphaColor( TColorArgb c1 )
{
    TUsineColor c2 = 0;

    c2 += ((uint8_t)(c1.b * 255.0f));
    c2 += ((uint8_t)(c1.g * 255.0f)) << 8;
    c2 += ((uint8_t)(c1.r * 255.0f)) << 16;
    c2 += ((uint8_t)(c1.a * 255.0f)) << 24;

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color format to RGB color
TColorArgb sdkAlphaColorToColorArgb(TUsineColor c1)
{
    TColorArgb c2;

    c2.b = ((float)( c1        & 0x000000FF)) / 255.0f;
    c2.g = ((float)((c1 >> 8)  & 0x000000FF)) / 255.0f;
    c2.r = ((float)((c1 >> 16) & 0x000000FF)) / 255.0f;
    c2.a = ((float)((c1 >> 24) & 0x000000FF)) / 255.0f;

    return c2;
}

//-----------------------------------------------------------------------------
// HSL color to Usine color format
TUsineColor sdkColorAhslToAlphaColor(TColorAhsl c1)
{
    TUsineColor c2 = sdkColorArgbToAlphaColor( sdkColorAhslToColorArgb( c1 ) );
    return c2;
}

//-----------------------------------------------------------------------------
// Usine color format to HSL color
TColorAhsl sdkAlphaColorToColorAhsl(TUsineColor c1)
{
    TColorAhsl c2 = sdkColorArgbToColorAhsl( sdkAlphaColorToColorArgb( c1 ) );
    return c2;
}

//-----------------------------------------------------------------------------
// RGB color constructor
TColorArgb sdkColorAgrb(float r, float g, float b, float a)
{
    TColorArgb c2;
    
    c2.a = a;
    c2.r = r;
    c2.g = g;
    c2.b = b;

    return c2;
}

//-----------------------------------------------------------------------------
// HSL color constructor
TColorAhsl sdkColorAhsl(float h, float s, float l, float a)
{
    TColorAhsl c2;

    c2.a = a;
    c2.h = h;
    c2.s = s;
    c2.l = l;

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color constructor
TUsineColor sdkAlphaColor(  float r, float g, float b, float a)
{
    TUsineColor c2 = 0;

    c2 = sdkColorArgbToAlphaColor(sdkColorAgrb(r, g, b, a));

    return c2;
}

//-----------------------------------------------------------------------------
// Usine color constructor
TUsineColor sdkAlphaColor( int r, int g, int b, int a)
{
    TUsineColor c2 = 0;

    c2 = sdkColorArgbToAlphaColor(sdkColorAgrb(r / 255.f, g / 255.f, b / 255.f, a / 255.f));

    return c2;
}

//-----------------------------------------------------------------------------
// global geometric utilities functions
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// TPointF constructor
TPointF sdkPointF(float x, float y)
{
    TPointF p1;

    p1.x = x;
    p1.y = y;

    return p1;
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(int X, int Y, TPointI LeftTop, TPointI RightBottom)
{
    TPointI Coords;
    Coords.x = X;	
    Coords.y = Y;	
    return sdkPointInRect(Coords, LeftTop, RightBottom);
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(TPointI Coords, TPointI LeftTop, TPointI RightBottom)
{
    return ((Coords.x >= LeftTop.x) && (Coords.y >= LeftTop.y)
        && (Coords.x < RightBottom.x)
        && (Coords.y < RightBottom.y));
}

//-----------------------------------------------------------------------------
// Test if coordinate is in a rect bound
bool sdkPointInRect(float X, float Y, TRectF rect)
{
    return ((X >= rect.left) && (Y >= rect.top) && (X < rect.right) && (Y < rect.bottom));
}

//-----------------------------------------------------------------------------
// Four control point Bezier interpolation
TPointF sdkBezier4(TPointF p1, TPointF p2, TPointF p3, TPointF p4, float mu)
{
    float mum1,mum13,mu3;
    TPointF p;

    mum1 = 1 - mu;
    mum13 = mum1 * mum1 * mum1;
    mu3 = mu * mu * mu;

    p.x = mum13*p1.x + 3*mu*mum1*mum1*p2.x + 3*mu*mu*mum1*p3.x + mu3*p4.x;
    p.y = mum13*p1.y + 3*mu*mum1*mum1*p2.y + 3*mu*mu*mum1*p3.y + mu3*p4.y;

    return(p);
}

//
float sdkBezier4(float p1, float p2, float p3, float p4, float mu)
{
    float mum1,mum13,mu3;
    float p;

    mum1 = 1 - mu;
    mum13 = mum1 * mum1 * mum1;
    mu3 = mu * mu * mu;

    p= mum13*p1 + 3*mu*mum1*mum1*p2 + 3*mu*mu*mum1*p3 + mu3*p4;

    return(p);
}

//
float sdkCoeffToDb(const float Coeff)
{
    float result = COEFF_TO_DB_MIN;

    if (Coeff > 0.0f)
        result = log10(Coeff) / LN10S20;

    return result;
}

float sdkDbToCoeff(const float Gain)
{
    float result = 0.0f;
    if (Gain == 0.0f)
        result = 1.0f;
    else if (Gain > GAIN_MIN)
        result = exp(Gain * LN10S20);
    return result;
}

//
std::string sdkFloatToString(const float Number, const int Prec, const int Width)
{
    std::ostringstream os;
    os << std::setiosflags(std::ios::fixed) << std::setprecision(Prec) << Number; // << std::setw(Width) 
    return os.str();
}

//
std::string sdkIntToString(const int Number)
{
    std::ostringstream os;
    os << Number; // << std::setw(Width) 
    return os.str();
}
