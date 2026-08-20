cmake_minimum_required(VERSION 3.15)

project(MiniDB VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) # do not silently fail back to older version of cpp

file(GLOB_RECURSE SOURCES "src/*.cpp") # glob recurse through all cpp files in source

add_executable(minidb ${SOURCES}) # compile into executable called minidb

target_include_directories(minidb PRIVATE src) # what does this mean? - inspect PRIVATE tag

target_compile_options(minidb PRIVATE -Wall -Wextra -Wpedantic) # enable compiler warnings