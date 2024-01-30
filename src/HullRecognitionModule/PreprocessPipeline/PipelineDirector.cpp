#include "PipelineDirector.h"

/**
     * @brief Sets up the default pipeline configuration.
     *
     * Default pipeline is as follows: Grayscale -> GaussianBlur
     * -> MOG2BackgroundSubtraction -> Threshold -> Dilation -> Erosion
     *
     * @param builder Reference to a PipelineBuilder instance.
     */
void PipelineDirector::setupDefaultPipeline(PipelineBuilder& builder)
{
    builder.addStep(StepType::Grayscale, StepParameters{GrayscaleParams{}})
        .addStep(StepType::GaussianBlur, StepParameters{GaussianBlurParams{}})
        .addStep(StepType::MOG2BackgroundSubtraction,
                 StepParameters{MOG2BackgroundSubtractionParams{}})
        .addStep(StepType::Threshold, StepParameters{ThresholdParams{}})
        .addStep(StepType::Dilation, StepParameters{DilationParams{}})
        .addStep(StepType::Erosion, StepParameters{ErosionParams{}});
}
