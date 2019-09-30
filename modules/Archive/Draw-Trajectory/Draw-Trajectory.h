//-----------------------------------------------------------------------------
//@file  
//	DrawTrajectory.h
//
//@author
//	Martin FLEURENT aka 'martignasse'
//
//@brief 
//	Definitions of the TDrawTrajectoryModule class.
//
//  A simple panel with to draw lines on :
//    - a clear button
//    - point counter
//
//  Good example to show how to use :
//    - the mouse events callbacks
//    - the chunk system to save some module infos in the workspace
//
//HISTORIC 
//	2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226 
//
//IMPORTANT
//	This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  http://www.sensomusic.com/wiki2/doku.php?id=hollyhock:bugsreport 
//  
// All dependencies are under there own licence.
//
//@copyright
// Copyright (C) 2013 Sensomusic
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
#ifndef INCLUDED_USERDRAWPAD_H
#define INCLUDED_USERDRAWPAD_H

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "../../sdk/UserDefinitions.h"  
#include <iostream>
#include <cmath>

//-----------------------------------------------------------------------------
// defines and constantes
//-----------------------------------------------------------------------------
// max points stored in the module
const int MAXPOINTS    = 5000;
const int ARRAY_OUT_LINES = 3;
const int LISS_MAXPOINTS = 200; 
// minimun space between two points
//const float SPACEPOINTS    = 0.001f; 

// Panel Height
const int DEFAULT_MODULEHEIGHT = 134;  

//-----------------------------------------------------------------------------
// structures and typedef
//-----------------------------------------------------------------------------
// Array of TPointF to store the trajectory coords datas
struct TTabPoints 
{
	int num;				// number of points
	TPointF Tab[MAXPOINTS]; // coords data for every points
	TPointF TabControls[MAXPOINTS]; // coords data for every control points if smooth line mode
};

//-----------------------------------------------------------------------------
// class definition
//-----------------------------------------------------------------------------
class TDrawTrajectoryModule : public UserModuleBase
{
	//-------------------------------------------------------------------------
	// public constructors/destructors
	//-------------------------------------------------------------------------
public:

    // constructor
	TDrawTrajectoryModule(){};

    // destructor
	~TDrawTrajectoryModule(){};


	//-------------------------------------------------------------------------
	// public methodes inherited from UserModule
	//-------------------------------------------------------------------------
public:
	//-----------------------------------------------------------------------------
	// module info
	void onGetModuleInfo (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// query system and init
	//int  onGetNumberOfParams( int queryIndex);
	//void onAfterQuery (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo, int queryIndex);
	void onInitModule (MasterInfo* pMasterInfo, ModuleInfo* pModuleInfo);

	//-----------------------------------------------------------------------------
	// parameters and process
	void onGetParamInfo (int ParamIndex, TParamInfo* pParamInfo);
	void onSetEventAddress (int ParamIndex, UsineEventPtr pEvent);
	void onCallBack (UsineMessage *Message);
	void onProcess ();
	//-----------------------------------------------------------------------------
	// chunk system
	int  onGetChunkLen (LongBool Preset);
	void onGetChunk (void* chunk, LongBool Preset);
	void onSetChunk (const void* chunk, int sizeInBytes, LongBool Preset);
    //-----------------------------------------------------------------------------
	// Global Randomize
    void onRandomize ();
	//-----------------------------------------------------------------------------
	// settings
	void onCreateSettings();
	void onSettingsHasChanged();
	//-----------------------------------------------------------------------------
	// contextual menu
    void onCreateCommands();
	//-----------------------------------------------------------------------------
	// UI Canvas
	void onPaint ();

	//-----------------------------------------------------------------------------
	// mouse and multi touch interaction
	void onMouseMove(TShiftState Shift, float X, float Y);
	void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
	//void onMouseMoveMultiProc(TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseDownMultiProc(TMouseButton MouseButton, TShiftState Shift, UsineEventPtr X, UsineEventPtr Y);
	//void onMouseUpMultiProc (TMouseButton MouseButton, TShiftState Shift,UsineEventPtr X, UsineEventPtr Y);
	//void onOpenEditor();
	//void onBringToFront();

	//-----------------------------------------------------------------------------
	// audio setup update
	//void onBlocSizeChange (int BlocSize);
	//void onSampleRateChange (double SampleRate);
	
	//-----------------------------------------------------------------------------
	// recording 
	// void onSetRecordedValue (TPrecision X, TPrecision Y, TPrecision Z);
    
    //-----------------------------------------------------------------------------
    // usine preset randomize
	//-------------------------------------------------------------------------
	// private members
	//-------------------------------------------------------------------------
private:
	
	//-------------------------------------------------------------------------
	// parameters events
	UsineEventPtr m_dtfPointsCount;		// PointsCount data field output
	UsineEventPtr m_dtfSelPointIndex;	// Selected point index data field input/output
	UsineEventPtr m_fdrSelPointX;		// Selected point X coord fader output
	UsineEventPtr m_fdrSelPointY;		// Selected point Y coord fader output
	UsineEventPtr m_btnErase;			// Erase trajectory button input
	UsineEventPtr m_btnRandomize;       // randomize button input
    UsineEventPtr arrPointsCoordsIn;    // to populate the lines tab
    UsineEventPtr arrPointsCoordsOut;   // lines tab points coords

	//-------------------------------------------------------------------------
	// Other	
		
    // datas for the edit layout parameters
	LongBool    m_boolInverseYAxis;

	// TODO : make it protected member
	// store the mouse left button state
	LongBool	m_mouseLeftButtonDown;			

	// store the trajectory coords datas
	// used in the chunk system 
	TTabPoints	m_tabTrajectoryCoords;
	//int			m_tabTrajectoryCoordsNum;

	int			m_selPointIndexValue;

	float		m_trajectoryThickness;
	int			m_indexDiameter;

	LongBool    m_boolIsYAxisInversed;
	
	int thicknessTrajectory;
	TColorUsine colorTrajectory;
	TColorUsine colorCurrentPoint;

    LongBool m_boolClosedLine;
    LongBool m_boolSmoothLine;

	//-------------------------------------------------------------------------
	// private methodes
	//-------------------------------------------------------------------------
private:
	    //-------------------------------------------------------------------------
	// deal with the inverse Y axis option
	inline void toggleInverseYAxis();
	//inline void applyInverseYAxis();
	//-------------------------------------------------------------------------
	// append a point coords in the m_tabTrajectoryCoords structure
	// TODO : make it protected methode
	inline void appendTrajectory( float X, float Y );

    void computeLissajous ();

    // compute control points for the smooth line mode
    void computeControlPoint (int pointIndex);
    void computeAllControlPoints ();

}; // class TDrawTrajectoryModule

#endif // INCLUDED_USERDRAWPAD_H