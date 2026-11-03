//-----------------------------------------------------------------------------
// @file    TemplateModule.h
// @author  BrainModular team
// @brief   Complete template to start from: overrides every callback of UserModuleBase.
//          Remove the callbacks you don't need.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include "../../sdk/UserDefinitions.h"

class TemplateModule : public UserModuleBase
{
public:
	// module info, query and init
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;

	// parameters and process
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;
	void onProcessVideo() override;

	// background job, started with startJob()
	void onJobBegin() override;
	void onJobProcess() override;
	void onJobEnd() override;

	// chunks: the module state saved in workspaces, patches and presets
	int  onGetStateChunkLen(LongBool Preset) override;
	void onGetStateChunk(void* chunk, LongBool Preset) override;
	void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) override;
	void onAfterLoading() override;

	// layout, panel and contextual menu
	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onCreateCommands() override;
	void onResize(float contentWidth, float contentHeight) override;
	void onPaint() override;

	// mouse and multi touch
	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseWheel(TShiftState Shift, int WheelDelta) override;
	void onMouseMoveMulti(TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) override;
	void onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) override;
	void onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) override;

	// editor window
	void onOpenEditor() override;
	void onBringToFront() override;
	void onCloseEditor() override;
	void onResizeEditor(int width, int height) override;

	// audio setup
	void onBlocSizeChange(int BlocSize) override;
	void onSampleRateChange(double SampleRate) override;

	// global features enabled in TModuleInfo: CanRecord, CanBeRandomized, CanBeReset
	void onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z) override;
	void onRandomize() override;
	void onReset() override;
	void onSetQuickColor(TUsineColor color) override;

	// midi out devices
	void onMidiSendOut(int DeviceID, TUsineMidiCode Code) override;
	void onMidiSysexSendOut(int DeviceID, char** Sysex, int sysexSize) override;

private:
	UsineEventClass exampleTxtIn;
};
