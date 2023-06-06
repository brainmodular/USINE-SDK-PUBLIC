// ----------------------------------------------------------------------------
// ----------------- ITEM : Controlable Item composed of different possible modules
// ----------------------------------------------------------------------------
#include "Item.h"
Item::Item()
{

}

Item::Item(TFPStruct *TFParams, TFPaint *TFPaintP)
{
	IFader = Fader(TFParams, TFPaintP);
	IButton = Button(TFParams, TFPaintP);
	//ISwitch = Switch(TFParams, TFPaintP);  //swich is replaced by a submode of button to serialize more params
	IEncoder = Encoder(TFParams, TFPaintP);
	IXYpad = XYpad(TFParams, TFPaintP);
	IRangeFader = RangeFader(TFParams,  TFPaintP);
	IListBox = ListBox(TFParams, TFPaintP);
	IMatrix = Matrix(TFParams);
}

void Item::onPreset(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.onPreset(); break;
	case MButton: IButton.onPreset(); break;
	case MSwitch: IButton.onPreset(); break;
	case MEncoder: IEncoder.onPreset(); break;
	case MXYpad: IXYpad.onPreset(); break;
	case MRangeFader: IRangeFader.onPreset(); break;
	case MListBox: IListBox.onPreset(); break;
	//case 7: IMatrix.onPreset(); break;
	default: break;
	}
}

void Item::onInputChange(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.onInputChange(); break;
	case MButton: IButton.onInputChange();  break; //mmt
	case MSwitch: IButton.onInputChange();  break; //toggle break;
	case MEncoder: IEncoder.onInputChange(); break;
	case MXYpad: IXYpad.onInputChange(); break;
	case MRangeFader: IRangeFader.onInputChange(); break;
	case MListBox: IListBox.onInputChange(); break;
//	case 7: IMatrix.onInputChange(); break;
	default: break;
	}
}

void Item::UPD_PAINT(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.UPD_PAINT(); break;
	case MButton: IButton.UPD_PAINT(); break;
	case MSwitch: IButton.UPD_PAINT(); break;
	case MEncoder: IEncoder.UPD_PAINT(); break;
	case MXYpad: IXYpad.UPD_PAINT(); break;
	case MRangeFader: IRangeFader.UPD_PAINT(); break;
	case MListBox: IListBox.UPD_PAINT(); break;
//	case 7: IMatrix.UPD_PAINT(); break;
	default: break;
	}
}

void Item::onMouseDown(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.onMouseDown(); break;
	case MButton: IButton.onMouseDown(); break;
	case MSwitch: IButton.onMouseDown(); break;
	case MEncoder: IEncoder.onMouseDown(); break;
	case MXYpad: IXYpad.onMouseDown(); break;
	case MRangeFader: IRangeFader.onMouseDown(); break;
	case MListBox: IListBox.onMouseDown(); break;
//	case 7: IMatrix.onMouseDown(); break;
	default: break;
	}
}
void Item::onMouseUp(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.onMouseUp(); break;
	case MButton: IButton.onMouseUp(); break;
	case MSwitch: IButton.onMouseUp(); break;
	case MEncoder: IEncoder.onMouseUp(); break;
	case MXYpad: IXYpad.onMouseUp(); break;
	case MRangeFader: IRangeFader.onMouseUp(); break;
	case MListBox: IListBox.onMouseUp(); break;
//	case 7: IMatrix.onMouseUp(); break;
	default: break;
	}
}
void Item::onMouseMove(TMode mode)
{
	switch (mode)
	{
	case MFader: IFader.onMouseMove(); break;
	case MButton: IButton.onMouseMove(); break;
	case MSwitch: IButton.onMouseMove(); break;
	case MEncoder: IEncoder.onMouseMove(); break;
	case MXYpad: IXYpad.onMouseMove(); break;
	case MRangeFader: IRangeFader.onMouseMove(); break;
	case MListBox: IListBox.onMouseMove(); break;
//	case 7: IMatrix.onMouseMove(); break;
	default: break;
	}
}
	
void Item::onSettingsHasChanged(TMode mode)
{
		switch (mode)
		{
		case MFader: IFader.onSettingsHasChanged(); break;
		case MButton: IButton.onSettingsHasChanged(); IButton.TOGGLE_TYPE = TTMomentary; break;
		case MSwitch: IButton.onSettingsHasChanged(); IButton.TOGGLE_TYPE = TTToggle; break;
		case MEncoder: IEncoder.onSettingsHasChanged(); break;
		case MXYpad: IXYpad.onSettingsHasChanged(); break;
		case MRangeFader: IRangeFader.onSettingsHasChanged(); break;
		case MListBox: IListBox.onSettingsHasChanged(); break;
		//case 7: IMatrix.onMouseMove(); break;
		default: break;
		}
}//

TDrawType Item::GET_PAINT_LAYER(TMode mode, int layer)
{
	switch (mode)
	{
	case MFader: return IFader.GET_PAINT_LAYER(layer); break;
	case MButton: return IButton.GET_PAINT_LAYER(layer); break;
	case MSwitch: return IButton.GET_PAINT_LAYER(layer); break;
	case MEncoder: return IEncoder.GET_PAINT_LAYER(layer); break;
	case MXYpad: return IXYpad.GET_PAINT_LAYER(layer); break;
	case MRangeFader: return IRangeFader.GET_PAINT_LAYER(layer); break;
	default: return DT_None; break;/////////////////////////////////////////
	}

}

void Item::onSkinLoad(TMode mode, int skin_nb)
{	
		switch (mode)
		{
		case MFader: IFader.PRESET_LOAD(skin_nb); break;
		case MButton: IButton.PRESET_LOAD(skin_nb); break;
		case MSwitch: IButton.PRESET_LOAD(skin_nb); break;
		case MEncoder: IEncoder.PRESET_LOAD(skin_nb); break;
		case MXYpad: IXYpad.PRESET_LOAD(skin_nb); break;
		case MRangeFader: IRangeFader.PRESET_LOAD(skin_nb); break;
		case MListBox: IListBox.PRESET_LOAD(skin_nb); break;
			//	case 7: IMatrix.onMouseDown(); break;
		default: break;
		}
	
}

void Item::onSetQuickColor(TMode mode)
{

	switch (mode)
	{
	case MFader: IFader.onSetQuickColor(); break;
	case MButton: IButton.onSetQuickColor(); break;
	case MSwitch: IButton.onSetQuickColor(); break;
	case MEncoder: IEncoder.onSetQuickColor(); break;
	case MXYpad: IXYpad.onSetQuickColor(); break;
	case MRangeFader: IRangeFader.onSetQuickColor(); break;
	case MListBox: IListBox.onSetQuickColor(); break;
		//	case 7: IMatrix.onMouseDown(); break;
	default: break;
	}



}







