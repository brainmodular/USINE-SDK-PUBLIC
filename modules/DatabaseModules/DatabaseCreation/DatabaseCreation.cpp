//-----------------------------------------------------------------------------
//@file
//    DatabaseCreation.cpp
//
//@author
//    BrainModular team
//
//@brief
//    Implementation of the DatabaseCreation class.
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

#include "DatabaseCreation.h"

//----------------------------------------------------------------------------
// callback ids
//----------------------------------------------------------------------------
constexpr NativeInt SOUNDS_FOLDER_CBID   = 0x00F100B0;
constexpr NativeInt DATABASE_FOLDER_CBID = 0x00F100B1;
constexpr NativeInt DATABASE_NAME_CBID   = 0x00F100B2;
constexpr NativeInt START_ANALYSIS_CBID  = 0x00F100B3;
constexpr NativeInt STOP_ANALYSIS_CBID   = 0x00F100B4;

void CreateModule(void*& pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
    pModule = new DatabaseCreation();
}

void DestroyModule(void* pModule)
{
    delete ((DatabaseCreation*)pModule);
}

DatabaseCreation::DatabaseCreation() :
m_HanningWindow(BUFFER_SIZE),
spectrumHanningWindow(SPECTRUM_SIZE),
m_Re(BUFFER_SIZE/2+1),
m_Im(BUFFER_SIZE/2+1),
descriptors(BUFFER_SIZE, BUFFER_SIZE/2, static_cast<float>(sdkGetSampleRate()), 25)
{
    InitializeHanningWindow(m_HanningWindow,BUFFER_SIZE);
    InitializeHanningWindow(spectrumHanningWindow,SPECTRUM_SIZE);
    
    // could be changed to a more dynamic way
    std::vector<std::string> features = {
        "spectralCentroid_mean",
        "spectralCentroid_std",
        "spectralSpread_mean",
        "spectralSpread_std",
        "spectralFlatness_mean",
        "spectralFlatness_std",
        "RMS_mean",
        "RMS_std",
        "f0_frequency_mean",
        "f0_frequency_std",
        "f0_probability_mean",
        "f0_probability_std"
    };
    for (int i = 1; i <= 13; ++i)
    {
        features.push_back("mfcc_mean_" + std::to_string(i));
    }
    for (int i = 1; i <= 13; ++i)
    {
        features.push_back("mfcc_std_" + std::to_string(i));
    }
    
    featuresListForTSNE = features;
}

DatabaseCreation::~DatabaseCreation()
{
    needToStopAnalysis.store(true, std::memory_order_release);
    sdkCriticalSectionLock(criticalSection);
    try {
        sqlite3_close(db);
        sdkFFTDestroy(fftPtr);
        sdkFFTDestroy(fftSpectrumPtr);
    } catch (...) {
        sdkTraceLogChar("Error in DatabaseCreation destructor");
    }
    sdkCriticalSectionUnLock(criticalSection);
    sdkCriticalSectionDestroy(criticalSection);
    
}

const AnsiCharPtr UserModuleBase::MODULE_NAME = "Database Creation";
const AnsiCharPtr UserModuleBase::MODULE_DESC = "DatabaseCreation";
const AnsiCharPtr UserModuleBase::MODULE_VERSION = "1.0";

void GetBrowserInfo(TModuleInfo* pModuleInfo)
{
    pModuleInfo->Name = UserModuleBase::MODULE_NAME;
    pModuleInfo->Description = UserModuleBase::MODULE_DESC;
    pModuleInfo->Version = UserModuleBase::MODULE_VERSION;
}

void DatabaseCreation::onGetModuleInfo(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo)
{
    pModuleInfo->Name = MODULE_NAME;
    pModuleInfo->Description = MODULE_DESC;
    pModuleInfo->ModuleType = mtSimple;
    pModuleInfo->BackColor = sdkGetUsineColor(clDataModuleColor);
    pModuleInfo->NumberOfParams = 7;
    pModuleInfo->Version = MODULE_VERSION;
    pModuleInfo->DontProcess = TRUE;
    pModuleInfo->CanBeRandomized = FALSE;
}

void processThread(void* pModule, TThreadPtr pThread)
{
    DatabaseCreation* module = static_cast<DatabaseCreation*>(pModule);
    try {
        module->isProcessing = true;
        module->needToStopAnalysis.store(false,std::memory_order_release);
        module->sdkCriticalSectionLock(module->criticalSection);
        module->AudioFilesAnalysis();
    } catch (...) {
        sdkTraceLogChar("Error in processThread section of Database Creation");
    }
    module->isProcessing = false;
    module->sdkCriticalSectionUnLock(module->criticalSection);
}

void progressBarThread(void* pModule, TThreadPtr pThread)
{
    DatabaseCreation* module = static_cast<DatabaseCreation*>(pModule);
    try {
        bool startedTsne = false;
        while(module->isProcessing)
        {
            if(module->analysisOver.load(std::memory_order_acquire))
            {
                return;
            }
            
            std::string progString = "Finishing the analysis";
            module->progCounter = (module->progCounter + 1) % 4;
            std::string dotsString(module->progCounter, '.');
            
            module->sdkProgressBar((progString + dotsString).c_str(), module->white, module->tsneState.currentIter.load(std::memory_order_acquire));
            


            if (module->tsneState.iterStarted.load(std::memory_order_acquire))
            {
                if (!startedTsne)
                {
                    sdkTraceChar("Finishing the analysis...");
                    startedTsne = true;
                }
                int step = module->tsneState.currentIter.load(std::memory_order_acquire);
                sdkTraceChar("Step [" + std::to_string(step) + " / 1000]");
            }

            std::unique_lock<std::mutex> lock(module->mtx);
            module->cv.wait_for(lock,std::chrono::milliseconds(500));
            
        }
        startedTsne = false;
    } catch (...) {
        sdkTraceLogChar("Error in progressBarThread section of Database Creation");
    }

}

void DatabaseCreation::onInitModule(TMasterInfo *pMasterInfo, TModuleInfo *pModuleInfo)
{
    fftPtr = sdkFFTCreate(BUFFER_SIZE);
    fftSpectrumPtr = sdkFFTCreate(SPECTRUM_SIZE);

    criticalSection = sdkCriticalSectionCreate("critical section", false);

    thread = sdkThreadCreate("analysis thread", &processThread, tpLow, UINT32_MAX);
    progressThread = sdkThreadCreate("progress thread", &progressBarThread, tpLow, UINT32_MAX);
}

