//-----------------------------------------------------------------------------
// @file    ArrayDistance.cpp
// @author  BrainModular team
// @brief   Dynamic time warping (DTW) distance between two arrays.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include <vector>
#include "ArrayDistance.h"
#include "ModuleDefinition.h"
#include "DTW.h"

namespace
{
	// DTW.h works on vectors: copies the values of an array parameter
	// the size of a Usine event can be -1 (NULL_EVT, an event without value): no value then
	std::vector<TPrecision> arrayValues(const UsineEventClass& arrayIn)
	{
		const int size = std::max(arrayIn.getSize(), 0);
		std::vector<TPrecision> values;
		values.reserve(static_cast<size_t>(size));
		for (int i = 0; i < size; ++i)
			values.push_back(arrayIn.getArrayData(i));
		return values;
	}

	// keeps the patch of a module locked while it exists, until the end of the scope or an exception
	class PatchLock
	{
	public:
		explicit PatchLock(const UserModuleBase& lockedModule)
			: owner(lockedModule)
		{
			owner.sdkLockPatch();
		}

		~PatchLock()
		{
			owner.sdkUnLockPatch();
		}

		PatchLock(const PatchLock&) = delete;
		PatchLock& operator=(const PatchLock&) = delete;

	private:
		const UserModuleBase& owner;
	};
}

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new ArrayDistance();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<ArrayDistance*>(pModule);
}

void ArrayDistance::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;
	// no onProcess: the distance is only calculated by the calculate button
	pModuleInfo->DontProcess        = TRUE;
}

void ArrayDistance::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(array1ArrInParamInfo, array1ArrIn);
	sdkCreateParam(array2ArrInParamInfo, array2ArrIn);
	sdkCreateParam(distanceFdrOutParamInfo, distanceFdrOut);
	sdkCreateParam(minFdrInParamInfo, minFdrIn);
	sdkCreateParam(maxFdrInParamInfo, maxFdrIn);
	sdkCreateParam(calculateBtnInParamInfo, calculateBtnIn);
}

void ArrayDistance::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	switch (Message->wParam)
	{
		case CALCULATE_BTN_CBID:
			calculateDistance();
			break;
		default:
			break;
	}
}

void ArrayDistance::calculateDistance()
{
	std::vector<TPrecision> array1;
	std::vector<TPrecision> array2;
	TPrecision minValue = 0;
	TPrecision maxValue = 0;
	{
		// this callback runs on the main thread while the patch thread writes the inputs and can
		// reallocate an array: they are copied with the patch locked, the calculation runs unlocked
		const PatchLock lock(*this);
		array1   = arrayValues(array1ArrIn);
		array2   = arrayValues(array2ArrIn);
		minValue = minFdrIn.getData();
		maxValue = maxFdrIn.getData();
	}
	// 1 when an array is empty or when min equals max: the distance is then not scaled
	const TPrecision normalizationFactor = maxDTW<TPrecision>(array1, array2, minValue, maxValue);
	distanceFdrOut.setData(DTW<TPrecision>(array1, array2) / normalizationFactor);
}
