cmake_minimum_required(VERSION 3.25)
project(Test LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(HEADERS
    test/cv/recognition/digit.h
    test/cv/recognition/sudoku.h
    test/sudoku/solving.h
    test/utils.h
    )
set(SOURCES
    main.cpp
    test/cv/recognition/digit.cpp
    test/cv/recognition/sudoku.cpp
    test/sudoku/solving.cpp
    test/utils.cpp
    )

add_definitions(-DTEST_DATA_DIR=${PROJECT_SOURCE_DIR}/../TestData)
add_definitions(-DTEST_OUTPUT_DIR=${PROJECT_SOURCE_DIR}/TestOutput)

if(UNIX AND NOT APPLE AND NOT ANDROID)
    set(ENABLE_TESSERACT ON CACHE BOOL "")
endif()

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../CommonSource ${CMAKE_CURRENT_BINARY_DIR}/CommonSource)

add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES})
target_link_libraries(${PROJECT_NAME}
    PUBLIC
    CommonSource
    )
