//-----------------------------------------------------------------------------
//@file  
//	VideoDimmer.cpp
//
//@author
//	Olivier SENS aka senso
//
//@brief 
//	Implementation of the VideoDimmer class.
//
//  Example user module to show how to process video flows.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
//  support@brainmodular.com 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 BrainModular
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
#include "VideoDimmer.h"

//----------------------------------------------------------------------------
// create, general info and destroy methods
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new VideoDimmer();
}

// destroy
void DestroyModule(void* pModule) 
{

	// cast is important to call the good destructor
	delete ((VideoDimmer*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "video dimmer";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "video dimmer sdk module example";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

// browser info
void GetBrowserInfo(TModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-------------------------------------------------------------------------
// module constructors/destructors
//-------------------------------------------------------------------------

// constructor
VideoDimmer::VideoDimmer()
 
{
	//
}

// destructor
VideoDimmer::~VideoDimmer()
{

}

void VideoDimmer::onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtVideo;
	pModuleInfo->BackColor          = sdkGetUsineColor(clVideoModuleColor);
	pModuleInfo->Version			= MODULE_VERSION;
	pModuleInfo->NumberOfVideoInputs  = 1;
	pModuleInfo->NumberOfVideoOutputs = 1;
	pModuleInfo->NumberOfParams       = 3;

}


//-----------------------------------------------------------------------------
// initialisation
void VideoDimmer::onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) 
{
	fdrPixelColor.setColor(0xFFFF0000);
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void VideoDimmer::onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo)
{	
    switch (ParamIndex)
	{
    // fdrDimmer
    case 0:
	
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "dimmer";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
        pParamInfo->IsSeparator     = FALSE;
        pParamInfo->CallBackType    = ctNormal;
		pParamInfo->DefaultValue    = 1;
		pParamInfo->MaxValue        = 1;
		pParamInfo->setEventClass	(fdrDimmer);
		break;

	// fdrPixelColor
	case 1:
		pParamInfo->ParamType		= ptChooseColor;
		pParamInfo->Caption			= "pixel color";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		= FALSE;
		pParamInfo->CallBackType	= ctNormal;
		pParamInfo->setEventClass	(fdrPixelColor);
		break;
	// fdrSpace
	case 2:
		pParamInfo->ParamType		= ptDataFader;
		pParamInfo->Caption			= "inter space";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->IsSeparator		= FALSE;
		pParamInfo->CallBackType	= ctNormal;
		pParamInfo->MinValue		= 0;
		pParamInfo->MaxValue		= 20;
		pParamInfo->Format			= DEFAULT_FORMAT_INTEGER;
		pParamInfo->DefaultValue	= 1;
		pParamInfo->setEventClass	(fdrSpace);
		break;
	}
}


//-----------------------------------------------------------------------------
// Parameters callback
void VideoDimmer::onCallBack (TUsineMessage *Message) 
{
// no callback
}

void VideoDimmer::onProcessVideo () 
{
	sdkGetInputFrame(1, &framei);
	sdkGetNewFrame(&frameo, framei.Width, framei.Height, FALSE);

	int skip = (int)fdrSpace.getData();
	if (skip == 0) 
	{
	   sdkCopyFrame(&framei, &frameo);
	}
	else
	{
		TUsinePixel pix = sdkColorToPixel(fdrPixelColor.getColor());
		sdkClearFrame(&frameo,0xFF000000);
		for (int h = 0; h < frameo.Height; h++)
		{
			if ((h % skip) == 0)
			{
				for (int w = 0; w < frameo.Width; w++)
				{
					// dirty job to optimize
					if ((w % skip) == 0) { sdkSetPixel(&frameo, w, h, sdkGetPixel(&framei, w, h)); }
					if ((w % skip) == 1) { sdkSetPixel(&frameo, w, h, pix); }
				}
			}
		}
	}

	sdkSetDimmerFrame(&frameo,fdrDimmer.getData());
	sdkSetOutputFrame(1, &frameo);
}
