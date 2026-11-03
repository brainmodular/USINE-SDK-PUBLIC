//-----------------------------------------------------------------------------
//@file
//    DatabasePlayer.cpp
//
//@author
//    BrainModular team
//
//@brief
//    Implementation of the DatabasePlayer class.
//
//  A template include to start from for new user module.
//
//@historic
//    2013/05/15
//    first release for Hollyhock CPP SDK 6.00.226
//
//@IMPORTANT
//    This file is part of the Usine Hollyhock CPP SDK
//
//  Please, report bugs and patch to Usine forum :
//  support@brainmodular.com
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

#include "DatabasePlayer.h"

//----------------------------------------------------------------------------
// callback ids
//----------------------------------------------------------------------------
constexpr NativeInt DATABASE_FILE_CBID    = 0x00F200C0;
constexpr NativeInt RELOAD_DATABASE_CBID  = 0x00F200C1;
constexpr NativeInt FIRST_DIMENSION_CBID  = 0x00F200C2;
constexpr NativeInt SECOND_DIMENSION_CBID = 0x00F200C3;
constexpr NativeInt POLYPHONY_CBID        = 0x00F200C4;
constexpr NativeInt ATTACK_TIME_CBID      = 0x00F200C5;
constexpr NativeInt RELEASE_TIME_CBID     = 0x00F200C6;
constexpr NativeInt ON_RESYNTHESIS_CBID   = 0x00F200C7;
constexpr NativeInt CURSOR_DOWN_CBID      = 0x00F200C8;
constexpr NativeInt CURSOR_X_CBID         = 0x00F200C9;
constexpr NativeInt CURSOR_Y_CBID         = 0x00F200CA;
constexpr NativeInt CURSOR_RADIUS_CBID    = 0x00F200CB;

void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new DatabasePlayer();
}

void DestroyModule(void* pModule)
{
    delete ((DatabasePlayer*)pModule);
}

DatabasePlayer::DatabasePlayer() :
kdTree(2),
rBuffer(DEFAULT_SPECTRUM*2,DEFAULT_SPECTRUM),
lBuffer(DEFAULT_SPECTRUM*2,DEFAULT_SPECTRUM),
rTempBuffer(DEFAULT_SPECTRUM,0.0f),
lTempBuffer(DEFAULT_SPECTRUM,0.0f),
rReal(DEFAULT_SPECTRUM/2+1,0.0f),
rIm(DEFAULT_SPECTRUM/2+1,0.0f),
lReal(DEFAULT_SPECTRUM/2+1,0.0f),
lIm(DEFAULT_SPECTRUM/2+1,0.0f)
{
    generateLUT();
    InitializeHanningWindow(hanning, DEFAULT_SPECTRUM);
}


DatabasePlayer::~DatabasePlayer()
{
    try {
        sdkCriticalSectionLock(csPaint);
         
        sdkThreadDestroy(workThread);
        sdkThreadDestroy(initSearchThread);
        
        sdkFFTDestroy(ifftPtr);
        
        int errc = sqlite3_close(db);
        if(errc != SQLITE_OK)
        {
            sdkTraceErrorChar("Error closing the database");
            sdkTraceErrorChar(sqlite3_errmsg(db));
        } 
    } catch (...) {
        sdkTraceErrorChar("Error in DatabasePlayer destructor");
    }
    
    sdkCriticalSectionUnLock(csPaint);
    sdkCriticalSectionDestroy(csPaint);
}

const AnsiCharPtr UserModuleBase::MODULE_NAME = "Database Player";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "DatabasePlayer";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

void DatabasePlayer::onGetModuleInfo(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo)
{
    pModuleInfo->Name = MODULE_NAME;
    pModuleInfo->Description = MODULE_DESC;
    pModuleInfo->ModuleType = mtControl;
    pModuleInfo->BackColor = sdkGetUsineColor(clDataModuleColor);
    pModuleInfo->NumberOfParams = 16;
    pModuleInfo->Version = MODULE_VERSION;
    pModuleInfo->DefaultWidth = DEFAULT_PANEL_WIDTH;
    pModuleInfo->DefaultHeight = DEFAULT_PANEL_HEIGHT;
    pModuleInfo->CanBeRandomized = FALSE;
}

void ifftFunction(void* pModule)
{
    
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {
        module->listOfOutputSamples.clearSamplesToRelease();
        
        //debug
        bool isMouseDown = module->mouseInfo.leftBtnDown;
        
        module->listOfOutputSamples.updateMags(module->SPECTRUM_SIZE_2,isMouseDown);
        module->computeIFFTMag(module->MagPtr);
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer IFFT Function: ") + std::string(e.what()));
    }

}

void requestFunction(void* pModule)
{
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {
        if(module->isDBOpen)
        {
            module->isRequesting.store(true,std::memory_order_release);
            if(module->sendRequestToDB(module->dimX,module->dimY))
            {
                module->sdkRepaintPanel();
            } else
            {
                sdkTraceErrorChar("Error during the request, closing the database.");
                sdkTraceErrorChar("Please re-open a database.");
                sqlite3_close(module->db);
                module->isDBOpen = false;
            }
            module->isRequesting.store(false,std::memory_order_release);
        }
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer request Function : ") + std::string(e.what()));
    }
    
}

