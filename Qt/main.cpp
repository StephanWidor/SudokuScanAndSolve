#include "qt/FileSystem.h"
#include "qt/ImageProvider.h"
#include "qt/ProcessingFilter.h"
#include "qt/StateRAII.h"
#include "qt/Style.h"
#include "qt/SudokuInterface.h"
#include <QDebug>
#include <QFontDatabase>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <cv/recognition/digit.h>

#if QT_CONFIG(permissions)
#include <QPermission>
#endif

int main(int argc, char *argv[])
{
    QGuiApplication qApplication(argc, argv);
    qt::SudokuInterface sudoku;
    qt::ProcessingFilter processingFilter;
    QQmlApplicationEngine qmlEngine;

    cv::recognition::digit::Classifier classifier(cv::recognition::digit::Classifier::KNN, qt::provideTrainDataDir());
    qt::StateRAII stateRAII(sudoku.app(), processingFilter);

    qmlRegisterSingletonType<qt::Style>("stephanwidor.SudokuScanAndSolve", 1, 1, "Style",
                                        [](QQmlEngine *, QJSEngine *) -> QObject * { return new qt::Style(); });
    qmlRegisterUncreatableType<qt::ProcessingFilter>("stephanwidor.SudokuScanAndSolve", 1, 1, "ProcessingFilter",
                                                     "You don't want to instantiate this from QML!");
    qmlRegisterUncreatableType<qt::SudokuInterface>("stephanwidor.SudokuScanAndSolve", 1, 1, "Puzzle",
                                                    "You don't want to instantiate this from QML!");

    QFontDatabase::addApplicationFont(":/fonts/MaterialIconsOutlined-Regular.otf");

    qmlEngine.rootContext()->setContextProperty("processingFilter", &processingFilter);
    qmlEngine.rootContext()->setContextProperty("puzzle", &sudoku);

    auto pResultImageProvider = new qt::ImageProvider();    // m_qmlEngine will take over ownership
    qmlEngine.addImageProvider("scanResult", pResultImageProvider);

    QObject::connect(&processingFilter, &qt::ProcessingFilter::acceptScanResult, [&]() {
        sudoku.setSheet(cv::recognition::sudoku::votedSheet(processingFilter.resultBuffer(), classifier));
    });

    QObject::connect(&qApplication, &QGuiApplication::applicationStateChanged, [&](Qt::ApplicationState state) {
        if (state == Qt::ApplicationSuspended)
            stateRAII.saveState();
    });

    QObject::connect(
      &processingFilter, &qt::ProcessingFilter::processingFinished, &qApplication,
      [&]() {
          const auto &lastResult = processingFilter.resultBuffer().back();
          pResultImageProvider->setImage(lastResult.img);
          emit processingFilter.resultReady(
            lastResult.success);    // swdebug: shouldn't that be a signal from ImageProvider?
      },
      Qt::QueuedConnection);

#if QT_CONFIG(permissions)
    QCameraPermission cameraPermission;
    qApplication.requestPermission(cameraPermission, [&](const QPermission &permission) {
        if (permission.status() != Qt::PermissionStatus::Granted)
            qWarning() << "Didn't get Camera Permission";
        qmlEngine.load(":/qml/main.qml");
    });
#else
    m_qmlEngine.load(":/qml/main.qml");
#endif

    return qApplication.exec();
}
