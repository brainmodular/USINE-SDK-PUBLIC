//-----------------------------------------------------------------------------
//@file
//    DatabasePlayer.h
//
//@author
//    BrainModular team
//
//@brief
//    Definitions of the DatabasePlayer class.
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

#include <iostream>
#include <fstream>
#include <random>
#include <chrono>
#include <memory>
#include <atomic>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>

#ifdef  _WIN32
#include "UserDefinitions.h"
#include "../libs/sqlite3lib/sqlite3.h"
#include "../libs/utils/HaudioBuffer.h"
#include "../libs/utils/KDTree.h"
#endif // Windows

#ifdef __APPLE__
#include "UserDefinitions.h"
#include "sqlite3.h"
#include "KDTree.h"
#include "HaudioBuffer.h"
#endif // MacOs



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


// ---- CONSTANTS ----
constexpr int DEFAULT_PANEL_HEIGHT = 500;
constexpr int DEFAULT_PANEL_WIDTH = 500;

constexpr UINT32 BUFFER_SIZE = 2048;
constexpr float SAMPLE_RATE = 44100.0f;
constexpr UINT32 OVERLAPSIZE = 2048;

constexpr int LUT_SIZE = 10000;

constexpr float coeffThreshold = 0.01f;
constexpr float baseSmoothDuration = 100.0f;
constexpr int sampleQueueMaxPoly = 40;

constexpr int DEFAULT_SPECTRUM = 16384;

// enum for settings IDs and callbacks
enum SettingsId : NativeInt {
    POINT_RADIUS = 90,
    SPREAD_X,
    SPREAD_Y,
    NORMALIZE_COEFFS
};

// functions to convert normalized coordinates to RGB colors
inline void hsv_to_rgb(float h, float s, float v, uint8_t& r, uint8_t& g, uint8_t& b) {
    float c = v * s;
    float x = c * (1 - std::fabs(fmod(h / 60.0f, 2.0f) - 1));
    float m = v - c;
    float r1, g1, b1;
    
    if (h < 60)      { r1 = c; g1 = x; b1 = 0; }
    else if (h < 120){ r1 = x; g1 = c; b1 = 0; }
    else if (h < 180){ r1 = 0; g1 = c; b1 = x; }
    else if (h < 240){ r1 = 0; g1 = x; b1 = c; }
    else if (h < 300){ r1 = x; g1 = 0; b1 = c; }
    else             { r1 = c; g1 = 0; b1 = x; }
    
    r = static_cast<uint8_t>((r1 + m) * 255);
    g = static_cast<uint8_t>((g1 + m) * 255);
    b = static_cast<uint8_t>((b1 + m) * 255);
}
inline TUsineColor xy_to_rgb_colorwheel_radial(float x, float y) {
    static float DEG_PI = 180.0f / static_cast<float>(M_PI);
    
    x = std::clamp(x, 0.0f, 1.0f);
    y = std::clamp(y, 0.0f, 1.0f);
    
    float dx = x - 0.5f;
    float dy = y - 0.5f;
    float hue = std::atan2(dy, dx) * DEG_PI;
    if (hue < 0.0f) hue += 360.0f;
    
    float dist = std::sqrt(dx * dx + dy * dy) * 1.4142f;
    float sat = std::clamp(dist * 3.0f, 0.5f, 1.0f);
    
    float val = 1.0f;
    
    uint8_t r, g, b;
    hsv_to_rgb(hue, sat, val, r, g, b);
    return (255 << 24) | (r << 16) | (g << 8) | b; // ARGB
}

// structure for the circle cursor
struct CircleCursor
{
    float x = 0.0f;
    float y = 0.0f;
    float radius = 50.0f/(2.0f*(float)DEFAULT_PANEL_WIDTH);
    TColorArgb color;

    CircleCursor()
    {
        color.a = 0.2f;
        color.r = 0.5;
        color.b = 0.5;
        color.g = 0.5;
    }
    
    void setRadius(float newRadius)
    {
        this->radius = newRadius;
    }
    
};

// structure for the mouse information
struct MouseInfo
{
    float prevX = 0.0f;
    float prevY = 0.0f;
    bool leftBtnDown = false;
    
    bool hasMouseMoved(float newX, float newY)
    {
        return (prevX != newX || prevY != newY);
    }
    
    void storeNewCoords(float newX, float newY)
    {
        this->prevX = newX;
        this->prevY = newY;
    }
    
};

// linear smoother coefficient structure
// used to smooth the coefficients of the samples in the queue
struct SmoothCoeff
{
    float current = 0.0f;
    float target = 0.0f;
    float step = 0.0f;
    int remainingBlocs = 0;
    
