#pragma once
#include "cv/recognition/sudoku.h"
#include <QAbstractVideoFilter>
#include <QQuickImageProvider>
#include <atomic>

namespace qt {

class ProcessingFilter;

class ProcessingFilterRunnable : public QVideoFilterRunnable
{
public:
    ProcessingFilterRunnable(ProcessingFilter &filter): QVideoFilterRunnable(), m_filter(filter) {}

    QVideoFrame run(QVideoFrame *, const QVideoSurfaceFormat &, RunFlags) override;

private:
    ProcessingFilter &m_filter;
};

class ProcessingFilter : public QAbstractVideoFilter
{
    Q_OBJECT

    Q_PROPERTY(int cameraRotation READ cameraRotation WRITE setCameraRotation)
    Q_PROPERTY(
      bool outputProcessingImg READ outputProcessingImg WRITE outputProcessingImg NOTIFY outputProcessingImgChanged)

    friend ProcessingFilterRunnable;

public:
    ProcessingFilter(): QAbstractVideoFilter() {}

    QVideoFilterRunnable *createFilterRunnable() override;

    // swdebug: rename this (confusing, cf lastOutputImg)!
    bool outputProcessingImg() const { return m_outputProcessingImg; }
    void outputProcessingImg(bool doIt)
    {
        if (m_outputProcessingImg != doIt)
        {
            m_outputProcessingImg = doIt;
            emit outputProcessingImgChanged();
        }
    }

    const std::vector<cv::recognition::sudoku::Result> &resultBuffer() const { return m_resultBuffer; }

    static constexpr size_t resultBufferSize = 10;

public slots:

    void processImg(const QUrl &url);

signals:
    void processingFinished();
    void resultReady(bool success);
    void acceptScanResult();
    void outputProcessingImgChanged();

protected:
    void setCameraRotation(int rotation) { m_cameraRotation = rotation; }
    int cameraRotation() const { return m_cameraRotation; }

    bool m_outputProcessingImg{true};
    std::atomic<int> m_cameraRotation{0};
    std::vector<cv::recognition::sudoku::Result> m_resultBuffer;
};

}    // namespace qt
