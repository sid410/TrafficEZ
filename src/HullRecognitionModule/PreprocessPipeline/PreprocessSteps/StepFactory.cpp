#include "StepFactory.h"
#include "DilationStep.h"
#include "ErosionStep.h"
#include "GaussianBlurStep.h"
#include "GrayscaleStep.h"
#include "MOG2BackgroundSubtractionStep.h"
#include "ThresholdStep.h"

std::unique_ptr<IPreprocessStep>
StepFactory::createStep(StepType stepType, const StepParameters& params)
{
    switch(stepType)
    {
    case StepType::Grayscale:
        return std::make_unique<GrayscaleStep>();

    case StepType::GaussianBlur: {
        if(auto p = std::get_if<GaussianBlurParams>(&params.params))
        {
            return std::make_unique<GaussianBlurStep>(p->kernelSize, p->sigma);
        }
        break;
    }

    case StepType::MOG2BackgroundSubtraction: {
        if(auto p =
               std::get_if<MOG2BackgroundSubtractionParams>(&params.params))
        {
            return std::make_unique<MOG2BackgroundSubtractionStep>(
                p->history,
                p->varThreshold,
                p->varThresholdGen,
                p->nMixtures,
                p->detectShadows,
                p->shadowValue);
        }
        break;
    }

    case StepType::Threshold: {
        if(auto p = std::get_if<ThresholdParams>(&params.params))
        {
            return std::make_unique<ThresholdStep>(
                p->thresholdValue, p->maxValue, p->thresholdType);
        }
        break;
    }

    case StepType::Dilation: {
        if(auto p = std::get_if<DilationParams>(&params.params))
        {
            return std::make_unique<DilationStep>(
                p->morphShape, p->kernelSize, p->iterations);
        }
        break;
    }

    case StepType::Erosion: {
        if(auto p = std::get_if<ErosionParams>(&params.params))
        {
            return std::make_unique<ErosionStep>(
                p->morphShape, p->kernelSize, p->iterations);
        }
        break;
    }

    default:
        std::cerr << "Unsupported step type provided.\n";
        return nullptr;
    }
    return nullptr;
}
