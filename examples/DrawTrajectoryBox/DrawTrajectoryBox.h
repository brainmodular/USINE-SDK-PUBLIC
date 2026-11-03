//-----------------------------------------------------------------------------
// @file    DrawTrajectoryBox.h
// @author  BrainModular team
// @brief   Draws a trajectory with the mouse, shows painting, settings, commands and chunks.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <array>
#include "../../sdk/UserDefinitions.h"

constexpr int MAXPOINTS           = 5000;
constexpr int LISS_MAXPOINTS      = 200;
constexpr int DEFAULT_MODULE_SIZE = 134;
// coords in and coords out arrays store 3 lines per point: x, y and a third value
constexpr int LINES_PER_POINT     = 3;

struct TrajectoryPoints
{
	int                            num = 0;
	std::array<TPointF, MAXPOINTS> points {};
	// control points of the smooth line mode
	std::array<TPointF, MAXPOINTS> controls {};
};

class DrawTrajectoryBox : public UserModuleBase
{
public:
	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;
	void onProcess() override;

	int  onGetStateChunkLen(LongBool Preset) override;
	void onGetStateChunk(void* chunk, LongBool Preset) override;
	void onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset) override;

	void onRandomize() override;

	void onCreateSettings() override;
	void onSettingsHasChanged() override;
	void onCreateCommands() override;
	void onPaint() override;

	void onMouseMove(TShiftState Shift, float X, float Y) override;
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;
	void onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y) override;

private:
	UsineEventClass nbPointsFdrOut;
	UsineEventClass selPointIndexFdrIn;
	UsineEventClass selPointXFdrOut;
	UsineEventClass selPointYFdrOut;
	UsineEventClass resetBtnIn;
	UsineEventClass randomizeBtnIn;
	UsineEventClass coordsArrIn;
	UsineEventClass coordsArrOut;

	// saved in the patch with the chunk system
	TrajectoryPoints trajectory;

	bool  mouseLeftButtonDown = false;
	int   selPointIndex       = -1;
	float selPointFraction    = 0.0f;

	// settings of the 'edit layout' panel
	LongBool    inverseYAxis    = FALSE;
	LongBool    closedLine      = FALSE;
	LongBool    smoothLine      = FALSE;
	int         lineThickness   = 2;
	int         cursorDiameter  = 4;
	TUsineColor colorTrajectory = 0;
	TUsineColor colorCursor     = 0;

	// inverseYAxis is the wanted state, isYAxisInversed the state of the stored points
	LongBool isYAxisInversed = FALSE;

	void clearTrajectory();
	void toggleInverseYAxis();
	void appendTrajectory(float X, float Y);
	void addPoint(const TPointF& point);
	void writePointOut(int index);
	void computeLissajous();
	void computeControlPoint(int pointIndex);
	void computeAllControlPoints();
};
