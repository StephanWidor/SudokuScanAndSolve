#include "cv/recognition/sudoku.h"
#include "base/ExecutionHelp.h"
#include "base/colors.h"
#include "cv/contourUtils.h"
#include "cv/imageUtils.h"
#include "cv/recognition/digit.h"
#include "cv/utils2d.h"
#include <numeric>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

namespace {

using cv::recognition::sudoku::Config;

cv::Mat preprocess(const cv::Mat &img)
{
    cv::Mat grayImg;
    if (img.type() == CV_8UC1)
        img.copyTo(grayImg);
    else
        cv::cvtColor(img, grayImg, cv::COLOR_BGR2GRAY);
    if (cv::fitToSize(grayImg, cv::Size(Config::processImgSize, Config::processImgSize), false, grayImg) !=
        std::numeric_limits<double>::infinity())
    {
        cv::GaussianBlur(grayImg, grayImg, cv::Size(5, 5), 1.0);
        return grayImg;
    }
    return cv::Mat();
}

/// helper struct, only used in extractPuzzleRect
struct RectContour
{
    size_t index = 0;
    size_t numChildren = 0;
    double rectArea = 0.0;
    double rectRatio = 1.0;
    double minDiagonalSize = 0.0;
    double relativeDistortion = 0.0;
    cv::Rect rect{0, 0, 0, 0};
    cv::CornerPoints cornerPoints = utils::makeFilledArray<4>(cv::Point{0, 0});
};

struct PuzzleRect
{
    bool empty() const { return rect.empty(); }
    cv::Rect rect{0, 0, 0, 0};
    cv::Mat warpTransform{};
    cv::ContourVector contours{};
};

PuzzleRect extractPuzzleRect(const cv::Mat &processingImg, const cv::recognition::sudoku::Config &config)
{
    cv::ContourVector contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(processingImg, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);

    auto outerContourCandidates = [&]() {
        std::vector<RectContour> candidates;
        const auto outerIndices = cv::outerIndices(hierarchy, true);
        for (const auto index : outerIndices)
        {
            const cv::Contour &contour = contours[index];
            const auto rect = cv::boundingRect(contour);
            const auto rectArea = rect.area();
            const auto rectRatio = cv::rectRatio(rect);
            if (rectArea >= config.minPuzzleBBoxArea && rectRatio < 1.5)
            {
                if (cv::CornerPoints cornerPoints; cv::findCornerPoints(contour, cornerPoints))
                {
                    const auto relativeDistortion = cv::relativeDistortion(cornerPoints, rect);
                    if (relativeDistortion <= config.distortionTolerance &&
                        cv::contourWithinCornerConnections(contour, cornerPoints, 0.1))
                    {
                        const auto numChildren = cv::childrenIndices(hierarchy, index, 3, 50).size();
                        if (numChildren >= 10)
                        {
                            candidates.emplace_back(RectContour{index, numChildren, static_cast<double>(rectArea),
                                                                rectRatio, cv::minDiagonalSize(cornerPoints),
                                                                relativeDistortion, rect, cornerPoints});
                        }
                    }
                }
            }
        }
        return candidates;
    }();
    if (outerContourCandidates.empty())
        return {};

    auto &outerContour = *std::max_element(outerContourCandidates.begin(), outerContourCandidates.end(),
                                           [](const RectContour &r0, const RectContour &r1) {
                                               if (r0.numChildren != r1.numChildren)
                                                   return (r0.numChildren < r1.numChildren);
                                               return (r0.rectRatio > r1.rectRatio);
                                           });

    cv::filterByHierarchy(contours, hierarchy, outerContour.index);
    cv::filter(contours, [minRectArea = static_cast<int>((0.5 / 121) * outerContour.minDiagonalSize *
                                                         outerContour.minDiagonalSize)](const cv::Contour &contour) {
        return contour.size() >= 4 && (boundingRect(contour).area() >= minRectArea);
    });

    cv::Mat warpTransform;
    if (outerContour.relativeDistortion > config.warpTolerance)
    {
        warpTransform = cv::warpTransform(outerContour.cornerPoints, outerContour.rect, true);
        for (auto &contour : contours)
            cv::perspectiveTransform(contour, warpTransform);
    }
    return {outerContour.rect, std::move(warpTransform), std::move(contours)};
}

cv::Mat extractLinesImage(const cv::Mat &gridImg, bool vertical, bool doSobel)
{
    cv::Mat o_lines;
    if (doSobel)
    {
        cv::Sobel(gridImg, o_lines, CV_16S, vertical ? 1 : 0, vertical ? 0 : 1, 7);
        cv::convertScaleAbs(o_lines, o_lines);
    }
    else
        o_lines = gridImg.clone();

    cv::Size size = vertical ? cv::Size(2, Config::processImgSize / 20) : cv::Size(Config::processImgSize / 20, 2);
    cv::Mat kernel = getStructuringElement(cv::MORPH_RECT, size);
    cv::erode(o_lines, o_lines, kernel);

    size = vertical ? cv::Size(2, Config::processImgSize / 10) : cv::Size(Config::processImgSize / 10, 2);
    kernel = cv::getStructuringElement(cv::MORPH_RECT, size);
    cv::dilate(o_lines, o_lines, kernel);

    cv::ContourVector contours;
    cv::findContours(o_lines, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);

    if (contours.size() > 10)
    {
        constexpr auto minRange = Config::processImgSize / 5;
        if (vertical)
        {
            cv::filter(contours,
                       [&](const cv::Contour &contour) { return cv::boundingRect(contour).height >= minRange; });
        }
        else
        {
            cv::filter(contours,
                       [&](const cv::Contour &contour) { return cv::boundingRect(contour).width >= minRange; });
        }
    }

    if (contours.size() < 10)
        return cv::Mat();

    o_lines.setTo(cv::Scalar(0));
    cv::drawContours(o_lines, contours, -1, 255, cv::FILLED);

    return o_lines;
}

cv::Mat extractGridPointsImage(const cv::Mat &gridImg, bool doSobel)
{
    std::array<cv::Mat, 2> lines;
    std::array<bool, 2> vertical{false, true};
    execution_help::transform(vertical.begin(), vertical.end(), lines.begin(),
                              [&](bool vertical) { return extractLinesImage(gridImg, vertical, doSobel); });
    if (!lines[0].empty() && !lines[1].empty())
    {
        cv::Mat gridPointsImg(gridImg.size(), CV_8UC1, cv::Scalar(0));
        cv::bitwise_and(lines[0], lines[1], gridPointsImg);
        return gridPointsImg;
    }
    else
        return cv::Mat();
}

bool mergeGridPointContours(cv::ContourVector &io_contours, const cv::Size &puzzleSize)
{
    std::vector<cv::Rect> rects;
    rects.reserve(io_contours.size());
    std::vector<bool> absorbed(io_contours.size(), false);
    std::transform(io_contours.begin(), io_contours.end(), std::back_inserter(rects),
                   [](const cv::Contour &contour) { return cv::boundingRect(contour); });
    cv::CornerPoints cornerPoints;
    const auto tolerance =
      std::clamp(static_cast<int>(static_cast<double>(puzzleSize.width + puzzleSize.height) / 60.0), 2, 10);
    for (size_t i = 0; i < rects.size(); ++i)
    {
        if (absorbed[i])
            continue;
        for (size_t j = i + 1; j < rects.size(); ++j)
        {
            if (absorbed[j])
                continue;
            if (cv::intersecting(rects[i], rects[j], tolerance))
            {
                rects[i] = cv::join(rects[i], rects[j]);
                absorbed[j] = true;
                io_contours[i].insert(io_contours[i].end(), io_contours[j].begin(), io_contours[j].end());
                io_contours[j].resize(0);
            }
        }
    }
    cv::filter(io_contours, [](const auto &contour) { return !contour.empty(); });
    if (io_contours.size() == 100)
    {
        for (auto &contour : io_contours)
        {
            if (!cv::findCornerPoints(contour, cornerPoints))
                return false;
            contour.resize(0);
            contour.insert(contour.end(), cornerPoints.begin(), cornerPoints.end());
        }
        return true;
    }
    return false;
}

bool sortGridPointContours(cv::ContourVector &io_contours)
{
    if (io_contours.size() != 100)
        return false;

    const auto sortedPairs = [&io_contours]() {
        std::vector<std::pair<cv::Point2d, size_t>> sortPairs;
        sortPairs.reserve(io_contours.size());
        for (size_t i = 0; i < io_contours.size(); ++i)
            sortPairs.emplace_back(cv::baryCenter(io_contours[i]), i);
        std::sort(sortPairs.begin(), sortPairs.end(),
                  [](const auto &p0, const auto &p1) { return p0.first.y < p1.first.y; });
        for (size_t i = 0; i < 10; ++i)
        {
            auto itStart = sortPairs.begin() + (10 * i);
            auto itEnd = itStart + 10;
            std::sort(itStart, itEnd, [](const auto &p0, const auto &p1) { return p0.first.x < p1.first.x; });
        }
        return sortPairs;
    }();

    auto checkSorting = [&sortedPairs]() {
        std::vector<std::pair<double, double>> colXMinMax(
          10, {std::numeric_limits<double>::max(), -std::numeric_limits<double>::max()});
        for (size_t i = 0; i < 100; ++i)
        {
            const cv::Point2d &p = sortedPairs[i].first;
            auto &minMax = colXMinMax[i % 10];    // minmax for col
            if (p.x < minMax.first)
                minMax.first = p.x;
            if (p.x > minMax.second)
                minMax.second = p.x;
        }
        for (size_t col = 0; col < 9; ++col)
        {
            if (colXMinMax[col].second > colXMinMax[col + 1].first)
                return false;
        }
        return true;
    };
    if (!checkSorting())
        return false;

    io_contours = [&sortedPairs, &io_contours]() {
        cv::ContourVector sortedContours;
        sortedContours.reserve(io_contours.size());
        for (const auto &sortElement : sortedPairs)
            sortedContours.push_back(std::move(io_contours[sortElement.second]));
        return sortedContours;
    }();

    return true;
}

cv::ContourVector extractGridPointContours(const cv::Mat &gridImg, const cv::Size &puzzleSize, bool doSobel)
{
    if (cv::Mat gridPointsImg = extractGridPointsImage(gridImg, doSobel); !gridPointsImg.empty())
    {
        cv::ContourVector contours;
        cv::findContours(gridPointsImg, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
        if (mergeGridPointContours(contours, puzzleSize) && sortGridPointContours(contours))
            return contours;
    }
    return {};
}

cv::ContourVector extractCellContoursFromGridPointContours(const cv::ContourVector &gridPointContours)
{
    cv::ContourVector cellContours;
    cellContours.reserve(81);
    for (size_t i = 0; i < 81; ++i)
    {
        size_t row = sudoku::row(i);
        size_t col = sudoku::col(i);
        cellContours.push_back({gridPointContours[10 * row + col][2], gridPointContours[10 * (row + 1) + col][3],
                                gridPointContours[10 * (row + 1) + col + 1][0],
                                gridPointContours[10 * row + col + 1][1]});
    }
    return cellContours;
}

std::array<cv::Mat, 81> cellImages(const cv::Mat &img, const cv::ContourVector &cellContours)
{
    assert(cellContours.size() == 81);
    std::array<cv::Mat, 81> cellImages;
    execution_help::transform(cellContours.begin(), cellContours.end(), cellImages.begin(),
                              [&img](const auto &contour) { return cv::warpedContourInterior(img, contour); });
    return cellImages;
}

}    // namespace

cv::recognition::sudoku::Result cv::recognition::sudoku::process(const cv::Mat &img, const Config &config)
{
    Result result;
    result.img = preprocess(img);
    if (result.img.empty())
        return Result{};
    cv::Mat &processingImg = result.processingImg;
    for (auto threshBlockSize = config.threshBlockSizeRange.front();
         threshBlockSize != config.threshBlockSizeRange.back(); ++threshBlockSize)
    {
        cv::adaptiveThreshold(result.img, processingImg, 255.0, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV,
                              *threshBlockSize, 1.0);
        PuzzleRect puzzleRect = extractPuzzleRect(processingImg, config);
        if (puzzleRect.empty())
            break;

        result.processingImg.setTo(0);
        cv::drawContours(processingImg, puzzleRect.contours, -1, 255, cv::FILLED, 4);

        if (const auto gridPointContours =
              extractGridPointContours(processingImg, puzzleRect.rect.size(), config.doSobel);
            !gridPointContours.empty())
        {
            const auto cellContours = extractCellContoursFromGridPointContours(gridPointContours);
            if (!puzzleRect.warpTransform.empty())
            {
                cv::warpPerspective(processingImg, processingImg, puzzleRect.warpTransform, processingImg.size());
                cv::warpPerspective(result.img, result.img, puzzleRect.warpTransform, result.img.size());
            }
            result.cellImages = ::cellImages(result.img, cellContours);
            cv::cvtColor(result.img, result.img, cv::COLOR_GRAY2BGR);
            cv::drawContours(result.img, cellContours, -1, toBGR(colors::orange()), 2, 4);
            result.success = true;
            break;
        }
    }
    return result;
}

cv::recognition::sudoku::Result cv::recognition::sudoku::camFailResult()
{
    auto makeImg = []() {
        const int fontFace = cv::FONT_HERSHEY_DUPLEX;
        const double fontScale = 1.0;
        const int thickness = 2;
        int baseline = 0;
        const auto text = "  Sorry, seems your Camera is not supported  ";
        cv::Size textSize = cv::getTextSize(text, fontFace, fontScale, thickness, &baseline);
        cv::Point textOrigin(0, textSize.width / 2);
        cv::Mat img(textSize.width, textSize.width, CV_8UC3, cv::toBGR(colors::darkGray()));
        putText(img, text, textOrigin, fontFace, fontScale, cv::toBGR(colors::orange()), thickness, cv::LINE_AA);
        return img;
    };
    return {false, {}, makeImg()};
}

::sudoku::Sheet cv::recognition::sudoku::readSheet(const std::array<cv::Mat, 81> &cellImages,
                                                   const cv::recognition::digit::Classifier &classifier,
                                                   bool doParallel)
{
    ::sudoku::Sheet sheet;
    if (doParallel)
    {
        execution_help::transform(cellImages.begin(), cellImages.end(), sheet.begin(),
                                  [&classifier](const auto &img) { return classifier.classify(img); });
    }
    else
    {
        std::transform(cellImages.begin(), cellImages.end(), sheet.begin(),
                       [&classifier](const auto &img) { return classifier.classify(img); });
    }
    return sheet;
}

sudoku::Sheet cv::recognition::sudoku::votedSheet(const std::vector<Result> &results,
                                                  const cv::recognition::digit::Classifier &classifier, size_t offset)
{
    if (results.size() <= offset)
        return {};
    if (results.size() < offset + 3)
        return readSheet(results.back().cellImages, classifier, true);

    std::vector<::sudoku::Sheet> sheets(results.size() - offset);
    execution_help::transform(
      results.begin() + offset, results.end(), sheets.begin(),
      [&classifier](const sudoku::Result &result) { return readSheet(result.cellImages, classifier, false); });

    using ValueIncidences = std::array<size_t, 10>;
    using BufferGrid = std::array<ValueIncidences, 81>;

    const auto bufferGrid = std::accumulate(sheets.begin(), sheets.end(),
                                            ::utils::makeFilledArray<81>(::utils::makeFilledArray<10, size_t>(0)),
                                            [](BufferGrid sum, const ::sudoku::Sheet &sheet) {
                                                std::transform(sum.begin(), sum.end(), sheet.begin(), sum.begin(),
                                                               [](ValueIncidences incidences, ::sudoku::Value value) {
                                                                   ++incidences[value];
                                                                   return incidences;
                                                               });
                                                return sum;
                                            });
    ::sudoku::Sheet resultSheet;
    std::transform(bufferGrid.begin(), bufferGrid.end(), resultSheet.begin(), [](const ValueIncidences &incidences) {
        return static_cast<::sudoku::Value>(
          std::distance(incidences.begin(), std::max_element(incidences.begin(), incidences.end())));
    });
    return resultSheet;
}