void DatabaseCreation::onGetParamInfo(int ParamIndex, TParamInfo *pParamInfo)
{
    switch(ParamIndex)
    {
        case 0:
            pParamInfo->ParamType       = ptChooseFolder;
            pParamInfo->Caption         = "sounds folder";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->IsSeparator     = true;
            pParamInfo->SeparatorCaption= "Create a database";
            pParamInfo->CallBackId      = SOUNDS_FOLDER_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (folderPath);
            break;
            
        case 1:
            pParamInfo->ParamType       = ptChooseFolder;
            pParamInfo->Caption         = "database folder";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->CallBackId      = DATABASE_FOLDER_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (databasePath);
            break;
        
        case 2:
            pParamInfo->ParamType       = ptTextField;
            pParamInfo->Caption         = "database name";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->CallBackId      = DATABASE_NAME_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (databaseName);
            break;
            
        case 3:
            pParamInfo->ParamType       = ptDataFader;
            pParamInfo->Caption         = "max file duration";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->MinValue        = 100;
            pParamInfo->MaxValue        = 60000;
            pParamInfo->DefaultValue    = 2000;
            pParamInfo->Symbol          = "ms";
            pParamInfo->Format          = "%.0f";
            pParamInfo->Scale           = scLog;
            pParamInfo->IsSeparator     = true;
            pParamInfo->SeparatorCaption= "Parameters";
            pParamInfo->setEventClass   (maxFileLength);
            break;
        
        case 4:
            pParamInfo->ParamType = ptListBox;
            pParamInfo->Caption = "fft size";
            pParamInfo->IsInput = TRUE;
            pParamInfo->IsOutput = FALSE;
            pParamInfo->DefaultValue = 2;
            pParamInfo->MinValue = 0;
            pParamInfo->MaxValue = 4;
            pParamInfo->ListBoxStrings = "1024,2048,4096,8192,16384";
            pParamInfo->setEventClass(fftSize);
            break;
            
        case 5:
            pParamInfo->ParamType       = ptButton;
            pParamInfo->Caption         = "start analysis";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->IsSeparator     = true;
            pParamInfo->SeparatorCaption= "Analysis";
            pParamInfo->CallBackId      = START_ANALYSIS_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (launchAnalysis);
            break;

        case 6:
            pParamInfo->ParamType       = ptButton;
            pParamInfo->Caption         = "stop analysis";
            pParamInfo->IsInput         = true;
            pParamInfo->IsOutput        = false;
            pParamInfo->CallBackId      = STOP_ANALYSIS_CBID;
            pParamInfo->CallBackType    = ctNormal;
            pParamInfo->setEventClass   (stopAnalysis);
            break;
            
        default:
            break;
    }
    
}

void DatabaseCreation::onCallBack(TUsineMessage *Message)
{
    try {
        if (Message->message == NOTIFY_MSG_USINE_CALLBACK && Message->lParam == MSG_CHANGE)
        {
            switch (Message->wParam) {
                // folder path
                case SOUNDS_FOLDER_CBID:
                    if (fsx::exists(folderPath.getPChar()) && fsx::is_directory(folderPath.getPChar()))
                    {
                        if(GetAudioPaths(folderPath.getPChar()))
                        {
                            isValidDirectory = true;
                        } else
                        {
                            isValidDirectory = false;
                        }
                    } else {
                        sdkTraceErrorChar("Please specify a valid sound folder.\nUnexisting or deleted folder : " + std::string(folderPath.getPChar()));
                    }
                    break;

                // database path
                case DATABASE_FOLDER_CBID:
                    if(!fsx::exists(databasePath.getPChar()) || !fsx::is_directory(databasePath.getPChar()))
                    {
                        sdkTraceErrorChar("Please specify a valid database folder\nCurrent database folder : " + std::string(databasePath.getPChar()));
                    }
                    break;

                // database name
                case DATABASE_NAME_CBID:
                    dbName.clear();
                    dbName = databaseName.getPChar();
                    if(dbName.size() > 0)
                    {
                        dbName.append(".usine-analysis");
                        isValidName = true;
                    } else {
                        isValidName = false;
                    }
                    break;

                // launch analysis
                case START_ANALYSIS_CBID:
                    if(isValidDirectory)
                    {
                        if(isValidName)
                        {
                            if(!isProcessing)
                            {
                                sdkThreadRestart(thread);
                            } else {
                                sdkTraceErrorChar("The module is already processing");
                            }
                        } else {
                            sdkTraceErrorChar("Please specify a name for the database");
                        }
                    } else {
                        sdkTraceErrorChar("Please specify a valid sound folder \n Current sound folder : " + std::string(folderPath.getPChar()));
                    }
                    break;

                // stop analysis
                case STOP_ANALYSIS_CBID:
                    needToStopAnalysis = true;
                    break;
                    
                default:
                    break;
            }
            
        }
    } catch (...)
    {
        sdkTraceLogChar("Error on callback in DatabaseCreation module");
    }
}

void DatabaseCreation::InitializeHanningWindow(std::vector<float>& hanningWindow, int size)
{
    hanningWindow.resize(size);
    for (int i = 0; i < size; ++i)
    {
        hanningWindow[i] = 0.5f * (1.0f - std::cosf(2.0f * static_cast<float>(M_PI) * i / (size - 1)));
    }
}

void DatabaseCreation::ApplyHanningWindow(TPrecision* dataPtr, Features& features, int buffer_size)
{
    for (int i = 0; i < buffer_size; ++i)
    {
        features.windowedAudioBuffer[i] = dataPtr[i] * m_HanningWindow[i];
    }
}

