//-----------------------------------------------------------------------------
// MATRIX ITEM CLASS
//-----------------------------------------------------------------------------

// include once, no more
#ifndef __MATRIX_H__
#define __MATRIX_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "TouchFader_Tools.h"
#include "../../sdk/UserDefinitions.h" 
#include "TFParamTypes.h"
//#include <vector>
#define MAX_ARRAY_SIZE 127     // max X Y Z arrays size

using namespace std;
//TFParams_struct TFParams;

class Matrix
{
public:
	//contructeur
	Matrix();
	Matrix(TFPStruct *TFParams);
	// methods
	void onMouseDown();
	void onMouseUp();
	void onMouseMove();
	void onInputChange();

	void UPD_PAINT();

	void GET_CELL();
	void GET_CELL_MOVE();
	float GET_LOCAL_Y();
	float GET_LOCAL_X();
	void RESET_ALL_Z_VALUES();
	void RESET_COL_Z_VALUES();
	void RESET_ROW_Z_VALUES();
	void CHECK_SLIDE_MODE();

	TFPStruct *TFParams;

	float Y_VALUES[MAX_ARRAY_SIZE];
	float X_VALUES[MAX_ARRAY_SIZE];
	float Z_VALUES[MAX_ARRAY_SIZE];

	int ON_CLIC_ROW = 0;
	int ON_CLIC_COL = 0;
	LongBool SLIDE_ONLY_IN_ROW = FALSE;
	LongBool SLIDE_ONLY_IN_COL = FALSE;

	// variables
	int SLIDE_MODE = 0;
	int MODE = 0;
	int TOGGLE_SUB_MODE = 0;
	float DELTA_Y = 0;
	float ENCODER_COEFF_Y = 3.0f;
	float DELTA_X = 0;
	float ENCODER_COEFF_X = 3.0f;
	bool WILL_SW_OFF = FALSE;
	float fad_val = 0.5f;
	float real_val_Y = 0.5f;
	float real_val_X = 0.5f;
	int CELLS_HUE_OFFSET = 0;

	float ON_CLIC_Y_VAL = 0;
	float ON_CLIC_X_VAL = 0;

	int NB_LIST_ITEMS = 0;
	float FADERS_OPACITY = 0.5f;
	float FADERS_LUMINOSITY = 0.2f;
	float FADERS_SATURATION = 0.25f;
	float FADERS_LINE_OPACITY = 0.5f;
	float FADERS_LINE_SIZE = 2.0f;
	LongBool SHOW_FADERS = TRUE;
	LongBool SHOW_Y_FADER = TRUE;
	LongBool SHOW_X_FADER = TRUE;
	LongBool ROUND_CELL = TRUE;
	LongBool AUTO_RESET_Y = FALSE;
	LongBool RELATIVE_MODE = TRUE;
	float Y_RESET_VAL = 0;
	float X_RESET_VAL = 0;
	LongBool AUTO_RESET_X = FALSE;
	float ROUND_SIZE = 0.03f;
	LongBool SHOW_CELL_BORDER = TRUE;
	LongBool ALLOW_SLIDE_SELECTION = TRUE;
	float BORDER_SIZE = 2.0f;
	float CELL_OFF_LUMINOSITY = 0.10f;
	float CELL_SATURATION = 0.5f;
	float CELL_OPACITY = 0.5f;

	float CELL_BORDER_LUMINOSITY = 0.25f;
	float CELL_BORDER_SATURATION = 0.5f;
	float CELL_BORDER_OPACITY = 0.5f;

	float NAME_SIZE_MULTIPLIER = 1.0f;
	float VALUE_SIZE_MULTIPLIER = 1.0f;

	float SELECTED_HILIGHT = 0.25f;
	int SELECTED_HUE_OFFSET = 0;
	float SELECTED_SAT_OFFSET = 0;

	TUsineColor ON_COLOR = 65535;
	TUsineColor OFF_COLOR = 255;

	struct T_MX
	{
		int nb_cols = 4;
		int nb_rows = 4;
		int col = 0;
		int row = 0;
		int cell = 0;
		float gap = 0.01f;
		LongBool SHOW_NAME = TRUE;
		LongBool SHOW_INDEX = FALSE;
		LongBool SHOW_X_VAL = FALSE;
		LongBool SHOW_Y_VAL = FALSE;
		LongBool SHOW_Z_VAL = FALSE;
	};
	T_MX MX;
	string MX_CELL_STRING = "";

};
#endif //__MATRIX_H__
