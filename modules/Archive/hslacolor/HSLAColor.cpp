//-----------------------------------------------------------------------------
//@file  
//	HSLAColor.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the HSLAColorModule class.
//
//  A module to manipulate HSL composante of a usine color.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
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
#include "HSLAColor.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent) 
{
	pModule = new HSLAColorModule ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((HSLAColorModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "HSLA Col";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "HSLA color";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module description
void HSLAColorModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	//identification of the module
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->NumberOfParams     = 5;
	pModuleInfo->DontProcess		= TRUE;
	pModuleInfo->QueryString		= "";
	pModuleInfo->QueryListValues	= "";
	pModuleInfo->QueryDefaultIdx	= 1;
	pModuleInfo->Version			= MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// query system and init methodes
//-----------------------------------------------------------------------------
// query system not used

//-----------------------------------------------------------------------------
// initialisation
void HSLAColorModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	// default color to black
	m_colorData = sdkColorAhsl(0, 0, 0);
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void HSLAColorModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{	
	switch (ParamIndex) 
	{
	// for all parameters declared in the module class

	// H color input
	case 0:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "H";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->Scale				= scLinear;
		pParamInfo->MinValue			= 0.0f;
		pParamInfo->MaxValue			= 12.0f;
		pParamInfo->DefaultValue		= 0.0f;
		pParamInfo->Symbol				= "";
		pParamInfo->Format				= "%.2f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
		break;

	// S color input
	case 1:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "S";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->Scale				= scLinear;
		pParamInfo->MinValue			= 0.0f;
		pParamInfo->MaxValue			= 1.0f;
		pParamInfo->DefaultValue		= 0.0f;
		pParamInfo->Symbol				= "";
		pParamInfo->Format				= "%.2f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
		break;

	// L color input
	case 2:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "L";
		pParamInfo->IsInput				= TRUE;
		pParamInfo->IsOutput			= TRUE;
		pParamInfo->Scale				= scLinear;
		pParamInfo->MinValue			= 0.0f;
		pParamInfo->MaxValue			= 2.0f;
		pParamInfo->DefaultValue		= 0.0f;
		pParamInfo->Symbol				= "";
		pParamInfo->Format				= "%.2f";
		pParamInfo->ReadOnly			= FALSE;
		pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType		= ctImmediate;
		break;

	// m_fdrColorAlpha
	case 3:
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "A";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->Scale			= scLinear;
		pParamInfo->MinValue		= 0.0f;
		pParamInfo->MaxValue		= 1.0f;
		pParamInfo->DefaultValue	= 1.0f;
		pParamInfo->Symbol			= "";
		pParamInfo->Format			= "%.2f";
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->CallBackType    = ctImmediate;
		break;

	// choosen color
	case 4:
		pParamInfo->ParamType		= ptChooseColor;
		pParamInfo->Caption			= "color";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->Color			= sdkAlphaColor(0, 0, 0);
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->CallBackType    = ctImmediate;
		break;

	// default case
	default:
		break;

	}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void HSLAColorModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
	switch (ParamIndex) 
	{
	//  R color input event adress
	case 0:
		m_fdrColorHue = pEvent;
		break;

	//  G color input event adress
	case 1:
		m_fdrColorSat = pEvent;
		break;

	//  B color input event adress
	case 2:
		m_fdrColorLum = pEvent;
		break;
		
	//  A color input event adress
	case 3:
		m_fdrColorAlpha = pEvent;
		break;

	//  color output event adress
	case 4:
		m_colColorPicker = pEvent;
		break;

	// default case
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void HSLAColorModule::onCallBack (UsineMessage* Message) 
{
	// WParam contains the Param Number
	if ( (Message->wParam >= 0) && (Message->wParam <= 3) && (Message->lParam == MSG_CHANGE) ) 
	{
		updateColorChooserFromHsla();
	}

	// color
	else if ( (Message->wParam == 4) && (Message->lParam == MSG_CHANGE) ) 
	{
		updateHslaFromColorChooser();
	}
}

/*
//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// 
int HSLAColorModule::onGetChunkLen (LongBool Preset) 
{
	// compute size of the chunk
	// a int to store the number of color + the color array
	int ChunkLen = sizeof(int) + (sizeof(float) * sdkGetEvtSize(m_colColorPicker));
	
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// 
void HSLAColorModule::onGetChunk (void* chunk, LongBool Preset) 
{
	int sizeColorArray		= sdkGetEvtSize(m_colColorPicker);
	float* addrColorArray	= GetEvtDataAddr(m_colColorPicker);

	// copy the color array size into the chunk
	memcpy(chunk, (void*)&sizeColorArray, sizeof(int) );
	// copy the color array content into the chunk
	memcpy((void*)((int*)chunk + 1), (void*)addrColorArray, (sizeof(float) * sizeColorArray) );
}

//-----------------------------------------------------------------------------
//
void HSLAColorModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) 
{
	int sizeColorArray;

	// extract the color array size from the chunk
	memcpy( (void*)&sizeColorArray, chunk, sizeof(int) );

	sdkSetEvtSize(m_colColorPicker, sizeColorArray);
	
	// important to get the address after the sdkSetEvtSize()
	float* addrColorArray = GetEvtDataAddr(m_colColorPicker);

	// extract the color array content from the chunk
	memcpy( (void*)addrColorArray, (void*)((int*)chunk + 1), (sizeof(float) * sizeColorArray) );

	updateHslaFromColorChooser();
}
*/

//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------
// HSL 
void HSLAColorModule::updateHslaFromColorChooser()
{
	int size = sdkGetEvtSize( m_colColorPicker );
		
	sdkSetEvtSize( m_fdrColorHue, size);
	sdkSetEvtSize( m_fdrColorSat, size);
	sdkSetEvtSize( m_fdrColorLum, size);
	sdkSetEvtSize( m_fdrColorAlpha, size);

	// besause of the way colors are coded in events, 
	// we should avoid casting to float, thus use void pointer
	for (int i = 0; i < size; i++)
	{			
		// adjust HSLA sliders if another color chosen
		//TColorUsine tmp = sdkGetEvtArrayColor( m_colColorPicker, i );
		m_colorData = sdkAlphaColorToColorAhsl( sdkGetEvtArrayColor( m_colColorPicker, i ));
			
		sdkSetEvtArrayData( m_fdrColorHue, i, m_colorData.h  / 30.0f);
		sdkSetEvtArrayData( m_fdrColorSat, i, m_colorData.s );
		sdkSetEvtArrayData( m_fdrColorLum, i, m_colorData.l * 2.0f );
		sdkSetEvtArrayData( m_fdrColorAlpha, i, m_colorData.a );
	}
}

//-------------------------------------------------------------------------
// ColorChooser
void HSLAColorModule::updateColorChooserFromHsla()
{
	int sizeH = sdkGetEvtSize( m_fdrColorHue );
	int sizeS = sdkGetEvtSize( m_fdrColorSat );
	int sizeL = sdkGetEvtSize( m_fdrColorLum );
	int sizeA = sdkGetEvtSize( m_fdrColorAlpha );
	int sizeEvent = std::min( sizeH, std::min( sizeS, std::min (sizeL, sizeA) ) );

	float colorH, colorS, colorL, colorA = 0.0f;

	sdkSetEvtSize( m_colColorPicker, sizeEvent);

	for (int i = 0; i < sizeEvent; i++)
	{
		colorH = sdkGetEvtArrayData( m_fdrColorHue, i );
		colorS = sdkGetEvtArrayData( m_fdrColorSat, i );
		colorL = sdkGetEvtArrayData( m_fdrColorLum, i );
		colorA = sdkGetEvtArrayData( m_fdrColorAlpha, i );
				
		// place each HSL composante in an HSL color
		m_colorData = sdkColorAhsl( colorH * 30.0f, colorS, colorL / 2.0f, colorA );

		// update the color
		sdkSetEvtArrayColor( m_colColorPicker, i,  sdkColorAhslToAlphaColor(m_colorData) );
	}
}

