//-----------------------------------------------------------------------------
// @file    DatabaseCreation.cpp
// @author  BrainModular team
// @brief   Analyzes the audio files of a folder and saves their descriptors in an SQLite database.
//
// This file is part of the Usine Hollyhock CPP SDK.
// Copyright (C) 2013-2026 BrainModular, BeSpline, Adamson
// SPDX-License-Identifier: MIT
//-----------------------------------------------------------------------------
#include <algorithm>
#include <cctype>
#include <chrono>
#include <cmath>
#include <cstring>
#include <limits>
#include <numbers>
#include <numeric>
#include <sstream>
#include "DatabaseCreation.h"
#include "ModuleDefinition.h"

void CreateModule(void* &pModule, AnsiCharPtr optionalString, LongBool Flag, TMasterInfo* pMasterInfo, AnsiCharPtr optionalContent)
{
	pModule = new DatabaseCreation();
}

void DestroyModule(void* pModule)
{
	// cast is important to call the good destructor
	delete static_cast<DatabaseCreation*>(pModule);
}

DatabaseCreation::DatabaseCreation()
{
	InitializeHanningWindow(m_HanningWindow, BUFFER_SIZE);
	InitializeHanningWindow(spectrumHanningWindow, SPECTRUM_SIZE);

	// the t-SNE uses the mean and the standard deviation of the descriptors, the loudness excepted
	featuresListForTSNE = {
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
		featuresListForTSNE.push_back("mfcc_mean_" + std::to_string(i));
	for (int i = 1; i <= 13; ++i)
		featuresListForTSNE.push_back("mfcc_std_" + std::to_string(i));
}

DatabaseCreation::~DatabaseCreation()
{
	// an analysis running or about to start stops at once
	needToStopAnalysis.store(true, std::memory_order_release);
	// a running analysis holds the critical section: waits for its end. An analysis thread restarted
	// but not running yet takes it later, sees needToStopAnalysis and returns at once
	if (criticalSection != nullptr)
	{
		sdkCriticalSectionLock(criticalSection);
		sdkCriticalSectionUnLock(criticalSection);
	}
	// the threads end before what they use is destroyed
	if (thread != nullptr)
		sdkThreadDestroy(thread);
	endFinishingPhase();
	if (progressThread != nullptr)
		sdkThreadDestroy(progressThread);
	try
	{
		sqlite3_close(db);
		sdkFFTDestroy(fftPtr);
		sdkFFTDestroy(fftSpectrumPtr);
	}
	catch (...)
	{
		sdkTraceLogChar("error in the destructor");
	}
	if (criticalSection != nullptr)
		sdkCriticalSectionDestroy(criticalSection);
}

void DatabaseCreation::onGetModuleInfo(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	pModuleInfo->ModuleType      = mtSimple;
	pModuleInfo->BackColor       = sdkGetUsineColor(clDataModuleColor);
	pModuleInfo->DontProcess     = TRUE;
	pModuleInfo->CanBeRandomized = FALSE;
}

void DatabaseCreation::onInitModule(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	fftPtr         = sdkFFTCreate(BUFFER_SIZE);
	fftSpectrumPtr = sdkFFTCreate(SPECTRUM_SIZE);

	criticalSection = sdkCriticalSectionCreate("critical section", FALSE);

	thread         = sdkThreadCreate("analysis thread", &DatabaseCreation::processThread, tpLow, UINT32_MAX);
	progressThread = sdkThreadCreate("progress thread", &DatabaseCreation::progressBarThread, tpLow, UINT32_MAX);
}

void DatabaseCreation::onCreateParameters(TMasterInfo* pMasterInfo, TModuleInfo* pModuleInfo)
{
	sdkCreateParam(soundsDirInParamInfo, soundsDirIn);
	sdkCreateParam(databaseDirInParamInfo, databaseDirIn);
	sdkCreateParam(databaseNameTxtInParamInfo, databaseNameTxtIn);
	sdkCreateParam(maxFileDurationFdrInParamInfo, maxFileDurationFdrIn);
	sdkCreateParam(fftSizeLboxInParamInfo, fftSizeLboxIn);
	sdkCreateParam(startAnalysisBtnInParamInfo, startAnalysisBtnIn);
	sdkCreateParam(stopAnalysisBtnInParamInfo, stopAnalysisBtnIn);
}

void DatabaseCreation::onCallBack(TUsineMessage* Message)
{
	if ((Message->message != NOTIFY_MSG_USINE_CALLBACK) || (Message->lParam != MSG_CHANGE))
		return;

	try
	{
		switch (Message->wParam)
		{
			// the folders are checked again by [start analysis]: no callback is sent for a default value
			case SOUNDS_DIR_CBID:
				if (!fsx::is_directory(soundsDirIn.getPChar()))
					sdkReport(ERROR_INVALID_SOUNDS_FOLDER, soundsDirIn.getPChar());
				break;
			case DATABASE_DIR_CBID:
				if (!fsx::is_directory(databaseDirIn.getPChar()))
					sdkReport(ERROR_INVALID_DATABASE_FOLDER, databaseDirIn.getPChar());
				break;
			case START_ANALYSIS_BTN_CBID:
				startAnalysis();
				break;
			case STOP_ANALYSIS_BTN_CBID:
				needToStopAnalysis = true;
				break;
			default:
				break;
		}
	}
	catch (...)
	{
		sdkTraceLogChar("error in onCallBack");
	}
}

void DatabaseCreation::startAnalysis()
{
	if (isProcessing.load(std::memory_order_acquire))
	{
		sdkReport(ERROR_ALREADY_PROCESSING);
		return;
	}
	const std::string soundsDir = soundsDirIn.getPChar();
	if (!fsx::is_directory(soundsDir))
	{
		sdkReport(ERROR_INVALID_SOUNDS_FOLDER, soundsDir);
		return;
	}
	const std::string databaseDir = databaseDirIn.getPChar();
	if (!fsx::is_directory(databaseDir))
	{
		sdkReport(ERROR_INVALID_DATABASE_FOLDER, databaseDir);
		return;
	}
	const std::string databaseName = databaseNameTxtIn.getPChar();
	if (databaseName.empty())
	{
		sdkReport(ERROR_NO_DATABASE_NAME);
		return;
	}
	// the analysis thread only reads this copy: the parameters can change during the analysis
	const int fftSizeItem = std::clamp(static_cast<int>(fftSizeLboxIn.getData()), 0, static_cast<int>(FftSize::Size16384));
	analysisSoundsDir     = soundsDir;
	analysisDatabasePath  = databaseDir + "/" + databaseName + DATABASE_EXTENSION;
	// the items are the powers of 2 from 1024
	analysisFftSize       = 1024 << fftSizeItem;
	analysisMaxDurationMs = maxFileDurationFdrIn.getData();
	needToStopAnalysis.store(false, std::memory_order_release);
	isProcessing.store(true, std::memory_order_release);
	sdkThreadRestart(thread);
}

void DatabaseCreation::processThread(void* pModule, TThreadPtr pThread)
{
	auto* module = static_cast<DatabaseCreation*>(pModule);
	module->sdkCriticalSectionLock(module->criticalSection);
	try
	{
		// stopped, or module destroyed, since [start analysis]: the previous database is kept
		if (module->needToStopAnalysis.load(std::memory_order_acquire))
			module->sdkReport(INFO_ANALYSIS_STOPPED);
		else
			module->AudioFilesAnalysis();
	}
	catch (...)
	{
		sdkTraceLogChar("error in the analysis thread");
	}
	// whatever the way out of the analysis
	module->endFinishingPhase();
	if (module->db != nullptr)
	{
		sqlite3_close_v2(module->db);
		module->db = nullptr;
	}
	module->isProcessing.store(false, std::memory_order_release);
	module->sdkCriticalSectionUnLock(module->criticalSection);
}

void DatabaseCreation::progressBarThread(void* pModule, TThreadPtr pThread)
{
	auto* module = static_cast<DatabaseCreation*>(pModule);
	try
	{
		bool startedTsne = false;
		int  lastStep    = -1;
		int  dots        = 0;
		// the analysis ends the finishing phase under the lock: nothing is shown after its last progress
		std::unique_lock<std::mutex> lock(module->mtx);
		while (module->finishingPhase)
		{
			const int step = module->tsneState.currentIter.load(std::memory_order_acquire);
			if (module->tsneState.iterStarted.load(std::memory_order_acquire))
				startedTsne = true;

			// percentage of the t-SNE iterations, the host shows the progress modulo 100
			const int percent = startedTsne ? std::clamp(step * 100 / TSNE_MAX_ITERATIONS, 0, 99) : 0;
			dots = (dots + 1) % 4;
			module->sdkProgressBar(("Finishing the analysis" + std::string(dots, '.')).c_str(), module->white, percent);

			if (startedTsne && step != lastStep)
			{
				if (lastStep < 0)
					module->sdkReport(INFO_TSNE_STARTED);
				module->sdkReport(INFO_TSNE_STEP, std::to_string(step) + " / " + std::to_string(TSNE_MAX_ITERATIONS));
				lastStep = step;
			}

			module->cv.wait_for(lock, std::chrono::milliseconds(500), [module] { return !module->finishingPhase; });
		}
	}
	catch (...)
	{
		sdkTraceLogChar("error in the progress thread");
	}
}

void DatabaseCreation::endFinishingPhase()
{
	{
		std::lock_guard<std::mutex> lock(mtx);
		finishingPhase = false;
	}
	cv.notify_all();
}

void DatabaseCreation::InitializeHanningWindow(std::vector<float>& hanningWindow, int size)
{
	hanningWindow.resize(size);
	for (int i = 0; i < size; ++i)
		hanningWindow[i] = 0.5f * (1.0f - cosf(2.0f * std::numbers::pi_v<float> * i / (size - 1)));
}

void DatabaseCreation::ApplyHanningWindow(TPrecision* dataPtr, Features& features, int buffer_size)
{
	for (int i = 0; i < buffer_size; ++i)
		features.windowedAudioBuffer[i] = dataPtr[i] * m_HanningWindow[i];
}

std::vector<std::string> DatabaseCreation::GetAudioPaths(const std::string& dirPath)
{
	std::vector<std::string> audioPaths;
	std::vector<std::string> allFiles;
	int unrepresentableNames = 0;
	try
	{
		// [stop analysis] and the destructor interrupt the scan of a huge folder
		fsx::list_regular_files(dirPath, allFiles, needToStopAnalysis, unrepresentableNames);
	}
	catch (const std::exception& e)
	{
		sdkReport(ERROR_SCAN_FOLDER, dirPath + " - " + e.what());
	}
	if (unrepresentableNames > 0)
		sdkReport(ERROR_UNREPRESENTABLE_NAMES, std::to_string(unrepresentableNames));

	for (const std::string& path : allFiles)
	{
		std::string filename = fsx::filename(path);

		// hidden file
		if (!filename.empty() && filename[0] == '.')
			continue;

		auto ext = fsx::extension(path);
		// through unsigned char: tolower of the negative chars of a non-ASCII name is undefined
		std::transform(ext.begin(), ext.end(), ext.begin(), [](unsigned char c) { return static_cast<char>(std::tolower(c)); });

		if (ext == ".wav" || ext == ".aiff" || ext == ".aif")
		{
			try
			{
				audioPaths.push_back(path);
			}
			catch (const std::exception& e)
			{
				sdkReport(ERROR_LIST_FILE, path + " - " + e.what());
			}
		}
	}

	return audioPaths;
}

void DatabaseCreation::AudioFilesAnalysis()
{
	sdkProgressBar("Scanning sounds directory", white);
	const std::vector<std::string> audioPaths = GetAudioPaths(analysisSoundsDir);
	if (needToStopAnalysis.load(std::memory_order_acquire))
	{
		sdkProgressBar("Analysis stopped", white);
		sdkReport(INFO_ANALYSIS_STOPPED);
		return;
	}
	// nothing to analyze: the previous database is kept
	if (audioPaths.empty())
	{
		sdkReport(ERROR_INVALID_SOUNDS_FOLDER, analysisSoundsDir);
		return;
	}

	const int fileFftSize = analysisFftSize;

	sdkFFTDestroy(fftSpectrumPtr);
	fftSpectrumPtr = sdkFFTCreate(fileFftSize);

	InitializeHanningWindow(spectrumHanningWindow, fileFftSize);

	int fileNumber = 0;

	auto startTime = std::chrono::steady_clock::now();

	const std::string dbPathName = analysisDatabasePath;

	if (fsx::exists(dbPathName))
	{
		if (!fsx::remove(dbPathName))
		{
			sdkReport(ERROR_DELETE_DATABASE, dbPathName);
			return;
		}
		sdkReport(INFO_DATABASE_DELETED, dbPathName);
	}

	if (sqlite3_open(fsx::to_utf8(dbPathName).c_str(), &db) != SQLITE_OK)
	{
		sdkReport(ERROR_CREATE_DATABASE, dbPathName + ": " + sqlite3_errmsg(db));
		// the connection is allocated even when the opening fails
		sqlite3_close(db);
		db = nullptr;
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
	if (sqlite3_exec(db, createTablesSQL, nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_CREATE_TABLE, sqlite3_errmsg(db));
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

	if (sqlite3_exec(db, fftSizeTableSQL, nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_CREATE_TABLE, sqlite3_errmsg(db));
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
		sdkReport(ERROR_PREPARE_INSERT, sqlite3_errmsg(db));
		sqlite3_close(db);
		db = nullptr;
		return;
	}
	sqlite3_bind_int(stmtFFT, 1, fileFftSize);
	if (sqlite3_step(stmtFFT) != SQLITE_DONE)
	{
		sdkReport(ERROR_INSERT_FFT_SIZE, sqlite3_errmsg(db));
		sqlite3_finalize(stmtFFT);
		sqlite3_close(db);
		db = nullptr;
		return;
	}
	sqlite3_finalize(stmtFFT);

	sdkProgressBar("Initialization complete starting the analysis...", white);

	// the files are committed by groups of FILES_PER_TRANSACTION: the files analyzed before a stop or an error are kept
	auto commitAndClose = [this]()
	{
		if (sqlite3_get_autocommit(db) == 0 && sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
		{
			sdkReport(ERROR_COMMIT_TRANSACTION, sqlite3_errmsg(db));
			sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
		}
		sqlite3_close(db);
		db = nullptr;
	};
	if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_BEGIN_TRANSACTION, sqlite3_errmsg(db));
		sqlite3_close(db);
		db = nullptr;
		return;
	}
	int filesInTransaction = 0;

	for (size_t currFile = 0; currFile < audioPaths.size(); currFile++)
	{
		// also between skipped files
		if (needToStopAnalysis.load(std::memory_order_acquire))
		{
			sdkProgressBar("Analysis stopped", white);
			sdkReport(INFO_ANALYSIS_STOPPED);
			commitAndClose();
			return;
		}

		TAudioFilePtr audioFilePtr = sdkCreateAudioFile();
		sdkLoadInMemoryAudioFile(audioFilePtr, audioPaths[currFile].c_str());

		std::string fileName = fsx::filename(audioPaths[currFile]);
		std::string filePath = audioPaths[currFile];
		uint32_t fileLength  = sdkGetSizeAudioFile(audioFilePtr);
		int numChannels      = sdkGetChannelAudioFile(audioFilePtr);
		float fileSampleRate = static_cast<float>(sdkGetSampleRateAudioFile(audioFilePtr));
		float lengthInSec    = (fileSampleRate > 0.0f) ? static_cast<float>(fileLength) / fileSampleRate : 0.0f;

		std::ostringstream fileInfo;
		fileInfo << "[" << fileNumber << "] " << fileName << ", duration: " << lengthInSec << " s";
		sdkReport(INFO_FILE_LOADED, fileInfo.str());

		std::ostringstream ossFileName;
		ossFileName << "[" << fileNumber << "] : " << fileName << "\n";
		sdkTraceLogChar(ossFileName.str());

		int progressStatus = static_cast<int>(static_cast<float>(currFile) / static_cast<float>(audioPaths.size()) * 100.0f);

		// a file that could not be loaded has no samples and no sample rate
		if (fileLength == 0 || numChannels == 0 || !(fileSampleRate > 0.0f))
		{
			sdkDestroyAudioFile(audioFilePtr);
			sdkReport(ERROR_EMPTY_FILE, fileName);
			continue;
		}

		float durationMs = static_cast<float>(fileLength) / fileSampleRate * 1000.0f;
		if (durationMs > analysisMaxDurationMs)
		{
			sdkDestroyAudioFile(audioFilePtr);
			sdkReport(ERROR_FILE_TOO_LONG, fileName);
			continue;
		}

		const std::string progressCaption = "[" + std::to_string(currFile) + "/" + std::to_string(audioPaths.size()) + "] - Analyzing " + fileName;
		sdkProgressBar(progressCaption.c_str(), white, progressStatus);

		// initialization
		FileResults fileResults;
		fileResults.setGlobalInfos(filePath, lengthInSec, fileSampleRate, fileLength, fileFftSize);
		std::vector<FeaturesResults> sumFileFeatures;
		// sum of the magnitude spectra of the frames, for their mean
		std::vector<float> spectrumSum(fileFftSize / 2 + 1, 0.0f);
		int numSpectrumFrames = 0;
		Features features = Features(BUFFER_SIZE, BUFFER_SIZE / 2 + 1, fileSampleRate);
		descriptors.mfcc.setFftSize(BUFFER_SIZE, fileSampleRate);
		descriptors.loudness.setFftSize(BUFFER_SIZE, fileSampleRate);

		// only the first channel is analyzed
		for (int channel = 0; channel < 1; ++channel)
		{
			const TPrecision* audioData = sdkGetSampleArrayAudioFile(audioFilePtr, channel + 1);
			if (audioData == nullptr)
			{
				sdkReport(ERROR_NO_SAMPLES, fileName);
				sdkDestroyAudioFile(audioFilePtr);
				continue;
			}

			const int spectrumSize = fileFftSize;
			const int hopSize      = spectrumSize / 4;
			const int analysisSize = BUFFER_SIZE;

			std::vector<float> analysisBuffer;
			std::vector<float> spectrumBuffer;

			int spectrumCounter = 0;
			int analysisCounter = 0;

			// a file shorter than a frame is analyzed without window
			bool toWindowSpectrum = !(static_cast<int>(fileLength) < spectrumSize);
			bool toWindowAnalysis = !(static_cast<int>(fileLength) < analysisSize);

			// magnitudes: frames of [fft size] samples, overlapping by 75%
			while (spectrumCounter < static_cast<int>(fileLength))
			{
				int remaining = fileLength - spectrumCounter;

				if (remaining < spectrumSize)
				{
					spectrumBuffer.assign(audioData + spectrumCounter, audioData + spectrumCounter + remaining);
					spectrumBuffer.resize(spectrumSize, 0.0f);
					SpectrumFFT(spectrumBuffer.data(), spectrumSum, spectrumSize, toWindowSpectrum);
					numSpectrumFrames++;
					break;
				}
				else
				{
					spectrumBuffer.assign(audioData + spectrumCounter, audioData + spectrumCounter + spectrumSize);
					SpectrumFFT(spectrumBuffer.data(), spectrumSum, spectrumSize, toWindowSpectrum);
					numSpectrumFrames++;
				}

				spectrumCounter += hopSize;
			}

			// descriptors: frames of BUFFER_SIZE samples, without overlap
			while (analysisCounter < static_cast<int>(fileLength))
			{
				int remaining = fileLength - analysisCounter;

				if (remaining < analysisSize)
				{
					analysisBuffer.assign(audioData + analysisCounter, audioData + analysisCounter + remaining);
					analysisBuffer.resize(analysisSize, 0.0f);
					Calculation(analysisBuffer.data(), descriptors, sumFileFeatures, features, fileSampleRate, toWindowAnalysis);
					break;
				}
				else
				{
					analysisBuffer.assign(audioData + analysisCounter, audioData + analysisCounter + analysisSize);
					Calculation(analysisBuffer.data(), descriptors, sumFileFeatures, features, fileSampleRate, toWindowAnalysis);
				}

				analysisCounter += analysisSize;
			}

			computeFileStatistics(&fileResults, sumFileFeatures, spectrumSum, numSpectrumFrames);

			if (!insertFileResultsInDB(db, fileResults))
			{
				sdkReport(ERROR_STORE_FILE, fileName);
				commitAndClose();
				sdkDestroyAudioFile(audioFilePtr);
				return;
			}

			sdkDestroyAudioFile(audioFilePtr);

			fileNumber++;
			if (needToStopAnalysis.load(std::memory_order_acquire))
			{
				sdkProgressBar("Analysis stopped", white);
				sdkReport(INFO_ANALYSIS_STOPPED);
				commitAndClose();
				return;
			}

			if (++filesInTransaction >= FILES_PER_TRANSACTION)
			{
				filesInTransaction = 0;
				if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK || sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
				{
					sdkReport(ERROR_COMMIT_TRANSACTION, sqlite3_errmsg(db));
					commitAndClose();
					return;
				}
			}
		}
	}

	if (sqlite3_exec(db, "COMMIT;", nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_COMMIT_TRANSACTION, sqlite3_errmsg(db));
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
		sqlite3_close(db);
		db = nullptr;
		return;
	}

	const char* statTableSQL = R"SQL(
        CREATE TABLE IF NOT EXISTS FileStats (
        featureName TEXT PRIMARY KEY,
        mean REAL,
        stddev REAL
        );
        )SQL";
	if (sqlite3_exec(db, statTableSQL, nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_CREATE_TABLE, sqlite3_errmsg(db));
		sqlite3_close(db);
		db = nullptr;
		return;
	}

	// the progress thread shows the t-SNE iterations of this analysis
	tsneState.currentIter.store(0, std::memory_order_release);
	tsneState.iterStarted.store(false, std::memory_order_release);
	{
		std::lock_guard<std::mutex> lock(mtx);
		finishingPhase = true;
	}
	sdkThreadRestart(progressThread);

	// total mean and standard deviation of each feature, for the Z-score normalization
	if (!computeAndStoreFeatureStats(db, featuresListForTSNE))
	{
		sdkReport(ERROR_FEATURE_STATS, sqlite3_errmsg(db));
		sqlite3_close(db);
		db = nullptr;
		return;
	}

	// t-SNE of the features of featuresListForTSNE
	if (!requestForTSNE())
	{
		// the errors are raised by the functions of the t-SNE, the database only has to be closed
		sqlite3_close(db);
		db = nullptr;
		return;
	}

	// stops the progress bar thread before the last progress
	endFinishingPhase();

	auto endTime  = std::chrono::steady_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime).count();

	if (sqlite3_close(db) != SQLITE_OK)
	{
		sdkReport(ERROR_CLOSE_DATABASE);
		return;
	}
	// closed: the destructor must not close it again
	db = nullptr;

	sdkReport(INFO_ANALYSIS_COMPLETED, std::to_string(fileNumber) + " files in " + std::to_string(duration) + " s");

	sdkProgressBar(std::string("Database successfully saved at :\n" + dbPathName).c_str(), white, 100);
}

