//-----------------------------------------------------------------------------
// @file    MidiTranspose.cpp
// @author  BrainModular team
// @brief   Transposes midi notes, shows how to process midi messages.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2015-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include "MidiTranspose.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new MidiTranspose();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<MidiTranspose*>(pModule);
}

void MidiTranspose::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clMIDIModuleColor);
	pModuleInfo->CanBeSavedInPreset = FALSE;
}

void MidiTranspose::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(midiInParamInfo, midiIn);
	sdkCreateParam(midiOutParamInfo, midiOut);
	sdkCreateParam(pitchFdrInParamInfo, pitchFdrIn);
}

void MidiTranspose::onProcess()
{
	const int sizeMidiIn = midiIn.getSize();
	midiOut.setSize(0);
	if (sizeMidiIn == 0)
		return;

	// copy everything, then only transpose the notes
	midiOut.copyfrom(midiIn);
	const int pitch = static_cast<int>(std::round(pitchFdrIn.getData()));

	for (int i = 0; i < sizeMidiIn; i++)
	{
		TUsineMidiCode code = midiOut.getArrayMidi(i);
		if (code.Msg == MIDI_NOTEON || code.Msg == MIDI_NOTEOFF)
		{
			code.Data1 = std::clamp(static_cast<int>(code.Data1) + pitch, 0, 127);
			midiOut.setArrayMidi(i, code);
		}
	}
}
