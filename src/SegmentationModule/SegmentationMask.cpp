#include "SegmentationMask.h"
#include <iostream>
#include <opencv2/opencv.hpp>

SegmentationMask::SegmentationMask()
{
    isModelInitialized = false;
    detectionResultCount = 0;
}

void SegmentationMask::initializeModel(
    const std::string& modelPath,
    std::unique_ptr<ISegmentationStrategy> strategy)
{
    // set to either vehicle or person strategy
    segmentationStrategy = std::move(strategy);

    // Ensure OnnxProviders::CPU is correctly defined
    const char* onnx_provider = OnnxProviders::CPU.c_str();

    const char* onnx_logid = "segmentation";

    model = std::make_unique<AutoBackendOnnx>(
        modelPath.c_str(), onnx_logid, onnx_provider);

    isModelInitialized = true;
}

cv::Mat SegmentationMask::generateMask(const cv::Mat& img, bool isBinaryMask)
{
    if(!isModelInitialized)
    {
        std::cerr << "Failed to initialize model.\n";
    }

    cv::Mat converted_img;
    cv::cvtColor(img, converted_img, cv::COLOR_BGR2RGB);

    float conf_threshold = 0.30f;
    float iou_threshold = 0.45f;
    float mask_threshold = 0.5f;
    int conversion_code = cv::COLOR_BGR2RGB;

    auto results = model->predict_once(converted_img,
                                       conf_threshold,
                                       iou_threshold,
                                       mask_threshold,
                                       conversion_code);

    auto filteredResults = segmentationStrategy->filterResults(results);

    countsByClassType = getCountsByClassType(filteredResults);

    detectionResultCount = filteredResults.size();
    cv::Mat output = processResults(img, filteredResults);

    if(isBinaryMask)
    {
        return output;
    }
    else
    {
        return processResultsDebug(img, output);
    }
}

cv::Mat
SegmentationMask::processResults(const cv::Mat& img,
                                 const std::vector<YoloResults>& results)
{
    cv::Mat mask = cv::Mat::zeros(img.size(), img.type());

    for(const auto& result : results)
    {
        if(result.mask.rows > 0 && result.mask.cols > 0)
        {
            mask(result.bbox).setTo(cv::Scalar(255, 255, 255), result.mask);
        }
    }

    return mask;
}

cv::Mat SegmentationMask::processResultsDebug(const cv::Mat& img,
                                              const cv::Mat& mask)
{
    cv::Mat highlightedImg = img.clone();
    cv::Mat colorMask(img.size(), CV_8UC3, cv::Scalar(0, 255, 0)); // Green mask

    colorMask.copyTo(highlightedImg, mask);

    cv::addWeighted(
        highlightedImg, 0.5, img, 0.5, 0, highlightedImg, img.type());

    return highlightedImg;
}

float SegmentationMask::getWhiteArea(const cv::Mat& mask)
{
    // Convert to binary if not already
    cv::Mat binaryMask;
    if(mask.channels() > 1)
    {
        cv::cvtColor(mask, binaryMask, cv::COLOR_BGR2GRAY);
    }
    else
    {
        binaryMask = mask.clone();
    }

    cv::threshold(binaryMask, binaryMask, 127, 255, cv::THRESH_BINARY);

    // Find all white contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(
        binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // Calculate the total area of the contours
    float totalArea = 0.0;
    for(const auto& contour : contours)
    {
        totalArea += cv::contourArea(contour);
    }

    return totalArea;
}

int SegmentationMask::getContourCount(const cv::Mat& mask)
{
    // Convert to binary if not already
    cv::Mat binaryMask;
    if(mask.channels() > 1)
    {
        cv::cvtColor(mask, binaryMask, cv::COLOR_BGR2GRAY);
    }
    else
    {
        binaryMask = mask.clone();
    }

    cv::threshold(binaryMask, binaryMask, 127, 255, cv::THRESH_BINARY);

    // Find all white contours
    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(
        binaryMask, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    return contours.size();
}

int SegmentationMask::getDetectionResultSize()
{
    return detectionResultCount;
}

std::unordered_map<std::string, int> SegmentationMask::getClassTypeAndCounts()
{
    return countsByClassType;
}

std::unordered_map<std::string, int> SegmentationMask::getCountsByClassType(
    const std::vector<YoloResults>& filteredResults)
{
    // Map of class indices to names
    std::unordered_map<int, std::string> classNames = {{0, "person"},
                                                       {1, "bicycle"},
                                                       {2, "car"},
                                                       {3, "motorcycle"},
                                                       {5, "bus"},
                                                       {6, "train"},
                                                       {7, "truck"}};

    // Count occurrences of each class by name
    std::unordered_map<std::string, int> classCounts;
    for(const auto& result : filteredResults)
    {
        const std::string& className = classNames[result.class_idx];
        classCounts[className]++;
    }

    return classCounts;
}