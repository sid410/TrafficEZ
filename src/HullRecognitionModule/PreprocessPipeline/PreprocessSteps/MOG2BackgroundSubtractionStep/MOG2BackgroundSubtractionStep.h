#ifndef MOG2_BACKGROUND_SUBTRACTION_STEP_H
#define MOG2_BACKGROUND_SUBTRACTION_STEP_H

#include "IPreprocessStep.h"

/**
 * @brief Represents a MOG2 Background Subtraction preprocessing step.
 * Implements a MOG2 Background Subtraction operation as part of the
 * image preprocessing pipeline. It extends the IPreprocessStep interface.
 */
class MOG2BackgroundSubtractionStep : public IPreprocessStep
{
public:
    MOG2BackgroundSubtractionStep(int history,
                                  double varThreshold,
                                  double varThresholdGen,
                                  int nMixtures,
                                  bool detectShadows,
                                  int shadowValue);

    void process(cv::Mat& frame) const override;

    void updateParameterById(int paramId, const std::any& value) override;
    void setStepParameters(const StepParameters& newParams) override;

    StepType getType() const override;
    StepParameters getCurrentParameters() const override;

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