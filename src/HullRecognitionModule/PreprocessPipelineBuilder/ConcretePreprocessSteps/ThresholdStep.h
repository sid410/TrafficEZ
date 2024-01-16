#ifndef THRESHOLD_STEP_H
#define THRESHOLD_STEP_H

#include "PreprocessStep.h"

class ThresholdStep : public PreprocessStep
{
private:
    int thresholdValue;
    int maxValue;
    int thresholdType;

public:
    ThresholdStep(int thresholdValue, int maxValue, int thresholdType);
    void process(cv::Mat& frame) const override;
};

#endif