bool DatabaseCreation::GetAudioPaths(const std::string dirPath)
{
    m_AudioFilePaths.clear();
    int fileNumbers = 0;
    int audioFilesNumber = 0;
    
    std::vector<std::string> allFiles;
    try {
        fsx::list_regular_files(dirPath, allFiles);
    } catch (const std::exception& e) {
        sdkTraceErrorChar(std::string("Error scanning folder : ") + dirPath + " — " + e.what());
    }

    for (const std::string& path : allFiles)
    {
        std::string filename = fsx::filename(path);

        if (!filename.empty() && filename[0] == '.')
            continue;
        
        fileNumbers++;
        
        auto ext = fsx::extension(path);
        std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
        
        if (ext == ".wav" || ext == ".aiff" || ext == ".aif")
        {
            try
            {
                m_AudioFilePaths.push_back(path);
                audioFilesNumber++;
            }
            catch (const std::exception& e)
            {
                sdkTraceErrorChar(std::string("Error reading file : ") + path + " — " + e.what());
            }
        }
    }
    
    return (audioFilesNumber > 0);
}

void DatabaseCreation::AudioFilesAnalysis()
{
    analysisOver.store(false, std::memory_order_release); // for the progress bar thread

    sdkProgressBar("Scanning sounds directory", white);
    GetAudioPaths(folderPath.getPChar());

    int fileFftSize = (1 << static_cast<int>(fftSize.getData())) * 1024;
    if(fileFftSize < 1024 || fileFftSize > 16384)
    {
        sdkTraceErrorChar("Bad FFT Size");
        return;
    }
    
    std::vector<std::string> audioPaths = m_AudioFilePaths;
    
    sdkFFTDestroy(fftSpectrumPtr);
    fftSpectrumPtr = sdkFFTCreate(fileFftSize);

    
    InitializeHanningWindow(spectrumHanningWindow, fileFftSize);
    
    int fileNumber = 0;
    
    auto startTime = std::chrono::steady_clock::now();

    std::string dbPathName = std::string(databasePath.getPChar()) + "/" + dbName;
    
    if(fsx::exists(dbPathName))
    {

        if(!fsx::remove(dbPathName))
        {
            sdkTraceErrorChar("Error while trying to delete an older database at :");
            sdkTraceErrorChar(dbPathName);
            return;
        }
        else {
            sdkTraceChar("Removed an already existing older database at " + dbPathName);
        }
    }
    
    int ec = sqlite3_open(dbPathName.c_str(), &db);
    if(ec != SQLITE_OK)
    {
        sdkTraceErrorChar("Error creating the database");
        sdkTraceErrorChar(sqlite3_errmsg(db));
        return;
    } 
    
    // create tables
    const char* createTablesSQL = R"SQL(
        CREATE TABLE IF NOT EXISTS FileResults (
            id INTEGER PRIMARY KEY,
        
            filePath TEXT UNIQUE,
            duration REAL,
            sampleRate REAL,
            numSamples INTEGER,
        
            spectralCentroid_mean REAL,
            spectralCentroid_std REAL,
        
            spectralSpread_mean REAL,
            spectralSpread_std REAL,
        
            spectralFlatness_mean REAL,
            spectralFlatness_std REAL,
        
            RMS_mean REAL,
            RMS_std REAL,
        
            loudness_mean REAL,
            loudness_std REAL,
        
            mfcc_mean_1 REAL,
            mfcc_mean_2 REAL,
            mfcc_mean_3 REAL,
            mfcc_mean_4 REAL,
            mfcc_mean_5 REAL,
            mfcc_mean_6 REAL,
            mfcc_mean_7 REAL,
            mfcc_mean_8 REAL,
            mfcc_mean_9 REAL,
            mfcc_mean_10 REAL,
            mfcc_mean_11 REAL,
            mfcc_mean_12 REAL,
            mfcc_mean_13 REAL,
            mfcc_std_1 REAL,
            mfcc_std_2 REAL,
            mfcc_std_3 REAL,
            mfcc_std_4 REAL,
            mfcc_std_5 REAL,
            mfcc_std_6 REAL,
            mfcc_std_7 REAL,
            mfcc_std_8 REAL,
            mfcc_std_9 REAL,
            mfcc_std_10 REAL,
            mfcc_std_11 REAL,
            mfcc_std_12 REAL,
            mfcc_std_13 REAL,
            
            f0_frequency_mean REAL,
            f0_frequency_std REAL,
            f0_probability_mean REAL,
            f0_probability_std REAL,
        
            mag_mean BLOB,
        
            tsne_x REAL,
            tsne_y REAL
            
        );

        )SQL";
    
    // load tables in the database
    char* errMsg = nullptr;
    if (sqlite3_exec(db, createTablesSQL, nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        sdkTraceErrorChar("Error creating tables");
        sdkTraceErrorChar(sqlite3_errmsg(db));
        sqlite3_free(errMsg);
        sqlite3_close(db);
        db = nullptr;
        return;
    }


    const char* fftSizeTableSQL = R"SQL(
        CREATE TABLE IF NOT EXISTS FileFFTSize (
            id INTEGER PRIMARY KEY CHECK(id = 0),
            fftSizeValue INTEGER
        );
        )SQL";


    char* errMsg2 = nullptr;
    if (sqlite3_exec(db, fftSizeTableSQL, nullptr, nullptr, &errMsg2) != SQLITE_OK)
    {
        sdkTraceErrorChar("Error creating tables");
        sdkTraceErrorChar(sqlite3_errmsg(db));
        sqlite3_free(errMsg2);
        sqlite3_close(db);
        db = nullptr;
        return;
    }
    
    const char* insertFFTSizeSQL = R"SQL(
    INSERT INTO FileFFTSize (id, fftSizeValue)
    VALUES (0, ?)
    ON CONFLICT(id) DO UPDATE SET fftSizeValue = excluded.fftSizeValue;
    )SQL";
    
    sqlite3_stmt* stmtFFT = nullptr;
    if (sqlite3_prepare_v2(db, insertFFTSizeSQL, -1, &stmtFFT, nullptr) != SQLITE_OK)
    {
        sdkTraceErrorChar(std::string("[DB] Failed to prepare insert: ") + sqlite3_errmsg(db));
        sqlite3_close(db);
        db = nullptr;
        return;
    }
    sqlite3_bind_int(stmtFFT, 1, fileFftSize);
    if (sqlite3_step(stmtFFT) != SQLITE_DONE)
    {
        sdkTraceErrorChar(std::string("[DB] Failed to insert fft size : ") + sqlite3_errmsg(db));
        sqlite3_finalize(stmtFFT);
        sqlite3_close(db);
        db = nullptr;
        return;
    }
    sqlite3_finalize(stmtFFT);
    
    sdkProgressBar("Initialization complete starting the analysis...", white);
    
    for(size_t currFile = 0; currFile < audioPaths.size(); currFile++)
    {
        //-------------------------------------- FOR EACH FILE --------------------------------------

        TAudioFilePtr audioFilePtr = sdkCreateAudioFile();
        sdkLoadInMemoryAudioFile(audioFilePtr,audioPaths[currFile].c_str());
        
        std::string fileName = fsx::filename(audioPaths[currFile]);
        std::string filePath = audioPaths[currFile];
        uint32_t fileLength = sdkGetSizeAudioFile(audioFilePtr);
        int numChannels = sdkGetChannelAudioFile(audioFilePtr);
        float fileSampleRate = static_cast<float>(sdkGetSampleRateAudioFile(audioFilePtr));
        float lengthInSec = (float)fileLength / fileSampleRate;
        
        //debugPrint((int)currFile, fileNumber, lengthInSec , fileSampleRate, fileLength, audioPaths);
        std::ostringstream ossFileName;
        ossFileName << "[" << fileNumber << "] : " << fsx::filename(audioPaths[currFile]) << "\n";
        
        std::ostringstream ossDuration;
        ossDuration << "Duration : " << lengthInSec << " s\n";

        std::string infoString = ossFileName.str() + ossDuration.str();

        sdkTraceChar(infoString);
        
        sdkTraceLogChar(ossFileName.str());
        
        int progressStatus = static_cast<int>(float(currFile)/float(audioPaths.size())*100.0f);
        
        
        
        if(fileLength == 0 || numChannels == 0)
        {
            sdkDestroyAudioFile(audioFilePtr);
            sdkTraceErrorChar("Empty file, skipping " + fileName);
            continue;
        }

        float durationMs = (float)fileLength / fileSampleRate * 1000.0f;
        if (durationMs > maxFileLength.getData())
        {
            sdkDestroyAudioFile(audioFilePtr);
            sdkTraceErrorChar("File is too long, skipping" + fileName);
            continue;
        }

            
           
        
        sdkProgressBar(std::string(std::string("[" + std::to_string(currFile) + "/" + std::to_string(audioPaths.size()) + "]") + std::string(" - Analyzing " + fileName)).c_str() , white, progressStatus);
        

        
        // initialization
        FileResults fileResults;
        fileResults.setGlobalInfos(filePath, lengthInSec, fileSampleRate, fileLength, fileFftSize);
        std::vector<FeaturesResults> sumFileFeatures;
        std::vector<FeaturesResults> sumFileSpectrum;
        Features features = Features(BUFFER_SIZE,BUFFER_SIZE/2+1,fileSampleRate);
        descriptors.mfcc.setFftSize(BUFFER_SIZE, fileSampleRate);
        descriptors.loudness.setFftSize(BUFFER_SIZE,fileSampleRate);
        
        int numberOfFrames = 0;
        numberOfFrames = (fileLength - BUFFER_SIZE) / OVERLAPSIZE;
        
        //--------------------------------------------------------------------------------------------
        for (int channel = 0; channel < 1; ++channel)
        {
            audioDataPtr[channel] = sdkGetSampleArrayAudioFile(audioFilePtr, channel + 1);
            
            if (!audioDataPtr[channel])
            {
                sdkTraceErrorChar("Error for channel" + std::to_string(channel) + " audioDataPtr[" + std::to_string(channel) + "] is null");
                continue;
            }
            

            const int spectrumSize = fileFftSize;
            const int hopSize = spectrumSize/4;
            const int analysisSize = BUFFER_SIZE;

            std::vector<float> analysisBuffer;
            std::vector<float> spectrumBuffer;
            
            int spectrumCounter = 0;
            int analysisCounter = 0;
            
            bool toWindowSpectrum = !(static_cast<int>(fileLength) < spectrumSize);
            bool toWindowAnalysis = !(static_cast<int>(fileLength) < analysisSize);
            
            //----- MAGNITUDES -----
            while (spectrumCounter < static_cast<int>(fileLength))
            {
                int remaining = fileLength - spectrumCounter;
                
                if (remaining < spectrumSize)
                {
                    spectrumBuffer.assign(audioDataPtr[channel] + spectrumCounter, audioDataPtr[channel] + spectrumCounter + remaining);
                    spectrumBuffer.resize(spectrumSize, 0.0f);
                    SpectrumFFT(spectrumBuffer.data(), sumFileSpectrum, spectrumSize, toWindowSpectrum);
                    break;
                }
                else
                {
                    spectrumBuffer.assign(audioDataPtr[channel] + spectrumCounter, audioDataPtr[channel] + spectrumCounter + spectrumSize);
                    SpectrumFFT(spectrumBuffer.data(), sumFileSpectrum, spectrumSize, toWindowSpectrum);
                }
                
                spectrumCounter += hopSize;
            }
            
            //------ FEATURES ANALYSIS ----
            while (analysisCounter < static_cast<int>(fileLength))
            {
                int remaining = fileLength - analysisCounter;
                
                if (remaining < analysisSize)
                {
                    analysisBuffer.assign(audioDataPtr[channel] + analysisCounter, audioDataPtr[channel] + analysisCounter + remaining);
                    analysisBuffer.resize(analysisSize, 0.0f);
                    Calculation(analysisBuffer.data(), descriptors, sumFileFeatures, features, fileSampleRate,toWindowAnalysis);
                    break;
                }
                else
                {
                    analysisBuffer.assign(audioDataPtr[channel] + analysisCounter, audioDataPtr[channel] + analysisCounter + analysisSize);
                    Calculation(analysisBuffer.data(), descriptors, sumFileFeatures, features, fileSampleRate,toWindowAnalysis);
                }
                
                analysisCounter += analysisSize;
            }
            
            computeFileStatistics(&fileResults, sumFileFeatures, sumFileSpectrum, fileFftSize);
            
            bool errc = insertFileResultsInDB(db, fileResults);
            if (!errc)
            {
                sdkTraceErrorChar("Error during analysis for file : " + fileName);
                sdkTraceErrorChar("Couldn't store the results in the database");
                sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
                sqlite3_close(db);
                db = nullptr;
                sdkDestroyAudioFile(audioFilePtr);
                return;
            }

            sdkDestroyAudioFile(audioFilePtr);

            fileNumber++;
            if(needToStopAnalysis.load(std::memory_order_acquire))
            {
                sdkProgressBar("Analysis stopped", white);
                sdkTraceChar("Analysis stopped");
                sqlite3_close(db);
                db = nullptr;
                return;
            }
        }
        
    }
    // ----------------- END OF ANALYSIS -------------------
    
    const char* statTableSQL = R"SQL(
        CREATE TABLE IF NOT EXISTS FileStats (
        featureName TEXT PRIMARY KEY,
        mean REAL,
        stddev REAL
        );
        )SQL";
    char* errmsg = nullptr;
    if (sqlite3_exec(db, statTableSQL, nullptr, nullptr, &errmsg) != SQLITE_OK)
    {
        sdkTraceErrorChar("Error creating tables");
        sdkTraceErrorChar(sqlite3_errmsg(db));
        sqlite3_free(errmsg);
        sqlite3_close(db);
        return;
    }
    
    

    sdkThreadRestart(progressThread);
    
    // compute total mean and std for each feature for Z-score normalisation
    if(!computeAndStoreFeatureStats(db, featuresListForTSNE))
    {
        sdkTraceErrorChar("Error computing and storing feature stats in FileStats table");
        sdkTraceErrorChar(sqlite3_errmsg(db));
        sqlite3_close(db);
        db = nullptr;
        return;
    }
    

    // compute TSNE Algo using features in featuresListForTSNE
    if(!requestForTSNE())
    {
        // error messages are in their respective functions
        // here we just need to close the handle
        sqlite3_close(db);
        db = nullptr;
        return;
    }

    analysisOver.store(true,std::memory_order_release); // stop the progress bar thread
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();
    
    if (sqlite3_close(db) != SQLITE_OK)
    {
        sdkTraceErrorChar("Error while closing the database at the end of the analysis");
        return;
    }
    
    std::string analysisCompleteString = std::string("Analysis completed for ") + std::to_string(fileNumber) + std::string(" files in ")
    + std::to_string(duration) + std::string("seconds");
    sdkTraceChar(analysisCompleteString);
    


    sdkProgressBar(std::string("Database successfully saved at :\n" + dbPathName).c_str() , white, 100);
}





