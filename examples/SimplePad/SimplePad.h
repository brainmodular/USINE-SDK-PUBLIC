//-----------------------------------------------------------------------------
// @file    SimplePad.h
// @author  BrainModular team
// @brief   Pads moved with the mouse, shows chunks, automation recording and quick color.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

constexpr int DEFAULT_MODULE_SIZE = 134;
constexpr int PAD_OBJ_MAX         = 4;

class SimplePad : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

	int  onGetStateChunkLen(LongBool Preset) override;
	void onGetStateChunk(void* chunk, LongBool Preset) override;
	void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onCreateCommands() override;
	void onPaint() override;

	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;

	void onRandomize() override;
	void onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z) override;
	void onSetQuickColor(TUsineColor color) override;

private:
	UsineEventClass randomizeBtnIn;
	UsineEventClass posXArrOut;
	UsineEventClass posYArrOut;

	// saved in the patch with the chunk system
	std::array<TPointF, PAD_OBJ_MAX> posTab {};
	static constexpr int             CHUNK_SIZE = static_cast<int>(sizeof(TPointF) * PAD_OBJ_MAX);

	bool  mouseLeftButtonDown = false;
	int   selectedPad         = -1;
	float selectedPadOffsetX  = 0.0f;
	float selectedPadOffsetY  = 0.0f;

	// settings of the 'edit layout' panel
	TUsineColor colorPad = 0;
	int         sizePad  = 20;

	void resetPads();
	void updatePad(int padIndex, float x, float y);
	// returns the index of the pad under (x, y), or -1
	[[nodiscard]] int padHitTest(float x, float y);
};
