//-----------------------------------------------------------------------------
//@file  
//	RollArray.cpp
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Implementation of the TRollArrayModule class.
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.org 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015, 2019 BrainModular
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), 
// to deal in the Software without restriction, including without limitation 
// the rights to use, copy, modify, merge, publish, distribute, sublicense, 
// and/or sell copies of the Software, and to permit persons to whom the Software 
// is furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
//     copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
// SOFTWARE.
//
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "RollArray.h"

//----------------------------------------------------------------------------
// create, general info and destroy methodes

//-----------------------------------------------------------------------------
// Create
void CreateModule (void* &pModule, AnsiCharPtr optionalString, LongBool Flag, MasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new TRollArrayModule ();
}

//-----------------------------------------------------------------------------
// destroy
void DestroyModule (void* pModule) 
{
	// cast is important to call the good destructor
	delete ((TRollArrayModule*)pModule);
}

// module constants for browser info and module info
const AnsiCharPtr UserModuleBase::MODULE_NAME = "RollArr";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "Roll Array";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "2.0";

// browser info
void GetBrowserInfo(ModuleInfo* pModuleInfo) 
{
	pModuleInfo->Name				= UserModuleBase::MODULE_NAME;
	pModuleInfo->Description		= UserModuleBase::MODULE_DESC;
	pModuleInfo->Version			= UserModuleBase::MODULE_VERSION;
}

//-----------------------------------------------------------------------------
// module constructor and destructor
TRollArrayModule::TRollArrayModule ()
{
	m_rollStepValue = 0;
}

TRollArrayModule::~TRollArrayModule ()
{
}

//-----------------------------------------------------------------------------
// module description
void TRollArrayModule::onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo) 
{
	// identification of the module
	pModuleInfo->Name				= MODULE_NAME;
	pModuleInfo->Description		= MODULE_DESC;
	pModuleInfo->ModuleType         = mtSimple;
	pModuleInfo->BackColor          = sdkGetUsineColor(clArrayModuleColor);
	pModuleInfo->NumberOfParams     = 3;
	pModuleInfo->DontProcess		= TRUE;
    pModuleInfo->Version			= MODULE_VERSION;
}

//----------------------------------------------------------------------------
// parameters and process
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Parameters description
void TRollArrayModule::onGetParamInfo (int n, TParamInfo* pParamInfo) 
{		
	switch (n) 
	{
	// Array input
	case 0:
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array in";
		pParamInfo->IsInput			= TRUE;
		pParamInfo->IsOutput		= FALSE;
		pParamInfo->ReadOnly		= FALSE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		pParamInfo->CallBackType    = ctImmediate;
		break;

	// Array ouput
	case 1:
		pParamInfo->ParamType		= ptArray;
		pParamInfo->Caption			= "array out";
		pParamInfo->IsInput			= FALSE;
		pParamInfo->IsOutput		= TRUE;
		pParamInfo->ReadOnly		= TRUE;
		pParamInfo->MinValue		= - FLT_MAX;
		pParamInfo->MaxValue		= FLT_MAX;
		break;

	// Roll fader
	case 2:
		pParamInfo->ParamType           = ptDataFader;
		pParamInfo->Caption             = "roll";
		pParamInfo->IsInput             = TRUE;
		pParamInfo->IsOutput            = FALSE;
		pParamInfo->Scale               = scLinear;
		pParamInfo->MinValue            = -1024;
		pParamInfo->MaxValue            = 1024;
		pParamInfo->DefaultValue        = 0;
		pParamInfo->Format              = "%.0f";
        pParamInfo->ReadOnly            = FALSE;
        pParamInfo->IsStoredInPreset	= TRUE;
		pParamInfo->CallBackType        = ctImmediate;
		break;

	// default case
	default:
		// do nothing
		break;
	}
}

//-----------------------------------------------------------------------------
// set the parameters events address
void TRollArrayModule::onSetEventAddress (int n, UsineEventPtr pEvent) 
{

	switch (n) 
	{
	// Array ouput
	case 0:
		m_arrArrayToRoll = pEvent;
		break;

	// Array ouput
	case 1:
		m_arrRolledArray = pEvent;
		break;

	// roll fader
	case 2:
		m_fdrRollStep = pEvent;
		break;	

	// default case
	default:
		// do nothing
		break;
	}
}

