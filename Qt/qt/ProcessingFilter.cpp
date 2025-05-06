#include "qt/ProcessingFilter.h"
#include "cv/recognition/sudoku.h"
#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <opencv2/highgui.hpp>
#include <thread>

#if QT_CONFIG(permissions)
#include <QPermission>
#endif

namespace qt {

void ProcessingFilter::InputSocket::push(const QVideoFrame &frame)
{
    if (!frame.isValid())
    {
        qWarning() << "frame is invalid";
        return;
    }

    std::lock_guard lock(mutex);

    if (QVideoFrame copyFrame(frame);    // shallow copy
        copyFrame.map(QVideoFrame::ReadOnly))    // (map is a const member function)
    {
        img = copyFrame.toImage().convertToFormat(QImage::Format_Grayscale8);
        copyFrame.unmap();
        newImgAvailable = true;
    }
    else
        qWarning() << "couldn't map video frame";
}

cv::Mat ProcessingFilter::InputSocket::get()
{
    std::lock_guard lock(mutex);
    if (newImgAvailable)
    {
        newImgAvailable = false;
        return cv::Mat(img.height(), img.width(), CV_8UC1, img.bits(), img.bytesPerLine()).clone();
    }
    return cv::Mat();
}

void ProcessingFilter::OutputSocket::push(const cv::Mat &img)
{
    if (mutex.try_lock())
    {
        if (pSink)
        {
            pSink->setVideoFrame(
              QVideoFrame(QImage(img.data, img.cols, img.rows, img.step, QImage::Format_Grayscale8).copy()));
        }
        mutex.unlock();
    }
}

void ProcessingFilter::OutputSocket::setSink(QVideoSink *pNewSink)
{
    if (pNewSink == pSink)
        return;
    std::lock_guard lock(mutex);
    pSink = pNewSink;
}

ProcessingFilter::ProcessingFilter(QObject *pParent): QObject(pParent)
{
    connect(&m_inputSocket.sink, &QVideoSink::videoFrameChanged, this, &ProcessingFilter::onInputFrame);
}

ProcessingFilter::~ProcessingFilter()
{
    disconnect(&m_inputSocket.sink, &QVideoSink::videoFrameChanged, this, &ProcessingFilter::onInputFrame);
    stopCameraThread();
    while (m_state.load(std::memory_order::acquire) != State::Idle)
        ;
}

void ProcessingFilter::onInputFrame(const QVideoFrame &frame)
{
    m_inputSocket.push(frame);
}

bool ProcessingFilter::startCameraThread()
{
#if QT_CONFIG(permissions)
    QCameraPermission cameraPermission;
    if (qApp->checkPermission(cameraPermission) != Qt::PermissionStatus::Granted)
    {
        qWarning() << "Don't have camera permission, will not start processing";
        return false;
    }
#endif

    State idleState = State::Idle;
    if (!m_state.compare_exchange_strong(idleState, State::ThreadProcessing, std::memory_order::acq_rel))
        return false;

    const auto runProcessingLoop = [&]() {
        m_resultBuffer.clear();
        while (m_state.load(std::memory_order::acquire) == State::ThreadProcessing)
        {
            auto img = m_inputSocket.get();
            if (img.empty() || m_resultBuffer.size() >= ProcessingFilter::resultBufferSize)
                continue;

            auto scanResult = cv::recognition::sudoku::process(img, cv::recognition::sudoku::Config::defaultVideo());
            m_outputSocket.push(m_showProcessingImg ? scanResult.processingImg : scanResult.img);
            if (scanResult)
            {
                m_resultBuffer.push_back(std::move(scanResult));
                if (m_resultBuffer.size() >= ProcessingFilter::resultBufferSize)
                {
                    emit processingFinished();
                    stopCameraThread();
                }
            }
        }
        m_state.store(State::Idle, std::memory_order::release);
    };

    std::thread processingThread(runProcessingLoop);
    processingThread.detach();

    return true;
}

void ProcessingFilter::stopCameraThread()
{
    State threadProcessingState = State::ThreadProcessing;
    m_state.compare_exchange_strong(threadProcessingState, State::ThreadShouldStop, std::memory_order::acq_rel);
}

void qt::ProcessingFilter::processSingleImg(const QUrl &url)
{
    State idleState = State::Idle;
    if (!m_state.compare_exchange_strong(idleState, State::SingleProcessing, std::memory_order::acq_rel))
        return;

    m_resultBuffer.clear();

    QImageReader reader;
    reader.setAutoTransform(true);    // consider EXIF orientation
    // don't know why, but Android needs 'toString', while desktop wants 'toLocalFile'
#ifdef ANDROID
    reader.setFileName(url.toString());
#else
    reader.setFileName(url.toLocalFile());
#endif
    QImage qImg;
    if (!reader.read(&qImg))
        qDebug() << "couldn't open " << url.toString();

    qImg = qImg.convertToFormat(QImage::Format_BGR888);
    auto cvImg = cv::Mat(qImg.height(), qImg.width(), CV_8UC3, qImg.bits(), qImg.bytesPerLine()).clone();

    auto scanResult = cv::recognition::sudoku::process(cvImg, cv::recognition::sudoku::Config::defaultPicture());
    m_resultBuffer.push_back(std::move(scanResult));
    emit processingFinished();
    m_state.store(State::Idle, std::memory_order::release);
}

}    // namespace qt
