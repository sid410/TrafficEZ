#ifndef MOG2_BACKGROUND_SUBTRACTION_STEP_H
#define MOG2_BACKGROUND_SUBTRACTION_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents a MOG2 Background Subtraction preprocessing step.
 * Implements a MOG2 Background Subtraction operation as part of the
 * image preprocessing pipeline. It extends the PreprocessStep interface.
 */
class MOG2BackgroundSubtractionStep : public PreprocessStep
{
public:
    MOG2BackgroundSubtractionStep(int history,
                                  double varThreshold,
                                  double varThresholdGen,
                                  int nMixtures,
                                  bool detectShadows,
                                  int shadowValue);

    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& newParams) override;

private:
    cv::Ptr<cv::BackgroundSubtractorMOG2> bgSubtractor;
    int history;
    double varThreshold;
    double varThresholdGen;
    int nMixtures;
    bool detectShadows;
    int shadowValue;
};

#endif