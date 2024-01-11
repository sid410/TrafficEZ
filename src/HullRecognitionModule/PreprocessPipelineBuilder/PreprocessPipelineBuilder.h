#ifndef PREPROCESS_PIPELINE_BUILDER_H
#define PREPROCESS_PIPELINE_BUILDER_H

#include "PreprocessStep.h"
#include <memory>
#include <vector>

class PreprocessPipelineBuilder
{
private:
    std::vector<std::unique_ptr<PreprocessStep>> steps;

public:
    PreprocessPipelineBuilder& addGrayscaleStep();
    PreprocessPipelineBuilder& addGaussianBlurStep(int kernelSize = 5,
                                                   double sigma = 1.5);
    PreprocessPipelineBuilder&
    addMOG2BackgroundSubtractionStep(int history = 150,
                                     double varThreshold = 16,
                                     double varThresholdGen = 9,
                                     int nMixtures = 5,
                                     bool detectShadows = true,
                                     int shadowValue = 200);
    PreprocessPipelineBuilder&
    addThresholdStep(int thresholdValue = 200,
                     int maxValue = 255,
                     int thresholdType = cv::THRESH_BINARY);
    PreprocessPipelineBuilder&
    addDilationStep(int morphShape = cv::MORPH_ELLIPSE,
                    cv::Size kernelSize = cv::Size(5, 5),
                    int iterations = 1);
    PreprocessPipelineBuilder&
    addErosionStep(int morphShape = cv::MORPH_ELLIPSE,
                   cv::Size kernelSize = cv::Size(5, 5),
                   int iterations = 1);
    void process(cv::Mat& frame);
};

#endif