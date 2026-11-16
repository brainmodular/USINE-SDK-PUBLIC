cmake_minimum_required (VERSION 3.27)

# Usine SDK examples: one Usine module (a shared library) per example folder, written to ../bin/SDK Examples:
#
#   Windows x64    <Example>.usr-win64        clang-cl 22 (MSVC ABI), static C runtime (/MT)
#   macOS          <Example>.usr-macos64uni   Apple clang of Xcode, universal: x86_64 (macOS 10.13+) and arm64
#   Linux x86_64   <Example>.usr-linint64     clang 22
#
# clang 22 is recommended (Apple clang on macOS). When no compiler is given, this file picks it if it is
# installed: the ClangCL toolset with a Visual Studio generator, clang-cl with Ninja on Windows, clang++-22 on
# Linux. Another compiler builds the modules too, with a warning. Build in a folder outside the source tree,
# for example:
#
#   Windows, Visual Studio 2026 with its components "C++ Clang Compiler for Windows" and
#   "MSBuild support for LLVM (clang-cl) toolset":
#       cmake -S examples -B C:/Temp/Examples -G "Visual Studio 18 2026" -A x64
#       cmake --build C:/Temp/Examples --config Release
#   Windows, Ninja, in an "x64 Native Tools Command Prompt for VS":
#       cmake -S examples -B C:/Temp/Examples -G Ninja -DCMAKE_BUILD_TYPE=Release
#       cmake --build C:/Temp/Examples
#   macOS:
#       cmake -S examples -B /tmp/Examples -G Xcode
#       cmake --build /tmp/Examples --config Release
#   Linux, clang-22 of apt.llvm.org:
#       cmake -S examples -B /tmp/Examples -G Ninja -DCMAKE_BUILD_TYPE=Release
#       cmake --build /tmp/Examples
#
# One module only: cmake --build <build folder> --target TemplateModule (add --config Release with Visual Studio
# or Xcode). Your own module: copy an example folder, rename it and add its name to USINE_EXAMPLES below.

# Recommended version of clang (clang-cl on Windows) on Windows and Linux.
set (USINE_CLANG_MAJOR 22)

# ---- Before project(): the compiler and the macOS targets, read when CMake sets up the compiler ----
if (CMAKE_HOST_WIN32)
    if (CMAKE_GENERATOR MATCHES "^Visual Studio ([0-9]+)")
        # the ClangCL toolset if this version of Visual Studio has its clang-cl components, else the MSVC compiler
        set (USINE_VS_MAJOR ${CMAKE_MATCH_1})
        math (EXPR USINE_VS_NEXT "${USINE_VS_MAJOR} + 1")
        set (USINE_PROGRAM_FILES_X86 "ProgramFiles(x86)")
        set (USINE_VSWHERE "$ENV{${USINE_PROGRAM_FILES_X86}}/Microsoft Visual Studio/Installer/vswhere.exe")
        if (NOT CMAKE_GENERATOR_TOOLSET AND EXISTS "${USINE_VSWHERE}")
            execute_process (COMMAND "${USINE_VSWHERE}" -products * -version "[${USINE_VS_MAJOR}.0,${USINE_VS_NEXT}.0)"
                                     -requires Microsoft.VisualStudio.Component.VC.Llvm.Clang
                                               Microsoft.VisualStudio.Component.VC.Llvm.ClangToolset
                                     -property installationPath
                             OUTPUT_VARIABLE USINE_VS_WITH_CLANG OUTPUT_STRIP_TRAILING_WHITESPACE ERROR_QUIET)
            if (USINE_VS_WITH_CLANG)
                set (CMAKE_GENERATOR_TOOLSET "ClangCL")
            endif ()
        endif ()
    elseif (NOT DEFINED CMAKE_CXX_COMPILER AND NOT DEFINED ENV{CXX})
        # clang-cl of Visual Studio (VCINSTALLDIR is set by the Native Tools Command Prompt), else of LLVM
        file (TO_CMAKE_PATH "$ENV{VCINSTALLDIR}" USINE_VC_DIR)
        file (TO_CMAKE_PATH "$ENV{ProgramFiles}" USINE_PROGRAM_FILES)
        find_program (USINE_CLANG_CL clang-cl
                      HINTS "${USINE_VC_DIR}/Tools/Llvm/x64/bin" "${USINE_PROGRAM_FILES}/LLVM/bin" NO_CACHE)
        if (USINE_CLANG_CL)
            set (CMAKE_CXX_COMPILER "${USINE_CLANG_CL}")
        endif ()
    endif ()
elseif (CMAKE_HOST_APPLE)
    set (CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE STRING "Architectures of the modules")
    # clang raises the arm64 slice to macOS 11.0, the first macOS of Apple silicon
    set (CMAKE_OSX_DEPLOYMENT_TARGET "10.13" CACHE STRING "Minimum macOS version of the x86_64 slice")
elseif (NOT DEFINED CMAKE_CXX_COMPILER AND NOT DEFINED ENV{CXX})
    find_program (USINE_CLANGXX clang++-${USINE_CLANG_MAJOR} NO_CACHE)
    if (USINE_CLANGXX)
        set (CMAKE_CXX_COMPILER "${USINE_CLANGXX}")
    endif ()
endif ()

project (UsineExamples LANGUAGES CXX)

