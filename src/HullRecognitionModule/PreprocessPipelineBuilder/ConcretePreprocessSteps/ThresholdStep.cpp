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