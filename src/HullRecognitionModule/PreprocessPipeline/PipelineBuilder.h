#ifndef PREPROCESS_PIPELINE_BUILDER_H
#define PREPROCESS_PIPELINE_BUILDER_H

#include "IPreprocessStep.h"

/**
 * @brief Class for building the steps required
 * for image processing. Follows the builder pattern.
 */
class PipelineBuilder
{
public:
    PipelineBuilder& addStep(StepType type, const StepParameters& params);
    void setStepParameters(size_t stepIndex, const StepParameters& params);

    void updateStepParameterById(size_t stepIndex,
                                 int paramId,
                                 const std::any& value);

    size_t getNumberOfSteps() const;
    StepType getStepType(size_t stepIndex) const;
    StepParameters getStepCurrentParameters(size_t stepIndex) const;

    void process(cv::Mat& frame);
    void processDebugStack(cv::Mat& frame, int hStackLength = 3);

private:
    std::vector<std::unique_ptr<IPreprocessStep>> steps;
};

#endif