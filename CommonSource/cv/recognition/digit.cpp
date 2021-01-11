#include "cv/recognition/digit.h"
#include "base/ExecutionHelp.h"
#include "cv/imageUtils.h"
#include "cv/types.h"
#include "cv/utils2d.h"
#include <atomic>
#include <exception>
#include <execution>
#include <filesystem>
#include <iostream>
#include <numeric>
#include <opencv2/imgproc.hpp>
#include <opencv2/text/ocr.hpp>

#ifdef ENABLE_TESSERACT
#include <tesseract/baseapi.h>
#endif

using namespace cv::recognition::digit;

namespace {

int charToDigit(char c)
{
    switch (c)
    {
        case '1':
            return 1;
        case 'l':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        default:
            return 0;
    }
}

std::filesystem::path trainDataPath(Classifier::Type type, const std::filesystem::path &trainDataDir)
{
    switch (type)
    {
        case Classifier::CNN:
            return trainDataDir / "OCRBeamSearch_CNN_model_data.xml.gz";
        case Classifier::KNN:
            return trainDataDir / "OCRHMM_knn_model_data.xml.gz";
        default:
            return trainDataDir / "eng.traineddata";
    }
    return "";
}

bool needsBlackOnWhite(Classifier::Type type)
{
    switch (type)
    {
        case Classifier::KNN:
            return false;
        default:
            return true;
    }
    return "";
}

class HMM : public ClassifierImpl
{
public:
    HMM(Classifier::Type type, const std::filesystem::path &trainDataDir)
        : ClassifierImpl(::needsBlackOnWhite(type))
        , m_ocr(cv::text::loadOCRHMMClassifier(trainDataPath(type, trainDataDir).string(), type))
    {}

    int classify(const cv::Mat &preprocessedImg) override
    {
        std::vector<int> classes;
        std::vector<double> confidences;
        m_ocr->eval(preprocessedImg, classes, confidences);
        return bestChoice(classes, confidences);
    }

private:
    static int bestChoice(const std::vector<int> &classes, const std::vector<double> &confidences)
    {
        int digit = 0;
        double confidence = 0.0;
        for (size_t i = 0; i < classes.size(); ++i)
        {
            if (confidences[i] < 0.4)
                continue;
            if (int _digit = charToDigit(vocabulary[classes[i]]); _digit != 0)
            {
                if (confidences[i] > confidence)
                {
                    digit = _digit;
                    confidence = confidences[i];
                }
            }
        }
        return digit;
    }

    cv::Ptr<cv::text::OCRHMMDecoder::ClassifierCallback> m_ocr;
    static constexpr std::string_view vocabulary = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
};

#ifdef ENABLE_TESSERACT
class Tesseract : public ClassifierImpl
{
public:
    Tesseract(Classifier::Type type, const std::filesystem::path &trainDataDir)
        : ClassifierImpl(::needsBlackOnWhite(type))
    {
        for (auto &api : m_apis)
        {
            api = new tesseract::TessBaseAPI();
            api->Init(trainDataDir.c_str(), "eng");
            api->SetPageSegMode(tesseract::PSM_SINGLE_CHAR);
            api->SetVariable("tessedit_char_whitelist", "123456789");
        }
    }

    ~Tesseract()
    {
        for (auto api : m_apis)
        {
            api->End();
            delete api;
        }
    }