void DatabaseCreation::Calculation(TPrecision* dataPtr, Descriptors& descriptors, std::vector<FeaturesResults>& sumFileFeatures, Features& features, float sample_rate, bool windowing)
{
	features.ptrAudio = dataPtr;
	if (windowing)
	{
		ApplyHanningWindow(dataPtr, features, features.buffer_size);
	}
	else
	{
		// a file shorter than a frame: the samples themselves, zero padded, for the FFT and the pitch
		std::copy(dataPtr, dataPtr + features.buffer_size, features.windowedAudioBuffer.begin());
	}
	sdkFFTForward(fftPtr, features.windowedAudioBuffer.data(), m_Re.data(), m_Im.data());

	for (int i = 0; i < BUFFER_SIZE / 2 + 1; ++i)
	{
		features.magnitudes[i]               = sqrtf(m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i]);
		features.amplitudes[i]               = sqrtf(m_Re[i] * m_Re[i] + m_Im[i] * m_Im[i]);
		features.frequencies[i]              = i * sample_rate / BUFFER_SIZE;
		features.sum_magnitudes             += features.magnitudes[i];
		features.sum_amplitudes             += features.amplitudes[i];
		features.sum_magnitudes_times_freqs += features.magnitudes[i] * features.frequencies[i];
		features.sum_amplitudes_times_freqs += features.amplitudes[i] * features.frequencies[i];
	}
	features.unnormalized_magnitudes = features.magnitudes;
	if (features.sum_magnitudes != 0.0f)
	{
		for (int i = 0; i < BUFFER_SIZE / 2 + 1; i++)
			features.magnitudes[i] /= features.sum_magnitudes;
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
	calcResults.harmonicMfcc     = features.harmonicMfcc;
	calcResults.temporalRMS      = features.temporalRMS;
	calcResults.spectralCentroid = features.spectralCentroid;
	calcResults.spectralFlatness = features.spectralFlatness;
	calcResults.spectralSpread   = features.spectralSpread;
	calcResults.harmonicF0_YIN   = features.harmonicF0_YIN;
	calcResults.loudness         = features.loudness;

	sumFileFeatures.push_back(calcResults);

	m_Re.clear();
	m_Im.clear();
	m_Re.resize(BUFFER_SIZE / 2 + 1);
	m_Im.resize(BUFFER_SIZE / 2 + 1);

	features.sum_magnitudes             = 0.0f;
	features.sum_magnitudes_times_freqs = 0.0f;
	features.sum_amplitudes             = 0.0f;
	features.sum_amplitudes_times_freqs = 0.0f;
}

