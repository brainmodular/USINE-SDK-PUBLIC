# DatabaseCreation: analyzes a sound folder and stores its descriptors in a SQL database
#
# Builds the DatabaseCreation module for the three platforms of Usine:
#   Windows  64-bit          Database Creation.usr-win64
#   macOS    universal       Database Creation.usr-macos64uni   (x86_64 10.13 + arm64)
#   Linux    64-bit          Database Creation.usr-linint64
# The binaries land in <repo>/bin/DatabaseModules at the end of the build, like the Xcode and Visual Studio projects.
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

PROJECT (DatabaseCreation CXX C)

SET (CMAKE_CXX_STANDARD 23)
SET (CMAKE_CXX_STANDARD_REQUIRED ON)
SET (CMAKE_C_STANDARD 11)

# Linux: only the functions marked USINE_MODULE_EXPORT are exported (examples/CMakeLists.txt asks for it on
# macOS too; here macOS is left as it is).
# Usine loads many modules in one process: with every symbol exported, two modules can end up sharing a
# variable or a function that each of them defines (the constants of ModuleDefinition.h, a library compiled in
# such as sqlite, which Database Creation and Database Player both contain).
IF (CMAKE_SYSTEM_NAME STREQUAL "Linux")
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

SET (OUTDIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../bin/DatabaseModules")
SET (SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../sdk")
FILE (GLOB SDK_SRC "${SDK_DIR}/UserModule.cpp" "${SDK_DIR}/UserUtils.cpp")

# must be set before ADD_LIBRARY: the output directory of a target is read when it is created
SET (CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTDIR}")
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTDIR}")

ADD_LIBRARY (DatabaseCreation SHARED
        DatabaseCreation.cpp
        ../libs/utils/functions/Loudness.cpp
        ../libs/utils/functions/MFCC.cpp
        ../libs/utils/functions/RMS.cpp
        ../libs/utils/functions/SpectralCentroid.cpp
        ../libs/utils/functions/SpectralFlatness.cpp
        ../libs/utils/functions/SpectralSpread.cpp
        ../libs/utils/functions/f0_YIN.cpp
        ../libs/utils/tsne/sptree.cpp
        ../libs/utils/tsne/tsne.cpp
        ../libs/sqlite3lib/sqlite3.c
        ${SDK_SRC})
SET_TARGET_PROPERTIES (DatabaseCreation PROPERTIES OUTPUT_NAME "Database Creation")
TARGET_INCLUDE_DIRECTORIES (DatabaseCreation PRIVATE ${SDK_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/../libs" "${CMAKE_CURRENT_SOURCE_DIR}/../libs/utils" "${CMAKE_CURRENT_SOURCE_DIR}/../libs/utils/functions" "${CMAKE_CURRENT_SOURCE_DIR}/../libs/utils/tsne" "${CMAKE_CURRENT_SOURCE_DIR}/../libs/sqlite3lib")

IF (WIN32)
    SET_TARGET_PROPERTIES (DatabaseCreation PROPERTIES SUFFIX ".usr-win64" PREFIX "")
ELSEIF (APPLE)
    # Xcode links the dylib with a fixed install name, in /usr/local/lib
    TARGET_LINK_OPTIONS (DatabaseCreation PRIVATE "-Wl,-install_name,/usr/local/lib/Database Creation.usr-macos64uni,-compatibility_version,1,-current_version,1")
    SET_TARGET_PROPERTIES (DatabaseCreation PROPERTIES SUFFIX ".usr-macos64uni" PREFIX "")
ELSE ()
    SET_TARGET_PROPERTIES (DatabaseCreation PROPERTIES SUFFIX ".usr-linint64" PREFIX "")
ENDIF ()

# copy the built module next to the Usine application
ADD_CUSTOM_COMMAND (TARGET DatabaseCreation POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUTDIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:DatabaseCreation>" "${OUTDIR}"
    COMMENT "Copying the module to bin/DatabaseModules")
