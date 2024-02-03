#include "PipelineBuilder.h"

/**
 * @brief Adds a preprocessing step to the pipeline created from the StepFactory.
 * This method creates and adds a specific type of preprocessing step
 * to the pipeline based on the provided type and parameters.
 * @param type The type of preprocessing step to add, as defined in StepType.
 * @param params The parameters for the preprocessing step, as defined in StepParameters.
 * @return Reference to the modified PipelineBuilder object.
 */
PipelineBuilder& PipelineBuilder::addStep(StepType type,
                                          const StepParameters& params)
{
    auto step = StepFactory::createStep(type, params);
    if(step != nullptr)
    {
        steps.emplace_back(std::move(step));
    }
    else
    {
        std::cerr << "Unsupported step type provided or parameters mismatch in "
                     "addStep.\n";
    }
    return *this;
}

/**
 * @brief Sets the parameters of a specific preprocessing step at the given index.
 * @param stepIndex The index of the step in the pipeline to be set.
 * @param params The new parameters for the step, as defined in StepParameters.
 */
void PipelineBuilder::setStepParameters(size_t stepIndex,
                                        const StepParameters& params)
{
    if(stepIndex >= steps.size())
    {
        std::cerr << "Step index " << stepIndex << "out of range.\n";
        return;
    }

    steps[stepIndex]->setStepParameters(params);
}

/**
 * @brief This method provides a way to adjust just one parameter of a step
 * without needing to update all parameters of the step. It's useful
 * for fine-tuning individual aspects of a step's behavior during runtime.
 * @param stepIndex The index of the step in the pipeline to be updated.
 * @param paramId An identifier for the specific parameter within the step to update.
 * The meaning of this identifier is specific to the type of step being modified.
 * @param value The new value to be assigned to the parameter identified by `paramId`.
 * The type and value range of this parameter must be appropriate for the parameter
 * being updated.
 */
void PipelineBuilder::updateStepParameterById(size_t stepIndex,
                                              int paramId,
                                              const std::any& value)
{
    if(stepIndex >= steps.size())
    {
        std::cerr << "Step index " << stepIndex << "out of range.\n";
        return;
    }

    steps[stepIndex]->updateParameterById(paramId, value);
}

/**
 * @brief Clearing all the added steps, basically a way to reset the builder 
 * to start constructing a new pipeline.
 */
void PipelineBuilder::clearAllSteps()
{
    steps.clear();
}

/**
 * @brief Retrieves the current number of preprocessing steps in the pipeline.
 * @return The total number of steps currently in the pipeline.
 */
size_t PipelineBuilder::getNumberOfSteps() const
{
    return steps.size();
}

/**
 * @brief Gets the type of a preprocessing step at a specified index.
 * @param stepIndex Zero-based index of the step in the pipeline whose type
 * is being queried.
 * @return The type of the preprocessing step at the specified index, or
 * StepType::Undefined if the index is invalid.
 */
StepType PipelineBuilder::getStepType(size_t stepIndex) const
{
    if(stepIndex >= steps.size())
    {
        std::cerr << "Step index " << stepIndex << " out of range.\n";
        return StepType::Undefined;
    }

    return steps[stepIndex]->getType();
}

/**
 * @brief Retrieves the current parameters of a preprocessing step at a given index.
 * @param stepIndex Zero-based index of the step in the pipeline whose type
 * is being queried.
 * @return The current parameters of the step at the specified index, wrapped
 * in a StepParameters object. If the step index is invalid, returns an empty
 * StepParameters object.
 */
StepParameters PipelineBuilder::getStepCurrentParameters(size_t stepIndex) const
{
    if(stepIndex >= steps.size())
    {
        std::cerr << "Step index " << stepIndex << " out of range.\n";
        return StepParameters{};
    }

    return steps[stepIndex]->getCurrentParameters();
}

/**
 * @brief Preprocess image with the builder pattern.
 * This is a faster implementation as it uses the reference
 * of a frame to process, as compared to the processDebugStack method.
 * @param frame The image frame to be processed.
 */
void PipelineBuilder::process(cv::Mat& frame)
{
    if(steps.size() < 1)
        return; // do nothing if there are no steps.

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
    if(steps.size() < 1)
        return; // do nothing if there are no steps.

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