void DatabaseCreation::SpectrumFFT(TPrecision* dataPtr, std::vector<float>& spectrumSum, int fftSize, bool windowing)
{
	std::vector<float> audioBuffer         = std::vector<float>(fftSize, 0.0f);
	std::vector<float> audioBufferWindowed = std::vector<float>(fftSize, 0.0f);
	std::memcpy(audioBuffer.data(), dataPtr, sizeof(float) * fftSize);

	if (windowing)
	{
		for (int i = 0; i < fftSize; i++)
			audioBufferWindowed[i] = audioBuffer[i] * spectrumHanningWindow[i];
	}
	else
	{
		for (int i = 0; i < fftSize; i++)
			audioBufferWindowed[i] = audioBuffer[i];
	}

	std::vector<float> real = std::vector<float>(fftSize / 2 + 1, 0.0f);
	std::vector<float> im   = std::vector<float>(fftSize / 2 + 1, 0.0f);

	sdkFFTForward(fftSpectrumPtr, audioBufferWindowed.data(), real.data(), im.data());

	// only the sum of the frames is kept (a long file has thousands of frames), in their order
	for (int i = 0; i < fftSize / 2 + 1; i++)
		spectrumSum[i] += sqrtf(powf(real[i], 2.0f) + powf(im[i], 2.0f));
}

