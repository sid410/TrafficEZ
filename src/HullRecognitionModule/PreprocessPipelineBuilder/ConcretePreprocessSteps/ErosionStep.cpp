#include "ErosionStep.h"

ErosionStep::ErosionStep(int morphShape, cv::Size kernelSize, int iterations)
    : iterations(iterations)
{
    erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
}

void ErosionStep::process(cv::Mat& frame) const
{
    cv::erode(frame, frame, erodeKernel, cv::Point(-1, -1), iterations);
}