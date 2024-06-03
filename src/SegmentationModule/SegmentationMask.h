#ifndef SEGMENTATION_MASK_H
#define SEGMENTATION_MASK_H

#include "AutoBackendOnnx.h"
#include "ISegmentationStrategy.h"
#include <memory>
#include <opencv2/opencv.hpp>

class SegmentationMask
{
public:
    SegmentationMask();

    void initializeModel(const std::string& modelPath,
                         std::unique_ptr<ISegmentationStrategy> strategy);

    cv::Mat generateMask(const cv::Mat& img);

    cv::Mat processResults(const cv::Mat& img,
                           const std::vector<YoloResults>& results);
    cv::Mat processResultsDebug(const cv::Mat& img, const cv::Mat& mask);

    float getWhiteArea(const cv::Mat& mask);
    int getContourCount(const cv::Mat& mask);

private:
    bool isModelInitialized;
    std::unique_ptr<AutoBackendOnnx> model;
    std::unique_ptr<ISegmentationStrategy> segmentationStrategy;
};

#endif