void DatabaseCreation::computeFileStatistics(FileResults* fileResults, const std::vector<FeaturesResults>& calcResults, const std::vector<float>& spectrumSum, int numSpectrumFrames)
{
	// Mean
	fileResults->spectralCentroid.mean = CalcMeanValue(calcResults, &FeaturesResults::spectralCentroid);
	fileResults->spectralSpread.mean   = CalcMeanValue(calcResults, &FeaturesResults::spectralSpread);
	fileResults->spectralFlatness.mean = CalcMeanValue(calcResults, &FeaturesResults::spectralFlatness);
	fileResults->RMS.mean              = CalcMeanValue(calcResults, &FeaturesResults::temporalRMS);
	fileResults->loudness.mean         = CalcMeanValue(calcResults, &FeaturesResults::loudness);
	fileResults->MFCC.mean             = CalcMeanVector(calcResults, &FeaturesResults::harmonicMfcc, 13);
	fileResults->f0_yin.mean           = CalcMeanVector(calcResults, &FeaturesResults::harmonicF0_YIN, 2);
	fileResults->fftMags.mean          = spectrumSum;
	if (numSpectrumFrames > 0)
	{
		for (float& mag : fileResults->fftMags.mean)
			mag /= static_cast<float>(numSpectrumFrames);
	}

	// the spectrum is scaled up so that its maximum reaches at least adjustThreshold
	float maxAdjust       = 1.0f;
	float adjustThreshold = 600.0f;
	float maxMagInOutput  = 0.0f;
	for (const float& mag : fileResults->fftMags.mean)
	{
		if (mag > maxMagInOutput)
			maxMagInOutput = mag;
	}

	// a silent file keeps its spectrum of zeros (600 / 0 made it NaN)
	if (maxMagInOutput > 0.0f && maxMagInOutput < adjustThreshold)
		maxAdjust = adjustThreshold / maxMagInOutput;
	if (!std::isfinite(maxAdjust))
		maxAdjust = 1.0f;

	for (float& mag : fileResults->fftMags.mean)
		mag *= sqrtf(maxAdjust);

	// Std Dev
	fileResults->spectralCentroid.stdDev = CalcStdDev(calcResults, &FeaturesResults::spectralCentroid);
	fileResults->spectralSpread.stdDev   = CalcStdDev(calcResults, &FeaturesResults::spectralSpread);
	fileResults->spectralFlatness.stdDev = CalcStdDev(calcResults, &FeaturesResults::spectralFlatness);
	fileResults->RMS.stdDev              = CalcStdDev(calcResults, &FeaturesResults::temporalRMS);
	fileResults->loudness.stdDev         = CalcStdDev(calcResults, &FeaturesResults::loudness);
	fileResults->MFCC.stdDev             = CalcStdDevVector(calcResults, &FeaturesResults::harmonicMfcc, 13);
	fileResults->f0_yin.stdDev           = CalcStdDevVector(calcResults, &FeaturesResults::harmonicF0_YIN, 2);
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
		sdkReport(ERROR_PREPARE_INSERT, sqlite3_errmsg(db));
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
	                  static_cast<int>(sizeof(float) * results.fftMags.mean.size()),
	                  SQLITE_TRANSIENT);

	if (sqlite3_step(stmt) != SQLITE_DONE)
	{
		sdkReport(ERROR_INSERT_FILE, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);
	return true;
}

