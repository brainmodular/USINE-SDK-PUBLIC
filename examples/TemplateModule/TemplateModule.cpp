//-----------------------------------------------------------------------------
// @file    TemplateModule.cpp
// @author  BrainModular team
// @brief   Complete template to start from: overrides every callback of UserModuleBase.
//          Remove the callbacks you don't need.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "TemplateModule.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TemplateModule();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<TemplateModule*>(pModule);
}

void TemplateModule::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType     = mtSimple;
	pModuleInfo->BackColor      = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess    = FALSE;
}

void TemplateModule::onAfterQuery(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2) {}

void TemplateModule::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(exampleTxtInParamInfo, exampleTxtIn);
}

void TemplateModule::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) {}

void TemplateModule::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	// Message->wParam holds the callback id of the parameter that changed
}

void TemplateModule::onProcess() {}
void TemplateModule::onProcessVideo() {}

void TemplateModule::onJobBegin() {}
void TemplateModule::onJobProcess() {}
void TemplateModule::onJobEnd() {}

int  TemplateModule::onGetStateChunkLen(LongBool Preset) { return 0; }
void TemplateModule::onGetStateChunk(void* chunk, LongBool Preset) {}
void TemplateModule::onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) {}
void TemplateModule::onAfterLoading() {}

void TemplateModule::onCreateSettings() {}
void TemplateModule::onSettingsHasChanged() {}
void TemplateModule::onCreateCommands() {}
void TemplateModule::onResize(float contentWidth, float contentHeight) {}
void TemplateModule::onPaint() {}

void TemplateModule::onMouseMove(TShiftState Shift, float X, float Y) {}
void TemplateModule::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) {}
void TemplateModule::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) {}
void TemplateModule::onMouseWheel(TShiftState Shift, int WheelDelta) {}
void TemplateModule::onMouseMoveMulti(TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) {}
void TemplateModule::onMouseDownMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) {}
void TemplateModule::onMouseUpMulti(TMouseButton MouseButton, TShiftState Shift, UsineEventClass& X, UsineEventClass& Y, UsineEventClass& Pressed) {}

void TemplateModule::onOpenEditor() {}
void TemplateModule::onBringToFront() {}
void TemplateModule::onCloseEditor() {}
void TemplateModule::onResizeEditor(int width, int height) {}

void TemplateModule::onBlocSizeChange(int BlocSize) {}
void TemplateModule::onSampleRateChange(double SampleRate) {}

void TemplateModule::onSetRecordedValue(TPrecision X, TPrecision Y, TPrecision Z) {}
void TemplateModule::onRandomize() {}
void TemplateModule::onReset() {}
void TemplateModule::onSetQuickColor(TUsineColor color) {}

void TemplateModule::onMidiSendOut(int DeviceID, TUsineMidiCode Code) {}
void TemplateModule::onMidiSysexSendOut(int DeviceID, char** Sysex, int sysexSize) {}
