#pragma once
#include <cv/recognition/digit.h>
#include <filesystem>

namespace test::cv::recognition::sudoku {

void run(const std::filesystem::path &picturesDir, const std::filesystem::path &videosDir,
         const std::filesystem::path &outputDir, bool showProcessedImages = false,
         ::cv::recognition::digit::Classifier::Type = ::cv::recognition::digit::Classifier::KNN);

void runCustom();

}    // namespace test::cv::recognition::sudoku
