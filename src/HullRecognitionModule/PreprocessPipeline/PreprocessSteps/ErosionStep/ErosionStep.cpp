#include "ErosionStep.h"

ErosionStep::ErosionStep(int morphShape, cv::Size kernelSize, int iterations)
    : morphShape(morphShape)
    , kernelSize(kernelSize)
    , iterations(iterations)
{
    checkErosionKernelValidity(kernelSize);
    erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
}

void ErosionStep::process(cv::Mat& frame) const
{
    cv::erode(frame, frame, erodeKernel, cv::Point(-1, -1), iterations);
}

void ErosionStep::updateParameterById(int paramId, const std::any& value)
{
    switch(paramId)
    {
    case 0: // morphShape
        if(value.type() == typeid(int))
        {
            morphShape = std::any_cast<int>(value);

            checkErosionKernelValidity(kernelSize);
            erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
        }
        break;

    case 1: // kernelSize
        if(value.type() == typeid(int))
        {
            int size = std::any_cast<int>(value);
            kernelSize = cv::Size(size, size);

            checkErosionKernelValidity(kernelSize);
            erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
        }
        break;

    case 2: // iterations
        if(value.type() == typeid(int))
        {
            iterations = std::any_cast<int>(value);
        }
        break;

    default:
        std::cerr << "Invalid parameter ID for ErosionStep.\n";
        break;
    }
}

void ErosionStep::setStepParameters(const StepParameters& newParams)
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

    checkErosionKernelValidity(kernelSize);
    erodeKernel = cv::getStructuringElement(morphShape, kernelSize);
}

StepType ErosionStep::getType() const
{
    return StepType::Erosion;
}

StepParameters ErosionStep::getCurrentParameters() const
{
    ErosionParams params;
    params.morphShape = morphShape;
    params.kernelSize = kernelSize;
    params.iterations = iterations;

    StepParameters stepParams;
    stepParams.params = params;

    return stepParams;
}

void ErosionStep::checkErosionKernelValidity(cv::Size checkSize)
{
    if(checkSize.width < 1)
    {
        kernelSize.width = 1;
    }
    if(checkSize.height < 1)
    {
        kernelSize.height = 1;
    }
}