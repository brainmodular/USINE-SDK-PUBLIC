//-----------------------------------------------------------------------------
// @file    VideoDimmer.cpp
// @author  Olivier Sens (senso), BrainModular team
// @brief   Dims a video and draws a grid on it, shows how to process video frames.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "VideoDimmer.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new VideoDimmer();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<VideoDimmer*>(pModule);
}

void VideoDimmer::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType           = mtVideo;
	pModuleInfo->BackColor            = sdkGetUsineColor(clVideoModuleColor);
	pModuleInfo->NumberOfVideoInputs  = 1;
	pModuleInfo->NumberOfVideoOutputs = 1;
}

void VideoDimmer::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pixelColIn.setColor(0xFFFF0000);
}

void VideoDimmer::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(dimmerFdrInParamInfo, dimmerFdrIn);
	sdkCreateParam(pixelColInParamInfo, pixelColIn);
	sdkCreateParam(spaceFdrInParamInfo, spaceFdrIn);
}

// called in the video thread instead of onProcess, because ModuleType is mtVideo
void VideoDimmer::onProcessVideo()
{
	// local frames: an input frame, and a frame of sdkGetNewFrame that is not permanent, are valid during this
	// call only. Usine then hands them to other modules and gives their pixels back to the system when they stay
	// unused, so never keep them, or their Pixels, for a later call: copy the picture into a permanent frame instead
	TUsineFrame frameIn;
	TUsineFrame frameOut;

	sdkGetInputFrame(1, &frameIn);
	// without a picture on the input, frameIn is not Valid and its size is 0 x 0: frameOut is then an empty frame,
	// which tells the next modules that there is no picture (the copy, the clear and the loops below do nothing)
	sdkGetNewFrame(&frameOut, frameIn.Width, frameIn.Height, FALSE);

	const int skip = static_cast<int>(spaceFdrIn.getData());
	if (skip == 0)
	{
		sdkCopyFrame(&frameIn, &frameOut);
	}
	else
	{
		// keeps one pixel out of skip, followed by a pixel of the grid color, the rest is black (frameOut may be a
		// frame used before: the clear writes every pixel the loops leave)
		const TUsinePixel pix = sdkColorToPixel(pixelColIn.getColor());
		sdkClearFrame(&frameOut, 0xFF000000);
		for (int h = 0; h < frameOut.Height; h += skip)
		{
			for (int w = 0; w < frameOut.Width; w++)
			{
				if ((w % skip) == 0)
					sdkSetFramePixel(&frameOut, w, h, sdkGetFramePixel(&frameIn, w, h));
				else if ((w % skip) == 1)
					sdkSetFramePixel(&frameOut, w, h, pix);
			}
		}
	}

	sdkSetDimmerFrame(&frameOut, dimmerFdrIn.getData());
	sdkSetOutputFrame(1, &frameOut);
}
