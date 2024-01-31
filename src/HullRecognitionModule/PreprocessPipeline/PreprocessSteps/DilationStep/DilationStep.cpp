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

void DilationStep::updateParameters(const StepParameters& newParams)
{
    auto params = std::get_if<DilationParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr << "Please provide a valid DilationParams, or check if "
                     "you are using the correct builder index.\n";
        return;
    }

    morphShape = params->morphShape;
    kernelSize = params->kernelSize;
    iterations = params->iterations;
    dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
}