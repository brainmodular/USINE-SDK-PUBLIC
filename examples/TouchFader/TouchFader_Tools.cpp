//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include <string>


//-----------------------------------------------------------------------------
// TouchFader_Tools Functions
//-----------------------------------------------------------------------------
float RE_RANGE(float In_val, float IMin, float IMax, float OMin, float OMax)
{
	return  (((In_val - IMin) / (IMax - IMin))*(OMax - OMin)) + OMin;
}
float RE_RANGE_NORM_IN(float In_val, float OMin, float OMax)
{
	return  (In_val *(OMax - OMin)) + OMin;
}
float RE_RANGE_NORM_OUT(float In_val, float IMin, float IMax)
{
	return  (In_val - IMin) / (IMax - IMin);
}
float CLAMP01(float val)
{
	return fminf(1, fmaxf(val, 0));
}
float CLAMP(float val, float min, float max)
{
	return fmaxf(min, fminf(max, val));
}
bool IN_01_RANGE(float val)
{
	return (val >= 0 && val <= 1.0);
}
TUsineColor HSLA(float H, float S, float L, float A)
{
	//H = fmin(360, fmax(0, H));
	H = fmodf(H, 360);
	L = CLAMP01(L);
	S = CLAMP01(S);
	A = CLAMP01(A);

	return sdkColorAhslToAlphaColor(sdkColorAhsl(H, S, L, A));
}

void OFFSET_TPOINTF(TPointF In_Points[],int size, float X,float Y)
{
	//int size = sizeof(In_Points) / sizeof(TPointF);

	//TPointF* Out_Points = new TPointF[size];

	//TPointF* Out_Points[16];

	for (int i = 0; i < size; i++)
	{
		In_Points[i].x +=  X;
		In_Points[i].y +=  Y;
	}

	//return Out_Points;

}

TPointF OFFSET_POINT(TPointF In_Point, float X, float Y)
{

	return sdkPointF(In_Point.x + X, In_Point.y + Y);
}

TRectF OFFSET_RECT(TRectF In_Rect, float X, float Y)
{
	TRectF Offseted;

	Offseted = In_Rect;

	Offseted.top += Y;
	Offseted.bottom += Y;
	Offseted.left += X;
	Offseted.right += X;

	return Offseted;
}

/* TPointF* GET_CORNERS(TRectF Rect)
{
	TPointF Corners[4];

	Corners[0].x = Rect.left;
	Corners[0].y = Rect.bottom;
	Corners[1].x = Rect.left;
	Corners[1].y = Rect.top;
	Corners[2].x = Rect.right;
	Corners[2].y = Rect.top;
	Corners[3].x = Rect.right;
	Corners[3].y = Rect.bottom;

	return Corners;

}
*/
TPointF SYM_POINT_X(TPointF in_Point)
{
	return sdkPointF(0.5f + (0.5f - in_Point.x), in_Point.y);
}

TRectF SCALE_RECT(TRectF in_Rect, float Xscale, float Yscale, float RATIO)
{
	TRectF RECT;
	RECT = in_Rect;

	float Height = (RECT.bottom - RECT.top)/2.0f;
	float Width = (RECT.right - RECT.left)/2.0f;

	float CTX = (RECT.right + RECT.left) / 2.0f;
	float CTY = (RECT.top + RECT.bottom) / 2.0f;


	float scaleY = 1-((1 - Yscale) / RATIO);


	RECT.left = CTX - Xscale*Width;
	RECT.right = CTX + Xscale*Width;
	RECT.bottom = CTY + scaleY*Height;
	RECT.top = CTY - scaleY*Height;
	

	return RECT;
}

