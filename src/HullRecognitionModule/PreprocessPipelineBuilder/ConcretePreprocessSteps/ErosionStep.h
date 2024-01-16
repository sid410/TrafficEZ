#ifndef EROSION_STEP_H
#define EROSION_STEP_H

#include "PreprocessStep.h"

class ErosionStep : public PreprocessStep
{
private:
    cv::Mat erodeKernel;
    int iterations;

public:
    ErosionStep(int morphShape, cv::Size kernelSize, int iterations);
    void process(cv::Mat& frame) const override;
};

#endif