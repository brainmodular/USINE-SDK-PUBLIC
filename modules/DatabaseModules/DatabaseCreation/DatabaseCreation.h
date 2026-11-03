//-----------------------------------------------------------------------------
//@file
//    DatabaseCreation.h
//
//@author
//    BrainModular team
//
//@brief
//    Definitions of the DatabaseCreation class.
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
#pragma once

//-----------------------------------------------------------------------------
// includes
//-----------------------------------------------------------------------------
#include <iostream>
#include <chrono>
#include <fstream>
#include <numeric>
#include <unordered_set>
#include <mutex>

#ifdef  _WIN32
#include "UserDefinitions.h"
#include "../libs/utils/tsne/tsne.h"
#include "../libs/utils/functions/HFeatures.h"
#include "../libs/utils/functions/StatisticsFunctions.h"
#include "../libs/sqlite3lib/sqlite3.h"
#include "../libs/utils/HaudioBuffer.h"
#include "../libs/utils/Hdescriptors.h"
#endif 

#ifdef __APPLE__
#include "UserDefinitions.h"
#include "tsne.h"
#include "HFeatures.h"
#include "StatisticsFunctions.h"
#include "sqlite3.h"
#include "HaudioBuffer.h"
#include "Hdescriptors.h"
#endif

#if defined(__APPLE__)
// std::filesystem needs macOS 10.15 at runtime (libc++ availability); the modules target 10.13,
// so file-system access goes through a tiny POSIX-based layer on macOS.
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <string>
#include <vector>
#else
#include <filesystem>
#endif

namespace fsx
{
#if defined(__APPLE__)
    inline bool exists(const std::string& p)          { struct stat st; return ::stat(p.c_str(), &st) == 0; }
    inline bool is_directory(const std::string& p)    { struct stat st; return ::stat(p.c_str(), &st) == 0 && S_ISDIR(st.st_mode); }
    inline bool is_regular_file(const std::string& p) { struct stat st; return ::stat(p.c_str(), &st) == 0 && S_ISREG(st.st_mode); }
    inline bool remove(const std::string& p)          { return std::remove(p.c_str()) == 0; }

    inline std::string filename(const std::string& p)
    {
        const auto i = p.find_last_of('/');
        return (i == std::string::npos) ? p : p.substr(i + 1);
    }

    inline std::string extension(const std::string& p)
    {
        const std::string f = filename(p);
        const auto i = f.find_last_of('.');
        return (i == std::string::npos || i == 0) ? std::string() : f.substr(i);
    }

    // Recursive listing of regular files (directory symlinks are not followed,
    // matching std::filesystem::recursive_directory_iterator defaults).
    inline void list_regular_files(const std::string& dir, std::vector<std::string>& out)
    {
        DIR* d = ::opendir(dir.c_str());
        if (!d) return;
        while (struct dirent* e = ::readdir(d))
        {
            const std::string name = e->d_name;
            if (name == "." || name == "..") continue;
            const std::string full = dir + ((!dir.empty() && dir.back() == '/') ? "" : "/") + name;
            struct stat st;
            if (::lstat(full.c_str(), &st) != 0) continue;
            if (S_ISDIR(st.st_mode))
                list_regular_files(full, out);
            else if (S_ISREG(st.st_mode) || (S_ISLNK(st.st_mode) && is_regular_file(full)))
                out.push_back(full);
        }
        ::closedir(d);
    }
#else
    inline bool exists(const std::string& p)          { return std::filesystem::exists(p); }
    inline bool is_directory(const std::string& p)    { return std::filesystem::is_directory(p); }
    inline bool is_regular_file(const std::string& p) { return std::filesystem::is_regular_file(p); }
    inline bool remove(const std::string& p)          { return std::filesystem::remove(p); }
    inline std::string filename(const std::string& p) { return std::filesystem::path(p).filename().string(); }
    inline std::string extension(const std::string& p){ return std::filesystem::path(p).extension().string(); }

    inline void list_regular_files(const std::string& dir, std::vector<std::string>& out)
    {
        for (const auto& entry : std::filesystem::recursive_directory_iterator(dir))
            if (std::filesystem::is_regular_file(entry.status()))
                out.push_back(entry.path().string());
    }
#endif
}

constexpr UINT32 BUFFER_SIZE = 2048;
constexpr float SAMPLE_RATE = 44100.0f;
constexpr UINT32 OVERLAPSIZE = 2048;

constexpr int SPECTRUM_SIZE = 16384;
constexpr int SPECTRUM_OVERLAP_SIZE = SPECTRUM_SIZE / 4;

// for float results
struct StatsStruct {
    float mean = 0.0f;
    float stdDev = 0.0f;
};

// for vec results
struct StatsVecStruct
{
    std::vector<float> mean;
    std::vector<float> stdDev;
    
    void resize(int newSize)
    {
        this->mean.resize(newSize);
        this->stdDev.resize(newSize);
    }
    