bool DatabaseCreation::computeAndStoreFeatureStats(sqlite3* db, const std::vector<std::string>& features)
{
	sqlite3_stmt* stmt = nullptr;

	if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_BEGIN_TRANSACTION, sqlite3_errmsg(db));
		return false;
	}

	bool success = true;

	for (const auto& feature : features)
	{
		std::string query = "SELECT " + feature + " FROM FileResults WHERE " + feature + " IS NOT NULL;";
		std::vector<double> values;

		if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
		{
			sdkReport(ERROR_PREPARE_FEATURE_QUERY, feature + ": " + sqlite3_errmsg(db));
			success = false;
			break;
		}

		int rc = SQLITE_OK;
		while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
			values.push_back(sqlite3_column_double(stmt, 0));

		if (rc != SQLITE_DONE)
		{
			// the statistics of a feature can not be computed from a part of its values
			sdkReport(ERROR_READ_DATABASE, feature + ": " + sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			success = false;
			break;
		}
		sqlite3_finalize(stmt);

		if (values.empty())
			continue;

		double mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();

		double sq_sum = 0.0;
		for (double v : values)
			sq_sum += (v - mean) * (v - mean);

		double stddev = std::sqrt(sq_sum / values.size());

		const char* insertSQL =
		"INSERT INTO FileStats (featureName, mean, stddev) VALUES (?, ?, ?) "
		"ON CONFLICT(featureName) DO UPDATE SET mean=excluded.mean, stddev=excluded.stddev;";

		if (sqlite3_prepare_v2(db, insertSQL, -1, &stmt, nullptr) != SQLITE_OK)
		{
			sdkReport(ERROR_PREPARE_STATS_INSERT, sqlite3_errmsg(db));
			success = false;
			break;
		}

		sqlite3_bind_text(stmt, 1, feature.c_str(), -1, SQLITE_STATIC);
		sqlite3_bind_double(stmt, 2, mean);
		sqlite3_bind_double(stmt, 3, stddev);

		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			sdkReport(ERROR_INSERT_FEATURE_STATS, feature + ": " + sqlite3_errmsg(db));
			success = false;
			sqlite3_finalize(stmt);
			break;
		}

		sqlite3_finalize(stmt);
	}

	if (success)
	{
		if (sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
		{
			sdkReport(ERROR_COMMIT_TRANSACTION, sqlite3_errmsg(db));
			return false;
		}
	}
	else
	{
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
	}

	return success;
}

