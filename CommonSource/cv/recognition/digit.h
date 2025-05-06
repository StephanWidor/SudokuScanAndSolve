#pragma once
#include <filesystem>
#include <opencv2/core.hpp>

namespace cv::recognition::digit {

class ClassifierImpl
{
public:
    ClassifierImpl(bool needsBlackOnWhite): needsBlackOnWhite(needsBlackOnWhite) {}
    virtual ~ClassifierImpl() {}
    virtual int classify(const cv::Mat &preprocessedImg) = 0;
    const bool needsBlackOnWhite{true};
};

class Classifier
{
public:
    enum Type
    {
        KNN = 0,
        CNN,
        Tesseract
    };

    explicit Classifier(Type type, const std::filesystem::path &trainDataDir = {});

    int classify(const cv::Mat &img) const;

private:
    std::unique_ptr<ClassifierImpl> m_pimpl;
};

}    // namespace cv::recognition::digit
