//-----------------------------------------------------------------------------
// @file    DataOscilloscope.cpp
// @author  BrainModular team
// @brief   Draws a scrolling curve of a data input in the module panel.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DataOscilloscope.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DataOscilloscope();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DataOscilloscope*>(pModule);
}

void DataOscilloscope::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth       = DEFAULT_MODULE_SIZE;
	pModuleInfo->DefaultHeight      = DEFAULT_MODULE_SIZE;
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void DataOscilloscope::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorBox = sdkGetUsineColor(clCursor);
	eraseBox();
}

void DataOscilloscope::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(yFdrInParamInfo, yFdrIn);
	sdkCreateParam(speedFdrInParamInfo, speedFdrIn);
	sdkCreateParam(boxColInParamInfo, boxColIn);
}

void DataOscilloscope::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case BOX_COL_CBID:
			colorBox = boxColIn.getColor();
			break;
		case Y_FDR_CBID:
			yValue = yFdrIn.getData();
			break;
		case SPEED_FDR_CBID:
			moveOffset = speedFdrIn.getData();
			break;
		default:
			break;
	}
}

void DataOscilloscope::onProcess()
{
	if (sdkRefreshSpeedClock())
		advance();
}

void DataOscilloscope::onCreateSettings()
{
	// adds our own section in the 'edit layout' panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorBox, "box color");
}

void DataOscilloscope::onSettingsHasChanged()
{
	sdkRepaintPanel();
}

void DataOscilloscope::onCreateCommands()
{
	sdkAddCommandSeparator("cmd_edit");
}

void DataOscilloscope::onPaint()
{
	sdkDrawPathStart();
	for (size_t i = 1; i < points.size(); i++)
	{
		sdkDrawPathMoveTo(points[i - 1]);
		sdkDrawPathLineTo(points[i]);
	}
	sdkDrawPathClose();
	sdkDrawPathDraw(colorBox, 0.5f);
}

void DataOscilloscope::eraseBox()
{
	points.clear();
	sdkRepaintPanel();
}

void DataOscilloscope::advance()
{
	points.push_back({ .x = 0.0f, .y = 1.0f - yValue });

	const TPrecision offset = moveOffset;
	for (auto& p : points)
		p.x += offset;

	// drop the points that scrolled out of the panel
	std::erase_if(points, [](const TPointF& p) { return p.x >= 1.0f; });

	sdkRepaintPanel();
}
