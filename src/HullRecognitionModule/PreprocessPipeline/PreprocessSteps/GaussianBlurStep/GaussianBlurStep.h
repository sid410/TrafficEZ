#ifndef GAUSSIAN_BLUR_STEP_H
#define GAUSSIAN_BLUR_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents a gaussian blur preprocessing step.
 * Implements a gaussian blur operation as part of the image preprocessing
 * pipeline. It extends the PreprocessStep interface.
 */
class GaussianBlurStep : public PreprocessStep
{
public:
    GaussianBlurStep(int kernelSize, double sigma);

    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& newParams) override;

private:
    int kernelSize;
    double sigma;

    void checkGaussianKernelValidity(int kSize);
};

#endif