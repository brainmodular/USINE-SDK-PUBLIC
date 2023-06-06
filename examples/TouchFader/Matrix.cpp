//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "Matrix.h"
#include "Params.h"

Matrix::Matrix()
{}

Matrix::Matrix(TFPStruct *TFParams_val)
{
	TFParams = TFParams_val;

	memset(Z_VALUES, 0, MAX_ARRAY_SIZE*sizeof(float));
	memset(X_VALUES, 0, MAX_ARRAY_SIZE*sizeof(float));
	memset(Y_VALUES, 0, MAX_ARRAY_SIZE*sizeof(float));
}

void Matrix::onMouseDown()
{
	GET_CELL();
	ON_CLIC_ROW = MX.row;
	ON_CLIC_COL = MX.col;


		if (MODE == 0 || MODE == 3) //MMT
		{
			Z_VALUES[MX.cell] = 1.0;
		}

		if (MODE == 1) //toggle
		{
			switch (TOGGLE_SUB_MODE)
			{
			case 0: Z_VALUES[MX.cell] = !Z_VALUES[MX.cell]; break; //normal
			case 1: RESET_ALL_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive
			case 2: RESET_COL_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive col
			case 3: RESET_ROW_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive row
			default:break;
			}			
		}

		if (MODE == 2 && !RELATIVE_MODE) //fade
		{
			Z_VALUES[MX.cell] = GET_LOCAL_Y();
		}

		if (MODE >= 2 && !RELATIVE_MODE) //sliders mode
		{
			Y_VALUES[MX.cell] = GET_LOCAL_Y();
			X_VALUES[MX.cell] = GET_LOCAL_X();
		}

		if (MODE == 4) //fade togg
		{
			if (Z_VALUES[MX.cell]==0)
			{
				WILL_SW_OFF = FALSE;
				Z_VALUES[MX.cell] = 1.0;
			}
			else
			{
				WILL_SW_OFF = TRUE;
			}			
			ON_CLIC_Y_VAL = Y_VALUES[MX.cell];
			ON_CLIC_X_VAL = X_VALUES[MX.cell];
			
		}


}

void Matrix::onInputChange()
{


}

void Matrix::onMouseUp()
{

	if (MODE == 0 || MODE == 3) //MMT
	{
		Z_VALUES[MX.cell] = 0;

		if (ALLOW_SLIDE_SELECTION)
		{
			memset(Z_VALUES, 0, MAX_ARRAY_SIZE*sizeof(float));
		}
	}

	if (MODE == 4 && WILL_SW_OFF)
	{
		Z_VALUES[MX.cell] = 0;
	}

	if (AUTO_RESET_Y)
	{
		Y_VALUES[MX.cell] = Y_RESET_VAL;
	}
	if (AUTO_RESET_X)
	{
		X_VALUES[MX.cell] = X_RESET_VAL;
	}


}

void Matrix::onMouseMove()
{

	int actual_cell = MX.cell;

	CHECK_SLIDE_MODE();

	if (ALLOW_SLIDE_SELECTION)
	{
		GET_CELL_MOVE();
	}

	if (MODE == 0 || MODE == 1) //mmt or toggle
	{
		if (MX.cell != actual_cell)
		{
			//	Z_VALUES[MX.cell] = !Z_VALUES[MX.cell];
			//}

			switch (TOGGLE_SUB_MODE)
			{
			case 0: Z_VALUES[MX.cell] = !Z_VALUES[MX.cell]; break; //normal
			case 1: RESET_ALL_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive
			case 2: RESET_COL_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive col
			case 3: RESET_ROW_Z_VALUES(); Z_VALUES[MX.cell] = 1.0; break; //exclusive row
			default:break;
			}
		}
	}

	else if (MODE == 2) //fade z
	{
		if (RELATIVE_MODE)
		{
			Z_VALUES[MX.cell] -= TFParams->DELTA_Y*ENCODER_COEFF_Y;
			Z_VALUES[MX.cell] = CLAMP01(Z_VALUES[MX.cell]);
		}
		else // absolute mode
		{
			Z_VALUES[MX.cell] = GET_LOCAL_Y();
		}
	}

	else if (MODE >= 3) //sliders XY
	{

		//if ((MX.row == ON_CLIC_ROW || !SLIDE_ONLY_IN_ROW) && (MX.col == ON_CLIC_COL || !SLIDE_ONLY_IN_COL))
		{

			if (RELATIVE_MODE)
			{
				Y_VALUES[MX.cell] -= TFParams->DELTA_Y*ENCODER_COEFF_Y;
				Y_VALUES[MX.cell] = CLAMP01(Y_VALUES[MX.cell]);

				X_VALUES[MX.cell] += TFParams->DELTA_X*ENCODER_COEFF_X;
				X_VALUES[MX.cell] = CLAMP01(X_VALUES[MX.cell]);
			}
			else //absolute mode
			{
				Y_VALUES[MX.cell] = GET_LOCAL_Y();
				X_VALUES[MX.cell] = GET_LOCAL_X();
			}


			if (MODE == 4) //fade tog chg check
			{
				if ((Y_VALUES[MX.cell] != ON_CLIC_Y_VAL) || (X_VALUES[MX.cell] != ON_CLIC_X_VAL))
				{
					WILL_SW_OFF = FALSE;
				}
			}
		}
	}


}

