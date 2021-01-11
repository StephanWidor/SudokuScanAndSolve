#pragma once
#include "base/math.h"
#include "cv/types.h"

namespace cv {

template<typename Scalar>
inline Point round(const Point_<Scalar> &point)
{
    return Point(static_cast<int>(std::round(point.x)), static_cast<int>(std::round(point.y)));
}

inline double dist(const cv::Point &p0, const cv::Point &p1)
{
    return cv::norm(cv::Vec2d(p1.x - p0.x, p1.y - p0.y));
}

template<typename T>
inline double signedDist(const cv::Line2d &line, const cv::Point_<T> &p)
{
    cv::Vec2d diff(static_cast<double>(p.x) - line.point().x, static_cast<double>(p.y) - line.point().y);
    return line.direction()[0] * diff[1] - line.direction()[1] * diff[0];
}

template<typename T>
inline double signedDist(const cv::Point_<T> &p, const cv::Line2d &line)
{
    return signedDist(line, p);
}

bool intersecting(const cv::Rect &, const cv::Rect &, int tolerance = 0);

bool totallyInside(const cv::Rect &innerRect, const cv::Rect &outerRect);

cv::Rect join(const cv::Rect &, const cv::Rect &);

inline double rectRatio(const cv::Rect &rect)
{
    return math::maxRatio(rect.width, rect.height);
}

inline std::array<cv::Point, 2> topLeftAndBottomRight(const cv::Rect &rect)
{
    return {rect.tl(), rect.br()};
}

}    // namespace cv
