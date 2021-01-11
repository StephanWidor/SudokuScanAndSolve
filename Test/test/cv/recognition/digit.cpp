#include "test/cv/recognition/digit.h"
#include "test/utils.h"
#include <filesystem>
#include <iostream>
#include <numeric>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>

void test::cv::recognition::digit::runAll(const std::filesystem::path &imgFolder,
                                          ::cv::recognition::digit::Classifier::Type classifierType)
{
    std::cout << "test::cv::recognition::digit::runAll(" << imgFolder.string() << ", " << classifierType << ")"
              << std::endl;

    std::array<size_t, 10> numImages{0u}, numCorrect{0u}, numFalsePositive{0u}, numFalseNegative{0u},
      numOtherRecognized{0u};

    ::cv::recognition::digit::Classifier classifier(classifierType);
    test::utils::Timer timer;
    for (int expectedDigit = 0; expectedDigit < 10; ++expectedDigit)
    {
        std::cout << "\ttesting images for " << expectedDigit << std::endl;
        for (auto &p : std::filesystem::directory_iterator(imgFolder / std::to_string(expectedDigit)))
        {
            //std::cout << "\t\t" << p.path() << std::endl;
            ::cv::Mat cellImg = ::cv::imread(p.path().string(), ::cv::IMREAD_GRAYSCALE);
            int recognizedDigit = classifier.classify(cellImg);
            ++numImages[static_cast<size_t>(expectedDigit)];
            const auto correct = (recognizedDigit == expectedDigit);
            if (correct)
                ++numCorrect[static_cast<size_t>(expectedDigit)];
            else
            {
                ++numFalsePositive[static_cast<size_t>(recognizedDigit)];
                ++numFalseNegative[static_cast<size_t>(expectedDigit)];
                if (recognizedDigit != 0)
                    ++numOtherRecognized[static_cast<size_t>(expectedDigit)];
            }
        }
    }

    const auto duration = timer.secondsAsDouble();

    auto ratio = [](size_t n0, size_t n1) { return static_cast<double>(n0) / static_cast<double>(n1); };
    const auto numTotal = std::accumulate(numImages.begin(), numImages.end(), static_cast<size_t>(0u));
    std::array<double, 10> ratioCorrect, ratioFalsePositive, ratioFalseNegative, ratioOtherRecognized;
    for (size_t digit = 0; digit < 10; ++digit)
    {
        ratioCorrect[digit] = ratio(numCorrect[digit], numImages[digit]);
        ratioFalsePositive[digit] = ratio(numFalsePositive[digit], numTotal - numImages[digit]);
        ratioFalseNegative[digit] = ratio(numFalseNegative[digit], numImages[digit]);
        ratioOtherRecognized[digit] = ratio(numOtherRecognized[digit], numImages[digit]);
    }
    const auto correctTotal = std::accumulate(numCorrect.begin(), numCorrect.end(), static_cast<size_t>(0u));
    const auto ratioCorrectTotal = ratio(correctTotal, numTotal);
    const auto ratioWrongNumber =
      ratio(std::accumulate(numFalsePositive.begin() + 1, numFalsePositive.end(), static_cast<size_t>(0u)), numTotal);
    const auto ratioFalsePositiveNumber = 1.0 - ratio(numCorrect[0], numImages[0]);
    const auto ratioFalseNegativeNumber =
      ratio(numFalsePositive[0], std::accumulate(numImages.begin() + 1, numImages.end(), static_cast<size_t>(0u)));

    std::cout << "Read " << numTotal << " images in " << duration << " seconds." << std::endl;
    std::cout << "Total ratio for correct results: " << ratioCorrectTotal << std::endl;
    std::cout << "Total ratio for incorrect numbers: " << ratioWrongNumber << std::endl;
    std::cout << "Total ratio for recognized number when there is none: " << ratioFalsePositiveNumber << std::endl;
    std::cout << "Total ratio for recognized no number when there is one: " << ratioFalseNegativeNumber << std::endl
              << std::endl;

    ::cv::destroyAllWindows();
}

void test::cv::recognition::digit::invertAll(const std::filesystem::path &imgFolder,
                                             const std::filesystem::path &invertedImgFolder)
{
    std::cout << "test::cv::recognition::digit::invertAll(" << imgFolder.string() << ", " << invertedImgFolder.string()
              << ")" << std::endl;
    if (!std::filesystem::exists(invertedImgFolder))
        std::filesystem::create_directory(invertedImgFolder);
    for (int expectedDigit = 0; expectedDigit < 10; ++expectedDigit)
    {
        const auto invertedFolder = invertedImgFolder / std::to_string(expectedDigit);
        if (!std::filesystem::exists(invertedFolder))
            std::filesystem::create_directory(invertedFolder);
        for (auto &p : std::filesystem::directory_iterator(imgFolder / std::to_string(expectedDigit)))
        {
            ::cv::Mat img = ::cv::imread(p.path().string(), ::cv::IMREAD_GRAYSCALE);
            ::cv::Mat inverted = ::cv::Scalar(255) - img;
            ::cv::imwrite((invertedImgFolder / std::to_string(expectedDigit) / p.path().filename()).string(), inverted);
        }
    }
    std::cout << "done\n" << std::endl;
}
