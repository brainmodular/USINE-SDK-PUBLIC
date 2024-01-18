//-----------------------------------------------------------------------------
//@file  
//	RingMod.h
//
//@author
//	BrainModular team
//
//@brief 
//	Definitions of the RingMod class.
//
//  Example user module to show how to process audio buffers.
//
//@historic 
//	2015/02/23
//    first release for Hollyhock CPP SDK 6.04.001
//
//@IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs :
//  support@brainmodular.com 
//
// All dependencies are under there own licence.
//
//@LICENCE
// Copyright (C) 2013, 2014, 2015 BrainModular
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

// include once, no more
#ifndef __RING_MOD_TEST_H__
#define __RING_MOD_TEST_H__

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class RingMod : public UserModuleBase
{
    //-------------------------------------------------------------------------
	// module constructors/destructors
	//-------------------------------------------------------------------------
public:
    // constructor
	RingMod();

    // destructor
	virtual ~RingMod();

	//-------------------------------------------------------------------------
	// public methods inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	int  onGetNumberOfParams(int queryResult1, int queryResult2);
	void onAfterQuery (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo, int queryResult1, int queryResult2);
	void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onCallBack (TUsineMessage *Message);
	void onProcess ();


	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	//-------------------------------------------------------------------------
	// parameters events	
    UsineEventClass audioInputs[USINE_MULTIPHONY_MAX];     // audio input
    UsineEventClass audioOutputs[USINE_MULTIPHONY_MAX];    // audio output
	UsineEventClass modInput;                            // mod input
	UsineEventClass bufferTemp;
	UsineEventClass fdrMix;
	
	//-------------------------------------------------------------------------
    static const int numOfParamAfterAudiotInOut = 2;

	int queryResult;
	int numOfAudiotInsOuts;

	TPrecision coeffMix;
    
}; // class RingMod

#endif //__RING_MOD_TEST_H__