void openDatabaseFunc(void* pModule)
{
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {

        module->isLoading.store(true,std::memory_order_release);
        
        if(module->isDBOpen)
        {
            int ecc = sqlite3_close(module->db);
            if(ecc != SQLITE_OK)
            {
                sdkTraceErrorChar("Error closing the database");
                sdkTraceErrorChar(sqlite3_errmsg(module->db));
            } else {
                module->isDBOpen = false;
            }
        }
        if(!module->isDBOpen)
        {
            if(fsx::exists(module->dbName))
            {
                int ec = sqlite3_open(module->dbName.c_str(), &module->db);
                if(ec != SQLITE_OK)
                {
                    sdkTraceErrorChar("Error opening the database");
                    sdkTraceErrorChar(sqlite3_errmsg(module->db));
                } else {
                    std::string openingDbString = std::string("Opening database at : \n") + module->dbName;
                    module->sdkProgressBar(openingDbString.c_str(),sdkAlphaColor(1.0f,1.0f,1.0f,1.0f));
                    if(module->openDB())
                    {
                        
                        module->listOfOutputSamples.changeFFTSize(module->SPECTRUM_SIZE);
                        
                        module->actionQueue.clear();
                        module->isDBOpen = true;
                        module->actionQueue.pushFunc(requestFunction, module);
                    } else
                    {
                        sdkTraceErrorChar("Error in the initialization after opening the database");
                        sqlite3_close(module->db);
                        module->isDBOpen = false;
                    }
                }
            } else {
                sdkTraceErrorChar("No existing database at : " + std::string(module->dbName.c_str()));
            }
        }
       
        module->isLoading.store(false,std::memory_order_release);
        
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer openDatabase Function : ") + std::string(e.what()));
    }
}

void searchFunc(void* pModule)
{
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {
        module->listOfOutputSamples.clearSamplesToRelease();
        if(module->isCursorOn || module->mouseInfo.leftBtnDown)
        {
            //using clock = std::chrono::high_resolution_clock;
            //using duration = std::chrono::duration<double, std::milli>;
            //auto startSearch = clock::now();
            
            module->findEveryNearestPoint(module->circleCursor, module->listOfSamplesInCircle);
            
            //auto endSearch = clock::now();
            //duration searchDuration = (endSearch - startSearch);
            //std::cout << "Search duration : " << searchDuration.count() << " ms" << std::endl;
            
        }
        else if(module->listOfOutputSamples.activeSamples > 0){
            module->listOfOutputSamples.softClear();
        }
        
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer search Function : ") + std::string(e.what()));
    }
}

void workThreadFunc(void* pModule, TThreadPtr thread)
{
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {
        module->actionQueue.run(module);
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer work Thread : ") + std::string(e.what()));
    }
}

// this thread pushes the search function to the action queue every 20 ms
void initSearchThreadFunc(void* pModule, TThreadPtr thread)
{
    DatabasePlayer* module = static_cast<DatabasePlayer*>(pModule);
    try {
        if(module->isDBOpen && module->actionQueue.m_IsWorking.load(std::memory_order_relaxed))
        {
            module->actionQueue.pushFunc(searchFunc, module);
        }
    } catch(const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer init search Thread : ") + std::string(e.what()));
    }
}


void DatabasePlayer::onInitModule(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo)
{
    csPaint = sdkCriticalSectionCreate("paint critical section", false);
    
    workThread = sdkThreadCreate("working Thread", workThreadFunc, tpMedium, 0);
    initSearchThread = sdkThreadCreate("init search Thread", initSearchThreadFunc, tpLow, 20);
    
    ifftPtr = sdkFFTCreate(SPECTRUM_SIZE);
}

void DatabasePlayer::onCreateSettings()
{
    sdkAddSettingLineCaption(PROPERTIES_TAB_NAME, "graphical settings");

    sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &canvasPointRadius, "points radius", 2.0f, 10.0f,scLinear,"pix",DEFAULT_FORMAT_INTEGER, 2.0f);
    sdkSetLastSettingCallbackID(SettingsId::POINT_RADIUS);

    sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &spreadFactorX, "spread factor X", 0.1f, 1.0f ,scLinear,"","%.2f", 0.25f);
    sdkSetLastSettingCallbackID(SettingsId::SPREAD_X);

    sdkAddSettingLineSingle(PROPERTIES_TAB_NAME, &spreadFactorY, "spread factor Y", 0.1f, 1.0f ,scLinear,"","%.2f", 0.25f);
    sdkSetLastSettingCallbackID(SettingsId::SPREAD_Y);

    sdkAddSettingLineCaption(PROPERTIES_TAB_NAME,"output settings");
    sdkAddSettingLineBoolean(PROPERTIES_TAB_NAME, &listOfOutputSamples.isCoeffNormalized, "normalize coeffs");
    sdkSetLastSettingCallbackID(SettingsId::NORMALIZE_COEFFS);

}

void DatabasePlayer::onSettingsHasChanged()
{
    
}


