//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

// include once, no more
#ifndef __TOUCH_FADER_TOOLS_H__
#define __TOUCH_FADER_TOOLS_H__

#include <cmath>
#include "../../sdk/UserDefinitions.h" 
#include <string>
#include <vector>
#include <iostream>
#include <iomanip> ///////////////////prec

//-----------------------------------------------------------------------------
// global
#define M2PI 6.2831853071795864f // 2pi 

//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------

enum TMode { MFader, MButton, MSwitch, MEncoder, MXYpad, MRangeFader, MListBox, MMatrix };
enum TToggleType { TTMomentary, TTToggle }; //button can be mmt or toggle

struct Tlist_item
{
	std::string name ="";
	int id = 0;
	float assigned_value = 0;
};

// functions
float RE_RANGE(float In_val, float IMin, float IMax, float OMin, float OMax);
float RE_RANGE_NORM_IN(float In_val, float OMin, float OMax);
float RE_RANGE_NORM_OUT(float In_val, float IMin, float IMax);
float CLAMP01(float val);
float CLAMP(float val, float min, float max);
bool IN_01_RANGE(float val);
TColorUsine HSLA(float H, float S, float L, float A);

void OFFSET_TPOINTF(TPointF In_Points[], int size, float X, float Y);
//TPointF* GET_CORNERS(TRectF Rect);

TPointF OFFSET_POINT(TPointF In_Point,float X, float Y);

TRectF OFFSET_RECT(TRectF In_Rect, float X, float Y);

TPointF SYM_POINT_X(TPointF in_Point);
TRectF SCALE_RECT(TRectF in_Rect, float Xscale, float Yscale, float RATIO = 1.0);



float GET_BEZIER_LINE(float P[], float t, bool mode);
float FIND_X_FOR_LINE(float Px[], float t, float precision, int it_max, bool mode);
float BEZIER_DER_LINE(float P[], float t);
float GET_S_CURVE(float tin, float mix, int mode);
float  WSHAPE_VAL(float val, float Imin, float Imax, float Ratio);

std::string VAL_TO_STRING(float in_val, int precision);
float STRING_TO_FLOAT(std::string s);

float FORMAT_FLOAT_PREC(float in_val, int precision);

void FORMAT_FLOAT_DECA(float *in_val, int precision);



bool sortByName(const Tlist_item &lhs, const Tlist_item &rhs); 


void split(const std::string &s, char delim, std::vector<std::string> &elems);
std::vector<std::string> split(const std::string &s, char delim);

float GET_DISTANCE(TPointF Pa, TPointF Pb, float canvas_ratio);

float GET_ANGLE(TPointF Pa, TPointF Pb, float  start_offset);

float APPLY_SCALE_TYPE(float inval, TScale SCALE_TYPE);
float REVERT_SCALE_TYPE(float inval, TScale SCALE_TYPE);


#endif //__TOUCH_FADER_TOOLS_H__