bool DatabaseCreation::requestForTSNE()
{
	std::vector<double> flatDataset;
	// the file of each row: the coordinates of the row i are stored for the file ids[i]
	std::vector<int> ids;
	const int numDims = static_cast<int>(featuresListForTSNE.size());

	// the features are normalized with their statistics: the Hz features (centroid, spread, f0) would hide the others
	std::string query = buildTSNEQuery(featuresListForTSNE, true);

	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_PREPARE_TSNE_QUERY, sqlite3_errmsg(db));
		return false;
	}

	int rc = SQLITE_OK;
	while ((rc = sqlite3_step(stmt)) == SQLITE_ROW)
	{
		// the column 0 is the id of the file
		ids.push_back(sqlite3_column_int(stmt, 0));
		for (int i = 1; i <= numDims; ++i)
		{
			// NULL (a zero standard deviation) is read as 0, the mean
			const double value = sqlite3_column_double(stmt, i);
			flatDataset.push_back(std::isfinite(value) ? value : 0.0);
		}
	}
	if (rc != SQLITE_DONE)
	{
		sdkReport(ERROR_READ_DATABASE, sqlite3_errmsg(db));
		sqlite3_finalize(stmt);
		return false;
	}

	sqlite3_finalize(stmt);

	const int numFiles = static_cast<int>(ids.size());
	if (numFiles == 0 || flatDataset.size() != static_cast<size_t>(numFiles) * static_cast<size_t>(numDims))
	{
		sdkReport(ERROR_TSNE_DIMENSIONS, std::to_string(flatDataset.size()) + " values for " + std::to_string(numFiles) + " files and " + std::to_string(numDims) + " dimensions");
		return false;
	}

	if (!processTSNE(flatDataset.data(), ids, numDims))
	{
		if (needToStopAnalysis.load(std::memory_order_acquire))
			sdkReport(INFO_TSNE_STOPPED);
		else
			sdkReport(ERROR_TSNE);
		return false;
	}

	return true;
}

