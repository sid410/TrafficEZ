#ifndef STEP_TYPE_H
#define STEP_TYPE_H

/**
 * @brief Enum class representing different types of preprocessing steps.
 *
 * This enum is used to specify the type of preprocessing step to be added
 * to the image processing pipeline.
 */
enum class StepType
{
    Grayscale,
    GaussianBlur,
    MOG2BackgroundSubtraction,
    Threshold,
    Dilation,
    Erosion
};

#endif
