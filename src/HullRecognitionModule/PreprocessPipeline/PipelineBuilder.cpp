#include "PipelineBuilder.h"
#include "DilationStep.h"
#include "ErosionStep.h"
#include "GaussianBlurStep.h"
#include "GrayscaleStep.h"
#include "MOG2BackgroundSubtractionStep.h"
#include "ThresholdStep.h"
#include <memory>
#include <vector>

/**
 * @brief Adds a preprocessing step to the pipeline.
 * This method creates and adds a specific type of preprocessing step
 * to the pipeline based on the provided StepType and parameters.
 * @param type The type of preprocessing step to add, as defined in StepType.
 * @param params The parameters for the preprocessing step, as defined in StepParameters.
 * @return Reference to the modified PipelineBuilder object.
 */
PipelineBuilder& PipelineBuilder::addStep(StepType type,
                                          const StepParameters& params)
{
    switch(type)
    {

    // Uses cv::cvtColor to convert to cv::COLOR_BGR2GRAY
    case StepType::Grayscale:
        steps.emplace_back(std::make_unique<GrayscaleStep>());
        break;

    // Uses cv::GaussianBlur
    case StepType::GaussianBlur:
        if(auto p = std::get_if<GaussianBlurParams>(&params.params))
        {
            steps.emplace_back(
                std::make_unique<GaussianBlurStep>(p->kernelSize, p->sigma));
        }
        break;

    // Uses cv::createBackgroundSubtractorMOG2
    case StepType::MOG2BackgroundSubtraction:
        if(auto p =
               std::get_if<MOG2BackgroundSubtractionParams>(&params.params))
        {
            steps.emplace_back(std::make_unique<MOG2BackgroundSubtractionStep>(
                p->history,
                p->varThreshold,
                p->varThresholdGen,
                p->nMixtures,
                p->detectShadows,
                p->shadowValue));
        }
        break;

    // Uses cv::threshold
    case StepType::Threshold:
        if(auto p = std::get_if<ThresholdParams>(&params.params))
        {
            steps.emplace_back(std::make_unique<ThresholdStep>(
                p->thresholdValue, p->maxValue, p->thresholdType));
        }
        break;

    // Uses cv::dilate
    case StepType::Dilation:
        if(auto p = std::get_if<DilationParams>(&params.params))
        {
            steps.emplace_back(std::make_unique<DilationStep>(
                p->morphShape, p->kernelSize, p->iterations));
        }
        break;

    // Uses cv::erode
    case StepType::Erosion:
        if(auto p = std::get_if<ErosionParams>(&params.params))
        {
            steps.emplace_back(std::make_unique<ErosionStep>(
                p->morphShape, p->kernelSize, p->iterations));
        }
        break;

    // For StepType not yet defined in the enum class
    default:
        std::cerr << "Unsupported step type provided in addStep.\n";
    }
    stepTypes.push_back(type);
    return *this;
}

/**
 * @brief Updates the parameters of a specific preprocessing step at the given index.
 * @param index The index of the step in the pipeline to be updated.
 * @param params The new parameters for the step, as defined in StepParameters.
 */
void PipelineBuilder::updateStepParameters(size_t index,
                                           const StepParameters& params)
{
    if(index < steps.size())
    {
        steps[index]->updateParameters(params);
    }
    else
    {
        std::cerr << "Cannot update! Step index " << index << "out of range.\n";
    }
}

/**
 * @brief Preprocess image with the builder pattern.
 * This is a faster implementation as it uses the reference
 * of a frame to process, as compared to the processDebugStack method.
 * @param frame The image frame to be processed.
 */
void PipelineBuilder::process(cv::Mat& frame)
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
 * @param frame The image frame to be processed.
 * @param hStackLength The max horizontal stack length for a tiled view of each step.
 */
void PipelineBuilder::processDebugStack(cv::Mat& frame, int hStackLength)
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
