#ifndef DILATION_STEP_H
#define DILATION_STEP_H

#include "PreprocessStep.h"

class DilationStep : public PreprocessStep
{
private:
    cv::Mat dilateKernel;
    int iterations;

public:
    DilationStep(int morphShape, cv::Size kernelSize, int iterations);
    void process(cv::Mat& frame) const override;
};

#endif