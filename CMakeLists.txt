cmake_minimum_required(VERSION 3.25)
project(SudokuScanAndSolve-main LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Enable building Tests" ON)

find_package(Qt6 QUIET)
include(CMakeDependentOption)
cmake_dependent_option(BUILD_QT_APP "Build Qt App" ON "DEFINED Qt6_DIR" OFF)

if(WIN32)
    set(OCV_FORCE_RELEASE_BUILD "" CACHE STRING OFF)    # otherwise we get problems with CRT
endif()
add_subdirectory(OpenCV)
add_subdirectory(CommonSource)

if(BUILD_QT_APP)
    add_subdirectory(Qt)
endif()
if(BUILD_TESTS)
    add_subdirectory(Test)
endif()