//-----------------------------------------------------------------------------
// Parameters callback
void TRollArrayModule::onCallBack (UsineMessage* Message) 
{
	// WParam contains Param Number
	// n° 0 = in array
	// n° 2 = roll fader
	
	// in array
	if ( (Message->wParam == 0) && (Message->lParam == MSG_CHANGE) ) 
	{
		// compute the new rolled array
		computeRoll();
	}
	// roll fader
	else if ( (Message->wParam == 2) && (Message->lParam == MSG_CHANGE) ) 
	{
		// update the roll step value 
		//pRollArrayModule->m_rollStepValue = (int)pRollArrayModule->sdkGetEvtData(pRollArrayModule->m_fdrRollStep);
        int tmp = (int)sdkGetEvtData(m_fdrRollStep);
		
        m_rollStepValue = tmp;
		
		// compute the new rolled array
		computeRoll();
	}
	// default case
	else
	{
		// do nothing
	}
}

////-----------------------------------------------------------------------------
//// Main process
//void Process (void* pModule)
//{ 
//	// nothing to do (see pModuleInfo->DontProcess == TRUE)
//	// all is done from CallBack()
//}

//----------------------------------------------------------------------------
// chunk system
//----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
//// GetChunkLen : No chunk to store --> result := 0;
//int TRollArrayModule::onGetChunkLen ( LongBool Preset) 
//{
//	return 0;
//}
//
////-----------------------------------------------------------------------------
//// GetChunk : No chunk to store --> result := 0;
//void GetChunk (void* pModule, AnsiCharPtr chunk, LongBool Preset) 
//{
//	chunk = "";
//}
//
////-----------------------------------------------------------------------------
//// SetChunk No chunk to store --> nothing to do...
//void SetChunk (void* pModule, const AnsiCharPtr chunk, LongBool Preset) {}

//----------------------------------------------------------------------------
// layout
//----------------------------------------------------------------------------

//layout not used

//-----------------------------------------------------------------------------
// mouse and multi touch
//-----------------------------------------------------------------------------

// Mouse callbacks not used
// Multi touch callbacks not used
void TRollArrayModule::computeRoll()
{
    try 
    {
        //throw 20;
	int iArrayLen = sdkGetEvtSize(m_arrArrayToRoll);

	if ( iArrayLen > 1)
	{			
		// out array size equal in array size
		sdkSetEvtSize(m_arrRolledArray, iArrayLen);

		TPrecision* pArrayOutDataAddr = sdkGetEvtDataAddr( m_arrRolledArray );
		TPrecision* pArrayInDataAddr = sdkGetEvtDataAddr( m_arrArrayToRoll );

        // modulo by the array size to found the real roll value
        int iRollValueMod = (m_rollStepValue % iArrayLen);

		// we do just a passthru
		if (iRollValueMod == 0 || iRollValueMod == iArrayLen || iRollValueMod == - iArrayLen )
		{
			memcpy((void*)(pArrayOutDataAddr), (void*)(pArrayInDataAddr), (sizeof(float) * iArrayLen));
		}
        // roll to right
		else if (iRollValueMod > 0)
		{
				
            int iArrayShiftedLen = iArrayLen - iRollValueMod;
				
			// the part of the array shifted
			memcpy((void*)(pArrayOutDataAddr + iRollValueMod), (void*)(pArrayInDataAddr), iArrayShiftedLen * sizeof(float));
				
			// the part of the array rolled
			memcpy((void*)(pArrayOutDataAddr), (void*)(pArrayInDataAddr + iArrayShiftedLen), iRollValueMod * sizeof(float));
		}
		// so m_rollStepValue < 0
        // rool to left
		else 
		{
			int iArrayShiftedLen = iRollValueMod + iArrayLen;

			// the part of the array shifted
			memcpy((void*)(pArrayOutDataAddr + iArrayShiftedLen), (void*)(pArrayInDataAddr), (- iRollValueMod) * sizeof(float));
				
			// the part of the array rolled
			memcpy((void*)(pArrayOutDataAddr), (void*)(pArrayInDataAddr - iRollValueMod), iArrayShiftedLen * sizeof(float));
		} 
	}
    }   
    catch (...) 
    {
        int toto = 2;
        toto += 4;
    }
}
