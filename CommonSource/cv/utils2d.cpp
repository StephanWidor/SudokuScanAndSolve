#include "cv/utils2d.h"

bool cv::intersecting(const cv::Rect &r0, const cv::Rect &r1, int tolerance)
{
    const auto [tl0, br0] = topLeftAndBottomRight(r0);
    const auto [tl1, br1] = topLeftAndBottomRight(r1);
    return (tl0.x <= br1.x + tolerance && br0.x >= tl1.x - tolerance && tl0.y <= br1.y + tolerance &&
            br0.y >= tl1.y - tolerance);
}

bool cv::totallyInside(const cv::Rect &innerRect, const cv::Rect &outerRect)
{
    const auto [outerTl, outerBr] = topLeftAndBottomRight(outerRect);
    const auto [innerTl, innerBr] = topLeftAndBottomRight(innerRect);
    return (outerTl.x <= innerTl.x && outerTl.y <= innerTl.y && outerBr.x >= innerBr.x && outerBr.y >= innerBr.y);
}

cv::Rect cv::join(const cv::Rect &r0, const cv::Rect &r1)
{
    if (r0.empty())
        return r1;
    if (r1.empty())
        return r0;
    const auto [tl0, br0] = topLeftAndBottomRight(r0);
    const auto [tl1, br1] = topLeftAndBottomRight(r1);
    cv::Point tl(std::min(tl0.x, tl1.x), std::min(tl0.y, tl1.y));
    cv::Point br(std::max(br0.x, br1.x), std::max(br0.y, br1.y));
    return cv::Rect(tl, br);
}
