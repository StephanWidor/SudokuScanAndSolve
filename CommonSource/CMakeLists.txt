cmake_minimum_required(VERSION 3.5)
project(CommonSource LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(OPENCV_VERSION 4.8.0 CACHE STRING "")
option(ENABLE_TESSERACT "" OFF)
set(DEFAULT_TRAINDATADIR ${CMAKE_CURRENT_SOURCE_DIR}/../assets/TrainData)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV ${CMAKE_CURRENT_BINARY_DIR}/OpenCV)

if(ANDROID)
    add_compile_options(-fopenmp)
    add_link_options(-fopenmp -static-openmp)
    set(ANDROID_MIN_SDK_VERSION 26)
endif()

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
   OpenCV
   )

if(ENABLE_TESSERACT)
    message("Tesseract enabled")
    add_definitions(-DENABLE_TESSERACT)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
        tesseract
        )
endif()