std::string DatabaseCreation::buildTSNEQuery(const std::vector<std::string>& features, bool normalizeTSNE)
{
	// the id first, then a column per feature, the rows in the order of the ids
	std::string select = "SELECT r.id";
	std::string from   = " FROM FileResults r\n";
	int aliasCount = 0;

	for (const auto& feature : features)
	{
		if (normalizeTSNE)
		{
			// Z-score with the statistics of FileStats, 0 (the mean) for a feature of zero standard deviation
			const std::string alias = "f" + std::to_string(aliasCount++);
			select += ",\n  CASE WHEN " + alias + ".stddev > 0 THEN (r." + feature + " - " + alias + ".mean) / " + alias + ".stddev ELSE 0.0 END AS " + feature + "_norm";
			from += "LEFT JOIN FileStats " + alias + " ON " + alias + ".featureName = '" + feature + "'\n";
		}
		else
		{
			select += ",\n  r." + feature;
		}
	}

	return select + from + "ORDER BY r.id;";
}

bool DatabaseCreation::processTSNE(double* dataset, const std::vector<int>& ids, int dimensions)
{
	double perplexity = 30;
	double theta      = 0.3;

	const int NFiles = static_cast<int>(ids.size());
	std::vector<double> results(2 * NFiles);

	if (NFiles < TSNE_MIN_FILES)
	{
		// the perplexity (NFiles - 1) / 3 would be 0: the files are placed on the diagonal, in their order
		sdkReport(INFO_TSNE_TOO_FEW_FILES, std::to_string(NFiles));
		for (int i = 0; i < NFiles; ++i)
			results[2 * i] = results[2 * i + 1] = (NFiles == 1) ? 0.5 : static_cast<double>(i) / (NFiles - 1);
		if (!storeTSNEinDB(results, ids))
		{
			sdkReport(ERROR_STORE_TSNE);
			return false;
		}
		return true;
	}

	TSNE::run(dataset, NFiles, dimensions, results.data(),
	          2,                   // num output dims
	          perplexity, theta,
	          1,                   // random seed
	          false,               // skip random init
	          TSNE_MAX_ITERATIONS, // maximum iterations
	          250,                 // stop_lying_iter
	          250,                 // mom_switch_iter
	          tsneState,           // state of the algorithm for progress bar
	          needToStopAnalysis);

	if (needToStopAnalysis.load(std::memory_order_acquire))
		return false;

	// a coordinate that is not a number would be stored as NULL
	if (!std::all_of(results.begin(), results.end(), [](double v) { return std::isfinite(v); }))
		return false;

	// normalize between 0 and 1
	double x_min = std::numeric_limits<double>::max();
	double x_max = std::numeric_limits<double>::lowest();
	double y_min = std::numeric_limits<double>::max();
	double y_max = std::numeric_limits<double>::lowest();

	for (int i = 0; i < NFiles; ++i)
	{
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
		results[2 * i]     = (results[2 * i] - x_min) / x_range;
		results[2 * i + 1] = (results[2 * i + 1] - y_min) / y_range;
	}

	if (!storeTSNEinDB(results, ids))
	{
		sdkReport(ERROR_STORE_TSNE);
		return false;
	}
	return true;
}

