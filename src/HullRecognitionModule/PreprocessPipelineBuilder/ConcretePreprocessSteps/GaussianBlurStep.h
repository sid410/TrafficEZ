#ifndef GAUSSIAN_BLUR_STEP_H
#define GAUSSIAN_BLUR_STEP_H

#include "PreprocessStep.h"

class GaussianBlurStep : public PreprocessStep
{
private:
    int kernelSize;
    double sigma;

public:
    GaussianBlurStep(int kernelSize, double sigma);
    void process(cv::Mat& frame) const override;
};

#endif