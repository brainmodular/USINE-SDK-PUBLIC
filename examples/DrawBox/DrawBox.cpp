//-----------------------------------------------------------------------------
// @file    DrawBox.cpp
// @author  BrainModular team
// @brief   Draws a box with the mouse, shows painting and mouse interactions.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "DrawBox.h"
#include "ModuleDefinition.h"

namespace
{
	float clamp01(float value)
	{
		return std::clamp(value, 0.0f, 1.0f);
	}

	float randomUnit()
	{
		return static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
	}
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DrawBox();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DrawBox*>(pModule);
}

void DrawBox::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth       = DEFAULT_MODULE_SIZE;
	pModuleInfo->DefaultHeight      = DEFAULT_MODULE_SIZE;
	pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void DrawBox::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorBox = sdkGetUsineColor(clCursor);
	eraseBox();
}

void DrawBox::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(resetBtnInParamInfo, resetBtnIn);
	sdkCreateParam(randomizeBtnInParamInfo, randomizeBtnIn);
}

void DrawBox::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case RESET_BTN_CBID:
			eraseBox();
			break;
		case RANDOMIZE_BTN_CBID:
			onRandomize();
			break;
		default:
			break;
	}
}

void DrawBox::onCreateSettings()
{
	// adds our own section in the 'edit layout' panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "box");
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorBox, "box color");
}

void DrawBox::onSettingsHasChanged()
{
	sdkRepaintPanel();
}

void DrawBox::onCreateCommands()
{
	// a command triggers onCallBack with its callback id, like the matching button
	sdkAddCommandSeparator("cmd_edit");
	sdkAddCommand("cmd_reset", RESET_BTN_CBID);
	sdkAddCommand("cmd_randomize", RANDOMIZE_BTN_CBID);
}

void DrawBox::onPaint()
{
	if (boxCoords.top >= 0)
		sdkFillRect(boxCoords, colorBox, 0, colorBox, 0);
}

void DrawBox::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (mouseLeftButtonDown)
		updateBox(clamp01(X), clamp01(Y));

	// kept for the mouse wheel zoom, which is centered on the mouse
	lastMousePos = { .x = X, .y = Y };
}

void DrawBox::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton != mbLeft)
		return;

	anchorPoint = { .x = clamp01(X), .y = clamp01(Y) };
	boxCoords   = { .left = anchorPoint.x, .top = anchorPoint.y, .right = anchorPoint.x, .bottom = anchorPoint.y };

	mouseLeftButtonDown = true;
}

void DrawBox::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	mouseLeftButtonDown = false;
}

void DrawBox::onMouseWheel(TShiftState Shift, int WheelDelta)
{
	if (boxCoords.top < 0)
		return;

	const float boxScale = (WheelDelta > 0) ? 1.1f : 0.9f;

	boxCoords.left   = clamp01((boxCoords.left - lastMousePos.x) * boxScale + lastMousePos.x);
	boxCoords.right  = clamp01((boxCoords.right - lastMousePos.x) * boxScale + lastMousePos.x);
	boxCoords.top    = clamp01((boxCoords.top - lastMousePos.y) * boxScale + lastMousePos.y);
	boxCoords.bottom = clamp01((boxCoords.bottom - lastMousePos.y) * boxScale + lastMousePos.y);

	sdkRepaintPanel();
}

void DrawBox::onRandomize()
{
	boxCoords.left   = randomUnit();
	boxCoords.right  = boxCoords.left + (1.0f - boxCoords.left) * randomUnit();
	boxCoords.top    = randomUnit();
	boxCoords.bottom = boxCoords.top + (1.0f - boxCoords.top) * randomUnit();

	sdkRepaintPanel();
}

void DrawBox::eraseBox()
{
	boxCoords = { .left = -1.0f, .top = -1.0f, .right = -1.0f, .bottom = -1.0f };

	sdkRepaintPanel();
}

void DrawBox::updateBox(float x, float y)
{
	const auto [left, right] = std::minmax(anchorPoint.x, x);
	const auto [top, bottom] = std::minmax(anchorPoint.y, y);
	boxCoords = { .left = left, .top = top, .right = right, .bottom = bottom };

	sdkRepaintPanel();
}
