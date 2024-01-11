#include "GaussianBlurStep.h"

GaussianBlurStep::GaussianBlurStep(int kernelSize, double sigma)
    : kernelSize(kernelSize)
    , sigma(sigma)
{}

void GaussianBlurStep::process(cv::Mat& frame) const
{
    cv::GaussianBlur(frame, frame, cv::Size(kernelSize, kernelSize), sigma);
}