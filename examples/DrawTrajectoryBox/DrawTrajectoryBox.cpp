//-----------------------------------------------------------------------------
// @file    DrawTrajectoryBox.cpp
// @author  BrainModular team
// @brief   Draws a trajectory with the mouse, shows painting, settings, commands and chunks.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <cmath>
#include <numbers>
#include "DrawTrajectoryBox.h"
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
	pModule = new DrawTrajectoryBox();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DrawTrajectoryBox*>(pModule);
}

void DrawTrajectoryBox::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtControl;
	pModuleInfo->BackColor          = sdkGetUsineColor(clInterfaceDesignModuleColor);
	pModuleInfo->DefaultWidth       = DEFAULT_MODULE_SIZE;
	pModuleInfo->DefaultHeight      = DEFAULT_MODULE_SIZE;
	pModuleInfo->DontProcess        = FALSE;
	pModuleInfo->CanBeRandomized    = TRUE;
	pModuleInfo->CanBeSavedInPreset = TRUE;
}

void DrawTrajectoryBox::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	colorTrajectory = sdkGetUsineColor(clDataFlow);
	colorCursor     = sdkGetUsineColor(clCursor);
}

void DrawTrajectoryBox::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(nbPointsFdrOutParamInfo, nbPointsFdrOut);
	sdkCreateParam(selPointIndexFdrInParamInfo, selPointIndexFdrIn);
	sdkCreateParam(selPointXFdrOutParamInfo, selPointXFdrOut);
	sdkCreateParam(selPointYFdrOutParamInfo, selPointYFdrOut);
	sdkCreateParam(resetBtnInParamInfo, resetBtnIn);
	sdkCreateParam(randomizeBtnInParamInfo, randomizeBtnIn);
	sdkCreateParam(coordsArrInParamInfo, coordsArrIn);
	sdkCreateParam(coordsArrOutParamInfo, coordsArrOut);
}

void DrawTrajectoryBox::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case SEL_POINT_INDEX_FDR_CBID:
		{
			const TPrecision cursorPos = selPointIndexFdrIn.getData();
			const int        index     = static_cast<int>(cursorPos);
			selPointFraction = cursorPos - index;
			if (index != selPointIndex)
			{
				selPointIndex = index;
				sdkRepaintPanel();
			}
			break;
		}
		case RESET_BTN_CBID:
			clearTrajectory();
			sdkRepaintPanel();
			break;
		case RANDOMIZE_BTN_CBID:
			onRandomize();
			break;
		case COORDS_ARR_IN_CBID:
		{
			const int inPoints = coordsArrIn.getSize() / LINES_PER_POINT;
			if (inPoints > MAXPOINTS)
				sdkRaiseError(WARNING_MAX_POINTS);
			const int total = std::min(inPoints, MAXPOINTS);

			coordsArrOut.setSize(total * LINES_PER_POINT);
			for (int i = 0; i < total; i++)
			{
				TPointF point { .x = coordsArrIn.getArrayData(i * LINES_PER_POINT), .y = coordsArrIn.getArrayData(i * LINES_PER_POINT + 1) };
				if (isYAxisInversed != FALSE)
					point.y = 1.0f - point.y;
				trajectory.points[i] = point;

				coordsArrOut.setArrayData(i * LINES_PER_POINT, point.x);
				coordsArrOut.setArrayData(i * LINES_PER_POINT + 1, point.y);
				coordsArrOut.setArrayData(i * LINES_PER_POINT + 2, coordsArrIn.getArrayData(i * LINES_PER_POINT + 2));
			}
			trajectory.num = total;
			nbPointsFdrOut.setData(static_cast<float>(total));
			computeAllControlPoints();
			sdkRepaintPanel();
			break;
		}
		case COORDS_ARR_OUT_CBID:
		{
			const int total = std::min(coordsArrOut.getSize() / LINES_PER_POINT, MAXPOINTS);
			for (int i = 0; i < total; i++)
				trajectory.points[i] = { .x = coordsArrOut.getArrayData(i * LINES_PER_POINT), .y = coordsArrOut.getArrayData(i * LINES_PER_POINT + 1) };
			trajectory.num = total;
			nbPointsFdrOut.setData(static_cast<float>(total));
			computeAllControlPoints();
			sdkRepaintPanel();
			break;
		}
		default:
			break;
	}
}

