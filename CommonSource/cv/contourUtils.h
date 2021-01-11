#pragma once
#include "cv/utils2d.h"

namespace cv {

bool contourWithinCornerConnections(const cv::Contour &contour, const cv::CornerPoints &cornerPoints,
                                    double toleranceForDistance);

double relativeDistortion(const cv::CornerPoints &cornerPoints, const cv::Rect &boundaryRect);

bool findCornerPoints(const cv::Contour &contour, cv::CornerPoints &o_cornerPoints);

inline double minDiagonalSize(const cv::CornerPoints &cornerPoints)
{
    return std::min(cv::dist(cornerPoints[0], cornerPoints[2]), cv::dist(cornerPoints[1], cornerPoints[3]));
}

cv::Point2d baryCenter(const cv::Contour &contour);

inline bool smallerCenterX(const cv::Contour &c0, const cv::Contour &c1)
{
    return baryCenter(c0).x < baryCenter(c1).x;
}

inline bool smallerCenterY(const cv::Contour &c0, const cv::Contour &c1)
{
    return baryCenter(c0).y < baryCenter(c1).y;
}

/// Finf indices of the outer contours as found by cv::findContours
std::vector<size_t> outerIndices(const std::vector<cv::Vec4i> &hierarchy, bool onlyWithChildren = false);

std::vector<size_t> childrenIndices(const std::vector<cv::Vec4i> &hierarchy, size_t index,
                                    size_t maxDepth = std::numeric_limits<size_t>::max(),
                                    size_t maxSize = std::numeric_limits<size_t>::max());

void filterByHierarchy(cv::ContourVector &io_contours, const std::vector<cv::Vec4i> &hierarchy, size_t indexOfOuter);

template<class FilterFunction>
inline void filter(cv::ContourVector &io_contours, const FilterFunction &filterFunc)
{
    io_contours.erase(
      std::remove_if(io_contours.begin(), io_contours.end(), [&](const cv::Contour &c) { return !filterFunc(c); }),
      io_contours.end());
}

template<class FilterFunction>
inline cv::ContourVector filtered(const cv::ContourVector &contours, const FilterFunction &filterFunc)
{
    cv::ContourVector filteredContours;
    filteredContours.reserve(contours.size());
    std::copy_if(contours.begin(), contours.end(), std::back_inserter(filteredContours), filterFunc);
    return filteredContours;
}

void perspectiveTransform(cv::Contour &io_contour, const cv::Mat &transform);

void perspectiveTransform(cv::ContourVector &io_contours, const cv::Mat &transform);

}    // namespace cv
