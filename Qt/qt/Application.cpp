#include "qt/Application.h"
#include "qt/FileSystem.h"
#include "qt/ImageProvider.h"
#include "qt/Style.h"
#include <QFontDatabase>
#include <QQmlContext>

qt::Application::Application(int argc, char *argv[])
    : m_qApplication(argc, argv), m_classifier(cv::recognition::digit::Classifier::KNN, m_assets.trainDataPath())
{
    // FileDialog complains without this
    m_qApplication.setOrganizationName("");
    m_qApplication.setOrganizationDomain("sudokuscanandsolve.widor.site");

    QFontDatabase::addApplicationFont(":/fonts/MaterialIconsOutlined-Regular.otf");

    sudoku::loadState(FileSystem::appDataDir() / "state.yaml", m_sudoku.app(), [this](bool outputProcessingImg) {
        m_processingFilter.outputProcessingImg(outputProcessingImg);
    });

    QObject::connect(&m_processingFilter, &qt::ProcessingFilter::acceptScanResult, [this]() {
        m_sudoku.setSheet(cv::recognition::sudoku::votedSheet(m_processingFilter.resultBuffer(), m_classifier));
    });

    qmlRegisterSingletonType<qt::FileSystem>(
      "stephanwidor.SudokuScanAndSolve", 1, 0, "FileSystem",
      [](QQmlEngine *, QJSEngine *) -> QObject * { return new qt::FileSystem(); });
    qmlRegisterSingletonType<qt::Style>("stephanwidor.SudokuScanAndSolve", 1, 0, "Style",
                                        [](QQmlEngine *, QJSEngine *) -> QObject * { return new qt::Style(); });
    qmlRegisterUncreatableType<qt::ProcessingFilter>("stephanwidor.SudokuScanAndSolve", 1, 0, "ProcessingFilter",
                                                     "You don't want to instantiate this from QML!");
    qmlRegisterUncreatableType<qt::SudokuInterface>("stephanwidor.SudokuScanAndSolve", 1, 0, "Puzzle",
                                                    "You don't want to instantiate this from QML!");

    m_qmlEngine.rootContext()->setContextProperty("processingFilter", &m_processingFilter);
    m_qmlEngine.rootContext()->setContextProperty("puzzle", &m_sudoku);

    auto pResultImageProvider = new ResultImageProvider();
    QObject::connect(&m_processingFilter, &qt::ProcessingFilter::processingFinished, [this, pResultImageProvider]() {
        const auto &lastResult = m_processingFilter.resultBuffer().back();
        pResultImageProvider->setImage(lastResult.img);
        emit m_processingFilter.resultReady(lastResult.success);
    });
    m_qmlEngine.addImageProvider("scanResult", pResultImageProvider);

    m_qmlEngine.load(QUrl(QLatin1String("qrc:/qml/main.qml")));
}

qt::Application::~Application()
{
    sudoku::saveState(FileSystem::appDataDir() / "state.yaml", m_sudoku.app(),
                      m_processingFilter.outputProcessingImg());
}
