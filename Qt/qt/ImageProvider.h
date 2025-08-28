#pragma once
#include <QQuickImageProvider>
#include <opencv2/imgproc.hpp>

namespace qt {

class ImageProvider : public QQuickImageProvider
{
public:
    ImageProvider(): QQuickImageProvider(QQmlImageProviderBase::Image) {}

    QImage requestImage(const QString &, QSize *size, const QSize &requestedSize) override;

public slots:
    void setImage(const cv::Mat &img) { m_img = img; }

private:
    cv::Mat m_img;
};

}    // namespace qt
