#pragma once
#include "cv/recognition/digit.h"
#include <filesystem>

namespace test::cv::recognition::digit {

void runAll(const std::filesystem::path &imgFolder,
            ::cv::recognition::digit::Classifier::Type = ::cv::recognition::digit::Classifier::Tesseract);

void invertAll(const std::filesystem::path &imgFolder, const std::filesystem::path &invertedImgFolder);

}    // namespace test::cv::recognition::digit