void DatabaseCreation::Calculation(TPrecision* dataPtr, Descriptors& descriptors, std::vector<FeaturesResults>& sumFileFeatures, Features& features, float sample_rate, bool windowing )
{
    
    features.ptrAudio = dataPtr;
    if(windowing)
    {
        ApplyHanningWindow(dataPtr, features, features.buffer_size);
        sdkFFTForward(fftPtr, features.windowedAudioBuffer.data(), m_Re.data(), m_Im.data());
    } else {
        sdkFFTForward(fftPtr, dataPtr, m_Re.data(), m_Im.data());
    }
    
    for (int i = 0; i < BUFFER_SIZE/2+1; ++i) {
        features.magnitudes[i] = std::sqrtf(m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i]);
        features.amplitudes[i] = std::sqrtf(m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i]);
        features.frequencies[i] = i * sample_rate / BUFFER_SIZE;
        features.sum_magnitudes += features.magnitudes[i];
        features.sum_amplitudes += features.amplitudes[i];
        features.sum_magnitudes_times_freqs += features.magnitudes[i] * features.frequencies[i];
        features.sum_amplitudes_times_freqs += features.amplitudes[i] * features.frequencies[i];
    }
    features.unnormalized_magnitudes = features.magnitudes;
    if (features.sum_magnitudes != 0.0f)
    {
        for (int i = 0; i < BUFFER_SIZE/2+1; i++)
        {
            features.magnitudes[i] /= features.sum_magnitudes;
        }
    }
    
    // compute features
    descriptors.mfcc.process(features);
    descriptors.rms.process(features);
    descriptors.spectralCentroid.process(features);
    descriptors.spectralFlatness.process(features);
    descriptors.spectralSpread.process(features);
    descriptors.f0_yin.process(features);
    descriptors.loudness.process(features);
    
    // store results
    FeaturesResults calcResults;
    calcResults.harmonicMfcc = features.harmonicMfcc;
    calcResults.temporalRMS = features.temporalRMS;
    calcResults.spectralCentroid = features.spectralCentroid;
    calcResults.spectralFlatness= features.spectralFlatness;
    calcResults.spectralSpread = features.spectralSpread;
    calcResults.harmonicF0_YIN = features.harmonicF0_YIN;
    calcResults.loudness = features.loudness;
    
    sumFileFeatures.push_back(calcResults);

    m_Re.clear();
    m_Im.clear();
    m_Re.resize(BUFFER_SIZE/2+1);
    m_Im.resize(BUFFER_SIZE/2+1);
    
    features.sum_magnitudes = 0.0f;
    features.sum_magnitudes_times_freqs = 0.0f;
    features.sum_amplitudes = 0.0f;
    features.sum_amplitudes_times_freqs = 0.0f;

}

