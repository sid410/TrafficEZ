#ifndef THRESHOLD_STEP_H
#define THRESHOLD_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents a threshold preprocessing step.
 * Implements a threshold operation as part of the image preprocessing
 * pipeline. It extends the PreprocessStep interface.
 */
class ThresholdStep : public PreprocessStep
{
public:
    ThresholdStep(int thresholdValue, int maxValue, int thresholdType);

    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& newParams) override;

private:
    int thresholdValue;
    int maxValue;
    int thresholdType;
};

#endif