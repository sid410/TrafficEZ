#ifndef GRAYSCALE_STEP_H
#define GRAYSCALE_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents a grayscale preprocessing step.
 * Implements a grayscale operation as part of the image preprocessing
 * pipeline. It extends the IPreprocessStep interface.
 */
class GrayscaleStep : public IPreprocessStep
{
public:
    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& params) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;
};

#endif