void DrawTrajectoryBox::onProcess()
{
	if (selPointIndex < 0 || selPointIndex >= trajectory.num)
		return;

	TPointF point = trajectory.points[selPointIndex];
	if (selPointIndex < trajectory.num - 1)
	{
		const TPointF& next = trajectory.points[selPointIndex + 1];
		point.x += (next.x - point.x) * selPointFraction;
		point.y += (next.y - point.y) * selPointFraction;
	}

	selPointXFdrOut.setData(point.x);
	selPointYFdrOut.setData(point.y);
}

// same chunk for workspaces, patches and presets: the number of points, then the points
int DrawTrajectoryBox::onGetStateChunkLen(LongBool Preset)
{
	return static_cast<int>(sizeof(trajectory.num) + sizeof(TPointF) * trajectory.num);
}

void DrawTrajectoryBox::onGetStateChunk(void* chunk, LongBool Preset)
{
	auto* bytes = static_cast<std::byte*>(chunk);
	std::memcpy(bytes, &trajectory.num, sizeof(trajectory.num));
	std::memcpy(bytes + sizeof(trajectory.num), trajectory.points.data(), sizeof(TPointF) * trajectory.num);
}

void DrawTrajectoryBox::onSetStateChunk(const void* chunk, int sizeInBytes, LongBool Preset)
{
	if (chunk == nullptr || sizeInBytes <= 0)
		return;

	// the chunk comes from a file: never trust its point count
	const auto* bytes = static_cast<const std::byte*>(chunk);
	int         num   = 0;
	if (sizeInBytes >= static_cast<int>(sizeof(num)))
		std::memcpy(&num, bytes, sizeof(num));

	const int expectedSize = static_cast<int>(sizeof(num) + sizeof(TPointF) * std::max(num, 0));
	if (sizeInBytes < static_cast<int>(sizeof(num)) || num < 0 || num > MAXPOINTS || sizeInBytes < expectedSize)
	{
		sdkRaiseError(ERROR_INVALID_CHUNK);
		return;
	}

	trajectory.num = num;
	std::memcpy(trajectory.points.data(), bytes + sizeof(num), sizeof(TPointF) * num);
	computeAllControlPoints();

	coordsArrOut.setSize(num * LINES_PER_POINT);
	for (int i = 0; i < num; i++)
		writePointOut(i);

	nbPointsFdrOut.setData(static_cast<float>(num));
}

void DrawTrajectoryBox::onRandomize()
{
	clearTrajectory();
	computeLissajous();
	sdkRepaintPanel();
}

void DrawTrajectoryBox::onCreateSettings()
{
	// adds our own section in the 'edit layout' panel
	sdkAddSettingLineCaption(DESIGN_TAB_NAME, "trajectory");
	sdkAddSettingLineBoolean(DESIGN_TAB_NAME, &inverseYAxis, "inverse Y axis");
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorTrajectory, "line color");
	sdkAddSettingLineInteger(DESIGN_TAB_NAME, &lineThickness, "line width", 1, 100, scLog, "pix", 2);
	sdkAddSettingLineColor(DESIGN_TAB_NAME, &colorCursor, "cursor color");
	sdkAddSettingLineInteger(DESIGN_TAB_NAME, &cursorDiameter, "cursor width", 0, 100, scLog, "pix", 2);
	sdkAddSettingLineBoolean(DESIGN_TAB_NAME, &closedLine, "closed line");
	sdkAddSettingLineBoolean(DESIGN_TAB_NAME, &smoothLine, "smooth line");
}

void DrawTrajectoryBox::onSettingsHasChanged()
{
	toggleInverseYAxis();
	sdkRepaintPanel();
}

void DrawTrajectoryBox::onCreateCommands()
{
	// a command triggers onCallBack with its callback id, like the matching button
	sdkAddCommandSeparator("cmd_edit", TRUE);
	sdkAddCommand("cmd_reset", RESET_BTN_CBID, TRUE);
	sdkAddCommand("cmd_randomize", RANDOMIZE_BTN_CBID, TRUE);
}

void DrawTrajectoryBox::onPaint()
{
	if (trajectory.num > 1)
	{
		sdkDrawPathStart();
		sdkDrawPathMoveTo(trajectory.points[0]);
		for (int i = 1; i < trajectory.num; i++)
		{
			if (smoothLine != FALSE)
				sdkDrawPathQuadCurveTo(trajectory.controls[i], trajectory.points[i]);
			else
				sdkDrawPathLineTo(trajectory.points[i]);
		}
		if (closedLine != FALSE && trajectory.num > 2)
			sdkDrawPathClose();
		sdkDrawPathDraw(colorTrajectory, static_cast<float>(lineThickness));
	}

	if (selPointIndex >= 0 && selPointIndex < trajectory.num)
		sdkDrawPoint(trajectory.points[selPointIndex], colorCursor, static_cast<float>(std::max(1, cursorDiameter)), TRUE);
}

