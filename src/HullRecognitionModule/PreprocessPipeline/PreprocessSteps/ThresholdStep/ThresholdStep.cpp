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

void ThresholdStep::updateParameters(const StepParameters& newParams)
{
    auto params = std::get_if<ThresholdParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr << "Please provide a valid ThresholdParams, or check if "
                     "you are using the correct builder index.\n";
        return;
    }

    thresholdValue = params->thresholdValue;
    maxValue = params->maxValue;
    thresholdType = params->thresholdType;
}