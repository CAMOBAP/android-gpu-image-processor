cmake_minimum_required(VERSION 3.4.1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(jni-hpp INTERFACE)

file(GLOB jni-hpp_HEADERS ${CMAKE_CURRENT_LIST_DIR}/jni.hpp/include/jni/*.hpp)
target_sources(jni-hpp INTERFACE ${jni-hpp_HEADERS})

target_include_directories(jni-hpp INTERFACE "${CMAKE_CURRENT_LIST_DIR}/jni.hpp/include/")