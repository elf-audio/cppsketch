cmake_minimum_required(VERSION 3.18)

# Project's name
project(cppsketch)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add source files
add_library(cppsketch_lib
    Dylib.cpp
    FileWatcher.cpp
    liveCodeUtils.cpp
)

# Specify include directories
target_include_directories(cppsketch_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})