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

void ErosionStep::updateParameters(const StepParameters& newParams)
{
    auto params = std::get_if<ErosionParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr << "Please provide a valid ErosionParams, or check if "
                     "you are using the correct builder index.\n";
        return;
    }

    morphShape = params->morphShape;
    kernelSize = params->kernelSize;
    iterations = params->iterations;
    erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
}