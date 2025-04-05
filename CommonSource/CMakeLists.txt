cmake_minimum_required(VERSION 3.25)
project(CommonSource LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(ENABLE_TESSERACT "" OFF)
set(DEFAULT_TRAINDATADIR ${CMAKE_CURRENT_SOURCE_DIR}/../assets/TrainData)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../OpenCV ${CMAKE_CURRENT_BINARY_DIR}/OpenCV)

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
   BuildOpenCV
   )

if(ANDROID)
    # find_package doesn't seem to work on android
    add_compile_options(-fopenmp)
    add_link_options(-fopenmp -static-openmp)
else()
    find_package(OpenMP COMPONENTS CXX)
    if(OpenMP_FOUND)
        target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)
    endif()
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # gcc uses tbb for parallel algorithms
    find_package(TBB)
endif()

if(TBB_FOUND)
    message("linking to tbb for parallel execution in gcc build")
    target_link_libraries(${PROJECT_NAME}
       PUBLIC
       TBB::tbb
       )
endif()

if(ENABLE_TESSERACT)
    message("Tesseract enabled")
    add_definitions(-DENABLE_TESSERACT)
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
        tesseract
        )
endif()