bool DatabaseCreation::storeTSNEinDB(const std::vector<double>& TSNEresults, const std::vector<int>& ids)
{
	const int numFiles = static_cast<int>(ids.size());
	if (TSNEresults.size() != static_cast<size_t>(numFiles) * 2)
	{
		sdkReport(ERROR_TSNE_SIZE);
		return false;
	}

	if (sqlite3_exec(db, "BEGIN TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_BEGIN_TRANSACTION, sqlite3_errmsg(db));
		return false;
	}

	const char* updateSQL = "UPDATE FileResults SET tsne_x = ?, tsne_y = ? WHERE id = ?;";

	sqlite3_stmt* stmt = nullptr;
	if (sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_PREPARE_TSNE_UPDATE, sqlite3_errmsg(db));
		sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
		return false;
	}

	for (int i = 0; i < numFiles; ++i)
	{
		sqlite3_bind_double(stmt, 1, TSNEresults[2 * i]);     // tsne_x
		sqlite3_bind_double(stmt, 2, TSNEresults[2 * i + 1]); // tsne_y
		sqlite3_bind_int(stmt, 3, ids[i]);                    // id

		if (sqlite3_step(stmt) != SQLITE_DONE)
		{
			sdkReport(ERROR_UPDATE_TSNE, "id " + std::to_string(ids[i]) + ": " + sqlite3_errmsg(db));
			sqlite3_finalize(stmt);
			sqlite3_exec(db, "ROLLBACK;", nullptr, nullptr, nullptr);
			return false;
		}
		sqlite3_reset(stmt);
	}

	sqlite3_finalize(stmt);

	if (sqlite3_exec(db, "END TRANSACTION;", nullptr, nullptr, nullptr) != SQLITE_OK)
	{
		sdkReport(ERROR_COMMIT_TRANSACTION, sqlite3_errmsg(db));
		return false;
	}

	return true;
}
