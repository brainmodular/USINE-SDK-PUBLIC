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
/// 2023/11/28
///    added a more modern class system to the CPP SDK
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
#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include "UsineDefinitions.h"
#include "UserModule.h"
#include "UserUtils.h"
#include "UsineEventClass.h"

/** @mainpage API Reference Documentation
    This documentation is divided in three sections who provide functions, class and datatypes needed to construct user's module to extend Usine.<br> <br>
    @ref Globals "Global Functions" <br>
     - This page contains the three global functions needed by Usine to get infos about a user module, instantiate and destroy it.
     - You'll found also some geometric and color helpers.
   
    @ref UserModuleBase "UserModuleBase Class" <br>
     - This page contains the base class of every user's module.
     - It combines callbacks and utility functions to integrate the module in Usine.
     - You are only required to implement the features needed by your module.

    @ref UsineEvents "Usine's main data structure: Events"
     - This page contains contains all the information about Usine's main data holding class, UsineEventClass.
     - Many of the most useful SDK functions are located here.
     - Provides two ways communication with Usine.

    @ref Datatypes "Datatypes" <br>
     - This page contains all the data types used at one point in the SDK.
     - From simple typedef and constant to structure that define entire part of the module.
    

    Creating a module for Usine can become very addictive and complex.  <br>
    You can discuss about the SDK on the <a href="https://www.brainmodular.com/forums/viewforum.php?f=7">official forum section</a>

    Happy coding. <br>
    Olivier SENS & the BrainModular Team
    @defgroup Globals Global functions
   
    @defgroup Datatypes Datatypes
*/
