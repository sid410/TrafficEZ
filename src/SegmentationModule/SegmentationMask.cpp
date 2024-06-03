#include "SegmentationMask.h"
#include <iostream>
#include <opencv2/opencv.hpp>

SegmentationMask::SegmentationMask()
{
    isModelInitialized = false;
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

cv::Mat SegmentationMask::generateMask(const cv::Mat& img)
{
    if(!isModelInitialized)
    {
        std::cerr << "Failed to initialize model.\n";
    }

    cv::Mat converted_img;
    cv::cvtColor(img, converted_img, cv::COLOR_BGR2RGB);

    // Create variables for the floating-point arguments
    float conf_threshold = 0.30f;
    float iou_threshold = 0.45f;
    float mask_threshold = 0.5f;
    int conversion_code = cv::COLOR_BGR2RGB;

    // Pass variables instead of literals to predict_once
    auto results = model->predict_once(converted_img,
                                       conf_threshold,
                                       iou_threshold,
                                       mask_threshold,
                                       conversion_code);

    auto filteredResults = segmentationStrategy->filterResults(results);

    return processResults(img, filteredResults);
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