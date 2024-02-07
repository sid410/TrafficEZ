#include "DilationStep.h"

DilationStep::DilationStep(int morphShape, cv::Size kernelSize, int iterations)
    : morphShape(morphShape)
    , kernelSize(kernelSize)
    , iterations(iterations)
{
    checkDilationKernelValidity(kernelSize);
    dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
}

void DilationStep::process(cv::Mat& frame) const
{
    cv::dilate(frame, frame, dilateKernel, cv::Point(-1, -1), iterations);
}

void DilationStep::updateParameterById(int paramId, const std::any& value)
{
    switch(paramId)
    {
    case 0: // morphShape
        if(value.type() == typeid(int))
        {
            morphShape = std::any_cast<int>(value);

            checkDilationKernelValidity(kernelSize);
            dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
        }
        break;

    case 1: // kernelSize
        if(value.type() == typeid(int))
        {
            int size = std::any_cast<int>(value);
            kernelSize = cv::Size(size, size);

            checkDilationKernelValidity(kernelSize);
            dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
        }
        break;

    case 2: // iterations
        if(value.type() == typeid(int))
        {
            iterations = std::any_cast<int>(value);
        }
        break;

    default:
        std::cerr << "Invalid parameter ID for DilationStep.\n";
        break;
    }
}

void DilationStep::setStepParameters(const StepParameters& newParams)
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

    checkDilationKernelValidity(kernelSize);
    dilateKernel = cv::getStructuringElement(morphShape, kernelSize);
}

StepType DilationStep::getType() const
{
    return StepType::Dilation;
}

StepParameters DilationStep::getCurrentParameters() const
{
    DilationParams params;
    params.morphShape = morphShape;
    params.kernelSize = kernelSize;
    params.iterations = iterations;

    StepParameters stepParams;
    stepParams.params = params;

    return stepParams;
}

void DilationStep::checkDilationKernelValidity(cv::Size checkSize)
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