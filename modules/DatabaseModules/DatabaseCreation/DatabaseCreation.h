//-----------------------------------------------------------------------------
// @file    DatabaseCreation.h
// @author  BrainModular team
// @brief   Analyzes the audio files of a folder and saves their descriptors in an SQLite database.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#pragma once

#include <atomic>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <string>
#include <vector>

#include "UserDefinitions.h"
#include "../libs/utils/tsne/tsne.h"
#include "../libs/utils/functions/HFeatures.h"
#include "../libs/utils/functions/StatisticsFunctions.h"
#include "../libs/sqlite3lib/sqlite3.h"
#include "../libs/utils/HaudioBuffer.h"
#include "../libs/utils/Hdescriptors.h"

#if defined(__APPLE__)
// std::filesystem needs macOS 10.15 at runtime (libc++ availability); the modules target 10.13,
// so file-system access goes through a tiny POSIX-based layer on macOS.
#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
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
	// the paths of Usine are UTF-8 on macOS, as the file names of SQLite
	inline std::string to_utf8(const std::string& p)  { return p; }

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
	// matching std::filesystem::recursive_directory_iterator defaults), until stop is set.
	// skipped counts the names that can not be passed to Usine (never on macOS)
	inline void list_regular_files(const std::string& dir, std::vector<std::string>& out, const std::atomic<bool>& stop, int& skipped)
	{
		DIR* d = ::opendir(dir.c_str());
		if (!d) return;
		while (struct dirent* e = ::readdir(d))
		{
			if (stop.load(std::memory_order_relaxed)) break;
			const std::string name = e->d_name;
			if (name == "." || name == "..") continue;
			const std::string full = dir + ((!dir.empty() && dir.back() == '/') ? "" : "/") + name;
			struct stat st;
			if (::lstat(full.c_str(), &st) != 0) continue;
			if (S_ISDIR(st.st_mode))
				list_regular_files(full, out, stop, skipped);
			else if (S_ISREG(st.st_mode) || (S_ISLNK(st.st_mode) && is_regular_file(full)))
				out.push_back(full);
		}
		::closedir(d);
	}
#else
	// the error_code overloads: an inaccessible path is reported as missing instead of throwing
	inline bool exists(const std::string& p)          { std::error_code ec; return std::filesystem::exists(p, ec); }
	inline bool is_directory(const std::string& p)    { std::error_code ec; return std::filesystem::is_directory(p, ec); }
	inline bool is_regular_file(const std::string& p) { std::error_code ec; return std::filesystem::is_regular_file(p, ec); }
	inline bool remove(const std::string& p)          { std::error_code ec; return std::filesystem::remove(p, ec); }
	inline std::string filename(const std::string& p) { return std::filesystem::path(p).filename().string(); }
	inline std::string extension(const std::string& p){ return std::filesystem::path(p).extension().string(); }

	// the paths of Usine are in the ANSI code page on Windows, SQLite opens UTF-8 file names
	inline std::string to_utf8(const std::string& p)
	{
		try
		{
			const auto u8 = std::filesystem::path(p).u8string();
			return std::string(u8.begin(), u8.end());
		}
		catch (const std::exception&)
		{
			return p;
		}
	}

	// recursive listing of regular files until stop is set, the folders that can not be read are skipped.
	// skipped counts the names that the ANSI code page of Usine can not represent: Usine could not open them
	inline void list_regular_files(const std::string& dir, std::vector<std::string>& out, const std::atomic<bool>& stop, int& skipped)
	{
		const auto options = std::filesystem::directory_options::skip_permission_denied;
		for (auto it = std::filesystem::recursive_directory_iterator(dir, options); it != std::filesystem::recursive_directory_iterator(); ++it)
		{
			if (stop.load(std::memory_order_relaxed))
				break;
			std::error_code ec;
			if (!it->is_regular_file(ec))
				continue;
			try
			{
				out.push_back(it->path().string());
			}
			catch (const std::system_error&)
			{
				skipped++;
			}
		}
	}
#endif
}

// frames of the audio descriptors, without overlap
constexpr int BUFFER_SIZE   = 2048;
// initial size of the spectrum frames, then set by [fft size]
constexpr int SPECTRUM_SIZE = 16384;

// for float results
struct StatsStruct
{
	float mean   = 0.0f;
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
	std::string filePath   = "emptyFile";
	float       duration   = 0.0f;
	float       sampleRate = 0.0f;
	uint32_t    numSamples = 0;

	// results
	StatsStruct    spectralCentroid;
	StatsStruct    spectralSpread;
	StatsStruct    spectralFlatness;
	StatsStruct    RMS;
	StatsStruct    loudness;
	StatsVecStruct MFCC    = StatsVecStruct(13);
	StatsVecStruct f0_yin  = StatsVecStruct(2);
	StatsVecStruct fftMags = StatsVecStruct(SPECTRUM_SIZE / 2 + 1);

	FileResults() = default;

	void setGlobalInfos(const std::string& filePath, float duration, float sampleRate, uint32_t numSamples, int fftSize)
	{
		this->filePath   = filePath;
		this->duration   = duration;
		this->sampleRate = sampleRate;
		this->numSamples = numSamples;

		fftMags.resize(fftSize / 2 + 1);
	}
};