void DatabaseCreation::SpectrumFFT(TPrecision *dataPtr, std::vector<FeaturesResults> &sumFileSpectrum, int fftSize, bool windowing)
{
    std::vector<float> audioBuffer = std::vector<float>(fftSize,0.0f);
    std::vector<float> audioBufferWindowed = std::vector<float>(fftSize,0.0f);
    memcpy(audioBuffer.data(), dataPtr, sizeof(float) * fftSize);

    if(windowing)
    {
        for(int i = 0; i < fftSize; i++)
        {
            audioBufferWindowed[i] = audioBuffer[i] * spectrumHanningWindow[i];
            
        }
    } else {
        for(int i = 0; i < fftSize; i++)
        {
            audioBufferWindowed[i] = audioBuffer[i];
        }
    }
    
    std::vector<float> mag = std::vector<float>(fftSize/2+1,0.0f);
    std::vector<float> real = std::vector<float>(fftSize/2+1,0.0f);
    std::vector<float> im = std::vector<float>(fftSize/2+1,0.0f);
    
    sdkFFTForward(fftSpectrumPtr, audioBufferWindowed.data(), real.data(), im.data());
    
    for(int i = 0; i < fftSize/2+1; i++)
    {
        mag[i] = std::sqrtf(std::powf(real[i],2.0f) + std::powf(im[i],2.0f));
    }
    
    FeaturesResults calcSpectrumResults;
    calcSpectrumResults.fftMags = mag;
    sumFileSpectrum.push_back(calcSpectrumResults);
    
}