//////////////////////////// fox' bezier wavshaper
float GET_BEZIER_LINE(float P[], float t, bool mode)
{
	float  bezier_value;
	float u = 1 - t;
	float tt = t*t;
	float uu = u*u;
	//float uuu = uu * u;
	float ttt = tt * t;
	if (!mode) //=cubic bezier
	{
		bezier_value = 3 * uu * t * P[0] + 3 * u * tt * P[1] + ttt;
	}
	else //quadratic formula
	{
		bezier_value = 2 * u * t * P[1] + tt;
	}
	return  bezier_value;
}
float FIND_X_FOR_LINE(float Px[], float t, float precision, int it_max, bool mode)
{
	int i; float x, z;
	i = 0; x = t;

	while (i < it_max)
	{
		z = GET_BEZIER_LINE(Px, x, mode) - t;
		if (fabs(z) < precision)
		{
			break;
		}
		else
		{
			x = x - (z / BEZIER_DER_LINE(Px, x));
			x = fmaxf(0, fminf(0.9999f, x));
			i++;
		}
	}
	return x;
}
float BEZIER_DER_LINE(float P[], float t)
{
	float A, B, C;
	float bezier_der_value;
	C = 3 * P[0];   B = 3 * (P[1] - P[0]) - C;     A = 1 - C - B;
	bezier_der_value = C + (t * (2 * B + t * 3 * A));
	return bezier_der_value;
}