    int classify(const cv::Mat &preprocessedImg) override
    {
        int digit = 0;
        const size_t index = ++m_parallelCount % numParallel;
        auto api = m_apis[index];
        std::lock_guard lock(m_mutexes[index]);
        api->SetImage(preprocessedImg.data, preprocessedImg.size().width, preprocessedImg.size().height,
                      preprocessedImg.channels(), static_cast<int>(preprocessedImg.step1()));
        api->SetSourceResolution(70);
        char *text = api->GetUTF8Text();
        if (text != nullptr && api->MeanTextConf() >= 50)
            digit = charToDigit(*text);
        delete[] text;
        return digit;
    }

private:
    static constexpr size_t numParallel = 4;
    std::atomic<size_t> m_parallelCount{0};
    std::vector<std::mutex> m_mutexes{numParallel};
    std::vector<tesseract::TessBaseAPI *> m_apis{numParallel};
};
#endif

std::unique_ptr<ClassifierImpl> makePimpl(Classifier::Type type, std::filesystem::path trainDataDir)
{
    if (trainDataDir.empty())
        trainDataDir = TOSTRING(DEFAULT_TRAINDATADIR);
    if (type == Classifier::Type::Tesseract)
#ifdef ENABLE_TESSERACT
        return std::make_unique<Tesseract>(type, trainDataDir);
#else
        throw std::runtime_error("Tesseract classifier is not available");
#endif
    return std::make_unique<HMM>(type, trainDataDir);
}

/// helper for preprocess function below
cv::Rect rectWithoutBorder(const cv::ContourVector &contours, const cv::Size &imgSize)
{
    auto inner = [](int outer) { return static_cast<int>(std::round(0.2 * static_cast<double>(outer))); };
    cv::Point tl(inner(imgSize.width), inner(imgSize.height));
    cv::Point br = cv::Point(imgSize.width, imgSize.height) - tl;
    auto contourWithinBorder = [&](const cv::Contour &contour) {
        return std::all_of(contour.begin(), contour.end(), [&](const cv::Point &point) {
            return (point.x <= tl.x || point.x >= br.x || point.y <= tl.y || point.y >= br.y);
        });
    };
    auto bound = std::accumulate(contours.begin(), contours.end(), cv::Rect(),
                                 [&](const cv::Rect &bound, const cv::Contour &contour) {
                                     if (contour.size() > 4 && !contourWithinBorder(contour))
                                         return cv::join(bound, cv::boundingRect(contour));
                                     return bound;
                                 });
    if (10 * bound.area() < imgSize.area())
        bound.width = 0;
    return bound;
}

cv::Mat preprocess(const cv::Mat &cellImg, bool blackOnWhite)
{
    if (cellImg.empty())
        return cv::Mat();
    cv::Mat grayImg, threshImg;
    if (cellImg.type() == CV_8UC1)
        grayImg = cellImg;
    else
        cv::cvtColor(cellImg, grayImg, cv::COLOR_BGR2GRAY);
    cv::Mat mean, stdDev;
    cv::meanStdDev(grayImg, mean, stdDev);
    int blockSize = std::max(cellImg.size().height, cellImg.size().height);
    if (blockSize % 2 == 0)
        ++blockSize;
    cv::adaptiveThreshold(grayImg, threshImg, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, blockSize,
                          0.5 * stdDev.at<double>());
    cv::ContourVector contours;
    cv::findContours(threshImg, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);
    cv::Mat filteredImg;
    if (const cv::Rect boundingRect = rectWithoutBorder(contours, threshImg.size()); !boundingRect.empty())
    {
        cv::meanStdDev(grayImg(boundingRect), mean, stdDev);
        const auto relativeStdDev = stdDev.at<double>() / mean.at<double>();
        if (relativeStdDev > 0.05)
        {
            cv::Scalar threshMean = cv::mean(threshImg);
            if ((threshMean.val[0] > 127) != blackOnWhite)
                threshImg = 255 - threshImg;
            filteredImg = cv::Mat(threshImg.size(), CV_8UC1, blackOnWhite ? 255 : 0);
            threshImg(boundingRect).copyTo(filteredImg(boundingRect));
            //            cv::resize(filteredImg, filteredImg, cv::Size(200, 200));
            //            cv::showAndWait(filteredImg);
        }
    }
    return filteredImg;
}

}    // namespace

Classifier::Classifier(Type type, const std::filesystem::path &trainDataDir): m_pimpl(makePimpl(type, trainDataDir))
{}

int Classifier::classify(const cv::Mat &img) const
{
    const auto preprocessedImg = preprocess(img, m_pimpl->needsBlackOnWhite);
    if (preprocessedImg.empty())
        return 0;
    return m_pimpl->classify(preprocessedImg);
}