void DatabaseCreation::computeFileStatistics(FileResults* fileResults, const std::vector<FeaturesResults>& calcResults, const std::vector<FeaturesResults>& spectrumResults, int fftSize)
{
    // Mean
    fileResults->spectralCentroid.mean = CalcMeanValue(calcResults, &FeaturesResults::spectralCentroid);
    fileResults->spectralSpread.mean = CalcMeanValue(calcResults, &FeaturesResults::spectralSpread);
    fileResults->spectralFlatness.mean = CalcMeanValue(calcResults, &FeaturesResults::spectralFlatness);
    fileResults->RMS.mean = CalcMeanValue(calcResults, &FeaturesResults::temporalRMS);
    fileResults->loudness.mean = CalcMeanValue(calcResults, &FeaturesResults::loudness);
    fileResults->MFCC.mean = CalcMeanVector(calcResults, &FeaturesResults::harmonicMfcc,13);
    fileResults->f0_yin.mean = CalcMeanVector(calcResults, &FeaturesResults::harmonicF0_YIN,2);
    fileResults->fftMags.mean = CalcMeanVector(spectrumResults, &FeaturesResults::fftMags, fftSize/2 + 1);

    float maxAdjust = 1.0f;
    float adjustThreshold = 600.0f;
    float maxMagInOutput = 0.0f;
    for(const float& mag : fileResults->fftMags.mean)
    {
        if(mag > maxMagInOutput)
        {
            maxMagInOutput = mag;
        }
    }
    
    if(maxMagInOutput < adjustThreshold)
    {
        maxAdjust = adjustThreshold / maxMagInOutput;
    }
    
    for(float& mag : fileResults->fftMags.mean)
    {
        mag *= sqrtf(maxAdjust);
    }
    
    // Std Dev
    fileResults->spectralCentroid.stdDev = CalcStdDev(calcResults, &FeaturesResults::spectralCentroid);
    fileResults->spectralSpread.stdDev = CalcStdDev(calcResults, &FeaturesResults::spectralSpread);
    fileResults->spectralFlatness.stdDev = CalcStdDev(calcResults, &FeaturesResults::spectralFlatness);
    fileResults->RMS.stdDev = CalcStdDev(calcResults, &FeaturesResults::temporalRMS);
    fileResults->loudness.stdDev = CalcStdDev(calcResults, &FeaturesResults::loudness);
    fileResults->MFCC.stdDev = CalcStdDevVector(calcResults, &FeaturesResults::harmonicMfcc,13);
    fileResults->f0_yin.stdDev = CalcStdDevVector(calcResults, &FeaturesResults::harmonicF0_YIN,2);
    
}

bool DatabaseCreation::insertFileResultsInDB(sqlite3* db, const FileResults& results)
{
    sqlite3_stmt* stmt = nullptr;
    const char* insertSQL = R"SQL(
        INSERT INTO FileResults (
            filePath, duration, sampleRate, numSamples,
    
            spectralCentroid_mean, spectralCentroid_std,
            spectralSpread_mean,   spectralSpread_std,
            spectralFlatness_mean, spectralFlatness_std,
            RMS_mean,              RMS_std,
            loudness_mean,         loudness_std,
    
            mfcc_mean_1,  mfcc_mean_2,  mfcc_mean_3,  mfcc_mean_4,  mfcc_mean_5,
            mfcc_mean_6,  mfcc_mean_7,  mfcc_mean_8,  mfcc_mean_9,  mfcc_mean_10,
            mfcc_mean_11, mfcc_mean_12, mfcc_mean_13,
    
            mfcc_std_1,   mfcc_std_2,   mfcc_std_3,   mfcc_std_4,   mfcc_std_5,
            mfcc_std_6,   mfcc_std_7,   mfcc_std_8,   mfcc_std_9,   mfcc_std_10,
            mfcc_std_11,  mfcc_std_12,  mfcc_std_13,
    
            f0_frequency_mean, f0_frequency_std,
            f0_probability_mean, f0_probability_std,
    
            mag_mean
        )
        VALUES (
            ?, ?, ?, ?,
            ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,
            ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?,
            ?, ?, ?, ?,
            ?
        )
        ON CONFLICT(filePath) DO UPDATE SET
            duration                = excluded.duration,
            sampleRate              = excluded.sampleRate,
            numSamples              = excluded.numSamples,
            spectralCentroid_mean   = excluded.spectralCentroid_mean,
            spectralCentroid_std    = excluded.spectralCentroid_std,
            spectralSpread_mean     = excluded.spectralSpread_mean,
            spectralSpread_std      = excluded.spectralSpread_std,
            spectralFlatness_mean   = excluded.spectralFlatness_mean,
            spectralFlatness_std    = excluded.spectralFlatness_std,
            RMS_mean                = excluded.RMS_mean,
            RMS_std                 = excluded.RMS_std,
            loudness_mean           = excluded.loudness_mean,
            loudness_std            = excluded.loudness_std,
    
            mfcc_mean_1   = excluded.mfcc_mean_1,
            mfcc_mean_2   = excluded.mfcc_mean_2,
            mfcc_mean_3   = excluded.mfcc_mean_3,
            mfcc_mean_4   = excluded.mfcc_mean_4,
            mfcc_mean_5   = excluded.mfcc_mean_5,
            mfcc_mean_6   = excluded.mfcc_mean_6,
            mfcc_mean_7   = excluded.mfcc_mean_7,
            mfcc_mean_8   = excluded.mfcc_mean_8,
            mfcc_mean_9   = excluded.mfcc_mean_9,
            mfcc_mean_10  = excluded.mfcc_mean_10,
            mfcc_mean_11  = excluded.mfcc_mean_11,
            mfcc_mean_12  = excluded.mfcc_mean_12,
            mfcc_mean_13  = excluded.mfcc_mean_13,
    
            mfcc_std_1    = excluded.mfcc_std_1,
            mfcc_std_2    = excluded.mfcc_std_2,
            mfcc_std_3    = excluded.mfcc_std_3,
            mfcc_std_4    = excluded.mfcc_std_4,
            mfcc_std_5    = excluded.mfcc_std_5,
            mfcc_std_6    = excluded.mfcc_std_6,
            mfcc_std_7    = excluded.mfcc_std_7,
            mfcc_std_8    = excluded.mfcc_std_8,
            mfcc_std_9    = excluded.mfcc_std_9,
            mfcc_std_10   = excluded.mfcc_std_10,
            mfcc_std_11   = excluded.mfcc_std_11,
            mfcc_std_12   = excluded.mfcc_std_12,
            mfcc_std_13   = excluded.mfcc_std_13,
    
            f0_frequency_mean   = excluded.f0_frequency_mean,
            f0_frequency_std    = excluded.f0_frequency_std,
            f0_probability_mean = excluded.f0_probability_mean,
            f0_probability_std  = excluded.f0_probability_std,
    
            mag_mean = excluded.mag_mean
    )SQL";
    
    if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sdkTraceErrorChar(std::string("[DB] Failed to prepare insert: ") + sqlite3_errmsg(db));
        return false;
    }

    int idx = 1;
    sqlite3_bind_text(stmt, idx++, results.filePath.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, idx++, results.duration);
    sqlite3_bind_double(stmt, idx++, results.sampleRate);
    sqlite3_bind_int(stmt, idx++, results.numSamples);
    
    sqlite3_bind_double(stmt, idx++, results.spectralCentroid.mean);
    sqlite3_bind_double(stmt, idx++, results.spectralCentroid.stdDev);
    sqlite3_bind_double(stmt, idx++, results.spectralSpread.mean);
    sqlite3_bind_double(stmt, idx++, results.spectralSpread.stdDev);
    sqlite3_bind_double(stmt, idx++, results.spectralFlatness.mean);
    sqlite3_bind_double(stmt, idx++, results.spectralFlatness.stdDev);
    sqlite3_bind_double(stmt, idx++, results.RMS.mean);
    sqlite3_bind_double(stmt, idx++, results.RMS.stdDev);
    sqlite3_bind_double(stmt, idx++, results.loudness.mean);
    sqlite3_bind_double(stmt, idx++, results.loudness.stdDev);
    
    for (int i = 0; i < 13; ++i)
        sqlite3_bind_double(stmt, idx++, results.MFCC.mean[i]);
    
    for (int i = 0; i < 13; ++i)
        sqlite3_bind_double(stmt, idx++, results.MFCC.stdDev[i]);
    
    sqlite3_bind_double(stmt, idx++, results.f0_yin.mean[0]);
    sqlite3_bind_double(stmt, idx++, results.f0_yin.stdDev[0]);
    sqlite3_bind_double(stmt, idx++, results.f0_yin.mean[1]);
    sqlite3_bind_double(stmt, idx++, results.f0_yin.stdDev[1]);
    
    sqlite3_bind_blob(stmt, idx++, results.fftMags.mean.data(),
                      sizeof(float) * (int)results.fftMags.mean.size(),
                      SQLITE_TRANSIENT);
    
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sdkTraceErrorChar(std::string("[DB] Failed to insert/update row: ") + sqlite3_errmsg(db));
        sqlite3_finalize(stmt);
        return false;
    }
    
    sqlite3_finalize(stmt);
    return true;
}

