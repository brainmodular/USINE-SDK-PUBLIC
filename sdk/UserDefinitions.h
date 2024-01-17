//-----------------------------------------------------------------------------
///@file UserDefinitions.h
///
///@author
///	BrainModular team
///
///@brief  
///	All includes for making a user module
///
///@HISTORIC 
///	2013/05/15
///    first release for Hollyhock CPP SDK 6.00.226 
///
///@IMPORTANT
///	This file is part of the Usine Hollyhock CPP SDK
///
///  Please, report bugs and patch to Usine forum :
///  support@brainModular.com 
///  
/// All dependencies are under there own licence.
///
///@LICENSE
/// Copyright (C) 2023 BrainModular, BeSpline, Adamson
/// 
///-----------------------------------------------------------------------------

// include once, no more
#ifndef _USERDEFINITIONS_H_
#define _USERDEFINITIONS_H_

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "UsineDefinitions.h"
#include "UserModule.h"
#include "UserUtils.h"
#include "UsineEventClass.h"

// After the SDK, mark all use of this class as deprecated.
[[deprecated("Use UsineEventClass instead")]]
typedef UsineEvent* UsineEventPtr;

/** @mainpage API Reference Documentation
    This doumentation is divided in three sections who provide functions, class and datatypes needed to construct user's module to extend Usine.<br> <br>
    @ref Globals "Global Functions" <br>
     - This page contains the three globals functions needed by Usine to get infos about a user module, instantiate and destroy it.
     - You'll found also some geometric and color helpers.
   
    @ref UserModuleBase "UserModuleBase Class" <br>
     - This page contains the base class of every user's module.
     - It combine callbacks and utilities functions to integrate the module in Usine.
     - You can activate only the features needed by your module.
   
    @ref Datatypes "Datatypes" <br>
     - This page contains all the data types used at one point in the SDK.
     - From simple typedef and constant to structure that define entire part of the module.
    

    Developping a module for Usine can become very addictive and complex.  <br>
    You can discuss about the SDK on the <a href="https://www.brainmodular.com/forums/viewforum.php?f=7">official forum section</a>

    Happy coding. <br>
    Olivier SENS & the BrainModular Team
    @defgroup Globals Global functions
   
    @defgroup Datatypes Datatypes
*/



#endif // _USERDEFINITIONS_H_
