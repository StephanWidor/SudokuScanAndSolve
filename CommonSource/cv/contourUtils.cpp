#include "cv/contourUtils.h"
#include "base/ExecutionHelp.h"
#include <execution>
#include <numeric>
#include <opencv2/imgproc.hpp>

bool cv::contourWithinCornerConnections(const cv::Contour &contour, const cv::CornerPoints &cornerPoints,
                                        double toleranceForDistance)
{
    auto connection = [&](size_t i) {
        const cv::Point &p0 = cornerPoints[i];
        const cv::Point &p1 = cornerPoints[(i + 1) % 4];
        cv::Vec2d direction(p1.x - p0.x, p1.y - p0.y);
        const auto norm = cv::norm(direction);
        direction *= (1.0 / norm);
        return std::make_pair(cv::Line2d(p0, direction), toleranceForDistance * norm);
    };
    std::array<std::pair<cv::Line2d, double>, 4> connections = {connection(0), connection(1), connection(2),
                                                                connection(3)};
    return execution_help::all_of(contour.begin(), contour.end(), [&](const cv::Point &p) {
        return std::all_of(connections.begin(), connections.end(), [&](const auto &connection) {
            double dist = cv::signedDist(connection.first, p);
            return dist <= connection.second;
        });
    });
}

double cv::relativeDistortion(const cv::CornerPoints &cornerPoints, const cv::Rect &boundaryRect)
{
    double height = static_cast<double>(boundaryRect.height);
    double width = static_cast<double>(boundaryRect.width);
    return std::max({static_cast<double>(std::abs(cornerPoints[0].y - cornerPoints[3].y)) / height,
                     static_cast<double>(std::abs(cornerPoints[1].y - cornerPoints[2].y)) / height,
                     static_cast<double>(std::abs(cornerPoints[0].x - cornerPoints[1].x)) / width,
                     static_cast<double>(std::abs(cornerPoints[2].x - cornerPoints[3].x)) / width});
}

bool cv::findCornerPoints(const cv::Contour &contour, cv::CornerPoints &o_cornerPoints)
{
    const auto sumMinMax =
      std::minmax_element(contour.begin(), contour.end(),
                          [](const cv::Point &p0, const cv::Point &p1) { return (p0.x + p0.y < p1.x + p1.y); });
    const auto diffMinMax =
      std::minmax_element(contour.begin(), contour.end(),
                          [](const cv::Point &p0, const cv::Point &p1) { return (p0.x - p0.y < p1.x - p1.y); });
    o_cornerPoints[0] = *sumMinMax.first;    // top left, x+y minimal
    o_cornerPoints[1] = *diffMinMax.first;    // bottom left, x-y minimal
    o_cornerPoints[2] = *sumMinMax.second;    // bottom right, x+y maximal
    o_cornerPoints[3] = *diffMinMax.second;    // top right, x-y maximal
    return (o_cornerPoints[0] != o_cornerPoints[1] && o_cornerPoints[0] != o_cornerPoints[2] &&
            o_cornerPoints[0] != o_cornerPoints[3] && o_cornerPoints[1] != o_cornerPoints[2] &&
            o_cornerPoints[1] != o_cornerPoints[3] && o_cornerPoints[2] != o_cornerPoints[3]);
}

cv::Point2d cv::baryCenter(const cv::Contour &contour)
{
    const auto sum = std::accumulate(
      contour.begin(), contour.end(), cv::Point2d(0.0, 0.0), [](const cv::Point2d &p0, const cv::Point &p1) {
          return cv::Point2d(p0.x + static_cast<double>(p1.x), p0.y + static_cast<double>(p1.y));
      });
    return (1.0 / static_cast<double>(contour.size())) * sum;
}

std::vector<size_t> cv::outerIndices(const std::vector<cv::Vec4i> &hierarchy, bool onlyWithChildren)
{
    std::vector<size_t> indices;
    indices.reserve(hierarchy.size());
    for (size_t i = 0; i < hierarchy.size(); ++i)
    {
        const cv::Vec4i &node = hierarchy[i];
        if (node[3] < 0 && (!onlyWithChildren || node[2] >= 0))
            indices.push_back(i);
    }
    return indices;
}

std::vector<size_t> cv::childrenIndices(const std::vector<cv::Vec4i> &hierarchy, size_t index, size_t maxDepth,
                                        size_t maxSize)
{
    std::vector<size_t> indices(1, index);
    std::vector<size_t> depths(1, 0);
    for (size_t i = 0; i < indices.size(); ++i)
    {
        const auto depth = depths[i];
        if (depth == maxDepth)
            break;
        const auto nextDepth = depth + 1;
        const auto parentIndex = indices[i];
        auto childIndex = hierarchy[parentIndex][2];
        while (childIndex >= 0)
        {
            indices.push_back(childIndex);
            depths.push_back(nextDepth);
            if (indices.size() >= maxSize)
                return indices;
            childIndex = hierarchy[childIndex][0];
        }
    }
    return indices;
}

void cv::filterByHierarchy(cv::ContourVector &io_contours, const std::vector<cv::Vec4i> &hierarchy, size_t indexOfOuter)
{
    const auto indices = childrenIndices(hierarchy, indexOfOuter);
    cv::ContourVector filteredContours;
    filteredContours.reserve(indices.size());
    for (const auto index : indices)
        filteredContours.push_back(std::move(io_contours[index]));
    io_contours = std::move(filteredContours);
}

void cv::perspectiveTransform(cv::Contour &io_contour, const cv::Mat &transform)
{
    std::vector<cv::Point2f> fContour;
    std::transform(io_contour.begin(), io_contour.end(), std::back_inserter(fContour),
                   [](const cv::Point &p) { return p; });
    cv::perspectiveTransform(fContour, fContour, transform);
    std::transform(fContour.begin(), fContour.end(), io_contour.begin(),
                   [](const cv::Point2f &p) { return cv::round(p); });
}

void cv::perspectiveTransform(cv::ContourVector &io_contours, const cv::Mat &transform)
{
    for (auto &contour : io_contours)
        perspectiveTransform(contour, transform);
}