    void setTarget(float newTarget, float duration, float blocDuration)
    {
        target = newTarget;
        if(duration <= 0.0f || blocDuration <= 0.0f)
        {
            current = target;
            remainingBlocs = 0;
            step = 0.0f;
            return;
        }
        
        remainingBlocs = static_cast<int>(duration / blocDuration);
        if(remainingBlocs <= 0)
        {
            current = target;
            remainingBlocs = 0;
            step = 0.0f;
            return;
        }
        
        step = (target - current) / static_cast<float>(remainingBlocs);
        
    }
    
    void update()
    {
        if(remainingBlocs > 0)
        {
            current += step;
            remainingBlocs--;
            if(remainingBlocs == 0)
            {
                current = target;
            }
        }
        
    }
    
    float getCoeff() const
    {
        return current;
    }
    
    
};

// Implementation of a FIFO queue of actions to be processed in the worker thread. Max Size of 100 pending actions.
// It uses a mutex and a condition variable to synchronize access to the queue.
// Actions are store as std::function<void()> with the instance of the module class passed as a lambda
// No concurrency, single threaded processing - IFFT are prioritized over any other action
class ActionQueue {
public:
    // TODO :   
    //  - Change std::queue to a circular std::array to prevent memory allocations
    //  - Adjust the rate of the initSearchThread depending on the work load of the queue.

    void pushFunc(std::function<void(void*)> func, void* arg)
    {
        push([func,arg]() { func(arg); });
    }
    
    void pushIFFTFunc(std::function<void(void*)> func, void* arg)
    {
        pushIfftTask([func,arg]() { func(arg); });
    }
    
    void run(void* pModule) {
        /*
        using clock = std::chrono::high_resolution_clock;
        using duration = std::chrono::duration<double, std::milli>;
        
        duration totalWorkTime{0};
        duration totalIdleTime{0};
        duration zero{0};
        */
        
        m_IsWorking.store(true,std::memory_order_relaxed);
        
        while (true) {
            std::function<void()> task;
            //auto idleStart = clock::now();
            {
                std::unique_lock<std::mutex> lock(m_Mutex);
                m_CondVar.wait(lock, [this]() {
                    return m_StopFlag || !m_TaskQueue.empty() || !m_IfftTaskQueue.empty();
                });
                //auto idleEnd = clock::now();
                //totalIdleTime += (idleEnd - idleStart);
                
                if (m_StopFlag && m_TaskQueue.empty() && m_IfftTaskQueue.empty())
                    break;
                
                if (!m_IfftTaskQueue.empty()) {
                    task = std::move(m_IfftTaskQueue.front());
                    m_IfftTaskQueue.pop();
                } else if (!m_TaskQueue.empty()) {
                    task = std::move(m_TaskQueue.front());
                    m_TaskQueue.pop();
                }
            }
            
            if (task) {
                //auto workStart = clock::now();
                task();
                //auto workEnd = clock::now();
                //totalWorkTime += (workEnd - workStart);
            }
            
            /*
            static int counter = 0;
            if (++counter % 100 == 0) {
                std::cout << "Total working time (ms): " << totalWorkTime.count()
                << " | Total idle time (ms): " << totalIdleTime.count()
                << " | Ratio work/total: "
                << (totalWorkTime.count() / (totalWorkTime.count() + totalIdleTime.count())) << std::endl;
                totalWorkTime = zero;
                totalIdleTime = zero;
            }
            */
        }
    }
    
    
    void clear() {
        std::unique_lock<std::mutex> lock(m_Mutex);
        for (; !m_TaskQueue.empty(); m_TaskQueue.pop()){}
    }
    
    void stop() {
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_StopFlag = true;
        }
        m_CondVar.notify_all();
    }
    
    std::atomic<bool> m_IsWorking{false};
    
private:
    std::queue<std::function<void()>> m_TaskQueue;
    std::queue<std::function<void()>> m_IfftTaskQueue;
    std::mutex m_Mutex;
    std::condition_variable m_CondVar;

    std::atomic<bool> m_StopFlag{false};
    
    size_t m_MaxQueueSize = 100;
    
    void push(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        
        if(m_TaskQueue.size() < m_MaxQueueSize)
        {
            m_TaskQueue.push(std::move(task));
            m_CondVar.notify_one();
        } else
        {
            //std::cout << "ACTION QUEUE FULL, REJECTED ACTION" << std::endl;
        }

    }
    
    void pushIfftTask(std::function<void()> task) {
        std::lock_guard<std::mutex> lock(m_Mutex);
        m_IfftTaskQueue.push(std::move(task));
        m_CondVar.notify_one();
    }
    
};



