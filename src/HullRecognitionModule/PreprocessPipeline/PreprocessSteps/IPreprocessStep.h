#ifndef PREPROCESS_STEP_H
#define PREPROCESS_STEP_H

#include "StepParameters.h"
#include "StepType.h"
#include <any>
#include <opencv2/opencv.hpp>

/**
 * @brief Interface for image preprocessing steps.
 *
 * This class defines the interface for all concrete image preprocessing steps
 * in the pipeline.
 */
class IPreprocessStep
{
public:
    virtual ~IPreprocessStep() {}

    /**
     * @brief Applies the preprocessing operation to the given frame.
     * @param frame The image frame to be processed.
     */
    virtual void process(cv::Mat& frame) const = 0;

    /**
     * @brief Dynamically updates a specific parameter of the preprocessing step.
     * @param paramId An identifier for the parameter to update.
     * @param value The new value for the parameter, held in a std::any type.
     */
    virtual void updateParameterById(int paramId, const std::any& value) = 0;

    /**
     * @brief Sets the parameters of the preprocessing step.
     * @param params The new parameters for the step.
     */
    virtual void setStepParameters(const StepParameters& params) = 0;

    /**
     * @brief Gets the type of the preprocessing step.
     * @return The type of the step.
     */
    virtual StepType getType() const = 0;

    /**
     * @brief Retrieves the current parameters of the preprocessing step.
     * @return The current parameters of the step.
     */
    virtual StepParameters getCurrentParameters() const = 0;
};

#endif