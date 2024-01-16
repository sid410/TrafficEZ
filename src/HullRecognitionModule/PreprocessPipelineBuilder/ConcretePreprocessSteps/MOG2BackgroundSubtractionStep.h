#ifndef MOG2_BACKGROUND_SUBTRACTION_STEP_H
#define MOG2_BACKGROUND_SUBTRACTION_STEP_H

#include "PreprocessStep.h"

class MOG2BackgroundSubtractionStep : public PreprocessStep
{
private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;
    int history;
    double varThreshold;
    double varThresholdGen;
    int nMixtures;
    bool detectShadows;
    int shadowValue;

public:
    MOG2BackgroundSubtractionStep(int history,
                                  double varThreshold,
                                  double varThresholdGen,
                                  int nMixtures,
                                  bool detectShadows,
                                  int shadowValue);
    void process(cv::Mat& frame) const override;
};

#endif