void DatabasePlayer::onGetParamInfo(int ParamIndex, TParamInfo *pParamInfo)
{
    switch(ParamIndex)
    {
        case 0:
            pParamInfo->ParamType       = ptFileName;
            pParamInfo->Caption         = "database file";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->IsSeparator     = true;
            pParamInfo->SeparatorCaption= "database";
            pParamInfo->CallBackId      = DATABASE_FILE_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (databaseFile);
            break;
            
        case 1:
            pParamInfo->ParamType = ptButton;
            pParamInfo->Caption = "reload database";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->CallBackId   = RELOAD_DATABASE_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(reloadDatabase);
            break;
            
        case 2:
            pParamInfo->ParamType = ptListBox;
            pParamInfo->Caption = "first dimension";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->ListBoxStrings = "duration,spectral centroid mean,spectral centroid std,spectral spread mean,spectral spread std,spectral flatness mean,spectral flatness std,RMS mean,RMS std,fund frequency mean,fund frequency std,frequency probability mean,frequency probability std,combined 1,random";
            pParamInfo->DefaultValue = 13;
            pParamInfo->SeparatorCaption = "2D representation";
            pParamInfo->CallBackId   = FIRST_DIMENSION_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(firstDim);
            break;
        
        case 3:
            pParamInfo->ParamType = ptListBox;
            pParamInfo->Caption = "second dimension";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->ListBoxStrings = "duration,spectral centroid mean,spectral centroid std,spectral spread mean,spectral spread std,spectral flatness mean,spectral flatness std,RMS mean,RMS std,frequency mean,frequency std,frequency probability mean,frequency probability std,combined 2,random";
            pParamInfo->DefaultValue = 13;
            pParamInfo->CallBackId   = SECOND_DIMENSION_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(secondDim);
            break;
            
        case 4:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "polyphony";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 1;
            pParamInfo->MaxValue = 20;
            pParamInfo->DefaultValue = 10;
            pParamInfo->Format = DEFAULT_FORMAT_INTEGER;
            pParamInfo->IsSeparator = true;
            pParamInfo->SeparatorCaption = "closest samples";
            pParamInfo->CallBackId   = POLYPHONY_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(numberOfPoints);
            break;
            
        case 5:
            pParamInfo->ParamType = ptTextField;
            pParamInfo->Caption = "samples list";
            pParamInfo->IsInput = false;
            pParamInfo->IsOutput = true;
            pParamInfo->setEventClass(closestSamplesPath);
            break;
            
        case 6:
            pParamInfo->ParamType = ptArray;
            pParamInfo->Caption = "samples coeffs";
            pParamInfo->IsInput = false;
            pParamInfo->IsOutput = true;
            pParamInfo->setEventClass(coeffs);
            break;
            
        case 7:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "attack time";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 0.0f;
            pParamInfo->MaxValue = 2000.0f;
            pParamInfo->DefaultValue = 50.0f;
            pParamInfo->Format = "%.2f";
            pParamInfo->Symbol = "ms";
            pParamInfo->Scale = scLog;
            pParamInfo->CallBackId   = ATTACK_TIME_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(attackDuration);
            break;
            
        case 8:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "release time";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 0.0f;
            pParamInfo->MaxValue = 2000.0f;
            pParamInfo->DefaultValue = 200.0f;
            pParamInfo->Format = "%.2f";
            pParamInfo->Symbol = "ms";
            pParamInfo->Scale = scLog;
            pParamInfo->CallBackId   = RELEASE_TIME_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(releaseDuration);
            break;
        
        case 9:
            pParamInfo->ParamType = ptSwitch;
            pParamInfo->Caption = "on resynthesis";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->IsSeparator = true;
            pParamInfo->DefaultValue = true;
            pParamInfo->SeparatorCaption = "resynthesis";
            pParamInfo->CallBackId   = ON_RESYNTHESIS_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(toggleResynth);
            break;
            
        case 10:
            pParamInfo->ParamType = ptAudio;
            pParamInfo->Caption = "audio out 1";
            pParamInfo->IsInput = false;
            pParamInfo->IsOutput = true;
            pParamInfo->setEventClass(rAudioOut);
            break;
        
        case 11:
            pParamInfo->ParamType = ptAudio;
            pParamInfo->Caption = "audio out 2";
            pParamInfo->IsInput = false;
            pParamInfo->IsOutput = true;
            pParamInfo->setEventClass(lAudioOut);
            break;
            
        case 12:
            pParamInfo->ParamType = ptSwitch;
            pParamInfo->Caption = "cursor down";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = true;
            pParamInfo->IsSeparator = true;
            pParamInfo->SeparatorCaption = "controls";
            pParamInfo->CallBackId   = CURSOR_DOWN_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(cursorDown);
            break;
            
        case 13:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "cursor X";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 0.0f;
            pParamInfo->MaxValue = 1.0f;
            pParamInfo->DefaultValue = 0.0f;
            pParamInfo->Format = "%.2f";
            pParamInfo->CallBackId   = CURSOR_X_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(cursorX);
            break;
            
        case 14:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "cursor Y";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 0.0f;
            pParamInfo->MaxValue = 1.0f;
            pParamInfo->DefaultValue = 0.0f;
            pParamInfo->Format = "%.2f";
            pParamInfo->CallBackId   = CURSOR_Y_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(cursorY);
            break;
            
        case 15:
            pParamInfo->ParamType = ptDataFader;
            pParamInfo->Caption = "cursor radius";
            pParamInfo->IsInput = true;
            pParamInfo->IsOutput = false;
            pParamInfo->MinValue = 1.0f;
            pParamInfo->MaxValue = 20.0f;
            pParamInfo->DefaultValue = 5.0f;
            pParamInfo->Format = DEFAULT_FORMAT_INTEGER;
            pParamInfo->Symbol = "pix";
            pParamInfo->CallBackId   = CURSOR_RADIUS_CBID;
            pParamInfo->CallBackType = ctNormal;
            pParamInfo->setEventClass(cursorRadius);
            break;

        default:
            break;
    }
    
}

