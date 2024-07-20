# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("recorder")



set(OBOE_DIR oboe)
add_subdirectory(${OBOE_DIR} ./oboe)
# Include the Oboe headers and shared sample code
include_directories(${OBOE_DIR}/include/oboe ${OBOE_DIR}/samples/shared ${OBOE_DIR}/src/flowgraph)

add_subdirectory(libsndfile)
include_directories(libsndfile/include)

option(WITH_OGG "ogg support (default: test for libogg)" OFF)
set(INSTALL_MANPAGES OFF)
option(INSTALL_MANPAGES "Install MAN pages" OFF)
add_subdirectory(flac)
include_directories(flac/include)

#aac




# Debug utilities
set(DEBUG_UTILS_PATH "${OBOE_DIR}/samples/debug-utils")
set(DEBUG_UTILS_SOURCES ${DEBUG_UTILS_PATH}/trace.cpp)
include_directories(${DEBUG_UTILS_PATH})

# self

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.
#
# In this top level CMakeLists.txt, ${CMAKE_PROJECT_NAME} is used to define
# the target library name; in the sub-module's CMakeLists.txt, ${PROJECT_NAME}
# is preferred for the same purpose.
#
# In order to load a library into your app from Java/Kotlin, you must call
# System.loadLibrary() and pass the name of the library defined here;
# for GameActivity/NativeActivity derived applications, the same library name must be
# used in the AndroidManifest.xml file.
add_library(${CMAKE_PROJECT_NAME} SHARED
        recorder/RecorderOboeEngine.cpp
        recorder/AudioFileEncoder.cpp
        recorder/FormatConverterBox.cpp
        recorder/codec/WavCodec.cpp
        recorder/codec/FlacEncoder.cpp
        recorder/codec/AACEncoder.cpp
        recorder.cpp)

target_include_directories(recorder
        PRIVATE recorder recorder/codec)

# Specifies libraries CMake should link to your target library. You
# can link libraries from various origins, such as libraries defined in this
# build script, prebuilt third-party libraries, or Android system libraries.
target_link_libraries(${CMAKE_PROJECT_NAME}
        # List libraries link to the target library
        oboe
        sndfile
        fdk-aac
        FLAC++
        android
        log)