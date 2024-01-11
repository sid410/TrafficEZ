#include "PreprocessPipelineBuilder.h"
#include "DilationStep.h"
#include "ErosionStep.h"
#include "GaussianBlurStep.h"
#include "GrayscaleStep.h"
#include "MOG2BackgroundSubtractionStep.h"
#include "ThresholdStep.h"

PreprocessPipelineBuilder& PreprocessPipelineBuilder::addGrayscaleStep()
{
    steps.emplace_back(std::make_unique<GrayscaleStep>());
    return *this;
}

PreprocessPipelineBuilder&
PreprocessPipelineBuilder::addGaussianBlurStep(int kernelSize, double sigma)
{
    steps.emplace_back(std::make_unique<GaussianBlurStep>(kernelSize, sigma));
    return *this;
}

PreprocessPipelineBuilder&
PreprocessPipelineBuilder::addMOG2BackgroundSubtractionStep(
    int history,
    double varThreshold,
    double varThresholdGen,
    int nMixtures,
    bool detectShadows,
    int shadowValue)
{
    steps.emplace_back(
        std::make_unique<MOG2BackgroundSubtractionStep>(history,
                                                        varThreshold,
                                                        varThresholdGen,
                                                        nMixtures,
                                                        detectShadows,
                                                        shadowValue));
    return *this;
}

PreprocessPipelineBuilder& PreprocessPipelineBuilder::addThresholdStep(
    int thresholdValue, int maxValue, int thresholdType)
{
    steps.emplace_back(std::make_unique<ThresholdStep>(
        thresholdValue, maxValue, thresholdType));
    return *this;
}

PreprocessPipelineBuilder& PreprocessPipelineBuilder::addDilationStep(
    int morphShape, cv::Size kernelSize, int iterations)
{
    steps.emplace_back(
        std::make_unique<DilationStep>(morphShape, kernelSize, iterations));
    return *this;
}

PreprocessPipelineBuilder& PreprocessPipelineBuilder::addErosionStep(
    int morphShape, cv::Size kernelSize, int iterations)
{
    steps.emplace_back(
        std::make_unique<ErosionStep>(morphShape, kernelSize, iterations));
    return *this;
}

void PreprocessPipelineBuilder::process(cv::Mat& frame)
{
    for(const auto& step : steps)
    {
        step->process(frame);
    }
}