    StatsVecStruct(int vec_size)
    {
        this->mean.resize(vec_size);
        this->stdDev.resize(vec_size);
    }
    
    StatsVecStruct()
    {
        this->mean.resize(1);
        this->stdDev.resize(1);
    }
};


struct FileResults
{
    // global infos
    std::string filePath = "emptyFile";
    float duration = 0.0f;
    float sampleRate = 0.0f;
    uint32_t numSamples = 0;
    
    // results
    StatsStruct spectralCentroid;
    StatsStruct spectralSpread;
    StatsStruct spectralFlatness;
    StatsStruct RMS;
    StatsStruct loudness;
    StatsVecStruct MFCC = StatsVecStruct(13);
    StatsVecStruct f0_yin = StatsVecStruct(2);
    StatsVecStruct fftMags = StatsVecStruct(SPECTRUM_SIZE/2+1);
    
    FileResults() = default;
    
    void setGlobalInfos(const std::string& filePath, float duration, float sampleRate, uint32_t numSamples, int fftSize)
    {
        this->filePath = filePath;
        this->duration = duration;
        this->sampleRate = sampleRate;
        this->numSamples = numSamples;
        
        fftMags.resize(fftSize/2+1);
        
    }
    
};

class DatabaseCreation : public UserModuleBase
{
public:
    
    DatabaseCreation();
    virtual ~DatabaseCreation();

public:
    void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
    void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
    void onGetParamInfo(int ParamIndex, TParamInfo *pParamInfo);
    void onCallBack(TUsineMessage* Message);
protected:
    
    UsineEventClass folderPath;
    UsineEventClass databasePath;
    UsineEventClass databaseName;
    UsineEventClass databaseFile;
    UsineEventClass maxFileLength;
    UsineEventClass fftSize;
    UsineEventClass launchAnalysis;
    UsineEventClass stopAnalysis;

public:
    void AudioFilesAnalysis();

public:
    TCriticalSectionPtr criticalSection;

    bool isProcessing = false;
    std::atomic<bool> needToStopAnalysis { false };
    
private:

// General variables
    TThreadPtr thread;
    TThreadPtr progressThread;
    TUsineFFTPtr fftPtr = nullptr;
    TUsineFFTPtr fftSpectrumPtr = nullptr;
    TPrecision* audioDataPtr[USINE_AUDIO_CHANNEL_MODULE_MAX];

// Files Paths and directories variables
    bool isValidDirectory = false;
    bool isValidName = false;
    std::vector<std::string> m_AudioFilePaths;
    std::string dbName;

// Analysis variables
    sqlite3* db = nullptr;
    Descriptors descriptors;
    std::vector<TPrecision> m_HanningWindow;
    std::vector<TPrecision> spectrumHanningWindow;
    std::vector<TPrecision> m_Re;
    std::vector<TPrecision> m_Im;
    bool windowing = true;
    std::vector<std::string> featuresListForTSNE;
    
public:
    UsineColor white = sdkAlphaColor(1.0f,1.0f,1.0f,1.0f);

// Progress Bar 
    uint8_t progCounter = 0;
    std::atomic<bool> analysisOver {false};
    std::mutex mtx;
    std::condition_variable cv;
    TSNEState tsneState;
private:

    void InitializeHanningWindow(std::vector<float>& hanningWindow, int size);
    void ApplyHanningWindow(TPrecision* dataPtr, Features& features, int buffer_size);

    // scan the folder and get all audio files paths, return false if no valid audio files found
    bool GetAudioPaths(const std::string dirPath);

    // compute Features
    void Calculation(TPrecision* dataPtr, Descriptors& descriptors, std::vector<FeaturesResults>& sumFileFeatures, Features& features, float sample_rate, bool windowing );
    // compute Mag spectrum
    void SpectrumFFT(TPrecision *dataPtr, std::vector<FeaturesResults> &sumFileSpectrum, int fftSize, bool windowing);

    // compute and store mean and std dev for each file
    void computeFileStatistics(FileResults* fileResults, const std::vector<FeaturesResults>& calcResults, const std::vector<FeaturesResults>& spectrumResults, int fftSize);
    // store results in the database
    bool insertFileResultsInDB(sqlite3* db, const FileResults& results);

    //-- TSNE related methods --
    // compute and store global statistics for Z-Score normalization
    bool computeAndStoreFeatureStats(sqlite3* db, const std::vector<std::string>& features);
    bool requestForTSNE();
    std::string buildTSNEQuery(const std::vector<std::string>& features, bool normalizeTSNE);
    bool processTSNE(double* dataset, int NFiles, int dimensions);
    bool storeTSNEinDB(const std::vector<double>& TSNEresults);
    
    // Debug 
    void debugPrint(int currFile, int fileNumber, float lengthInSec, float fileSampleRate, int fileLength, const std::vector<std::string>& audioPaths);

}; // DatabaseCreation class

