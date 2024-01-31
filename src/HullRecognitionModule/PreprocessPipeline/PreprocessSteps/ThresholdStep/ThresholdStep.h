#ifndef THRESHOLD_STEP_H
#define THRESHOLD_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents a threshold preprocessing step.
 * Implements a threshold operation as part of the image preprocessing
 * pipeline. It extends the IPreprocessStep interface.
 */
class ThresholdStep : public IPreprocessStep
{
public:
    ThresholdStep(int thresholdValue, int maxValue, int thresholdType);

    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& newParams) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;

private:
    int thresholdValue;
    int maxValue;
    int thresholdType;
};

#endif