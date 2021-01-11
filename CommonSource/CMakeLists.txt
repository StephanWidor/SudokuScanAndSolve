cmake_minimum_required(VERSION 3.5)
project(CommonSource LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(OPENCV_VERSION 4.5.1 CACHE STRING "")
option(ENABLE_TESSERACT "" OFF)
set(DEFAULT_TRAINDATADIR ${CMAKE_CURRENT_SOURCE_DIR}/../assets/TrainData)

set(OPENCV_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV")
set(OPENCV_PATH "${OPENCV_MODULE_PATH}/OpenCV-${OPENCV_VERSION}")
if(ANDROID)
    add_compile_options(-fopenmp)
    add_link_options(-fopenmp -static-openmp)
    set(ANDROID_MIN_SDK_VERSION 26)
    set(OPENCV_BUILD_PLATFORM android)
    set(OPENCV_PLATFORM_PATH "${OPENCV_PATH}/${OPENCV_BUILD_PLATFORM}")
    set(OpenCV_DIR "${OPENCV_PLATFORM_PATH}/sdk/native/jni")
elseif(APPLE)
    set(OPENCV_BUILD_PLATFORM macos)
    set(OPENCV_PLATFORM_PATH "${OPENCV_PATH}/${OPENCV_BUILD_PLATFORM}")
    set(OpenCV_DIR "${OPENCV_PLATFORM_PATH}/lib/cmake/opencv4")
elseif(UNIX)
    set(OPENCV_BUILD_PLATFORM linux)
    set(OPENCV_PLATFORM_PATH "${OPENCV_PATH}/${OPENCV_BUILD_PLATFORM}")
    set(OpenCV_DIR "${OPENCV_PLATFORM_PATH}/lib/cmake/opencv4")
elseif(WIN32)
    set(OPENCV_BUILD_PLATFORM win)
    set(OPENCV_PLATFORM_PATH "${OPENCV_PATH}/${OPENCV_BUILD_PLATFORM}")
    set(OpenCV_DIR "${OPENCV_PLATFORM_PATH}")
endif()
if(NOT EXISTS "${OPENCV_PLATFORM_PATH}")
    message("Need to build OpenCV for ${OPENCV_BUILD_PLATFORM}")
    if(CMAKE_HOST_WIN32)
        execute_process (
            COMMAND "C:/Program Files/Git/bin/bash.exe" ./build.sh ${OPENCV_BUILD_PLATFORM} ${OPENCV_VERSION}
            WORKING_DIRECTORY "${OPENCV_MODULE_PATH}"
        )
    else()
        execute_process (
            COMMAND bash -c "./build.sh ${OPENCV_BUILD_PLATFORM} ${OPENCV_VERSION}"
            WORKING_DIRECTORY ${OPENCV_MODULE_PATH}
        )
    endif()
endif()
set(OpenCV_STATIC ON)
find_package(OpenCV ${OPENCV_VERSION} REQUIRED)

set(HEADERS
    base/ExecutionHelp.h
    base/colors.h
    base/math.h
    base/utils.h
    cv/contourUtils.h
    cv/imageUtils.h
    cv/recognition/digit.h
    cv/recognition/sudoku.h
    cv/types.h
    cv/utils2d.h
    sudoku/App.h
    sudoku/creator.h
    sudoku/deduction.h
    sudoku/History.h
    sudoku/IndexContainer.h
    sudoku/Sheet.h
    sudoku/solver.h
    )
set(SOURCES
    cv/contourUtils.cpp
    cv/imageUtils.cpp
    cv/recognition/digit.cpp
    cv/recognition/sudoku.cpp
    cv/utils2d.cpp
    sudoku/App.cpp
    sudoku/creator.cpp
    sudoku/deduction.cpp
    sudoku/IndexContainer.cpp
    sudoku/Sheet.cpp
    sudoku/solver.cpp
    )

add_definitions(-DDEFAULT_TRAINDATADIR=${DEFAULT_TRAINDATADIR})
add_library(${PROJECT_NAME} STATIC ${HEADERS} ${SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME}
   PUBLIC
   ${OpenCV_LIBS}
   )

if(ENABLE_TESSERACT)
    message("Tesseract enabled")
    add_definitions(-DENABLE_TESSERACT)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
        tesseract
        )
endif()