void DatabasePlayer::onCallBack(TUsineMessage *Message)
{
    try {
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
            switch (Message->wParam) {

                case DATABASE_FILE_CBID:
                    dbName = std::string(databaseFile.getPChar());
                    rAudioOut.clearAudio();
                    lAudioOut.clearAudio();
                    actionQueue.pushFunc(openDatabaseFunc,this);
                    break;

                case RELOAD_DATABASE_CBID:
                    if(!isDBOpen)
                    {
                        sdkTraceErrorChar("No database open, please open a database first");
                        break;
                    }

                    dbName = std::string(databaseFile.getPChar());
                    rAudioOut.clearAudio();
                    lAudioOut.clearAudio();
                    if(!isLoading.load(std::memory_order_acquire))
                    {
                        actionQueue.pushFunc(openDatabaseFunc,this);
                    }
                    break;

                case FIRST_DIMENSION_CBID:
                    if(firstDim.getData() >= 0 && firstDim.getData() < (int)dimensions_x.size())
                    {
                        dimX = dimensions_x[static_cast<size_t>(firstDim.getData())];
                        actionQueue.pushFunc(requestFunction,this);

                    }
                    break;

                case SECOND_DIMENSION_CBID:
                    if(secondDim.getData() >= 0 && secondDim.getData() < (int)dimensions_y.size())
                    {
                        dimY = dimensions_y[static_cast<size_t>(secondDim.getData())];
                        actionQueue.pushFunc(requestFunction, this);
                    }
                    break;

                case POLYPHONY_CBID:
                    listOfOutputSamples.setNbPoints(static_cast<int>(numberOfPoints.getData()));
                    break;

                case ATTACK_TIME_CBID:
                    listOfOutputSamples.setAttackDuration(attackDuration.getData(), static_cast<float>(sdkGetSampleRate()), sdkGetBlocSize());
                    break;

                case RELEASE_TIME_CBID:
                    listOfOutputSamples.setReleaseDuration(releaseDuration.getData(), static_cast<float>(sdkGetSampleRate()), sdkGetBlocSize());
                    break;

                case ON_RESYNTHESIS_CBID:
                    activeResynth = toggleResynth.getData();
                    if(!activeResynth)
                    {
                        rAudioOut.clearAudio();
                        lAudioOut.clearAudio();
                    }
                    break;

                case CURSOR_DOWN_CBID:
                    isCursorOn = cursorDown.getData();
                    actionQueue.pushFunc(searchFunc,this);
                    if(!mouseInfo.leftBtnDown && !isCursorOn)
                    {
                        listOfOutputSamples.softClear();
                    }
                    sdkRepaintPanel();
                    break;

                case CURSOR_X_CBID:
                    if(!mouseInfo.leftBtnDown)
                    {
                        circleCursor.x = cursorX.getData();
                    }
                    sdkRepaintPanel();
                    break;

                case CURSOR_Y_CBID:
                    if(!mouseInfo.leftBtnDown)
                    {
                        circleCursor.y = cursorY.getData();
                    }
                    sdkRepaintPanel();
                    break;

                case CURSOR_RADIUS_CBID:
                    circleCursor.setRadius(cursorRadius.getData() * 0.01f);
                    sdkRepaintPanel();
                    break;
                    
                case SettingsId::POINT_RADIUS :
                    actionQueue.pushFunc(requestFunction, this);
                    break;

                case SettingsId::SPREAD_X :
                    actionQueue.pushFunc(requestFunction, this);
                    break;

                case SettingsId::SPREAD_Y :
                    actionQueue.pushFunc(requestFunction, this);
                    break;

                case SettingsId::NORMALIZE_COEFFS :
                    break;

                default:
                    break;
            }
            
        }
    } catch (...)
    {
        sdkTraceErrorChar("Error on callback in DatabasePlayer module");
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
            sdkTraceErrorChar(std::string("For parameter number " + std::to_string(Message->wParam)));
        }
    }
}

