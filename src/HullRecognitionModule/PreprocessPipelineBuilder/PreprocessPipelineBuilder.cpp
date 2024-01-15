#include "PreprocessPipelineBuilder.h"
#include "DilationStep.h"
#include "ErosionStep.h"
#include "GaussianBlurStep.h"
#include "GrayscaleStep.h"
#include "MOG2BackgroundSubtractionStep.h"
#include "ThresholdStep.h"

/**
 * @brief Uses cv::cvtColor to convert to cv::COLOR_BGR2GRAY
 */
PreprocessPipelineBuilder& PreprocessPipelineBuilder::addGrayscaleStep()
{
    steps.emplace_back(std::make_unique<GrayscaleStep>());
    return *this;
}

/**
 * @brief Uses cv::GaussianBlur
 */
PreprocessPipelineBuilder&
PreprocessPipelineBuilder::addGaussianBlurStep(int kernelSize, double sigma)
{
    steps.emplace_back(std::make_unique<GaussianBlurStep>(kernelSize, sigma));
    return *this;
}

/**
 * @brief Uses cv::createBackgroundSubtractorMOG2
 */
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

/**
 * @brief Uses cv::threshold
 */
PreprocessPipelineBuilder& PreprocessPipelineBuilder::addThresholdStep(
    int thresholdValue, int maxValue, int thresholdType)
{
    steps.emplace_back(std::make_unique<ThresholdStep>(
        thresholdValue, maxValue, thresholdType));
    return *this;
}

/**
 * @brief Uses cv::dilate
 */
PreprocessPipelineBuilder& PreprocessPipelineBuilder::addDilationStep(
    int morphShape, cv::Size kernelSize, int iterations)
{
    steps.emplace_back(
        std::make_unique<DilationStep>(morphShape, kernelSize, iterations));
    return *this;
}

/**
 * @brief Uses cv::erode
 */
PreprocessPipelineBuilder& PreprocessPipelineBuilder::addErosionStep(
    int morphShape, cv::Size kernelSize, int iterations)
{
    steps.emplace_back(
        std::make_unique<ErosionStep>(morphShape, kernelSize, iterations));
    return *this;
}

/**
 * @brief Preprocess image with the builder pattern.
 * This is a faster implementation as it uses the reference
 * of a frame to process, as compared to the processDebugStack method.
 * @param frame the frame to undergo image processing.
 */
void PreprocessPipelineBuilder::process(cv::Mat& frame)
{
    for(const auto& step : steps)
    {
        step->process(frame);
    }
}

/**
 * @brief Preprocess image with the builder pattern.
 * This is a slower implementation as compared to the process method.
 * This is because we are creating a new frame for each process,
 * then stacking it horizontally and vertically to show
 * each step of the preprocessing defined by the builder.
 * @param frame the frame to undergo image processing.
 * @param hStackLength the max horizontal stack length for a tiled view of each step.
 */
void PreprocessPipelineBuilder::processDebugStack(cv::Mat& frame,
                                                  int hStackLength)
{
    std::vector<cv::Mat> stackedFrames;
    std::vector<cv::Mat> horizontalStacks;

    for(const auto& step : steps)
    {
        step->process(frame);

        cv::Mat processedFrame;
        frame.copyTo(processedFrame);
        stackedFrames.push_back(processedFrame);

        if(stackedFrames.size() == hStackLength)
        {
            cv::Mat horizontalStack;
            cv::hconcat(stackedFrames, horizontalStack);
            horizontalStacks.push_back(horizontalStack);

            stackedFrames.clear();
        }
    }

    // Pad with empty frames if needed to make the last horizontal stack complete
    if(!stackedFrames.empty())
    {
        while(stackedFrames.size() < hStackLength)
        {
            cv::Mat emptyFrame(
                frame.rows, frame.cols, frame.type(), cv::Scalar(0, 0, 0));
            stackedFrames.push_back(emptyFrame);
        }

        // Concatenate remaining frames in the last horizontal stack
        cv::Mat horizontalStack;
        cv::hconcat(stackedFrames, horizontalStack);
        horizontalStacks.push_back(horizontalStack);
    }

    // Vertically concatenate all horizontal stacks
    cv::Mat debugStack;
    cv::vconcat(horizontalStacks, debugStack);

    cv::imshow("Debug Preprocess Stack", debugStack);
}
