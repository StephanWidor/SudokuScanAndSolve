#pragma once
#include "base/colors.h"
#include "cv/types.h"

namespace cv {

inline cv::Scalar toBGR(const colors::Color &color)
{
    return cv::Scalar(color[2], color[1], color[0]);
};

void rotate(const cv::Mat &img, double angle, cv::Mat &o_rotatedImg);

double fitToSize(const cv::Mat &img, const cv::Size &size, bool inner, cv::Mat &o_fitImg);

cv::Mat warpTransform(const cv::CornerPoints &cornerPoints, const cv::Rect &warpRect, bool useRectOffset);

cv::Mat warpedContourInterior(const cv::Mat &img, const cv::Contour &contour);

char showAndWait(const ::cv::Mat &img, const std::string &text = "Nüschte");

}    // namespace cv
