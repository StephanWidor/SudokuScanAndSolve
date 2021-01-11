#include "qt/ProcessingFilter.h"
#include "cv/recognition/sudoku.h"
#include "qt/FileSystem.h"
#include "qt/imageConvert.h"
#include <QDebug>
#include <QImage>
#include <QImageReader>
#include <opencv2/highgui.hpp>
#include <thread>

namespace {

void rotate(cv::Mat &io_img, int cameraRotation)
{
    if (std::abs(cameraRotation) < 45)
        return;
    cameraRotation = cameraRotation % 360;
    if (cameraRotation < 0)
        cameraRotation += 360;
    if (cameraRotation < 135)
        cv::rotate(io_img, io_img, cv::ROTATE_90_COUNTERCLOCKWISE);
    else if (cameraRotation > 225)
        cv::rotate(io_img, io_img, cv::ROTATE_90_CLOCKWISE);
    else
        cv::rotate(io_img, io_img, cv::ROTATE_180);
}

}    // namespace

QVideoFrame qt::ProcessingFilterRunnable::run(QVideoFrame *input, const QVideoSurfaceFormat &, RunFlags)
{
    if (m_filter.m_resultBuffer.size() >= ProcessingFilter::resultBufferSize)
        return *input;
    cv::Mat img = toMat(*input);
    if (img.empty())
    {
        m_filter.m_resultBuffer.push_back(cv::recognition::sudoku::camFailResult());
        emit m_filter.processingFinished();
        return *input;
    }
#if defined(ANDROID) || defined(_WIN32)
    cv::flip(img, img, 0);    // whyever this is neccessary...
#endif
    const auto cameraRotation = m_filter.m_cameraRotation.load();
    rotate(img, cameraRotation);

    auto scanResult = cv::recognition::sudoku::process(img, cv::recognition::sudoku::Config::defaultVideo());
    img = m_filter.m_outputProcessingImg ? scanResult.processingImg : scanResult.img;
    if (scanResult)
    {
        m_filter.m_resultBuffer.push_back(std::move(scanResult));
        if (m_filter.m_resultBuffer.size() >= ProcessingFilter::resultBufferSize)
            emit m_filter.processingFinished();
    }

    rotate(img, -cameraRotation);
#if defined(_WIN32)    // not sure if this is only on my Win machine..
    cv::flip(img, img, 0);
#endif

    if (QVideoFrame output = toQVideoFrame(img); output.width() != 0)
        return output;
    return *input;
}

QVideoFilterRunnable *qt::ProcessingFilter::createFilterRunnable()
{
    m_resultBuffer.clear();
    return new ProcessingFilterRunnable(*this);
}

void qt::ProcessingFilter::processImg(const QUrl &url)
{
    FileSystem::requestPermission(FileSystem::AccessType::Read);
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
    auto scanResult = cv::recognition::sudoku::process(toMat(qImg), cv::recognition::sudoku::Config::defaultPicture());
    m_resultBuffer.push_back(std::move(scanResult));
    emit processingFinished();
}
