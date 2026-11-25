# AudioAnalysisDescriptors: audio descriptors of the input (pitch, spectrum, loudness, MFCC)
#
# Builds the AudioAnalysisDescriptors module for the three platforms of Usine:
#   Windows  64-bit          Audio Analysis Descriptors.usr-win64
#   macOS    universal       Audio Analysis Descriptors.usr-macos64uni   (x86_64 10.13 + arm64)
#   Linux    64-bit          Audio Analysis Descriptors.usr-linint64
# The binaries land in <repo>/bin/AudioAnalysis at the end of the build, like the Xcode and Visual Studio projects.
#
#   cmake -S <this folder> -B <build folder> -DCMAKE_BUILD_TYPE=Release
#   cmake --build <build folder> -j
#
# Cross builds: -DCMAKE_TOOLCHAIN_FILE=<toolchain> for MinGW (Windows) or zig (Linux).

CMAKE_MINIMUM_REQUIRED (VERSION 3.27)

# Minimum macOS: 10.13 for x86_64 (clang raises the arm64 slice to 11.0 by itself)
IF (APPLE AND NOT CMAKE_OSX_DEPLOYMENT_TARGET)
    SET (CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version")
ENDIF ()

PROJECT (AudioAnalysisDescriptors CXX C)

SET (CMAKE_CXX_STANDARD 23)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_C_STANDARD 11)

# macOS and Linux: only the functions marked USINE_MODULE_EXPORT are exported, as in examples/CMakeLists.txt
# (a Windows DLL exports only them anyway).
# Usine loads many modules in one process: with every symbol exported, two modules can end up sharing a
# variable or a function that each of them defines (the constants of ModuleDefinition.h, a library compiled in).
# On macOS, dyld gives every module the weak definitions of the first module loaded.
IF (NOT WIN32)
    SET (CMAKE_C_VISIBILITY_PRESET hidden)
    SET (CMAKE_CXX_VISIBILITY_PRESET hidden)
    SET (CMAKE_VISIBILITY_INLINES_HIDDEN ON)
ENDIF ()

IF (APPLE)
    SET (CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures")
ENDIF ()

# the same defines as the Xcode and Visual Studio projects
IF (WIN32)
    ADD_COMPILE_DEFINITIONS (_WIN32_WINNT=0x0601 NOMINMAX WIN32_LEAN_AND_MEAN)
ELSEIF (APPLE)
    ADD_COMPILE_DEFINITIONS (MACOSX=1 USINE_OSX_1068=1)
ENDIF ()

IF (MSVC)
    ADD_COMPILE_OPTIONS (/W3 /wd4996)
ELSE ()
    # some SDK fields are deprecated but still copied with TParamInfo, like in the examples
    ADD_COMPILE_OPTIONS (-Wno-write-strings -Wno-unused-parameter -Wno-deprecated-declarations)
ENDIF ()

SET (OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../bin/AudioAnalysis")
SET (SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../sdk")
FILE (GLOB SDK_SRC "${SDK_DIR}/UserModule.cpp" "${SDK_DIR}/UserUtils.cpp")

# must be set before ADD_LIBRARY: the output directory of a target is read when it is created
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTDIR}")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTDIR}")

ADD_LIBRARY (AudioAnalysisDescriptors SHARED
        AudioAnalysis.cpp
        Hanalysis/Hanalysis.cpp
        Hanalysis/functions/Inharmonicity.cpp
        Hanalysis/functions/Loudness.cpp
        Hanalysis/functions/MFCC.cpp
        Hanalysis/functions/OddToEvenRatio.cpp
        Hanalysis/functions/RMS.cpp
        Hanalysis/functions/Sharpness.cpp
        Hanalysis/functions/SpectralCentroid.cpp
        Hanalysis/functions/SpectralCrest.cpp
        Hanalysis/functions/SpectralDecrease.cpp
        Hanalysis/functions/SpectralEntropy.cpp
        Hanalysis/functions/SpectralFlatness.cpp
        Hanalysis/functions/SpectralKurtosis.cpp
        Hanalysis/functions/SpectralPeaks.cpp
        Hanalysis/functions/SpectralRolloff.cpp
        Hanalysis/functions/SpectralSkewness.cpp
        Hanalysis/functions/SpectralSlope.cpp
        Hanalysis/functions/SpectralSpread.cpp
        Hanalysis/functions/SpectralTonality.cpp
        Hanalysis/functions/Tristimulus.cpp
        Hanalysis/functions/f0_YIN.cpp
        ${SDK_SRC})
SET_TARGET_PROPERTIES (AudioAnalysisDescriptors PROPERTIES OUTPUT_NAME "Audio Analysis Descriptors")
TARGET_INCLUDE_DIRECTORIES (AudioAnalysisDescriptors PRIVATE ${SDK_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/Hanalysis")

IF (WIN32)
    SET_TARGET_PROPERTIES (AudioAnalysisDescriptors PROPERTIES SUFFIX ".usr-win64" PREFIX "")
ELSEIF (APPLE)
    # Xcode links the dylib with a fixed install name, in /usr/local/lib
    TARGET_LINK_OPTIONS (AudioAnalysisDescriptors PRIVATE "-Wl,-install_name,/usr/local/lib/Audio Analysis Descriptors.usr-macos64uni,-compatibility_version,1,-current_version,1")
    SET_TARGET_PROPERTIES (AudioAnalysisDescriptors PROPERTIES SUFFIX ".usr-macos64uni" PREFIX "")
ELSE ()
    SET_TARGET_PROPERTIES (AudioAnalysisDescriptors PROPERTIES SUFFIX ".usr-linint64" PREFIX "")
ENDIF ()

# copy the built module next to the Usine application
ADD_CUSTOM_COMMAND (TARGET AudioAnalysisDescriptors POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTDIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:AudioAnalysisDescriptors>" "${OUTDIR}"
    COMMENT "Copying the module to bin/AudioAnalysis")
