#ifndef EROSION_STEP_H
#define EROSION_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents an erosion preprocessing step.
 * Implements an erosion operation as part of the image preprocessing
 * pipeline. It extends the PreprocessStep interface.
 */
class ErosionStep : public PreprocessStep
{
public:
    ErosionStep(int morphShape, cv::Size kernelSize, int iterations);

    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& newParams) override;

private:
    cv::Mat erodeKernel;
    int morphShape;
    cv::Size kernelSize;
    int iterations;
};

#endif