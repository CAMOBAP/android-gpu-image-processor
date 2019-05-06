# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

cmake_minimum_required(VERSION 3.4.1)

find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    message(STATUS "Using ccache: ${CCACHE_PROGRAM}")
    set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
    set(CMAKE_C_COMPILER_LAUNCHER ${CCACHE_PROGRAM})
endif()

include(thirdparty/jni_hpp.cmake)

add_subdirectory(thirdparty/SOIL)

add_library(ndk-shader-mixer SHARED ImageMixer.cpp oglshaderutils.cpp)

set_target_properties(ndk-shader-mixer PROPERTIES CXX_STANDARD 14 CMAKE_CXX_STANDARD_REQUIRED ON)

target_include_directories(ndk-shader-mixer PRIVATE thirdparty/SOIL/src)

#find_library(liblog log)
#find_library(libglesv3 GLESv3)

target_link_libraries(ndk-shader-mixer jni-hpp SOIL log EGL GLESv3)