float GET_S_CURVE(float tin, float mix, int mode)
{
	if (mode < 3)  /// bezier mappings
	{
		float PAx[2], PAy[2], PBx[2], PBy[2];  //stoed tans of p1 & p2   p0 being0 and p3= 1
		float PCx[2], PCy[2];
		switch (mode)
		{                                //power 
		case 0: mix = 1 - mix;
			PAx[0] = 1; PAx[1] = 0;
			PAy[0] = 0; PAy[1] = 1;
			PBx[0] = 0; PBx[1] = 1;
			PBy[0] = 1; PBy[1] = 0;
			break;
		case 1: PAx[0] = 0; PAx[1] = 0; //log
			PAy[0] = 1; PAy[1] = 1;
			PBx[0] = 1; PBx[1] = 1;
			PBy[0] = 0; PBy[1] = 0;
			break;
		case 2: PAx[0] = 0; PAx[1] = mix;  // more round log
			PAy[0] = 1 - mix; PAy[1] = 1;
			PBx[0] = mix; PBx[1] = 1;
			PBy[0] = 0; PBy[1] = 1 - mix;
			break;
		default:
			break;
		}

		for (int i = 0; i < 2; i++)
		{
			PCx[i] = (PAx[i] * (1 - mix)) + (PBx[i] * mix);		PCy[i] = (PAy[i] * (1 - mix)) + (PBy[i] * mix);
		}
		return GET_BEZIER_LINE(PCy, FIND_X_FOR_LINE(PCx, tin, 0.001f, 10, FALSE), FALSE);
	}// mode0-3 bezier
	//-------------------------------------
	else if (mode == 3) //freg power
	{
		float AMT = 0;
		if (mix > 0.5f){ mix = (mix - 0.5f) * 2; AMT = mix * 10; }
		else if (mix < 0.5f){ mix = (mix * 2);       AMT = -(1 - mix)*0.9f; }

		float y = 0;
		float x = tin;
		x = x * 2;
		if (x < 1){ y = pow(x, 1 + AMT); }
		else if (x > 1)
		{
			x = 2 - x;
			y = pow(x, 1 + AMT);
			y = 2 - y;
		}
		y = y / 2;
		return y;
	}
	//-------------------------------------
	else if (mode == 4) //formula log
	{
		float y = 0;
		float x = tin;
		float AMT = 0;
		if (mix > 0.5f){ mix = (mix - 0.5f) * 2; AMT = mix * 10; }
		else if (mix < 0.5f){ mix = (mix * 2);       AMT = -(1 - mix)*0.9f; }

		y = pow(x, 1 + AMT);
		return y;
	}
	//-----------------------------------------------
	else if (mode == 5) //PWM style
	{
		float y = 0;
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix>0){ mix = 1 - mix;  y = fminf(1, tin / fmaxf(0.01f, mix)); }
		else if (mix < 0){ mix = fabs(mix); y = fminf(1, fmaxf(0, tin - fminf(mix, 0.99f)) / (1 - fminf(mix, 0.99f))); }
		return y;
	}
	//---------------------------
	else if (mode == 6) //PWM SYNCstyle
	{
		float y = 0;
		//float a = 0;
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix>0){ mix = 1 - mix;  y = fmodf(tin / fmaxf(0.01f, mix), 1.0f); }

		else if (mix < 0){
			mix = 1 - fabs(mix);
			y = 1 - tin;
			y = fmodf(y / fmaxf(0.01f, mix), 1.0f);
			y = 1 - y;
		}
		return y;
	}
	//---------------------------
	else if (mode == 7) //PWM SYNCstyle strecth
	{
		float y = 0;
		//float a = 0;
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix>0){
			mix = 1 - mix;
			if (tin < mix){ y = fmodf(tin / fmaxf(0.01f, mix), 1.0f); }
			else{ y = tin; }
		}

		else if (mix < 0){
			mix = 1 - fabs(mix);
			y = 1 - tin;
			if (y < mix)
			{
				y = fmodf(y / fmaxf(0.01f, mix), 1.0f);
				y = 1 - y;
			}
			else{ y = tin; }
		}
		return y;
	}
	//---------------------------
	else if (mode == 8) //PWM SYNCstyle MIRROR
	{
		float y = 0;
		//float a = 0;
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix>0){
			mix = 1 - mix;
			y = tin / fmaxf(0.01f, mix);
			if ((int)y % 2 == 0){ y = fmodf(y, 1.0f); }
			else{ y = 1 - fmodf(y, 1.0f); }
		}

		else if (mix < 0){
			mix = 1 - fabs(mix);
			y = 1 - tin;
			y = y / fmaxf(0.01f, mix);
			if ((int)y % 2 == 0){ y = fmodf(y, 1.0); y = 1 - y; }
			else { y = fmodf(y, 1.0f); }
		}
		return y;
	}
	//---------------------------
	else if (mode == 9) //Clamp Muliply
	{
		float y = 0;
		mix = (mix * 2) - 1;
		if (mix >0){ y = (fminf(1, tin * (1 - mix))); }
		else if (mix < 0){ y = fminf(1, tin + ((1 - tin)*fabs(mix))); }
		else if (mix == 0){ y = tin; }
		return y;
	}
	//------------------------------------------
	else if (mode == 10)   // DUAL stretch
	{
		float y = tin;
		float a = 0;
		float b = 0;
		a = y   * (1 / mix);
		b = (1 - y)*(1 / (1 - mix));
		if (tin < mix){ y = a; }
		else if (tin > mix){ y = 1 - b; }
		//y = fminf(1, fmaxf(0, y));
		return y;
	}
	//-----------------------------------
	else if (mode == 11)   // DUAL stretch mirror
	{
		float y = tin;
		float a = 0;
		float b = 0;
		a = y   * (1 / mix);
		b = (1 - y)*(1 / (1 - mix));
		if (tin < mix){ y = a; }
		else if (tin > mix){ y = b; }
		return y;
	}
	//-----------------------------------
	else if (mode == 12)   // Fractalize A
	{
		float y = tin;
		float b = 0;
		float maxfrac = (mix*8.0f);
		float a = y;

		b = fmodf((y * maxfrac), 1.0f);
		y = (a*1.0f) - (b / 3.0f);
		y = fminf(1, fmaxf(0, y));
		return y;
	}
	//----------------------------------
	else if (mode == 13)   // Fractalize B
	{
		float y = tin;
		float b = 0;
		float maxfrac = (mix*8.0f);
		float a = y;

		b = fmodf((y * maxfrac), 1.0f);
		y = (a*1.0f) + (b / 3.0f);
		y = fminf(1, fmaxf(0, y));
		return y;
	}
	//----------------------------------
	else if (mode == 14)   // Fractalize C mirror
	{
		float y = tin;
		float b = 0;
		float maxfrac = (mix*8.0f);
		float a = y;
		b = y*maxfrac;
		if ((int)b % 2 == 0){ b = fmodf(b, 1.0f); }
		else{ b = 1 - fmodf(b, 1.0f); }
		y = a - (b / 2.0f);
		y = fminf(1, fmaxf(0, y));
		return y;
	}
	//----------------------------------
	else if (mode == 15) // wavshaper log
	{
		float y = tin;
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix > 0){ mix = pow(fabs(mix), 5)*200.0f; }
		else if (mix < 0){ mix = mix*0.99f; }
		y = pow(y, 1 + mix);
		y = (fmin(y, 1.0f));
		return y;
	}
	//------------------------------------
	else if (mode == 16) // wavshaper power
	{
		float y = tin;
		float val_sign = 0;
		y = (y * 2) - 1;
		if (y > 0){ val_sign = 1; }
		else if (y < 0){ val_sign = -1; }
		else if (y == 0){ val_sign = 0; }
		mix = 1 - mix;
		mix = (mix * 2) - 1;
		if (mix > 0){ mix = pow(fabs(mix), 5)*200.0f; }
		else if (mix < 0){ mix = mix*0.99f; }
		y = pow(fabs(y), 1 + mix)*val_sign;
		y = (y / 2.0f) + 0.5f;
		y = (fminf(fmaxf(y, 0), 1.0f));
		return y;
	}
    return 0;
}


