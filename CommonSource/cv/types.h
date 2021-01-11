#pragma once
#include "base/math.h"
#include <opencv2/core.hpp>

namespace cv {

using Contour = std::vector<Point>;
using ContourVector = std::vector<Contour>;

/// corner points of more or less square contours.
/// Always ccw, starting at top left, no pairwise equal points!
/// (create by cv::findCornerPoints)
using CornerPoints = std::array<Point, 4>;

class Line2d
{
public:
    Line2d(const cv::Point2d &pointOnLine, const cv::Vec2d &unitDirection)
        : m_point(pointOnLine), m_direction(unitDirection)
    {
        assert(math::equal(cv::norm(m_direction), 1.0));
    }

    Line2d(const cv::Point2d &p0, const cv::Point2d &p1): m_point(p0), m_direction(p1.x - p0.x, p1.y - p0.y)
    {
        assert(!math::equal(cv::norm(m_direction), 0.0));
        m_direction *= (1.0 / cv::norm(m_direction));
    }

    const cv::Point2d &point() const { return m_point; }

    const cv::Vec2d &direction() const { return m_direction; }

private:
    cv::Point2d m_point{0.0, 0.0};
    cv::Vec2d m_direction{1.0, 0.0};
};

}    // namespace cv
