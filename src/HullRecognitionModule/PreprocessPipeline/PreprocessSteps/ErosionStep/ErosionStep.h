#ifndef EROSION_STEP_H
#define EROSION_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents an erosion preprocessing step.
 * Implements an erosion operation as part of the image preprocessing
 * pipeline. It extends the IPreprocessStep interface.
 */
class ErosionStep : public IPreprocessStep
{
public:
    ErosionStep(int morphShape, cv::Size kernelSize, int iterations);

    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& newParams) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;

private:
    cv::Mat erodeKernel;
    int morphShape;
    cv::Size kernelSize;
    int iterations;
};

#endif