get_property (USINE_MULTI_CONFIG GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if (NOT USINE_MULTI_CONFIG AND NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type: Release or Debug" FORCE)
endif ()

# ---- The compiler: clang 22 recommended, Apple clang on macOS; another one gives a warning ----
# CMake keeps the compiler of an existing build folder: after changing it, configure a new build folder.
if (APPLE)
    if (NOT CMAKE_CXX_COMPILER_ID MATCHES "Clang")
        message (WARNING "The Apple clang of Xcode is recommended for the Usine modules, not "
                         "${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER})")
    endif ()
elseif (NOT CMAKE_CXX_COMPILER_ID STREQUAL "Clang" OR NOT CMAKE_CXX_COMPILER_VERSION MATCHES "^${USINE_CLANG_MAJOR}\\.")
    if (WIN32)
        string (CONCAT USINE_CLANG_HINT "Visual Studio 2026 with its components \"C++ Clang Compiler for Windows\" and "
                                        "\"MSBuild support for LLVM (clang-cl) toolset\" (toolset ClangCL), or "
                                        "-DCMAKE_CXX_COMPILER=<clang-cl ${USINE_CLANG_MAJOR}> with Ninja, in a new build folder")
    else ()
        string (CONCAT USINE_CLANG_HINT "clang-${USINE_CLANG_MAJOR} of apt.llvm.org, or "
                                        "-DCMAKE_CXX_COMPILER=<clang++ ${USINE_CLANG_MAJOR}>, in a new build folder")
    endif ()
    message (WARNING "clang ${USINE_CLANG_MAJOR} is recommended for the Usine modules, not ${CMAKE_CXX_COMPILER_ID} "
                     "${CMAKE_CXX_COMPILER_VERSION} (${CMAKE_CXX_COMPILER}): ${USINE_CLANG_HINT}")
endif ()
if (WIN32 AND NOT CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    message (FATAL_ERROR "On Windows the Usine modules are built with clang-cl (MSVC ABI), not with the "
                         "${CMAKE_CXX_COMPILER_FRONTEND_VARIANT} front end of clang (MinGW)")
endif ()
if (NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
    message (FATAL_ERROR "Usine is 64-bit: build the modules for x64 (Visual Studio generator: -A x64)")
endif ()

# ---- Settings of every module ----
set (CMAKE_CXX_STANDARD 20)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_CXX_EXTENSIONS OFF)
# Windows: the C runtime linked into the module, nothing to install beside it
set (CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
# macOS, Linux: only the functions marked USINE_MODULE_EXPORT are exported, so that two modules loaded by
# Usine never share a C++ symbol
set (CMAKE_CXX_VISIBILITY_PRESET hidden)
set (CMAKE_VISIBILITY_INLINES_HIDDEN ON)

get_filename_component (USINE_SDK_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../sdk" ABSOLUTE)
get_filename_component (USINE_EXAMPLES_BIN "${CMAKE_CURRENT_SOURCE_DIR}/../bin/SDK Examples" ABSOLUTE)

if (WIN32)
    set (USINE_MODULE_SUFFIX ".usr-win64")
elseif (APPLE)
    set (USINE_MODULE_SUFFIX ".usr-macos64uni")
else ()
    set (USINE_MODULE_SUFFIX ".usr-linint64")
endif ()

# usine_add_module (<name>): the Usine module <name>, from the sources of the folder <name> and of the SDK
function (usine_add_module NAME)
    file (GLOB USINE_MODULE_SOURCES CONFIGURE_DEPENDS "${NAME}/*.cpp" "${NAME}/*.h" "${NAME}/*.hpp")
    add_library (${NAME} SHARED ${USINE_MODULE_SOURCES} "${USINE_SDK_DIR}/UserModule.cpp" "${USINE_SDK_DIR}/UserUtils.cpp")
    target_include_directories (${NAME} PRIVATE "${USINE_SDK_DIR}")
    # $<0:> keeps Visual Studio and Xcode from adding a Release or Debug subfolder. The import library
    # (.lib, .exp) and the debug database (.pdb) of Windows stay in the build folder.
    set_target_properties (${NAME} PROPERTIES
        PREFIX ""
        SUFFIX "${USINE_MODULE_SUFFIX}"
        RUNTIME_OUTPUT_DIRECTORY "${USINE_EXAMPLES_BIN}$<0:>"
        LIBRARY_OUTPUT_DIRECTORY "${USINE_EXAMPLES_BIN}$<0:>"
        PDB_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pdb")
endfunction ()

set (USINE_EXAMPLES
    ArrayDelay
    AudioVolume
    BackgroundJob
    DataMultiply
    DataOscilloscope
    DataSampler
    DragAndDrop
    DrawBox
    DrawTrajectoryBox
    ErrorHandling
    GlobalArray
    Granulator
    MidiTranspose
    MultiThreading
    Reverb
    RingMod
    RingModMultithread
    SimplePad
    TemplateModule
    UsineChunks
    UsineObjects
    UsinePointers
    VideoDimmer
)
foreach (USINE_EXAMPLE IN LISTS USINE_EXAMPLES)
    usine_add_module (${USINE_EXAMPLE})
endforeach ()

message (STATUS "Usine SDK examples: ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, "
                "${CMAKE_SYSTEM_NAME}, modules written to ${USINE_EXAMPLES_BIN}")
