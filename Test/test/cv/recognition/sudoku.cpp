#include "test/cv/recognition/sudoku.h"
#include "cv/recognition/sudoku.h"
#include "sudoku/Sheet.h"
#include "test/utils.h"
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <numeric>
#include <opencv2/highgui.hpp>

namespace {

struct Result
{
    std::string filePath;
    sudoku::Sheet sheet{};
    cv::Mat outputImg{};
    double durationInSeconds = 0.0;
    size_t diffFromSheet = 0;
    bool success = false;
};

struct ResultSum
{
    ResultSum(const std::string &i_name): name(i_name) {}
    ResultSum(const std::string &i_name, const std::vector<Result> &results): name(i_name)
    {
        for (const Result &result : results)
            operator+=(result);
    }
    ResultSum &operator+=(const ResultSum &other)
    {
        numResults += other.numResults;
        numRecognized += other.numRecognized;
        diff += other.diff;
        durationInSeconds += other.durationInSeconds;
        return *this;
    }
    ResultSum &operator+=(const Result &result)
    {
        ++numResults;
        if (result.success)
            ++numRecognized;
        diff += result.diffFromSheet;
        durationInSeconds += result.durationInSeconds;
        return *this;
    }
    const std::string name;
    size_t numResults = 0;
    size_t numRecognized = 0;
    size_t diff = 0;
    double durationInSeconds = 0.0;
};

size_t diffFromSheet(const sudoku::Sheet &result, const std::filesystem::path &testFile)
{
    try
    {
        return test::utils::diff(result, sudoku::loadSheet(std::filesystem::path(testFile).replace_extension(".sp")));
    }
    catch (...)
    {
        return 0;
    }
}

Result testImageFile(const std::filesystem::path &file, const cv::recognition::digit::Classifier &classifier)
{
    test::utils::Timer timer;
    Result result{file.string()};
    auto scanResult = cv::recognition::sudoku::process(cv::imread(file.string(), cv::IMREAD_GRAYSCALE),
                                                       cv::recognition::sudoku::Config::defaultPicture());
    result.success = scanResult;
    result.sheet = cv::recognition::sudoku::readSheet(scanResult.cellImages, classifier, std::execution::par);
    result.durationInSeconds = timer.secondsAsDouble();
    result.outputImg = scanResult.img;
    if (result.success)
        result.diffFromSheet = diffFromSheet(result.sheet, file);
    return result;
}

Result testVideoFile(const std::filesystem::path &file, const cv::recognition::digit::Classifier &classifier,
                     bool showImg)
{
    constexpr size_t resultBufferSize = 10;
    constexpr size_t resultBufferOffset = 0;
    Result result{file.string()};
    cv::VideoCapture cap;
    cap.open(file.string());

    std::vector<cv::recognition::sudoku::Result> resultBuffer;
    if (cap.isOpened())
    {
        cv::Mat inFrame, outFrame;
        test::utils::Timer timer;
        while (cap.read(inFrame))
        {
            auto scanResult =
              cv::recognition::sudoku::process(inFrame, cv::recognition::sudoku::Config::defaultVideo());
            outFrame = scanResult.processingImg;
            if (showImg)
            {
                cv::imshow("Scanning", outFrame);
                cv::waitKey(1);
            }
            if (scanResult)
            {
                resultBuffer.push_back(std::move(scanResult));
                if (resultBuffer.size() >= resultBufferSize)
                    break;
            }
        }
        result.success = !resultBuffer.empty();
        result.sheet = cv::recognition::sudoku::votedSheet(resultBuffer, classifier, resultBufferOffset);
        result.durationInSeconds = timer.secondsAsDouble();
        if (result.success)
        {
            result.diffFromSheet = diffFromSheet(result.sheet, file);
            result.outputImg = resultBuffer.back().img;
        }
        else
            result.outputImg = outFrame;
    }
    return result;
}

std::vector<Result> testAllFiles(const std::filesystem::path &directory,
                                 const cv::recognition::digit::Classifier &classifier, const std::string &preCout,
                                 bool showProcessedImages)
{
    std::vector<Result> results;
    for (auto &p : std::filesystem::directory_iterator(directory))
    {
        std::string extension = p.path().extension().string();
        std::locale loc;
        for (auto &c : extension)
            c = std::tolower(c, loc);
        bool isImageFile = (extension == ".jpg" || extension == ".png");
        bool isVideoFile = (extension == ".mp4" || extension == ".mpg");
        if (isImageFile || isVideoFile)
        {
            const auto filePath = p.path();
            std::cout << preCout << filePath << "... ";
            if (isImageFile)
                results.push_back(testImageFile(filePath, classifier));
            else
                results.push_back(testVideoFile(filePath, classifier, showProcessedImages));
            const auto successString = results.back().success ? "success" : "failed";
            if (showProcessedImages)
                cv::showAndWait(results.back().outputImg, successString);
            std::cout << successString << std::endl;
        }
    }
    return results;
}

std::ostream &operator<<(std::ostream &out, const Result &r)
{
    return out << r.filePath << ";" << r.durationInSeconds << ";" << r.success << ";" << r.diffFromSheet << std::endl;
}

void write(const std::vector<Result> &results, const std::filesystem::path &file)
{
    std::ofstream out;
    out.open(file);
    if (out.is_open())
    {
        out << "path;duration;recognized;diff\n";
        for (const auto &result : results)
            out << result;
        out.close();
    }
}

std::ostream &operator<<(std::ostream &out, const ResultSum &sum)
{
    double recognizeSuccess = static_cast<double>(sum.numRecognized) / static_cast<double>(sum.numResults);
    size_t numDigits = sum.numRecognized * 81;
    double digitSuccess = 1.0 - static_cast<double>(sum.diff) / static_cast<double>(numDigits);
    return out << sum.name << ";" << sum.durationInSeconds << ";" << sum.numRecognized << ";" << sum.diff << ";"
               << recognizeSuccess << ";" << digitSuccess << ";" << sum.numResults << "\n";
}

void write(const std::vector<ResultSum> &resultSums, const std::filesystem::path &file)
{
    std::ofstream out;
    out.open(file);
    if (out.is_open())
    {
        out << "path;duration;numRecognized;digitDiff;recognizeSuccess;digitSuccess;numResults\n";
        for (const auto &resultSum : resultSums)
            out << resultSum;
        out.close();
    }
}

}    // namespace

