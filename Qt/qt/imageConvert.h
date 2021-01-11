#pragma once
#include <QVideoFrame>
#include <opencv2/core.hpp>

namespace qt {

/// Convert QVideoFrame to cv::Mat.
/// Not all possible formats are covered, but hopefully the common ones.
cv::Mat toMat(QVideoFrame &frame);

/// Convert cv::Mat img to QVideoFrame in QVideoFrame::Format_BGR32
QVideoFrame toQVideoFrame(const cv::Mat &img);

cv::Mat toMat(const QImage &img);

}    // namespace qt