float WSHAPE_VAL(float val, float Imin, float Imax, float Ratio)
{
	val = val *(1.0f / Imax);
	val = (pow(val, Ratio));

	return (val*Imax);
}


bool sortByName(const Tlist_item &lhs, const Tlist_item &rhs)
{
	return lhs.name < rhs.name;
};

std::string VAL_TO_STRING(float in_val, int precision)
{
	return sdkFloatToString(in_val, precision, 0);
}

float STRING_TO_FLOAT(std::string s)
{

	return std::strtof((s).c_str(), 0);
}

void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss;
	ss.str(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}


std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}


float FORMAT_FLOAT_PREC(float in_val, int precision)
{
	// formats float to int, .x .xx .xxx ect higher cpu load than below func

	int mult = (int)pow(10, precision);

		in_val *= mult;
		in_val = round(in_val);
		in_val /= mult;

		return in_val;

}

void FORMAT_FLOAT_DECA(float *in_val, int deca_prec)
{
	//use pointer instead of return value and get prepared deca prec to save pow computation

	if (deca_prec == 1)  //if int prec only needs a rounding
	{
		*in_val = round(*in_val);
	}
	else
	{
		*in_val *= deca_prec;
		*in_val = round(*in_val);
		*in_val /= deca_prec;
	}
}

float GET_DISTANCE(TPointF Pa, TPointF Pb, float canvas_ratio)
{
	return fabs((sqrt((pow((Pa.x - Pb.x), 2)*(canvas_ratio * 2)) + pow((Pa.y - Pb.y), 2))));
} //Computes distance btwn pt a and b

float GET_ANGLE(TPointF Pa, TPointF Pb, float  start_offset)
{
	float angle;
	angle = float((atan2(Pb.y - Pa.y, Pb.x - Pa.x)) + PI) / (2 * PI);

	angle = fmodf(angle + start_offset, 1.0);

	return angle;
}


float APPLY_SCALE_TYPE(float inval, TScale SCALE_TYPE)
{
	switch (SCALE_TYPE)
	{
	case scLinear: return inval;
		break;
	case scLog: return pow(inval, 5);
		break;
	case scExp: return pow(inval, 1.0f / 3.0f);
		break;
	default:
        return inval;
		break;
	}

}
float REVERT_SCALE_TYPE(float inval, TScale SCALE_TYPE)
{


	switch (SCALE_TYPE)
	{
	case scLinear: return inval;
		break;
	case scLog: return pow(inval, 1.0f / 5.0f);
		break;
	case scExp: return pow(inval,  3.0f);
		break;
	default:
        return inval;
        break;
	}

}