void test::cv::recognition::sudoku::run(const std::filesystem::path &picturesDir,
                                        const std::filesystem::path &videosDir, const std::filesystem::path &outputDir,
                                        bool showProcessedImages,
                                        ::cv::recognition::digit::Classifier::Type classifierType)
{
    std::cout << "test::cv::recognition::sudoku::run(" << picturesDir.string() << ", " << videosDir.string() << ", "
              << classifierType << ", " << showProcessedImages << ")" << std::endl;
    std::filesystem::create_directory(outputDir);
    const auto thisRunOutputDir = outputDir / utils::currentTimeString();
    std::filesystem::create_directory(thisRunOutputDir);
    ::cv::recognition::digit::Classifier classifier(classifierType);
    std::vector<ResultSum> sums;
    sums.reserve(3);
    if (std::filesystem::exists(picturesDir))
    {
        const auto pictureResults = testAllFiles(picturesDir, classifier, "\t", showProcessedImages);
        write(pictureResults, thisRunOutputDir / "pictures.csv");
        sums.emplace_back("pictures", pictureResults);
    }
    if (std::filesystem::exists(videosDir))
    {
        const auto videoResults = testAllFiles(videosDir, classifier, "\t", showProcessedImages);
        write(videoResults, thisRunOutputDir / "videos.csv");
        sums.emplace_back("videos", videoResults);
    }
    if (!sums.empty())
    {
        sums.emplace_back("total");
        for (size_t i = 0; i + 1 < sums.size(); ++i)
            sums.back() += sums[i];
        write(sums, thisRunOutputDir / "total.csv");
    }

    std::cout << "done\n" << std::endl;
}

void test::cv::recognition::sudoku::runCustom()
{
    ::cv::recognition::digit::Classifier classifier(::cv::recognition::digit::Classifier::Type::KNN);
    const auto file = test::utils::testDataDir / "Recognition" / "Sheets" / "Video" / "SurfTab_SD2.mp4";
    const auto result = testVideoFile(file, classifier, true);
    ::cv::showAndWait(result.outputImg);
}
