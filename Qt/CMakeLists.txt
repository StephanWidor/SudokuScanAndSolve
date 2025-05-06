cmake_minimum_required(VERSION 3.25)
project(SudokuScanAndSolve LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Multimedia Quick)

qt_standard_project_setup(REQUIRES 6.9)

# add trainData to project, so we can use it in qrc
execute_process(
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_CURRENT_SOURCE_DIR}/trainData
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/../assets/TrainData ${CMAKE_CURRENT_SOURCE_DIR}/resources/trainData
    )

set(SOURCES
    qt/Style.h
    qt/FileSystem.h
    qt/ImageProvider.h
    qt/ProcessingFilter.h
    qt/StateRAII.h
    qt/SudokuInterface.h
    main.cpp
    qt/FileSystem.cpp
    qt/ImageProvider.cpp
    qt/ProcessingFilter.cpp
    qt/SudokuInterface.cpp
    )
qt_add_resources(SOURCES
    resources/fonts.qrc
    resources/qml.qrc
    resources/trainData.qrc
    )

qt_add_executable(${PROJECT_NAME} ${SOURCES})

if(ANDROID)
    set(ANDROID_VERSION_CODE 9)
    set(ANDROID_VERSION_NAME 2.0.4)
    set_target_properties(${PROJECT_NAME}
        PROPERTIES
        QT_ANDROID_PACKAGE_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/android
        QT_ANDROID_PACKAGE_NAME "com.stephanwidor.sudokuscanandsolve"
    )
endif()


target_link_libraries(${PROJECT_NAME}
    PRIVATE
    CommonSource
    Qt6::Core
#    Qt6::Gui
    Qt6::Multimedia
    Qt6::Quick
#    Qt6::QuickControls2
    )
