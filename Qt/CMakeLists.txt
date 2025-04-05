cmake_minimum_required(VERSION 3.25)
project(SudokuScanAndSolve LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(ANDROID)
    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    set(ANDROID_PERMISSIONS "android.permission.READ_EXTERNAL_STORAGE android.permission.CAMERA" CACHE INTERNAL "")
    set(ANDROID_VERSION_CODE 8)
    set(ANDROID_VERSION_NAME 2.0.3)
    find_package(Qt5 5.15 REQUIRED COMPONENTS AndroidExtras)
endif()

find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui Multimedia Quick QuickControls2)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../CommonSource ${CMAKE_CURRENT_BINARY_DIR}/CommonSource)

# add trainData to project, so we can use it in qrc
execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/trainData
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../assets/TrainData ${CMAKE_CURRENT_SOURCE_DIR}/resources/trainData
    )

set(HEADERS
    qt/Application.h
    qt/Assets.h
    qt/Style.h
    qt/FileSystem.h
    qt/imageConvert.h
    qt/ImageProvider.h
    qt/ProcessingFilter.h
    qt/SudokuInterface.h
    )
set(SOURCES
    main.cpp
    qt/Application.cpp
    qt/Assets.cpp
    qt/FileSystem.cpp
    qt/imageConvert.cpp
    qt/ImageProvider.cpp
    qt/ProcessingFilter.cpp
    qt/SudokuInterface.cpp
    )
set(RESOURCES
    resources/fonts.qrc
    resources/qml.qrc
    resources/trainData.qrc
    )

if(ANDROID)
    add_compile_options(-fopenmp)
    add_link_options(-fopenmp -static-openmp)
    add_library(${PROJECT_NAME} SHARED ${SOURCES} ${HEADERS} ${RESOURCES})
    target_link_libraries(${PROJECT_NAME}
        PUBLIC
        Qt5::AndroidExtras
        )
else()
    add_executable(${PROJECT_NAME} ${HEADERS} ${SOURCES} ${RESOURCES})
endif()


target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CommonSource
    Qt5::Core
    Qt5::Gui
    Qt5::Multimedia
    Qt5::Quick
    Qt5::QuickControls2
    )