void DatabasePlayer::onProcess()
{
    rAudioOut.setSize(sdkGetBlocSize());
    lAudioOut.setSize(sdkGetBlocSize());
    
    //------ COEFFS -------
    coeffs.setSize((int)listOfOutputSamples.queue.size());

    listOfOutputSamples.updateCoeffs();
    
    bool coeffsNonZero = listOfOutputSamples.sumCoeffs > 1e-5f;
    
    if(coeffsNonZero)
    {
        isCurrentlyPlaying = true;
        remainingFrames = 3 * SPECTRUM_SIZE_4;
    } else if(!coeffsNonZero && isCurrentlyPlaying)
    {
        if(remainingFrames > 0)
        {
            remainingFrames -= sdkGetBlocSize();
        } else {
            lBuffer.clearBuffer();
            rBuffer.clearBuffer();
            isCurrentlyPlaying = false;
        }
        
    }
    
    //---------------------
    
    //------ RESULTS ------
    for(int i = 0; i < listOfOutputSamples.queue.size(); i++)
    {
        coeffs.setArrayData(i,listOfOutputSamples.queue[i].coeff.getCoeff());
    }

    closestSamplesPath.setPChar(listOfOutputSamples.getCommaText().c_str());

    //---------------------
    
    //---- RESYNTHESIS ----
    if(isLoading) return;
    if(!activeResynth) return;
    if(ifftCounter >= SPECTRUM_SIZE_4 || ifftCounter == 0)
    {
        if(ifftCounter != 0)
        {
            ifftCounter -= SPECTRUM_SIZE_4;
        }

        MagPtr = listOfOutputSamples.globalMags.data();
        if(MagPtr)
        {
            actionQueue.pushIFFTFunc(ifftFunction, this);
        }
        
    }
    
    if(!isLoading){
        rBuffer.readAndZero(rAudioOut.getDataAddr(), sdkGetBlocSize());
        lBuffer.readAndZero(lAudioOut.getDataAddr(), sdkGetBlocSize());
    } else {
        rAudioOut.clearAudio();
        lAudioOut.clearAudio();
    }

    ifftCounter += sdkGetBlocSize();
    //---------------------
}

void DatabasePlayer::computeIFFTMag(float* magPtr)
{
    assert(magPtr != nullptr);
    
    std::fill(rReal.begin(), rReal.end(), 0.0f);
    std::fill(rIm.begin(), rIm.end(), 0.0f);
    std::fill(lReal.begin(), lReal.end(), 0.0f);
    std::fill(lIm.begin(), lIm.end(), 0.0f);

    int idx = 0;
    int lIdx = 0;
    
    for(int i = 0; i < SPECTRUM_SIZE_2; i++)
    {
        idx = rand() % LUT_SIZE;
        lIdx = rand() % LUT_SIZE;
        
        rReal[i] = magPtr[i] * trigoLUT[idx].cos;
        rIm[i] = magPtr[i] * trigoLUT[idx].sin;
        lReal[i] = magPtr[i] * trigoLUT[lIdx].cos;
        lIm[i] = magPtr[i] * trigoLUT[lIdx].sin;
    }
    
    rIm[0] = 0.0f;
    rIm[SPECTRUM_SIZE_2 - 1] = 0.0f;
    lIm[0] = 0.0f;
    lIm[SPECTRUM_SIZE_2 - 1] = 0.0f;
    
    if(isLoading)
    {
        std::fill(rReal.begin(),rReal.end(),0.0f);
        std::fill(rIm.begin(),rIm.end(),0.0f);
        std::fill(lReal.begin(),lReal.end(),0.0f);
        std::fill(lIm.begin(),lIm.end(),0.0f);
    }
    
    sdkFFTInverse(ifftPtr, rTempBuffer.data(), rReal.data(), rIm.data());
    sdkFFTInverse(ifftPtr, lTempBuffer.data(), lReal.data(), lIm.data());
    
    for(int i = 0; i < SPECTRUM_SIZE; i++)
    {
        rTempBuffer[i] *= hanning[i];
        lTempBuffer[i] *= hanning[i];
    }
    
    rBuffer.addPlus(rTempBuffer.data(), SPECTRUM_SIZE);
    rBuffer.moveWriteIdx(SPECTRUM_SIZE_4);
    lBuffer.addPlus(lTempBuffer.data(), SPECTRUM_SIZE);
    lBuffer.moveWriteIdx(SPECTRUM_SIZE_4);

}

void DatabasePlayer::onPaint()
{
    try {
        
        sdkCriticalSectionLock(csPaint);
            
            if (numberOfPointsToDraw > 0 && numberOfPointsToDraw == pointsToDraw.size() && numberOfPointsToDraw == colorsToDraw.size())
            {
                sdkDrawMultiPoints(pointsToDraw.data(), colorsToDraw.data(), canvasPointRadius, false, numberOfPointsToDraw);
            }

        sdkCriticalSectionUnLock(csPaint);
        
        if(mouseInfo.leftBtnDown || isCursorOn)
        {
            sdkDrawPoint(TPointF(circleCursor.x,circleCursor.y),
                         sdkAlphaColor(0.5f,0.5f,0.5f,0.2f),
                         2.0f*circleCursor.radius*(float)DEFAULT_PANEL_WIDTH,
                         true);
        }
        
    } catch (const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer OnPaint function : ") + std::string(e.what()));
    }

}

void DatabasePlayer::onMouseMove(TShiftState Shift, float X, float Y)
{
    if(mouseInfo.leftBtnDown)
    {
        mouseInfo.storeNewCoords(X,Y);
        circleCursor.x = X;
        circleCursor.y = Y;
    }
}

