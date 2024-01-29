#ifndef PREPROCESS_PIPELINE_BUILDER_H
#define PREPROCESS_PIPELINE_BUILDER_H

#include "PreprocessStep.h"

/**
 * @brief Class for building the steps required
 * for image processing. Follows the builder pattern.
 */
class PipelineBuilder
{
public:
    PipelineBuilder& addStep(StepType type, const StepParameters& params);
    void updateStepParameters(size_t index, const StepParameters& params);

    void process(cv::Mat& frame);
    void processDebugStack(cv::Mat& frame, int hStackLength = 3);

private:
    std::vector<std::unique_ptr<PreprocessStep>> steps;
    std::vector<StepType> stepTypes;
};

#endif