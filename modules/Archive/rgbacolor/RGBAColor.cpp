//-----------------------------------------------------------------------------
//@file  
//	RGBAColor.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the RGBAColorModule class.
//
//  A module to manipulate RGB composante of a usine color.
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
#include "RGBAColor.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new RGBAColorModule ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
    // cast is important to call the good destructor
	delete ((RGBAColorModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "RGBA Col";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "RGBA color";
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
void RGBAColorModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
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

void RGBAColorModule::onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	// default color to black
	m_colorData = sdkColorAgrb(0, 0, 0);
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void RGBAColorModule::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo) 
{
	// all parameters declared in the module class		
	switch (ParamIndex) 
	{
	// R color input
	case 0:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "R";
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

	// G color input
	case 1:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "G";
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

	// B color input
	case 2:
		pParamInfo->ParamType			= ptDataFader;
		pParamInfo->Caption				= "B";
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
void RGBAColorModule::onSetEventAddress (int ParamIndex, UsineEventPtr pEvent) 
{
	
	//Initialyse all events adress declared in your module class
	switch (ParamIndex) {

	//  R color input event adress
	case 0:
		m_fdrColorRed = pEvent;
		break;

	//  G color input event adress
	case 1:
		m_fdrColorGreen = pEvent;
		break;

	//  B color input event adress
	case 2:
		m_fdrColorBlue = pEvent;
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
void RGBAColorModule::onCallBack (UsineMessage* Message) 
{
	
	// WParam contains the Param Number
	if ( (Message->wParam >= 0) && (Message->wParam <= 3) && (Message->lParam == MSG_CHANGE) ) 
	{
		updateColorChooserFromRgba();
	}

	// color
	else if ( (Message->wParam == 4) && (Message->lParam == MSG_CHANGE) ) 
	{ 
		updateRgbaFromColorChooser();
	}
}

/*
//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// GetChunkLen : No chunk to store --> result := 0;
int RGBAColorModule::onGetChunkLen (LongBool Preset) 
{

	// compute size of the chunk
	// a int to store the number of color + the color array
	int ChunkLen = sizeof(int) + (sizeof(float) * sdkGetEvtSize(m_colColorPicker));
	
	return ChunkLen;
}

//-----------------------------------------------------------------------------
// GetChunk : No chunk to store --> result := 0;
void RGBAColorModule::onGetChunk (void* chunk, LongBool Preset) 
{
	int sizeColorArray = sdkGetEvtSize(m_colColorPicker);
	TPrecision* addrColorArray = GetEvtDataAddr(m_colColorPicker);

	// copy the color array size into the chunk
	memcpy(chunk, (void*)&sizeColorArray, sizeof(int) );
	// copy the color array content into the chunk
	memcpy((void*)((int*)chunk + 1), (void*)addrColorArray, (sizeof(float) * sizeColorArray) );
}

//-----------------------------------------------------------------------------
// SetChunk No chunk to store --> nothing to do...
void RGBAColorModule::onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset) 
{	
	int sizeColorArray;

	// extract the color array size from the chunk
	memcpy( (void*)&sizeColorArray, chunk, sizeof(int) );

	sdkSetEvtSize(m_colColorPicker, sizeColorArray);
	
	// important to get the address after the sdkSetEvtSize()
	TPrecision* addrColorArray = GetEvtDataAddr(m_colColorPicker);

	// extract the color array content from the chunk
	memcpy( (void*)addrColorArray, (void*)((int*)chunk + 1), (sizeof(float) * sizeColorArray) );

	updateRgbaFromColorChooser();
}
*/
//-------------------------------------------------------------------------
// private methodes implementation
//-------------------------------------------------------------------------

//RGB 
void RGBAColorModule::updateRgbaFromColorChooser()
{ 
	int size = sdkGetEvtSize( m_colColorPicker );
		
	sdkSetEvtSize( m_fdrColorRed, size);
	sdkSetEvtSize( m_fdrColorGreen, size);
	sdkSetEvtSize( m_fdrColorBlue, size);
	sdkSetEvtSize( m_fdrColorAlpha, size);

	for (int i = 0; i < size; i++)
	{			
		// adjust RGBA sliders if another color chosen
		m_colorData = sdkAlphaColorToColorArgb(sdkGetEvtArrayColor( m_colColorPicker, i ));
			
		sdkSetEvtArrayData( m_fdrColorRed, i, m_colorData.r );
		sdkSetEvtArrayData( m_fdrColorGreen, i, m_colorData.g );
		sdkSetEvtArrayData( m_fdrColorBlue, i, m_colorData.b );
		sdkSetEvtArrayData( m_fdrColorAlpha, i, m_colorData.a );
	}
}

//-------------------------------------------------------------------------
// ColorChooser
void RGBAColorModule::updateColorChooserFromRgba()
{
	int sizeR = sdkGetEvtSize( m_fdrColorRed );
	int sizeG = sdkGetEvtSize( m_fdrColorGreen );
	int sizeB = sdkGetEvtSize( m_fdrColorBlue );
	int sizeA = sdkGetEvtSize( m_fdrColorAlpha );
	int sizeEvent = std::min( sizeR, std::min( sizeG, std::min (sizeB, sizeA) ) );

	TPrecision colorR, colorG, colorB, colorA = 0.0f;

	sdkSetEvtSize( m_colColorPicker, sizeEvent);

	for (int i = 0; i < sizeEvent; i++)
	{
		colorR = sdkGetEvtArrayData( m_fdrColorRed, i );
		colorG = sdkGetEvtArrayData( m_fdrColorGreen, i );
		colorB = sdkGetEvtArrayData( m_fdrColorBlue, i );
		colorA = sdkGetEvtArrayData( m_fdrColorAlpha, i );
					
		// place each RGB composante in an RGB color
		m_colorData = sdkColorAgrb( colorR, colorG, colorB, colorA );

		// update the color
		sdkSetEvtArrayColor( m_colColorPicker, i, sdkColorArgbToAlphaColor(m_colorData) );
	}
}