void DatabasePlayer::onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y)
{
    if(MouseButton == mbLeft)
    {
        mouseInfo.leftBtnDown = true;
        mouseInfo.storeNewCoords(X,Y);
        circleCursor.x = X;
        circleCursor.y = Y;
        
        actionQueue.pushFunc(searchFunc, this);
    }
    
}

void DatabasePlayer::onMouseUp(TMouseButton MouseButton, TShiftState Shift, float X, float Y)
{
    if(MouseButton == mbLeft)
    {
        mouseInfo.leftBtnDown = false;
        if(!isCursorOn)
        {
            listOfOutputSamples.softClear();
            actionQueue.pushFunc(searchFunc, this);
        }

    }
}

void DatabasePlayer::onMouseWheel(TShiftState Shift, int WheelDelta)
{
    if(isCursorOn || mouseInfo.leftBtnDown)
    {
        if(circleCursor.radius > (minCursorRadius * 0.01f) && ( WheelDelta < 0 ))
        {
            circleCursor.setRadius(circleCursor.radius + WheelDelta * 0.005f);
        }
        else if (circleCursor.radius < (maxCursorRadius * 0.01f) && ( WheelDelta > 0 ))
        {
            circleCursor.setRadius(circleCursor.radius + WheelDelta * 0.005f);
        }
    }

    
}

bool DatabasePlayer::sendRequestToDB(const std::string firstDimension,const std::string secondDimension)
{

    try {
        std::string query = queryBuilder(firstDimension, secondDimension);
        
        sqlite3_stmt* stmt = nullptr;
        std::vector<Point2D> pendingRequestData = requestData;
        
        std::vector<TPointF> pendingPointsToDraw = pointsToDraw;
        std::vector<TUsineColor> pendingColorsToDraw = colorsToDraw;
        
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmt) == SQLITE_ROW)
            {
                float normX = static_cast<float>(sqlite3_column_double(stmt, 0));
                float normY = static_cast<float>(sqlite3_column_double(stmt, 1));
                int id = static_cast<int>(sqlite3_column_int(stmt, 2)) - 1; // NOTE : ID IS STARTING FROM 1 IN THE DB
                
                if(id >= 0 && id < (int)pendingRequestData.size())
                {
                    float coordX = std::powf(normX, spreadFactorX);
                    float coordY = std::powf(normY, spreadFactorY);
                    pendingRequestData[id].dim1 = coordX;
                    pendingRequestData[id].dim2 = 1.0f - coordY;
                    pendingRequestData[id].color = xy_to_rgb_colorwheel_radial(coordX,coordY);
                    
                    
                    //
                    pendingPointsToDraw[id] = TPointF(coordX,1.0f - coordY);
                    pendingColorsToDraw[id] = pendingRequestData[id].color;
                    
                } else {
                    sdkTraceErrorChar("Error during request with id " + std::to_string(id) + " : " + std::string(sqlite3_errmsg(db)));
                    sqlite3_finalize(stmt);
                    return false;
                }
            }
        } else {
            sdkTraceErrorChar("Error during request to the database :" + std::string(sqlite3_errmsg(db)));
            sdkTraceErrorChar(std::string("The error occurred for this query : " + query));
            return false;
        }
        sqlite3_finalize(stmt);
        
        std::vector<Point2D*> ptrVector;
        ptrVector.reserve(pendingRequestData.size());
        for(auto& point : pendingRequestData)
        {
            ptrVector.push_back(&point);
        }
        
        KDTree pendingTree(2);
        pendingTree.root = pendingTree.buildTree(ptrVector, 0);
        
        sdkCriticalSectionLock(csPaint);
        
            requestData = std::move(pendingRequestData);
            kdTree.root = std::move(pendingTree.root);
            if(!kdTree.root)
            {
                sdkCriticalSectionUnLock(csPaint);
                return false;
            }
        
            pointsToDraw = std::move(pendingPointsToDraw);
            colorsToDraw = std::move(pendingColorsToDraw);
            
        sdkCriticalSectionUnLock(csPaint);

        
    } catch (const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer Request Function : ") + std::string(e.what()));
        return false;
    }
    
    return true;
}

