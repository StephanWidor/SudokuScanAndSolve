#pragma once
#include "cv/recognition/digit.h"
#include "sudoku/Sheet.h"

namespace cv::recognition::sudoku {

struct Config
{
    static constexpr std::array<int, 7> threshBlockSizes{9, 11, 13, 15, 17, 19, 21};
    static constexpr int processImgSize{720};

    std::array<decltype(threshBlockSizes)::const_iterator, 2> threshBlockSizeRange;    ///< range of block sizes to try
    double minPuzzleBBoxArea;    ///< minimum area for bbox of whole puzzle
    double distortionTolerance;    ///< max tolerated distortion from rect (see cv::relativeDistortion)
    double warpTolerance;    ///< if relativeDistortion is greater this, puzzle img will be warped to rect
    bool doSobel;    ///< use sobel derivatives for finding vertical and horizontal lines of grid

    static constexpr Config defaultVideo()
    {
        return {{threshBlockSizes.begin() + 1, threshBlockSizes.begin() + 2},
                0.1 * static_cast<double>(processImgSize * processImgSize),
                0.1,
                0.1,
                true};
    }
    static constexpr Config defaultPicture()
    {
        return {{threshBlockSizes.begin(), threshBlockSizes.end()},
                0.1 * static_cast<double>(processImgSize * processImgSize),
                0.4,
                0.0,
                true};
    }
};

struct Result
{
    operator bool() const { return success; }
    bool success{false};
    cv::Mat processingImg{};    ///< shows us "how far we got"
    /// in case of failure: preprocessed (grayscale, resized to Config::processImgSize, gaussian blurred) input img
    /// in case of success: the above with sudoku cells drawn on it
    cv::Mat img{};
    std::array<cv::Mat, 81> cellImages{};
};

Result process(const cv::Mat &img, const Config &config = Config::defaultVideo());

Result camFailResult();

::sudoku::Sheet readSheet(const std::array<cv::Mat, 81> &cellImages,
                          const cv::recognition::digit::Classifier &classifier, bool doParallel);

::sudoku::Sheet votedSheet(const std::vector<Result> &results, const cv::recognition::digit::Classifier &classifier,
                           size_t offset = 0);

}    // namespace cv::recognition::sudoku
