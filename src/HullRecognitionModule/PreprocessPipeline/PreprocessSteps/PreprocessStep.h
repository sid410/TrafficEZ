#ifndef PREPROCESS_STEP_H
#define PREPROCESS_STEP_H

#include "StepParameters.h"
#include "StepType.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Interface for image preprocessing steps.
 *
 * This class defines the interface for all concrete image preprocessing steps
 * in the pipeline.
 *
 * The process method applies the specific preprocessing operation to a frame,
 * and the updateParameters method allows for dynamic updating of the step's
 * parameters.
 */
class PreprocessStep
{
public:
    virtual ~PreprocessStep() {}

    /**
     * @brief Applies the preprocessing operation to the given frame.
     * @param frame The image frame to be processed.
     */
    virtual void process(cv::Mat& frame) const = 0;

    /**
     * @brief Updates the parameters of the preprocessing step.
     * @param params The new parameters for the step.
     */
    virtual void updateParameters(const StepParameters& params) = 0;
};

#endif