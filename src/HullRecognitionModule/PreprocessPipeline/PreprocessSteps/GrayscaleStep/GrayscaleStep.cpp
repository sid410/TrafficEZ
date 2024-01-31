#include "GrayscaleStep.h"

void GrayscaleStep::process(cv::Mat& frame) const
{
    if(frame.channels() == 1)
        return;
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
}

void GrayscaleStep::updateParameterById(int paramId, const std::any& value)
{
    std::cout << "No parameter to set for grayscale\n";
}

void GrayscaleStep::setStepParameters(const StepParameters& params)
{
    std::cout << "No parameters to set for grayscale\n";
}

StepType GrayscaleStep::getType() const
{
    return StepType::Grayscale;
}

StepParameters GrayscaleStep::getCurrentParameters() const
{
    StepParameters params;
    params.params = GrayscaleParams{};

    return params;
}