// ----------------------------------------------------------------------------
// ----------------- ITEM : Controlable Item composed of different modules
// ----------------------------------------------------------------------------
// ----------------- Authors : 23fx23
// ----------------------------------------------------------------------------
#ifndef __ITEM_H__
#define __ITEM_H__

// ----------------------------------------------------------------------------
#include <iostream>
#include "../../sdk/UserDefinitions.h"
#include "../../sdk/UsineDefinitions.h"
#include "TouchFader_Tools.h"
#include "TFParamTypes.h"
//#include "Switch.h"
#include "Button.h"
#include "Fader.h"
#include "Encoder.h"
#include "XYpad.h"
#include "RangeFader.h"
#include "ListBox.h"
#include "Matrix.h"

using namespace std;

// ----------------------------------------------------------------------------
// ----------------------- Item : Item Class is a group of available modules
// ----------------------------------------------------------------------------
class Item
{
public:
	// Constructor
	// TODO : Asses relevence of giving this pointer everywhere
	Item();
	Item(TFPStruct *TFParams, TFPaint *TFPaintP);

	// Usine SDK UserModuleBase Item complements 
	void onPreset(TMode mode);
	void onMouseDown(TMode mode);
	void onMouseUp(TMode mode);
	void onMouseMove(TMode mode);
	void onInputChange(TMode mode);
	// TODO : Asses function use
	void UPD_PAINT(TMode mode);
	void onSettingsHasChanged(TMode mode);

	TDrawType GET_PAINT_LAYER(TMode mode, int layer);

	void onSkinLoad(TMode mode, int skin_nb);

	void onSetQuickColor(TMode mode);

	// Item Modules
	Fader IFader;
	Button IButton;
	//Switch ISwitch;
	Encoder IEncoder;
	XYpad IXYpad;
	RangeFader IRangeFader;
	ListBox IListBox;
	Matrix IMatrix;
};
// ----------------------------------------------------------------------------


#endif //__ITEM_H__
