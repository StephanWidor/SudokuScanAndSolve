#include "qt/ImageProvider.h"
#include <QQuickImageProvider>
#include <opencv2/imgproc.hpp>

QImage qt::ResultImageProvider::requestImage(const QString &, QSize *size, const QSize &requestedSize)
{
    if (!requestedSize.isEmpty())
        cv::resize(m_img, m_img, cv::Size(requestedSize.width(), requestedSize.height()), 0, 0, cv::INTER_AREA);
    if (size != nullptr)
        *size = QSize(m_img.cols, m_img.rows);
    if (m_img.type() == CV_8UC1)
        return QImage(m_img.data, m_img.cols, m_img.rows, static_cast<int>(m_img.step), QImage::Format_Grayscale8);
    return QImage(m_img.data, m_img.cols, m_img.rows, static_cast<int>(m_img.step), QImage::Format_BGR888);
}
