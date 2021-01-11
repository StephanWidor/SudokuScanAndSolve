#pragma once
#include "cv/recognition/digit.h"
#include "qt/Assets.h"
#include "qt/ProcessingFilter.h"
#include "qt/SudokuInterface.h"
#include <QGuiApplication>
#include <QQmlApplicationEngine>

namespace qt {

class Application
{
public:
    explicit Application(int argc, char *argv[]);

    ~Application();

    int run() { return m_qApplication.exec(); }

private:
    void saveState() const;
    void loadState();

    struct AttributeSettings
    {
        AttributeSettings() { QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling); }
    };
    AttributeSettings m_attributeSettings;
    QGuiApplication m_qApplication;
    Assets m_assets;
    cv::recognition::digit::Classifier m_classifier;
    SudokuInterface m_sudoku;
    ProcessingFilter m_processingFilter;
    QQmlApplicationEngine m_qmlEngine;
};

}    // namespace qt
