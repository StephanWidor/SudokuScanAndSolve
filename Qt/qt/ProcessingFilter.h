#pragma once
#include <QVideoFrame>
#include <QVideoSink>
#include <atomic>
#include <base/spin_mutex.h>
#include <cv/recognition/sudoku.h>

namespace qt {

class ProcessingFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool showProcessingImg READ showProcessingImg WRITE setShowProcessingImg NOTIFY showProcessingImgChanged)

public:
    ProcessingFilter(QObject *parent = nullptr);

    ~ProcessingFilter();

    bool showProcessingImg() const { return m_showProcessingImg; }

    void setShowProcessingImg(bool showProcessingImg)
    {
        if (m_showProcessingImg != showProcessingImg)
        {
            m_showProcessingImg = showProcessingImg;
            emit showProcessingImgChanged();
        }
    }

    const std::vector<cv::recognition::sudoku::Result> &resultBuffer() const { return m_resultBuffer; }

    static constexpr size_t resultBufferSize = 10;

public slots:
    QVideoSink *videoSink() { return &m_inputSocket.sink; }

    /// just setting pointer, no ownership!
    void setOutputSink(QVideoSink *pSink) { m_outputSocket.setSink(pSink); }

    bool startCameraThread();

    void stopCameraThread();

    void processSingleImg(const QUrl &url);

signals:
    void processingFinished() const;
    void resultReady(bool success) const;
    void acceptScanResult() const;
    void showProcessingImgChanged() const;

private:
    void onInputFrame(const QVideoFrame &);

    bool m_showProcessingImg{true};
    std::vector<cv::recognition::sudoku::Result> m_resultBuffer;

    enum class State
    {
        Idle,
        ThreadProcessing,
        SingleProcessing,
        ThreadShouldStop
    };

    std::atomic<State> m_state{State::Idle};

    struct InputSocket
    {
        void push(const QVideoFrame &);
        cv::Mat get();

        QVideoSink sink;
        QImage img;
        bool newImgAvailable = false;
        spin_mutex mutex;
    };
    InputSocket m_inputSocket;

    struct OutputSocket
    {
        void setSink(QVideoSink *pNewSink);
        void push(const cv::Mat &img);

        spin_mutex mutex;
        QVideoSink *pSink = nullptr;
    };
    OutputSocket m_outputSocket;
};

}    // namespace qt
