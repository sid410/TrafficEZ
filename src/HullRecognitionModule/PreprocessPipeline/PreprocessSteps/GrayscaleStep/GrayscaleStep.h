#ifndef GRAYSCALE_STEP_H
#define GRAYSCALE_STEP_H

#include "PreprocessStep.h"

/**
 * @brief Represents a grayscale preprocessing step.
 * Implements a grayscale operation as part of the image preprocessing
 * pipeline. It extends the PreprocessStep interface.
 */
class GrayscaleStep : public PreprocessStep
{
public:
    void process(cv::Mat& frame) const override;
    void updateParameters(const StepParameters& params) override;
};

#endif