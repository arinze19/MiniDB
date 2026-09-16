cmake_minimum_required(VERSION 3.15)

project(MiniDB VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True) # do not silently fail back to older version of cpp


# file(GLOB_RECURSE SOURCES "src/*.cpp") # glob recurse through all cpp files in source

# main executable 
file(GLOB_RECURSE LIB_SOURCES "src/storage/*.cpp" "src/index/*.cpp" "src/db.cpp")

add_executable(minidb src/main.cpp ${LIB_SOURCES}) # compile into executable called minidb
target_include_directories(minidb PRIVATE src)
target_compile_options(minidb PRIVATE -Wall -Wextra -Wpedantic) # enable compiler warnings

# test executable 
file(GLOB TEST_SOURCES "tests/*.cpp")
add_executable(minidb_tests ${TEST_SOURCES} ${LIB_SOURCES})
target_include_directories(minidb_tests PRIVATE src)
target_compile_options(minidb_tests PRIVATE -Wall -Wextra)

# benchmark executable 
add_executable(minidb_bench benchmarks/benchmarks.cpp ${LIB_SOURCES})
target_include_directories(minidb_bench PRIVATE src)

target_compile_options(minidb_bench PRIVATE -O2)

# custom targets 
add_custom_target(
    run_tests
    COMMAND ./minidb_tests
    DEPENDS minidb_tests
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_custom_target(
    run_bench
    COMMAND ./minidb_bench
    DEPENDS minidb_bench
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
)
