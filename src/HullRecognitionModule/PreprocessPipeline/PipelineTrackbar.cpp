#include "PipelineTrackbar.h"
#include <memory>
#include <vector>

PipelineTrackbar::PipelineTrackbar(PipelineBuilder& builder,
                                   const std::string& streamName)
    : pipelineBuilder(builder)
    , windowName(streamName + " Trackbars")
{
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);
    initializeTrackbars();
}

PipelineTrackbar::~PipelineTrackbar()
{
    if(cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) >= 0)
    {
        cv::destroyWindow(windowName);
    }
}

void PipelineTrackbar::initializeTrackbars()
{
    size_t stepCount = pipelineBuilder.getNumberOfSteps();
    for(size_t i = 0; i < stepCount; ++i)
    {
        StepType type = pipelineBuilder.getStepType(i);
        switch(type)
        {
        case StepType::GaussianBlur:
            addTrackbar(i, "Kernel Size", 1, 31, 0);
            addTrackbar(i, "Sigma", 1, 100, 1);
            break;

        case StepType::MOG2BackgroundSubtraction:
            addTrackbar(i, "History", 50, 500, 0);
            addTrackbar(i, "Var Threshold", 2, 100, 1);
            addTrackbar(i, "Var Threshold Gen", 1, 50, 2);
            addTrackbar(i, "NMixtures", 1, 5, 3);
            addTrackbar(i, "Detect Shadows", 0, 1, 4);
            addTrackbar(i, "Shadow Value", 0, 255, 5);
            break;

        case StepType::Threshold:
            addTrackbar(i, "Threshold Value", 0, 255, 0);
            addTrackbar(i, "Max Value", 0, 255, 1);
            addTrackbar(i, "Threshold Type", 0, 4, 2);
            break;

        case StepType::Erosion:
            addTrackbar(i, "Morph Shape", 0, 2, 0);
            addTrackbar(i, "Kernel Size", 1, 21, 1);
            addTrackbar(i, "Iterations", 1, 10, 2);
            break;

        case StepType::Dilation:
            addTrackbar(i, "Morph Shape", 0, 2, 0);
            addTrackbar(i, "Kernel Size", 1, 21, 1);
            addTrackbar(i, "Iterations", 1, 10, 2);
            break;
        }
    }
}

void PipelineTrackbar::addTrackbar(size_t stepIndex,
                                   const std::string& paramName,
                                   int minValue,
                                   int maxValue,
                                   int paramId)
{
    std::string trackbarName =
        "Step " + std::to_string(stepIndex) + " " + paramName;

    auto context = std::make_unique<TrackbarContext>(
        TrackbarContext{stepIndex, paramId, this});

    cv::createTrackbar(trackbarName,
                       windowName,
                       nullptr,
                       maxValue,
                       &PipelineTrackbar::onTrackbarChange,
                       context.get());

    // be careful with the lifetime of this callback context
    trackbarContexts.push_back(std::move(context));
}

void PipelineTrackbar::onTrackbarChange(int value, void* userdata)
{
    auto* context = static_cast<TrackbarContext*>(userdata);
    if(context && context->pipelineTrackbar)
    {
        context->pipelineTrackbar->pipelineBuilder.updateStepParameterById(
            context->stepIndex, context->paramId, value);
    }
}