std::string DatabasePlayer::queryBuilder(const std::string& firstDimension, const std::string& secondDimension)
{
    // The SQL query used
    /*
    std::string query =
    "SELECT\n"
    "    (" + firstDimension + " - min_max.min_" + firstDimension + ") / "
    "    (min_max.max_" + firstDimension + " - min_max.min_" + firstDimension + ") AS norm_x,\n"
    "    (" + secondDimension + " - min_max.min_" + secondDimension + ") / "
    "    (min_max.max_" + secondDimension + " - min_max.min_" + secondDimension + ") AS norm_y,\n"
    "    id\n"
    "FROM FileResults,\n"
    "     (SELECT \n"
    "         MIN(" + firstDimension + ") AS min_" + firstDimension + ", MAX(" + firstDimension + ") AS max_" + firstDimension + ",\n"
    "         MIN(" + secondDimension + ") AS min_" + secondDimension + ", MAX(" + secondDimension + ") AS max_" + secondDimension + "\n"
    "      FROM FileResults) AS min_max;";
    */
    std::string queryFunction = "SELECT\n";
    std::string normX, normY;
    std::string subSelectFunc, subSelectNorm, subSelectEnd;
    bool needsMinMax = (firstDimension != "random" || secondDimension != "random");
    
        // norm_x
    if (firstDimension == "random") {
        normX = "(abs(random()) / 9223372036854775807.0) AS norm_x,\n";
    } else {
        normX = "(" + firstDimension + " - min_max.min_" + firstDimension + ") / "
        + "(min_max.max_" + firstDimension + " - min_max.min_" + firstDimension + ") AS norm_x,\n";
        
        subSelectNorm += "MIN(" + firstDimension + ") AS min_" + firstDimension + ", MAX(" + firstDimension + ") AS max_" + firstDimension;
    }
    
        // norm_y
    if (secondDimension == "random") {
        normY = "(abs(random()) / 9223372036854775807.0) AS norm_y,\n";
    } else {
        normY = "(" + secondDimension + " - min_max.min_" + secondDimension + ") / "
        + "(min_max.max_" + secondDimension + " - min_max.min_" + secondDimension + ") AS norm_y,\n";
        
        if (!subSelectNorm.empty()) {
            subSelectNorm += ",\n";
        }
        subSelectNorm += "MIN(" + secondDimension + ") AS min_" + secondDimension + ", MAX(" + secondDimension + ") AS max_" + secondDimension;
    }
    

    if (needsMinMax) {
        subSelectFunc = "(SELECT\n";
        subSelectEnd = "\nFROM FileResults) AS min_max";
    }
    
    std::string finalQuery;
    finalQuery += queryFunction;
    finalQuery += normX;
    finalQuery += normY;
    finalQuery += "id\nFROM FileResults";
    if (needsMinMax) {
        finalQuery += ",\n";
        finalQuery += subSelectFunc;
        finalQuery += subSelectNorm;
        finalQuery += subSelectEnd;
    }
    finalQuery += ";\n";

    return finalQuery;
}


void DatabasePlayer::findEveryNearestPoint(const CircleCursor& circleCursor, std::vector<PointDistance2D>& listOfSamplesInCircle )
{
    listOfSamplesInCircle.clear();
    kdTree.searchInRange(circleCursor.x, circleCursor.y, circleCursor.radius, listOfSamplesInCircle);
    std::sort(listOfSamplesInCircle.begin(),listOfSamplesInCircle.end(), std::greater<>());
    
    listOfOutputSamples.compareListsAndUpdate(listOfSamplesInCircle);
    
    int step = ((int)listOfSamplesInCircle.size()) / listOfOutputSamples.NbPoints;
    if(step == 0)
        step = 1;
    if(listOfSamplesInCircle.size() > 0)
    {
        for(size_t i = 0; i < listOfSamplesInCircle.size(); i = i + step)
        {
            if(!listOfSamplesInCircle[i].point) continue;
            if(listOfSamplesInCircle[i].point->mag == nullptr) continue;
            
            if(listOfSamplesInCircle[i].point->mag->vec.size() != SPECTRUM_SIZE_2) continue;
            
            int checkPos = listOfOutputSamples.isInQueue(listOfSamplesInCircle[i].point->samplePath);
            if(checkPos < 0)
            {
                int idx = listOfOutputSamples.findEmptySpot();
                if(idx >= 0)
                {
                        // change here law of distance
                    listOfOutputSamples.putInQueue(listOfSamplesInCircle[i].point->samplePath,
                                                   listOfSamplesInCircle[i].point->mag->vec.data(),
                                                   listOfSamplesInCircle[i].distance * listOfSamplesInCircle[i].distance,
                                                   idx);
                    
                }
                    // if we are here there is no empty spots...
                    // we can create a waiting list here
            }
                // not in queue
            
        }
    }

}




