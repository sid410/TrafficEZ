#ifndef DILATION_STEP_H
#define DILATION_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents a dilation preprocessing step.
 * Implements a dilation operation as part of the image preprocessing
 * pipeline. It extends the PreprocessStep interface.
 */
class DilationStep : public PreprocessStep
{
public:
    DilationStep(int morphShape, cv::Size kernelSize, int iterations);

    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& newParams) override;

private:
    cv::Mat dilateKernel;
    int morphShape;
    cv::Size kernelSize;
    int iterations;
};

#endif