bool DatabaseCreation::computeAndStoreFeatureStats(sqlite3* db, const std::vector<std::string>& features)
{
    sqlite3_stmt* stmt = nullptr;
    char* errMsg = nullptr;
    
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        sdkTraceErrorChar("Error beginning transaction");
        sdkTraceErrorChar(errMsg);
        sqlite3_free(errMsg);
        return false;
    }
    
    bool success = true;
    
    for (const auto& feature : features)
    {
        std::string query = "SELECT " + feature + " FROM FileResults WHERE " + feature + " IS NOT NULL;";
        std::vector<double> values;
        
        if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        {
            sdkTraceErrorChar(std::string("[DB] Failed to prepare query for ") + feature + ": " + sqlite3_errmsg(db));
            success = false;
            break;
        }
        
        while (sqlite3_step(stmt) == SQLITE_ROW)
            values.push_back(sqlite3_column_double(stmt, 0));
        
        sqlite3_finalize(stmt);
        
        if (values.empty()) continue;
        
        double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        double sq_sum = 0.0;
        for (double v : values)
        {
            sq_sum += (v - mean) * (v - mean);
        }
        
        double stddev = std::sqrt(sq_sum / values.size());
        
        const char* insertSQL =
        "INSERT INTO FileStats (featureName, mean, stddev) VALUES (?, ?, ?) "
        "ON CONFLICT(featureName) DO UPDATE SET mean=excluded.mean, stddev=excluded.stddev;";
        
        if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
        {
            sdkTraceErrorChar(std::string("[DB] Failed to prepare insert for FileStats: ") + sqlite3_errmsg(db));
            success = false;
            break;
        }
        
        sqlite3_bind_text(stmt, 1, feature.c_str(), -1, SQLITE_STATIC);
        sqlite3_bind_double(stmt, 2, mean);
        sqlite3_bind_double(stmt, 3, stddev);
        
        if (sqlite3_step(stmt) != SQLITE_DONE)
        {
            sdkTraceErrorChar(std::string("[DB] Failed to insert feature stats for ") + feature + ": " + sqlite3_errmsg(db));
            success = false;
            sqlite3_finalize(stmt);
            break;
        }
        
        sqlite3_finalize(stmt);
    }
    
    if (success)
    {
        if (sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, &errMsg) != SQLITE_OK)
        {
            sdkTraceErrorChar(std::string("[DB] Failed to commit transaction: ") + errMsg);
            sqlite3_free(errMsg);
            return false;
        }
    } else {
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
    }
    
    return success;
}

