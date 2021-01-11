#include "qt/imageConvert.h"
#include <QDebug>
#include <QImage>
#include <iostream>
#include <opencv2/imgproc.hpp>

cv::Mat qt::toMat(QVideoFrame &frame)
{
    cv::Mat img(frame.height(), frame.width(), CV_8UC3);
    if (frame.map(QAbstractVideoBuffer::ReadOnly) && frame.isReadable())
    {
        switch (frame.pixelFormat())
        {
            case QVideoFrame::Format_ARGB32:    // seems this is rather BGRA!
            {
                cv::Mat view(frame.height(), frame.width(), CV_8UC4, frame.bits());
                cv::cvtColor(view, img, cv::COLOR_BGRA2BGR);
                break;
            }
            case QVideoFrame::Format_RGB32:
            {
                cv::Mat view(frame.height(), frame.width(), CV_8UC4, frame.bits());
                cv::cvtColor(view, img, cv::COLOR_RGBA2BGR);
                break;
            }
            case QVideoFrame::Format_RGB24:
            {
                cv::Mat view(frame.height(), frame.width(), CV_8UC3, frame.bits());
                cv::cvtColor(view, img, cv::COLOR_RGB2BGR);
                break;
            }
            case QVideoFrame::Format_BGRA32:
            {
                cv::Mat bgr32Mat(frame.height(), frame.width(), CV_8UC4, frame.bits());
                cv::cvtColor(bgr32Mat, img, cv::COLOR_BGRA2BGR);
                break;
            }
            case QVideoFrame::Format_ABGR32:    // seems this is rather RGBA!
            {
                cv::Mat rgbaMat(frame.height(), frame.width(), CV_8UC4, frame.bits());
                cv::cvtColor(rgbaMat, img, cv::COLOR_RGBA2BGR);
                break;
            }
            case QVideoFrame::Format_BGR32:
            {
                cv::Mat bgr32Mat(frame.height(), frame.width(), CV_8UC4, frame.bits());
                cv::cvtColor(bgr32Mat, img, cv::COLOR_BGRA2BGR);
                break;
            }
            case QVideoFrame::Format_BGR24:
            {
                img = cv::Mat(frame.height(), frame.width(), CV_8UC3, frame.bits()).clone();
                break;
            }
            case QVideoFrame::Format_YUV420P:
            {
                cv::Mat yuv420pMat(frame.height() + frame.height() / 2, frame.width(), CV_8UC1, frame.bits());
                // Don't know why, but we need YUV420p2RGB, not YUV420p2BGR here!
                cv::cvtColor(yuv420pMat, img, cv::COLOR_YUV420p2RGB);
                break;
            }
            case QVideoFrame::Format_YUYV:
            {
                cv::Mat yuvMat(frame.height(), frame.width(), CV_8UC2, frame.bits());
                cv::cvtColor(yuvMat, img, cv::COLOR_YUV2BGR_YUY2);
                break;
            }
            default:
            {
                // not optimal, but maybe worth a try...
                img = toMat(QImage(frame.bits(), frame.width(), frame.height(),
                                   QVideoFrame::imageFormatFromPixelFormat(frame.pixelFormat())));
                if (img.empty())
                    qDebug() << "Couldn't convert " << frame.pixelFormat() << "to cv::Mat";
                break;
            }
        }
        frame.unmap();
    }
    else
        std::cout << "Couldn't map or read QVideoFrame";
    return img;
}

QVideoFrame qt::toQVideoFrame(const cv::Mat &img)
{
    cv::Mat bufferMat(img.size(), CV_8UC4);
    if (img.channels() == 3)
        cv::cvtColor(img, bufferMat, cv::COLOR_RGB2BGRA);
    else
        cv::cvtColor(img, bufferMat, cv::COLOR_GRAY2BGRA);
    const auto numBytes = static_cast<int>(bufferMat.elemSize() * bufferMat.total());
    const auto bytesPerLine = static_cast<int>(bufferMat.elemSize()) * bufferMat.cols;
    QVideoFrame frame(numBytes, QSize(img.cols, img.rows), bytesPerLine, QVideoFrame::Format_BGR32);
    if (frame.map(QAbstractVideoBuffer::WriteOnly) && frame.isWritable())
    {
        std::memcpy((void *)frame.bits(), (const void *)bufferMat.data, numBytes);
        frame.unmap();
    }
    else
        std::cout << "couldn't map frame";
    return frame;
}

cv::Mat qt::toMat(const QImage &img)
{
    auto converted = img.convertToFormat(QImage::Format_RGB888);
    cv::Mat cvImg;
    if (converted.format() != QImage::Format_Invalid && !converted.size().isEmpty())
    {
        cv::Mat view(converted.height(), converted.width(), CV_8UC3, converted.bits(), converted.bytesPerLine());
        cv::cvtColor(view, cvImg, cv::COLOR_RGBA2BGR);
    }
    return cvImg;
}