bool DatabasePlayer::openDB()
{
    int spectrumSize = 0;
    try {
        // ------FFT SIZE-------
        std::string queryFFT =
        "SELECT "
        "fftSizeValue "
        "FROM FileFFTSize ";
        
        sqlite3_stmt* stmtFFT = nullptr;
        
        if (sqlite3_prepare_v2(db, queryFFT.c_str(), -1, &stmtFFT, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmtFFT) == SQLITE_ROW) {
                spectrumSize = sqlite3_column_int(stmtFFT, 0);
            } else {
                sdkTraceErrorChar("No FFT size entry found in FileFFTSize table");
                sqlite3_finalize(stmtFFT);
                return false;
            }
        } else {
            sdkTraceErrorChar("Error preparing FFT size query : " + std::string(sqlite3_errmsg(db)));
            return false;
        }
        sqlite3_finalize(stmtFFT);
        
        // ------POINTS-------

        int pointCount = 0;
        sqlite3_stmt* stmtCount = nullptr;
        if (sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM FileResults", -1, &stmtCount, nullptr) == SQLITE_OK) {
            if (sqlite3_step(stmtCount) == SQLITE_ROW) {
                pointCount = sqlite3_column_int(stmtCount, 0);
            }
        }
        sqlite3_finalize(stmtCount);
        
        std::vector<std::shared_ptr<const PointMag>> pendingPointMags;
        std::vector<Point2D> pendingRequestPoints;
        pendingRequestPoints.reserve(pointCount);
        
        std::vector<TPointF> pendingPointsToDraw;
        pendingPointsToDraw.resize(pointCount);
        
        std::vector<TUsineColor> pendingColorsToDraw;
        pendingColorsToDraw.resize(pointCount);
        
        
        
        
        std::string queryPoints = "SELECT id, filePath, mag_mean, tsne_x, tsne_y FROM FileResults ORDER BY id ASC";
        sqlite3_stmt* stmtPoints = nullptr;
        
        if (sqlite3_prepare_v2(db, queryPoints.c_str(), -1, &stmtPoints, nullptr) == SQLITE_OK) {
            while (sqlite3_step(stmtPoints) == SQLITE_ROW) {
                int id = sqlite3_column_int(stmtPoints, 0);
                
                const unsigned char* filepath_text = sqlite3_column_text(stmtPoints, 1);
                std::string filePath = filepath_text ? reinterpret_cast<const char*>(filepath_text) : "";
                
                const void* magBlob = sqlite3_column_blob(stmtPoints, 2);
                int magBlobSize = sqlite3_column_bytes(stmtPoints, 2);
                
                float normX = static_cast<float>(sqlite3_column_double(stmtPoints, 3));
                float normY = static_cast<float>(sqlite3_column_double(stmtPoints, 4));
                
                if(magBlob && magBlobSize > 0)
                {
                    std::vector<float> magData(magBlobSize / sizeof(float));
                    std::memcpy(magData.data(), magBlob, magBlobSize);
                    
                    auto magPtr = std::make_shared<PointMag>(PointMag{std::move(magData),id});
                    
                    Point2D point;
                    point.samplePath = filePath;
                    point.uID = id;
                    point.dim1 = normX;
                    point.dim2 = normY;
                    point.mag = magPtr;
                    
                    pendingPointMags.push_back(magPtr);
                    pendingRequestPoints.push_back(point);

                } else {
                    sdkTraceErrorChar("Error fetching magnitudes data, please recreate the database");
                    return false;
                }
                
            }
        } else {
            std::cerr << "SQLite error: " << sqlite3_errmsg(db) << "\n";
            sdkTraceErrorChar("Error preparing points creation query : " + std::string(sqlite3_errmsg(db)));
            return false;
        }
        sqlite3_finalize(stmtPoints);
                
        // ------APPLY NEW DB-------

        sdkCriticalSectionLock(csPaint);
        
            rAudioOut.clearAudio();
            lAudioOut.clearAudio();
            
            listOfOutputSamples.hardClear();
            listOfOutputSamples.clearMags();
            
            requestData = std::move(pendingRequestPoints);
            pointsMags = std::move(pendingPointMags);
        
            pointsToDraw = std::move(pendingPointsToDraw);
            colorsToDraw = std::move(pendingColorsToDraw);
            numberOfPointsToDraw = pointCount;
            if(spectrumSize > 0)
            {
                SPECTRUM_SIZE = spectrumSize;
                SPECTRUM_SIZE_2 = SPECTRUM_SIZE / 2;
                SPECTRUM_SIZE_4 = SPECTRUM_SIZE / 4;
                SPECTRUM_OVERLAP_SIZE = SPECTRUM_SIZE_4;
                sdkFFTDestroy(ifftPtr);
                ifftPtr = sdkFFTCreate(SPECTRUM_SIZE);
                rTempBuffer.clear();
                rBuffer.reset();
                lTempBuffer.clear();
                lBuffer.reset();
                rReal.clear();
                rIm.clear();
                lReal.clear();
                lIm.clear();
                rReal.resize(SPECTRUM_SIZE_2);
                rIm.resize(SPECTRUM_SIZE_2);
                lReal.resize(SPECTRUM_SIZE_2);
                lIm.resize(SPECTRUM_SIZE_2);
                rTempBuffer.resize(SPECTRUM_SIZE);
                lTempBuffer.resize(SPECTRUM_SIZE);
                InitializeHanningWindow(hanning,SPECTRUM_SIZE);
                ifftCounter = 0;
                
            } else {
                sdkTraceErrorChar("Error in FFT Size, please restart the analysis of the sound folder");
                sdkCriticalSectionUnLock(csPaint);
                return false;
            }

        sdkCriticalSectionUnLock(csPaint);
        
    } catch (const std::exception& e) {
        sdkTraceErrorChar(std::string("Error in DatabasePlayer Request FFT size function : ") + std::string(e.what()));
        return false;
    }
    
    return true;
}


void DatabasePlayer::generateLUT()
{
    int N = LUT_SIZE;
    trigoLUT.resize(N);
    
    for(int i = 0; i < N; i++)
    {
        float theta = 2.0f * static_cast<float>(M_PI) * static_cast<float>(i)/ static_cast<float>(N);
        trigoLUT[i].cos = std::cosf(theta);
        trigoLUT[i].sin = std::sinf(theta);
    }
    
}

void DatabasePlayer::InitializeHanningWindow(std::vector<float>& hanningWindow, int size)
{
    hanningWindow.resize(size);
    for (int i = 0; i < size; ++i)
    {
        hanningWindow[i] = 0.5f * (1.0f - std::cosf(2.0f * static_cast<float>(M_PI) * i / (size - 1)));
        hanningWindow[i] /= size;
    }
}