bool DatabaseCreation::requestForTSNE()
{
    std::vector<double> flatDataset;
    const int numDims = static_cast<int>(featuresListForTSNE.size());
    int numFiles = 0;
    
    std::string query = buildTSNEQuery(featuresListForTSNE, false);
    
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
        sdkTraceErrorChar(std::string("[DB] Failed to prepare t-SNE query: ") + sqlite3_errmsg(db));
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        numFiles++;
        for (int i = 1; i <= numDims; ++i)
        {
            double value = sqlite3_column_double(stmt, i);
            flatDataset.push_back(value);
        }
    }
    
    sqlite3_finalize(stmt);
    
    if (numFiles == 0 || flatDataset.size() != static_cast<size_t>(numFiles * numDims))
    {
        sdkTraceErrorChar(std::string("[t-SNE] Invalid data dimensions: ") + std::to_string(flatDataset.size()) + " values for " + std::to_string(numFiles) + " files and " + std::to_string(numDims) + " dims.");
        return false;
    }
    
    if(!processTSNE(flatDataset.data(), numFiles, numDims))
    {
        if (needToStopAnalysis.load(std::memory_order_acquire))
        {
            sdkTraceChar("Analysis stopped during combined dimensions computation");
            sdkTraceChar("Please restart the analysis to have valid values for combined dimensions");
        }
        else {
            sdkTraceErrorChar("An error occurred during the combined dimensions computation");
        }
        return false;
    }

    return true;
}

std::string DatabaseCreation::buildTSNEQuery(const std::vector<std::string>& features, bool normalizeTSNE)
{
    std::string select = "SELECT r.filePath";
    std::string from = " FROM FileResults r\n";
    std::unordered_set<std::string> alreadyJoined;
    int aliasCount = 0;
    
    for (const auto& feature : features)
    {
        if (normalizeTSNE)
        {
            std::string alias = "f" + std::to_string(aliasCount++);
            select += ",\n  (r." + feature + " - " + alias + ".mean) / " + alias + ".stddev AS " + feature + "_norm";
            
            if (!alreadyJoined.count(feature))
            {
                from += "JOIN FileStats " + alias + " ON " + alias + ".featureName = '" + feature + "'\n";
                alreadyJoined.insert(feature);
            }
        }
        else
        {
            select += ",\n  r." + feature;
        }
    }
    
    return select + from + ";";
}

bool DatabaseCreation::processTSNE(double* dataset, int NFiles, int dimensions)
{
    
    int max_iter = 1000;
    double perplexity = 30;
    double theta = 0.3;
    
    std::vector<double> results(2 * NFiles);
    
    
    
    TSNE::run(dataset, NFiles, dimensions, results.data(),
              2,                    // num output dims
              perplexity, theta,    
              1,                    // random seed
              false,                // skip random init
              max_iter,             // maximum iterations = 1000
              250,                  // stop_lying_iter
              250,                  // mom_switch_iter
              tsneState,            // state of the algorithm for progress bar
              needToStopAnalysis
        );
    
    if (needToStopAnalysis.load(std::memory_order_acquire))
    {
        return false;
    }

    // normalize between 0 and 1
    double x_min = std::numeric_limits<double>::max();
    double x_max = std::numeric_limits<double>::lowest();
    double y_min = std::numeric_limits<double>::max();
    double y_max = std::numeric_limits<double>::lowest();
    
    for (int i = 0; i < NFiles; ++i) {
        double x = results[2 * i];
        double y = results[2 * i + 1];
        if (x < x_min) x_min = x;
        if (x > x_max) x_max = x;
        if (y < y_min) y_min = y;
        if (y > y_max) y_max = y;
    }
    
    double x_range = x_max - x_min;
    double y_range = y_max - y_min;
    if (x_range == 0.0) x_range = 1.0;
    if (y_range == 0.0) y_range = 1.0;
    
    for (int i = 0; i < NFiles; ++i)
    {
        results[2 * i] = (results[2 * i] - x_min) / x_range;
        results[2 * i + 1] = (results[2 * i + 1] - y_min) / y_range;
    }
    
    if(!storeTSNEinDB(results))
    {
        sdkTraceErrorChar("Error storing results in the DB for tSNE");
        return false;
    }
    return true;
}

bool DatabaseCreation::storeTSNEinDB(const std::vector<double>& TSNEresults)
{
    std::vector<int> ids;
    const char* query = "SELECT id FROM FileResults ORDER BY filePath;";
    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sdkTraceErrorChar(std::string("Failed to prepare id select: ") + sqlite3_errmsg(db));
        return false;
    }
    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        ids.push_back(sqlite3_column_int(stmt, 0));
    }
    sqlite3_finalize(stmt);
    
    int numFiles = static_cast<int>(ids.size());
    if (TSNEresults.size() != size_t(numFiles * 2))
    {
        sdkTraceErrorChar("Mismatch between TSNE results size and number of files");
        return false;
    }
    
    char* errmsg = nullptr;
    if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK) {
        sdkTraceErrorChar(std::string("Failed to begin transaction: ") + errmsg);
        sqlite3_free(errmsg);
        return false;
    }
    
    const char* updateSQL = "UPDATE FileResults SET tsne_x = ?, tsne_y = ? WHERE id = ?;";
    
    if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) != SQLITE_OK)
    {
        sdkTraceErrorChar(std::string("Failed to prepare update statement: ") + sqlite3_errmsg(db));
        sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
        return false;
    }
    
    for (int i = 0; i < numFiles; ++i)
    {
        sqlite3_bind_double(stmt, 1, TSNEresults[2 * i]);       // tsne_x
        sqlite3_bind_double(stmt, 2, TSNEresults[2 * i + 1]);   // tsne_y
        sqlite3_bind_int(stmt, 3, ids[i]);                      // id
        
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sdkTraceErrorChar(std::string("Failed to update tsne coords for id ") + std::to_string(ids[i]) + ": " + sqlite3_errmsg(db));
            sqlite3_finalize(stmt);
            sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
            return false;
        }
        sqlite3_reset(stmt);
    }
    
    sqlite3_finalize(stmt);
    
    if (sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, &errmsg) != SQLITE_OK)
    {
        sdkTraceErrorChar(std::string("Failed to commit transaction: ") + errmsg);
        sqlite3_free(errmsg);
        return false;
    }
    
    return true;
}

inline void DatabaseCreation::debugPrint(int currFile, int fileNumber, float lengthInSec, float fileSampleRate, int fileLength, const std::vector<std::string>& audioPaths)
{
    std::cout << "["<< fileNumber << "] : " << fsx::filename(audioPaths[currFile]) <<  std::endl
    << "Duration : " << lengthInSec << " s " << std::endl
    << "Number of samples : " << fileLength << " samples" << std::endl
    << "Sample Rate : " << fileSampleRate <<  std::endl
    << "Processing ..." << std::endl;
}



