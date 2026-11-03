//-----------------------------------------------------------------------------
// @file    SimplePad.cpp
// @author  BrainModular team
// @brief   Pads moved with the mouse, shows chunks, automation recording and quick color.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "SimplePad.h"
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
	pModule = new SimplePad();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<SimplePad*>(pModule);
}

void SimplePad::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth       = DEFAULT_MODULE_SIZE;
	pModuleInfo->DefaultHeight      = DEFAULT_MODULE_SIZE;
	pModuleInfo->CanRecord          = TRUE;
	pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->DontProcess        = TRUE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void SimplePad::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorPad = sdkGetUsineColor(clVuMeterAudio);
	posXArrOut.setSize(PAD_OBJ_MAX);
	posYArrOut.setSize(PAD_OBJ_MAX);
	resetPads();
}

void SimplePad::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(randomizeBtnInParamInfo, randomizeBtnIn);
	sdkCreateParam(posXArrOutParamInfo, posXArrOut);
	sdkCreateParam(posYArrOutParamInfo, posYArrOut);
}

void SimplePad::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	try
	{
		switch (Message->wParam)
		{
			case RANDOMIZE_BTN_CBID:
				onRandomize();
				break;
			case POS_X_ARR_CBID:
			{
				const int size = std::min(posXArrOut.getSize(), PAD_OBJ_MAX);
				for (int i = 0; i < size; i++)
					posTab[i].x = posXArrOut.getArrayData(i);
				sdkRepaintPanel();
				break;
			}
			case POS_Y_ARR_CBID:
			{
				// the panel y axis goes down, the output goes up
				const int size = std::min(posYArrOut.getSize(), PAD_OBJ_MAX);
				for (int i = 0; i < size; i++)
					posTab[i].y = 1.0f - posYArrOut.getArrayData(i);
				sdkRepaintPanel();
				break;
			}
			default:
				break;
		}
	}
	catch (...)
	{
		sdkRaiseError(ERROR_CALLBACK);
	}
}

// same chunk for workspaces, patches and presets: the positions of the pads
int SimplePad::onGetStateChunkLen(LongBool Preset)
{
	return CHUNK_SIZE;
}

void SimplePad::onGetStateChunk(void* chunk, LongBool Preset)
{
	std::memcpy(chunk, posTab.data(), CHUNK_SIZE);
}

void SimplePad::onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset)
{
	if (chunk == nullptr || sizeInBytes <= 0)
		return;

	if (sizeInBytes < CHUNK_SIZE)
	{
		sdkRaiseError(ERROR_INVALID_CHUNK);
		return;
	}

	std::memcpy(posTab.data(), chunk, CHUNK_SIZE);
	for (int i = 0; i < PAD_OBJ_MAX; i++)
	{
		posXArrOut.setArrayData(i, posTab[i].x);
		posYArrOut.setArrayData(i, 1.0f - posTab[i].y);
	}

	sdkRepaintPanel();
}

void SimplePad::onCreateSettings()
{
	// adds our own section in the 'edit layout' panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "pad");
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorPad, "color");
	sdkAddSettingLineInteger(DESIGN_TAB_NAME, &sizePad, "size", 1, 30, scLinear, "px", 20);
}

void SimplePad::onSettingsHasChanged()
{
	sdkRepaintPanel();
}

void SimplePad::onCreateCommands()
{
	// a command triggers onCallBack with its callback id, like the matching button
	sdkAddCommandSeparator("cmd_edit");
	sdkAddCommand("cmd_randomize_position", RANDOMIZE_BTN_CBID);
}

void SimplePad::onPaint()
{
	for (const TPointF& pad : posTab)
		sdkDrawPoint(pad, colorPad, static_cast<float>(sizePad), TRUE);
}

void SimplePad::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (!mouseLeftButtonDown || selectedPad < 0)
		return;

	const float posX = clamp01(X + selectedPadOffsetX);
	const float posY = clamp01(Y + selectedPadOffsetY);
	updatePad(selectedPad, posX, posY);

	// replayed in onSetRecordedValue: X = pad index, Y and Z = pad position
	try
	{
		sdkProcessRecordAutomation(static_cast<TPrecision>(selectedPad), posX, posY);
	}
	catch (std::exception&)
	{
		sdkRaiseError(ERROR_RECORD_AUTOMATION);
	}
}

void SimplePad::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton != mbLeft)
		return;

	selectedPad         = padHitTest(clamp01(X), clamp01(Y));
	mouseLeftButtonDown = true;
}

void SimplePad::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	mouseLeftButtonDown = false;
	selectedPad         = -1;

	try
	{
		sdkStopRecordAutomation();
	}
	catch (std::exception&)
	{
		sdkRaiseError(ERROR_STOP_RECORD_AUTOMATION);
	}
}

void SimplePad::onRandomize()
{
	for (int i = 0; i < PAD_OBJ_MAX; i++)
		updatePad(i, randomUnit(), randomUnit());
}

void SimplePad::onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z)
{
	const int padIndex = static_cast<int>(X);
	if (padIndex >= 0 && padIndex < PAD_OBJ_MAX)
		updatePad(padIndex, Y, Z);
}

void SimplePad::onSetQuickColor(TUsineColor color)
{
	colorPad = color;
	sdkRepaintPanel();
}

void SimplePad::resetPads()
{
	// pads spread on the diagonal
	const float offset = 1.0f / static_cast<float>(PAD_OBJ_MAX + 1);
	for (int i = 0; i < PAD_OBJ_MAX; i++)
	{
		const float val = offset + offset * i;
		updatePad(i, val, val);
	}
}

void SimplePad::updatePad(int padIndex, float x, float y)
{
	posTab[padIndex] = { .x = x, .y = y };

	posXArrOut.setArrayData(padIndex, x);
	posYArrOut.setArrayData(padIndex, 1.0f - y);

	sdkRepaintPanel();
}

int SimplePad::padHitTest(float x, float y)
{
	// a pad is an ellipse in the panel coordinates, which go from 0 to 1 on both axes
	const float radiusX = sdkPixelToWidthPercent(static_cast<float>(sizePad)) / 2.0f;
	const float radiusY = sdkPixelToHeightPercent(static_cast<float>(sizePad)) / 2.0f;

	for (int i = 0; i < PAD_OBJ_MAX; i++)
	{
		const float distX = posTab[i].x - x;
		const float distY = posTab[i].y - y;
		if ((distX * distX) / (radiusX * radiusX) + (distY * distY) / (radiusY * radiusY) <= 1)
		{
			selectedPadOffsetX = distX;
			selectedPadOffsetY = distY;
			return i;
		}
	}
	return -1;
}
