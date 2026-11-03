//-----------------------------------------------------------------------------
// @file    DragAndDrop.h
// @author  BrainModular team
// @brief   Accepts the files dragged on the panel and shows the names of the dropped ones.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

#include <string>
#include <vector>

constexpr int DEFAULT_MODULE_WIDTH  = 200;
constexpr int DEFAULT_MODULE_HEIGHT = 134;

// not named DragDrop, which is the module export declared in UsineDefinitions.h
class DragAndDrop : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onPaint() override;

	void onMouseMove(TShiftState Shift, float X, float Y) override;

	LongBool onAcceptDragOver(AnsiCharPtr FilesList, float X, float Y) override;
	void onDragDrop(AnsiCharPtr FilesList, float X, float Y) override;

private:
	bool                     isDraggedOver = false;
	std::vector<std::string> droppedFileNames;

	TUsineColor colorDragOver = 0;
	TUsineColor colorText     = 0;

	void setDraggedOver(bool draggedOver);
};
