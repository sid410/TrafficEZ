#include "PipelineDirector.h"

/**
 * @brief Sets up the default pipeline configuration.
 * @param builder Reference to a PipelineBuilder instance, which is used to
 * sequentially add the processing steps to the pipeline.
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
