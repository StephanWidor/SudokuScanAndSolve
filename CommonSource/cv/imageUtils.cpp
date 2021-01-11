#include "cv/imageUtils.h"
#include "base/math.h"
#include "cv/contourUtils.h"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

void cv::rotate(const cv::Mat &img, double angle, cv::Mat &o_rotatedImg)
{
    if (angle != 0.0)
    {
        const cv::Point2f center(static_cast<float>(img.cols) / 2.0f, static_cast<float>(img.rows) / 2.0f);
        auto transform = cv::getRotationMatrix2D(center, angle, 1.0);
        const auto radianAngle = math::toRadian * angle;
        const auto cosAngle = std::abs(std::cos(radianAngle));
        const auto sinAngle = std::abs(std::sin(radianAngle));
        const auto width =
          static_cast<int>(static_cast<double>(img.cols) * cosAngle + static_cast<double>(img.rows) * sinAngle);
        const auto height =
          static_cast<int>(static_cast<double>(img.cols) * sinAngle + static_cast<double>(img.rows) * cosAngle);
        transform.at<double>(0, 2) += (width / 2.0 - static_cast<double>(center.x));
        transform.at<double>(1, 2) += (height / 2.0 - static_cast<double>(center.y));
        cv::warpAffine(img, o_rotatedImg, transform, cv::Size(width, height));
    }
    else if (img.data != o_rotatedImg.data)
        img.copyTo(o_rotatedImg);
}

double cv::fitToSize(const cv::Mat &img, const cv::Size &size, bool inner, cv::Mat &o_fitImg)
{
    if (img.rows == 0 || img.cols == 0)
        return std::numeric_limits<double>::infinity();
    const auto rowScale = math::ratio(size.height, img.rows);
    const auto colScale = math::ratio(size.width, img.cols);
    const auto scale = inner ? std::min(rowScale, colScale) : std::max(rowScale, colScale);
    if (scale != 1.0)
        cv::resize(img, o_fitImg, cv::Size(0, 0), scale, scale, cv::INTER_AREA);
    else if (img.data != o_fitImg.data)
        img.copyTo(o_fitImg);
    return scale;
}

cv::Mat cv::warpTransform(const cv::CornerPoints &cornerPoints, const cv::Rect &warpRect, bool useRectOffset)
{
    float xOffset = useRectOffset ? static_cast<float>(warpRect.tl().x) : 0.0f;
    float yOffset = useRectOffset ? static_cast<float>(warpRect.tl().y) : 0.0f;
    std::array<cv::Point2f, 4> border = {
      cv::Point2f(xOffset, yOffset), cv::Point2f(xOffset, yOffset + static_cast<float>(warpRect.height)),
      cv::Point2f(xOffset + static_cast<float>(warpRect.width), yOffset + static_cast<float>(warpRect.height)),
      cv::Point2f(xOffset + static_cast<float>(warpRect.width), yOffset)};
    std::array<cv::Point2f, 4> corner = {cv::Point2f(cornerPoints[0]), cv::Point2f(cornerPoints[1]),
                                         cv::Point2f(cornerPoints[2]), cv::Point2f(cornerPoints[3])};
    return cv::getPerspectiveTransform(corner.data(), border.data());
}

cv::Mat cv::warpedContourInterior(const cv::Mat &img, const cv::Contour &contour)
{
    cv::Rect warpRect = cv::boundingRect(contour);
    cv::CornerPoints cornerPoints;
    if (!cv::findCornerPoints(contour, cornerPoints))
        return cv::Mat();
    cv::Mat transform = warpTransform(cornerPoints, warpRect, false);
    cv::Mat warpedImg;
    cv::warpPerspective(img, warpedImg, transform, warpRect.size());
    return warpedImg;
}

char cv::showAndWait(const cv::Mat &img, const std::string &text)
{
#ifdef ANDROID
    (void)img;
    (void)text;
#else
    if (!img.empty())
    {
        cv::namedWindow(text, cv::WINDOW_AUTOSIZE);
        cv::imshow(text, img);
        char c = cv::waitKey();
        cv::destroyWindow(text);
        return c;
    }
#endif
    return ' ';
}