class DatabaseCreation : public UserModuleBase
{
public:
	DatabaseCreation();
	~DatabaseCreation() override;

	void onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo) override;
	void onCallBack(TUsineMessage* Message) override;

private:
	// items of the fft size list box
	enum class FftSize
	{
		Size1024,
		Size2048,
		Size4096,
		Size8192,
		Size16384
	};

	UsineEventClass soundsDirIn;
	UsineEventClass databaseDirIn;
	UsineEventClass databaseNameTxtIn;
	UsineEventClass maxFileDurationFdrIn;
	UsineEventClass fftSizeLboxIn;
	UsineEventClass startAnalysisBtnIn;
	UsineEventClass stopAnalysisBtnIn;

	// the analysis runs in thread and holds criticalSection until its end
	TThreadPtr          thread             = nullptr;
	// shows the progress of the t-SNE, started by the analysis once the files are analyzed
	TThreadPtr          progressThread     = nullptr;
	TCriticalSectionPtr criticalSection    = nullptr;
	// set by [start analysis], cleared by the analysis thread at its end
	std::atomic<bool>   isProcessing       {false};
	// set by [stop analysis] and the destructor, cleared by [start analysis] only
	std::atomic<bool>   needToStopAnalysis {false};
	TUsineFFTPtr        fftPtr             = nullptr;
	TUsineFFTPtr        fftSpectrumPtr     = nullptr;

	// parameters read by [start analysis] for the analysis thread, which never reads the parameters
	std::string analysisSoundsDir;
	std::string analysisDatabasePath;
	int         analysisFftSize       = 4096;
	float       analysisMaxDurationMs = 2000.0f;

	// analysis
	sqlite3*                 db                    = nullptr;
	// the mfcc and the loudness are set again to the sample rate of each analyzed file
	Descriptors              descriptors           {BUFFER_SIZE, BUFFER_SIZE, static_cast<float>(sdkGetSampleRate()), 25};
	std::vector<TPrecision>  m_HanningWindow       = std::vector<TPrecision>(BUFFER_SIZE);
	std::vector<TPrecision>  spectrumHanningWindow = std::vector<TPrecision>(SPECTRUM_SIZE);
	std::vector<TPrecision>  m_Re                  = std::vector<TPrecision>(BUFFER_SIZE / 2 + 1);
	std::vector<TPrecision>  m_Im                  = std::vector<TPrecision>(BUFFER_SIZE / 2 + 1);
	std::vector<std::string> featuresListForTSNE;

	// progress bar: the progress thread runs while finishingPhase is true, both guarded by mtx
	UsineColor              white          = sdkAlphaColor(1.0f, 1.0f, 1.0f, 1.0f);
	bool                    finishingPhase = false;
	std::mutex              mtx;
	std::condition_variable cv;
	TSNEState               tsneState;

	// thread functions must be static, the module comes back through pModule
	static void processThread(void* pModule, TThreadPtr pThread);
	static void progressBarThread(void* pModule, TThreadPtr pThread);

	// checks the parameters and starts the analysis thread, the previous database is kept when it is refused
	void startAnalysis();
	void AudioFilesAnalysis();
	// stops the progress thread: nothing is shown by it after this call
	void endFinishingPhase();

	void InitializeHanningWindow(std::vector<float>& hanningWindow, int size);
	void ApplyHanningWindow(TPrecision* dataPtr, Features& features, int buffer_size);

	// scans the folder (until the analysis is stopped) and returns the paths of its audio files
	std::vector<std::string> GetAudioPaths(const std::string& dirPath);

	// computes the features of a frame
	void Calculation(TPrecision* dataPtr, Descriptors& descriptors, std::vector<FeaturesResults>& sumFileFeatures, Features& features, float sample_rate, bool windowing);
	// adds the magnitude spectrum of a frame to spectrumSum
	void SpectrumFFT(TPrecision* dataPtr, std::vector<float>& spectrumSum, int fftSize, bool windowing);

	// computes the mean and the standard deviation of the features of a file
	void computeFileStatistics(FileResults* fileResults, const std::vector<FeaturesResults>& calcResults, const std::vector<float>& spectrumSum, int numSpectrumFrames);
	// stores the results of a file in the database
	[[nodiscard]] bool insertFileResultsInDB(sqlite3* db, const FileResults& results);

	// computes and stores the global statistics of the features, for the Z-score normalization
	[[nodiscard]] bool computeAndStoreFeatureStats(sqlite3* db, const std::vector<std::string>& features);
	[[nodiscard]] bool requestForTSNE();
	[[nodiscard]] std::string buildTSNEQuery(const std::vector<std::string>& features, bool normalizeTSNE);
	// t-SNE of the rows of dataset, the row i is the file ids[i]
	[[nodiscard]] bool processTSNE(double* dataset, const std::vector<int>& ids, int dimensions);
	[[nodiscard]] bool storeTSNEinDB(const std::vector<double>& TSNEresults, const std::vector<int>& ids);
};
