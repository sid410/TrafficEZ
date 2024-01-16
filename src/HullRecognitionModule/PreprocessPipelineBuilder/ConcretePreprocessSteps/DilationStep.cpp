#include "DilationStep.h"

DilationStep::DilationStep(int morphShape, cv::Size kernelSize, int iterations)
    : iterations(iterations)
{
    dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
}

void DilationStep::process(cv::Mat& frame) const
{
    cv::dilate(frame, frame, dilateKernel, cv::Point(-1, -1), iterations);
}