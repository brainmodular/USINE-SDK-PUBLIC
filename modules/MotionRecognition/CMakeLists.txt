cmake_minimum_required (VERSION 3.28)
project (MotionRecognition)

set (CMAKE_CXX_STANDARD 23)
set (CMAKE_OSX_ARCHITECTURES "x86_64;arm64")
SET (OUTDIR "${CMAKE_SOURCE_DIR}/../../bin/Machine Learning")
FILE (GLOB SDK_SRC ../../sdk/*.cpp)

FUNCTION (ADD_COPY_PHASE LIBNAME)
    IF (WIN32)
        STRING (REPLACE "/" "\\" WIN_OUTDIR ${OUTDIR})
        ADD_CUSTOM_COMMAND (TARGET ${LIBNAME} POST_BUILD COMMAND copy *.usr-win64 "${WIN_OUTDIR}" COMMENT "Copying files to bin...")
    ELSEIF (APPLE)
        ADD_CUSTOM_COMMAND (TARGET ${LIBNAME} POST_BUILD COMMAND cp -v {*.usr-osx64,*.usr-osxarm64,*.usr-macos64uni} "${OUTDIR}" || true COMMENT "Copying files to bin...")
    ELSE ()
        ADD_CUSTOM_COMMAND (TARGET ${LIBNAME} POST_BUILD COMMAND cp --verbose *.so ../../bin/uVector "${OUTDIR}" COMMENT "Copying files to bin...")
    ENDIF ()
ENDFUNCTION ()

add_library (MotionRecognition SHARED
        MotionRecognizer.cpp
        process/Handler.cpp
        ${SDK_SRC})
target_compile_options(MotionRecognition PRIVATE ${CXX_FLAGS})
target_include_directories (MotionRecognition PRIVATE ../../sdk process)
if (WIN32)
    set_target_properties(MotionRecognition PROPERTIES SUFFIX ".usr-win64" PREFIX "" LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
elseif (APPLE)
    target_link_options (MotionRecognition PRIVATE -U _objc_loadClassref)
    set_target_properties(MotionRecognition PROPERTIES SUFFIX ".usr-macos64uni" PREFIX "" LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
else ()
    set_target_properties(MotionRecognition PROPERTIES SUFFIX ".usr-linint64" PREFIX "" LIBRARY_OUTPUT_DIRECTORY ${OUTDIR})
endif ()
ADD_COPY_PHASE (MotionRecognition)