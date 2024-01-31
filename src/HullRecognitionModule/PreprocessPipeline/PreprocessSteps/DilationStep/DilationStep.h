#ifndef DILATION_STEP_H
#define DILATION_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents a dilation preprocessing step.
 * Implements a dilation operation as part of the image preprocessing
 * pipeline. It extends the IPreprocessStep interface.
 */
class DilationStep : public IPreprocessStep
{
public:
    DilationStep(int morphShape, cv::Size kernelSize, int iterations);

    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& newParams) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;

private:
    cv::Mat dilateKernel;
    int morphShape;
    cv::Size kernelSize;
    int iterations;
};

#endif