void Matrix::GET_CELL()
{
	if (TFParams->MSX < 1.0)
	{

		MX.col = (int)(TFParams->MSX * (float)MX.nb_cols);
		MX.row = (int)(TFParams->MSY * (float)MX.nb_rows);

		MX.cell = MX.col + (MX.row*MX.nb_cols);
	}
}
void Matrix::GET_CELL_MOVE()
{
	if (TFParams->MSX < 1.0)
	{

		MX.col = (int)(TFParams->MSX * (float)MX.nb_cols);
		MX.row = (int)(TFParams->MSY * (float)MX.nb_rows);

		if (SLIDE_ONLY_IN_ROW)
		{
			MX.row = ON_CLIC_ROW;
		}
		if (SLIDE_ONLY_IN_COL)
		{
			MX.col = ON_CLIC_COL;
		}

		MX.cell = MX.col + (MX.row*MX.nb_cols);
	}
}


float Matrix::GET_LOCAL_Y()
{
	float Y_local = 0;
	float row_size = 1.0f / (float)MX.nb_rows;
	float in_min = MX.row*row_size;
	float in_max = in_min + row_size;
	Y_local = 1-RE_RANGE_NORM_OUT(TFParams->MSY, in_min, in_max);
	Y_local = CLAMP01(Y_local);
	return Y_local;

}
float Matrix::GET_LOCAL_X()
{
	float X_local = 0;
	float col_size = 1.0f / (float)MX.nb_cols;
	float in_min = MX.col*col_size;
	float in_max = in_min + col_size;
	X_local = RE_RANGE_NORM_OUT(TFParams->MSX, in_min, in_max);
	X_local = CLAMP01(X_local);
	return X_local;

}
void Matrix::RESET_ALL_Z_VALUES()
{
	memset(Z_VALUES, 0, MAX_ARRAY_SIZE*sizeof(float));
}
void Matrix::RESET_COL_Z_VALUES()
{
	int ID = 0;

	for (int i = 0; i < MX.nb_rows; i++)
	{
		ID = MX.col + i*MX.nb_cols;
		Z_VALUES[ID] = 0;
	}
}
void Matrix::RESET_ROW_Z_VALUES()
{
	int ID = 0;

	for (int i = 0; i < MX.nb_cols; i++)
	{
		ID = (MX.row*MX.nb_cols) + i;
		Z_VALUES[ID] = 0;
	}
}
void Matrix::CHECK_SLIDE_MODE()
{
	switch (SLIDE_MODE)
	{
	case 0: ALLOW_SLIDE_SELECTION = FALSE; break; // none allowed
	case 1: ALLOW_SLIDE_SELECTION = TRUE; SLIDE_ONLY_IN_ROW = FALSE; SLIDE_ONLY_IN_COL = FALSE; break; // all allowed
	case 2: ALLOW_SLIDE_SELECTION = TRUE; SLIDE_ONLY_IN_ROW = FALSE; SLIDE_ONLY_IN_COL = TRUE; break; // only col
	case 3: ALLOW_SLIDE_SELECTION = TRUE; SLIDE_ONLY_IN_ROW = TRUE; SLIDE_ONLY_IN_COL = FALSE; break; // only row
	default: break;
	}

}

void Matrix::UPD_PAINT()
{}