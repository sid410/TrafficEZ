#ifndef GAUSSIAN_BLUR_STEP_H
#define GAUSSIAN_BLUR_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents a gaussian blur preprocessing step.
 * Implements a gaussian blur operation as part of the image preprocessing
 * pipeline. It extends the IPreprocessStep interface.
 */
class GaussianBlurStep : public IPreprocessStep
{
public:
    GaussianBlurStep(int kernelSize, double sigma);

    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& newParams) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;

private:
    int kernelSize;
    double sigma;

    void checkGaussianKernelValidity(int kSize);
};

#endif