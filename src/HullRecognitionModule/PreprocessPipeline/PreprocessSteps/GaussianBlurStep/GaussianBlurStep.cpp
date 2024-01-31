#include "GaussianBlurStep.h"

GaussianBlurStep::GaussianBlurStep(int kernelSize, double sigma)
    : kernelSize(kernelSize)
    , sigma(sigma)
{
    checkGaussianKernelValidity(kernelSize);
}

void GaussianBlurStep::process(cv::Mat& frame) const
{
    cv::GaussianBlur(frame, frame, cv::Size(kernelSize, kernelSize), sigma);
}

void GaussianBlurStep::updateParameterById(int paramId, const std::any& value)
{
    switch(paramId)
    {
    case 0: // Kernel size
        if(value.type() == typeid(int))
        {
            kernelSize = std::any_cast<int>(value);
            checkGaussianKernelValidity(kernelSize);
        }
        break;

    case 1: // Sigma
        if(value.type() == typeid(double))
        {
            sigma = std::any_cast<double>(value);
        }
        else if(value.type() == typeid(int))
        {
            sigma = static_cast<double>(std::any_cast<int>(value));
        }
        break;

    default:
        std::cerr << "Invalid parameter ID for GaussianBlurStep.\n";
        break;
    }
}

void GaussianBlurStep::setStepParameters(const StepParameters& newParams)
{
    auto params = std::get_if<GaussianBlurParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr << "Please provide a valid GaussianBlurParams, or check if "
                     "you are using the correct builder index.\n";
        return;
    }

    kernelSize = params->kernelSize;
    sigma = params->sigma;
    checkGaussianKernelValidity(kernelSize);
}

StepType GaussianBlurStep::getType() const
{
    return StepType::GaussianBlur;
}

StepParameters GaussianBlurStep::getCurrentParameters() const
{
    GaussianBlurParams params;
    params.kernelSize = kernelSize;
    params.sigma = sigma;

    StepParameters stepParams;
    stepParams.params = params;

    return stepParams;
}

/**
 * @brief Checks for the kernel size if it is odd or positive.
 * If it is even, it will add 1. If it is negative, it will set to 1.
 * @param kSize the kernel size to check.
 */
void GaussianBlurStep::checkGaussianKernelValidity(int kSize)
{
    // Check that kernelSize is odd and positive, otherwise add 1
    if(kSize % 2 == 0 && kSize >= 0)
    {
        kernelSize += 1;
    }

    // Check that kernelSize is positive, otherwise set to 1
    if(kSize < 0)
    {
        kernelSize = 1;
    }
}