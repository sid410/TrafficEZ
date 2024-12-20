#include "ThresholdStep.h"

ThresholdStep::ThresholdStep(int thresholdValue,
                             int maxValue,
                             int thresholdType)
    : thresholdValue(thresholdValue)
    , maxValue(maxValue)
    , thresholdType(thresholdType)
{}

void ThresholdStep::process(cv::Mat& frame) const
{
    cv::threshold(frame, frame, thresholdValue, maxValue, thresholdType);
}

void ThresholdStep::updateParameterById(int paramId, const std::any& value)
{
    switch(paramId)
    {
    case 0: // thresholdValue
        if(value.type() == typeid(int))
        {
            thresholdValue = std::any_cast<int>(value);
        }
        break;

    case 1: // maxValue
        if(value.type() == typeid(int))
        {
            maxValue = std::any_cast<int>(value);
        }
        break;

    case 2: // thresholdType
        if(value.type() == typeid(int))
        {
            thresholdType = std::any_cast<int>(value);
        }
        break;

    default:
        std::cerr << "Error: Invalid parameter ID for ThresholdStep.\n";
        break;
    }
}

void ThresholdStep::setStepParameters(const StepParameters& newParams)
{
    auto params = std::get_if<ThresholdParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr
            << "Error: Please provide a valid ThresholdParams, or check if "
               "you are using the correct builder index.\n";
        return;
    }

    thresholdValue = params->thresholdValue;
    maxValue = params->maxValue;
    thresholdType = params->thresholdType;
}

StepType ThresholdStep::getType() const
{
    return StepType::Threshold;
}

StepParameters ThresholdStep::getCurrentParameters() const
{
    ThresholdParams params;
    params.thresholdValue = thresholdValue;
    params.maxValue = maxValue;
    params.thresholdType = thresholdType;

    StepParameters stepParams;
    stepParams.params = params;

    return stepParams;
}
