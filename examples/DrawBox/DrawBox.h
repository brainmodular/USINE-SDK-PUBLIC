//-----------------------------------------------------------------------------
// @file    DrawBox.h
// @author  BrainModular team
// @brief   Draws a box with the mouse, shows painting and mouse interactions.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

constexpr int DEFAULT_MODULE_SIZE = 134;

class DrawBox : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onCreateCommands() override;
	void onPaint() override;

	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseWheel(TShiftState Shift, int WheelDelta) override;

	void onRandomize() override;

private:
	UsineEventClass resetBtnIn;
	UsineEventClass randomizeBtnIn;

	bool    mouseLeftButtonDown = false;
	TPointF lastMousePos        = {};
	TPointF anchorPoint         = {};

	// a negative top means there is no box to draw
	TRectF      boxCoords = {};
	TUsineColor colorBox  = 0;

	void eraseBox();
	void updateBox(float x, float y);
};