class DatabasePlayer : public UserModuleBase
{
public:
    
    DatabasePlayer();
    virtual ~DatabasePlayer();
    
public:
    
    void onGetModuleInfo (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
    void onInitModule (TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo);
    void onGetParamInfo(int ParamIndex, TParamInfo *pParamInfo);
    void onCreateSettings();
    void onSettingsHasChanged();
    void onCallBack(TUsineMessage* Message);
    void onProcess();
    void onPaint();
    
    void onMouseMove(TShiftState Shift, float X, float Y);
    void onMouseDown(TMouseButton MouseButton, TShiftState Shift, float X,float Y);
    void onMouseUp (TMouseButton MouseButton, TShiftState Shift, float X,float Y);
    void onMouseWheel (TShiftState Shift, int WheelDelta);
protected:
    
    UsineEventClass databaseFile;
    UsineEventClass reloadDatabase;
    UsineEventClass firstDim;
    UsineEventClass secondDim;
    UsineEventClass numberOfPoints;
    UsineEventClass closestSamplesPath;
    UsineEventClass toggleResynth;
    UsineEventClass rAudioOut;
    UsineEventClass lAudioOut;
    UsineEventClass coeffArray;
    UsineEventClass attackDuration;
    UsineEventClass releaseDuration;
    UsineEventClass coeffs;
    UsineEventClass cursorDown;
    UsineEventClass cursorX;
    UsineEventClass cursorY;
    UsineEventClass cursorRadius;
    
public:

    // methods
    void computeIFFTMag(float* magPtr);
    bool sendRequestToDB(const std::string firstDimension,const std::string secondDimension);
    std::string queryBuilder(const std::string& firstDimension, const std::string& secondDimension);
    bool openDB();
    void InitializeHanningWindow(std::vector<float>& hanningWindow, int size);
    void findEveryNearestPoint(const CircleCursor& circleCursor, std::vector<PointDistance2D>& resultQueue );

    // Critical section and threads related 
    TCriticalSectionPtr csPaint;
    TThreadPtr workThread;
    TThreadPtr initSearchThread;

    // Action queue for processing tasks in the worker thread
    ActionQueue actionQueue;

    // Database variables
    sqlite3* db = nullptr;
    std::string dbName;
    bool isDBOpen = false;
    std::atomic<bool> isRequesting {false};
    std::atomic<bool> isLoading {false};

    // Output audio buffers 
    AudioBuffer<float> rBuffer;
    AudioBuffer<float> lBuffer;

    // Resynth variables
    TUsineFFTPtr ifftPtr;
    bool activeResynth = false;
    float* MagPtr = nullptr;
        // temp vectors holding results of ifft on which we apply hann
    std::vector<float> rTempBuffer;
    std::vector<float> lTempBuffer;
    std::vector<float> rReal;
    std::vector<float> rIm;
    std::vector<float> lReal;
    std::vector<float> lIm;

    std::vector<float> hanning;
    int SPECTRUM_SIZE = 16384;
    int SPECTRUM_SIZE_2 = SPECTRUM_SIZE / 2;
    int SPECTRUM_SIZE_4 = SPECTRUM_SIZE / 4;
    int SPECTRUM_OVERLAP_SIZE = SPECTRUM_SIZE / 4;
        // for remaining frames
    int remainingFrames = 0;
    bool isCurrentlyPlaying = false;
    int ifftCounter = 0;

    // Points variables         
    std::vector<Point2D> requestData;
    std::vector<std::shared_ptr<const PointMag>> pointsMags;
    
    // inputs variables
    CircleCursor circleCursor;
    MouseInfo mouseInfo;
    bool isCursorOn = false;

    // drawing variables
    float canvasPointRadius = 2.0f;
    float minCursorRadius = 1.0f;
    float maxCursorRadius = 20.0f;
    
    float spreadFactorX = 0.25f;
    float spreadFactorY = 0.25f;

    int numberOfPointsToDraw = 0;
    std::vector<TPointF> pointsToDraw;
    std::vector<TUsineColor> colorsToDraw;

    // Points search variables
    KDTree kdTree;
        // output list of every sample in the cursor circle
    std::vector<PointDistance2D> listOfSamplesInCircle;
        

    // dimensions variables
    std::string dimX = "duration";
    std::string dimY = "centroid";
        // holders to set the dimensions since ptListBox are integers
        // could be changed to enums
    std::vector<std::string> dimensions_x = {
        "duration",
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
        "f0_probability_std",
        "tsne_x",
        "random"
    };
    std::vector<std::string> dimensions_y = {
        "duration",
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
        "f0_probability_std",
        "tsne_y",
        "random"
    };
    
    // LUT for random phase generation
    void generateLUT();
    struct CosSin
    {
        float cos = 0.0f;
        float sin = 0.0f;
    };
    std::vector<CosSin> trigoLUT;
    //----------- LIST RELATED -----------
public:
    
    // Structure representing a sample with its coefficient, magnitudes and a flag indicating its state.
    struct SampleString
    {
        std::string sample = "null";
        SmoothCoeff coeff;
        const float* magData = nullptr;        
        bool isToRelease = false;
        
        bool isEmpty() const
        {
            return (sample == "null" && magData == nullptr) ;
        }
        
        bool isToClear(float threshold) const
        {
            return (this->coeff.getCoeff() < threshold);
        }
        
        void clear()
        {
            coeff.setTarget(0.0f, 0.0f, 0.0f);
            sample = "null";
            magData = nullptr;
            isToRelease = false;
        }
        
    };
    

    // Implementation of a polyphonic sample queue. 
    // It holds a vector of SampleString objects, each representing a sample with its coefficient and magnitude data.
    struct SampleQueue {
        std::vector<SampleString> queue;
        
        std::vector<float> globalMags;
        
        int maxPoly = 40;
        
        int NbPoints = 10;
        
        float smoothDuration = 0.0f;
        float attackDuration = 0.0f;
        float blocDuration = 0.0f;
        
        float thresholdCoeff = 0.0f;
        
        int availSpace = 10;
        
        int isCoeffNormalized = true;
        
        float normalizedCoeff = 1.0f;
        
        int activeSamples = 0;
        
        float sumCoeffs = 0.0f;
        
        void setNbPoints(const int newNbPoints)
        {
            NbPoints = newNbPoints;
            maxPoly = 2*NbPoints;
            updateAvailSpace();
        }
        
        void changeFFTSize(const int newSize)
        {
            globalMags.resize(newSize/2+1);
        }
        
        // compares the current queue with the closest points found in the KDTree search
        // and updates the queue accordingly.
        // If a point in the queue is not found in the closest points, it is marked for release.
        // else its coefficient is updated based on the closest point's distance.
        void compareListsAndUpdate(const std::vector<PointDistance2D>& closestPoints)
        {
            for(size_t i = 0; i < queue.size(); i++)
            {
                bool toClear = true;
                for(size_t j = 0; j < closestPoints.size(); j++)
                {
                    if(queue[i].sample == closestPoints[j].point->samplePath)
                    {
                        if(queue[i].isToRelease && availSpace > 0)
                        {
                            queue[i].isToRelease = false;
                            updateAvailSpace();
                        }
                        toClear = false;
                        if(isCoeffNormalized)
                        {
                            updateTargetCoeff(normalizedCoeff , (int)i);
                        } else {
                            updateTargetCoeff(closestPoints[j].distance * closestPoints[j].distance , (int)i);
                        }
                        
                    }
                }
                
                if(toClear == true)
                {
                    if(!queue[i].isEmpty())
                    {
                        queue[i].isToRelease = true;
                        queue[i].coeff.setTarget(0.0f, smoothDuration, blocDuration);
                        updateAvailSpace();
                    }
                }
                
            }
            
        }
        
        // returns the index of the sample in the queue, or -1 if not found
        int isInQueue(const std::string& stringToCheck)
        {
            
            for(int i = 0; i < maxPoly; i++)
            {
                if(queue[i].sample == stringToCheck)
                {
                    return i;
                }
            }

            return -1;
        }
        
        // returns the index of the first empty spot in the queue, or -1 if no empty spot
        int findEmptySpot()
        {
            if (availSpace <= 0)
            {
                return -1;
            }
            
            for (int i = 0; i < maxPoly; i++)
            {
                if (queue[i].isEmpty())
                {
                    return i;
                }
            }
            return -1;
        }
        
        // puts a sample in the queue at the specified index and updates the available space.
        // assumes that the index is valid and that the sample is not already in the queue
        void putInQueue(const std::string& string,const float* magPtr, const float coeff, int index)
        {
            assert(availSpace >= 0);
            assert(magPtr);
            assert(queue[index].magData == nullptr);
            
            queue[index].sample = string;            
            queue[index].magData = magPtr;
            
            updateAvailSpace();
            
            if(isCoeffNormalized)
            {
                queue[index].coeff.setTarget(normalizedCoeff, smoothDuration, blocDuration);
            } else {
                queue[index].coeff.setTarget(coeff, smoothDuration, blocDuration);
            }
            
        }
        
        // updates the coefficient of a sample in the queue at the specified index
        void updateTargetCoeff(const float coeff, const int index)
        {
            if(coeff > queue[index].coeff.getCoeff())
            {
                    //attack-time
                queue[index].coeff.setTarget(coeff, attackDuration, blocDuration);
            } else {
                    //release-time
                queue[index].coeff.setTarget(coeff, smoothDuration, blocDuration);
            }
        }
        
        // updates the coefficients of all active samples in the queue according to their target coefficient.
        void updateCoeffs()
        {
            float sum = 0.0f;
            for(auto& sample : queue)
            {
                if(!sample.isEmpty())
                {
                    sample.coeff.update();
                    sum += sample.coeff.getCoeff();
                }
            }
            sumCoeffs = sum;
            
        }
        
        // clears the samples that are marked for release and have a coefficient below the threshold
        void clearSamplesToRelease()
        {
            for(auto& sample : queue)
            {
                if(!sample.isEmpty())
                {
                    if(sample.isToClear(coeffThreshold) && sample.isToRelease)
                    {
                        sample.clear();
                        updateAvailSpace();
                    }
                }
            }
        }
        
        // updates the global magnitudes by summing the magnitudes of all samples in the queue multiplied by their coefficient.
        void updateMags(const int fftSize_2, bool mouseState)
        {
            clearMags();
            
            for(auto& sample : queue)
            {
                
                const float* localMagPtr = sample.magData;
                float coeff = sample.coeff.getCoeff();
                if(sample.isEmpty()) continue;
                
                for(int i = 0; i < fftSize_2; i++)
                {
                    assert(localMagPtr);
                    globalMags[i] += localMagPtr[i] * coeff;
                }
            }
        }
        
        // updates the available space in the queue by counting the number of active samples.
        // An active sample is one that is not empty and not marked for release.
        // It also updates the activeSamples count and the normalized coefficient.
        void updateAvailSpace()
        {
            int counter = 0;
            for (const auto& sample : queue)
            {
                if (!sample.isEmpty() && !sample.isToRelease)
                {
                    counter++;
                }
            }
            availSpace = NbPoints - counter;
            activeSamples = NbPoints - availSpace;
            if(activeSamples > 0)
            {
                normalizedCoeff = 1.0f/(float)activeSamples;
            } else {
                normalizedCoeff = 0.0f;
            }
        }
        
        // resets the queue instantly, clearing all samples
        void hardClear()
        {
            for(int i = 0; i < maxPoly; i++)
            {
                if(!queue[i].isEmpty())
                {
                    queue[i].clear();
                    updateAvailSpace();
                }
            }
        }
        
        // resets the queue by setting all samples to release, and setting their coeff to 0 in release time
        void softClear()
        {
            for(int i = 0; i < maxPoly; i++)
            {
                if(!queue[i].isEmpty())
                {
                    queue[i].isToRelease = true;
                    queue[i].coeff.setTarget(0.0f, smoothDuration, blocDuration);
                    updateAvailSpace();
                }
            }
        }
        
        void clearMags()
        {
            std::fill(globalMags.begin(), globalMags.end(), 0.0f);
        }

        // returns a comma-separated string of the samples in the queue, with empty samples as empty strings
        std::string getCommaText()
        {
            std::string commaTextString;
            commaTextString.reserve(40*200);
            
            bool first = true;
            
            for (int i = 0; i < maxPoly; ++i)
            {
                if (!first)
                {
                    commaTextString += ",";
                } else {
                    first = false;
                }
                
                if (!queue[i].isEmpty() && !queue[i].sample.empty() && queue[i].sample != "null")
                {
                    commaTextString += "\"" + queue[i].sample + "\"";
                } else {
                    commaTextString += "\"\"";
                }
            }
            
            return commaTextString;
        }
        
        
        void setReleaseDuration(float releaseDuration, float sampleRate, int blocSize)
        {
            this->smoothDuration = releaseDuration;
            this->blocDuration = (static_cast<float>(blocSize)/sampleRate) * 1000.0f;
        }
        
        void setAttackDuration(float attackDuration, float sampleRate, int blocSize)
        {
            this->attackDuration = attackDuration;
            this->blocDuration = (static_cast<float>(blocSize)/sampleRate) * 1000.0f;
        }
        
        SampleQueue()
        {
            queue.resize(sampleQueueMaxPoly);
        }
        
    } listOfOutputSamples;
    

    
}; // DatabasePlayer class