void DrawTrajectoryBox::onMouseMove(TShiftState Shift, float X, float Y)
{
	if (mouseLeftButtonDown)
		appendTrajectory(clamp01(X), clamp01(Y));
}

void DrawTrajectoryBox::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	if (MouseButton != mbLeft)
		return;

	mouseLeftButtonDown = true;
	appendTrajectory(clamp01(X), clamp01(Y));
}

void DrawTrajectoryBox::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
	mouseLeftButtonDown = false;
}

void DrawTrajectoryBox::clearTrajectory()
{
	trajectory.num = 0;
	selPointIndex  = -1;
	coordsArrOut.setSize(0);
	nbPointsFdrOut.setData(0.0f);
	selPointIndexFdrIn.setData(-1.0f);
}

void DrawTrajectoryBox::toggleInverseYAxis()
{
	if ((inverseYAxis != FALSE) != (isYAxisInversed != FALSE))
	{
		for (int i = 0; i < trajectory.num; i++)
		{
			trajectory.points[i].y = 1.0f - trajectory.points[i].y;
			coordsArrOut.setArrayData(i * LINES_PER_POINT + 1, trajectory.points[i].y);
		}
		computeAllControlPoints();
	}
	isYAxisInversed = inverseYAxis;
}

void DrawTrajectoryBox::appendTrajectory(float X, float Y)
{
	if (trajectory.num >= MAXPOINTS)
		return;

	const TPointF point { .x = X, .y = Y };

	// ignore the mouse moves smaller than one pixel
	if (trajectory.num > 0)
	{
		const TPointF& last = trajectory.points[trajectory.num - 1];
		if (std::abs(point.x - last.x) <= sdkPixelToWidthPercent(1) && std::abs(point.y - last.y) <= sdkPixelToHeightPercent(1))
			return;
	}

	addPoint(point);
	if (trajectory.num == MAXPOINTS)
		sdkRaiseError(WARNING_MAX_POINTS);

	sdkRepaintPanel();
}

void DrawTrajectoryBox::addPoint(const TPointF& point)
{
	trajectory.points[trajectory.num] = point;
	trajectory.num++;

	coordsArrOut.setSize(trajectory.num * LINES_PER_POINT);
	writePointOut(trajectory.num - 1);
	computeControlPoint(trajectory.num - 1);
	nbPointsFdrOut.setData(static_cast<float>(trajectory.num));
}

void DrawTrajectoryBox::writePointOut(int index)
{
	coordsArrOut.setArrayData(index * LINES_PER_POINT, trajectory.points[index].x);
	coordsArrOut.setArrayData(index * LINES_PER_POINT + 1, trajectory.points[index].y);
	coordsArrOut.setArrayData(index * LINES_PER_POINT + 2, 0.5f);
}

void DrawTrajectoryBox::computeLissajous()
{
	const float a      = 1.0f + 2.0f * randomUnit();
	const float b      = 1.0f + 2.0f * randomUnit();
	const float phaseA = randomUnit();
	const float phaseB = randomUnit();

	TPointF first {};
	float   distToFirst = 0.0f;
	int     j           = 0;
	do
	{
		const float   t = j * 2.0f * std::numbers::pi_v<float> / 30.0f;
		const TPointF point { .x = 0.5f + std::cos(a * t + phaseA) / 2.0f, .y = 0.5f + std::cos(b * t + phaseB) / 2.0f };
		if (j == 0)
			first = point;

		addPoint(point);

		distToFirst = std::hypot(point.x - first.x, point.y - first.y);
		j++;
	}
	// stops when the curve comes back near its first point
	while (!((j > 10 && distToFirst < 0.1f) || j > LISS_MAXPOINTS));
}

// smoothing needs the previous point: the control point is one of the two other
// corners of the rectangle formed by the previous and the current point
void DrawTrajectoryBox::computeControlPoint(int pointIndex)
{
	if (pointIndex < 1 || pointIndex >= trajectory.num)
		return;

	const TPointF& last    = trajectory.points[pointIndex - 1];
	const TPointF& current = trajectory.points[pointIndex];
	const bool sameDirection = (current.x - last.x >= 0) == (current.y - last.y >= 0);

	trajectory.controls[pointIndex] = sameDirection ? TPointF { .x = current.x, .y = last.y }
	                                                : TPointF { .x = last.x, .y = current.y };
}

void DrawTrajectoryBox::computeAllControlPoints()
{
	for (int i = 1; i < trajectory.num; i++)
		computeControlPoint(i);
}
