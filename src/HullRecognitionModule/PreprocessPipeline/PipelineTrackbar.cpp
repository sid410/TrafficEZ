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
    trackbarContexts.clear();

    if(cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) >= 0)
    {
        cv::destroyWindow(windowName);
    }
}

void PipelineTrackbar::initializeTrackbars()
{
    size_t stepCount = pipelineBuilder.getNumberOfSteps();

    // initialize the text info of each steps in the pipeline
    int imageHeight = stepCount * 30 + 20;
    displayPipelineInfo =
        cv::Mat(imageHeight, 500, CV_8UC3, cv::Scalar(0, 0, 0));

    for(size_t i = 0; i < stepCount; ++i)
    {
        StepType type = pipelineBuilder.getStepType(i);
        StepParameters initParam = pipelineBuilder.getStepCurrentParameters(i);
        std::string stepName = std::to_string(i) + ". ";

        switch(type)
        {
        case StepType::Grayscale:
            stepName += "Grayscale";
            break;

        case StepType::GaussianBlur: {
            auto& p = std::get<GaussianBlurParams>(initParam.params);
            addTrackbar(i, "Kernel Size", 1, 31, 0, p.kernelSize);
            addTrackbar(i, "Sigma", 1, 100, 1, p.sigma);
        }
            stepName += "Gaussian Blur";
            break;

        case StepType::MOG2BackgroundSubtraction: {
            auto& p =
                std::get<MOG2BackgroundSubtractionParams>(initParam.params);
            addTrackbar(i, "History", 50, 500, 0, p.history);
            addTrackbar(i, "Var Threshold", 2, 100, 1, p.varThreshold);
            addTrackbar(i, "Var Threshold Gen", 1, 50, 2, p.varThresholdGen);
            addTrackbar(i, "NMixtures", 1, 5, 3, p.nMixtures);
            addTrackbar(i, "Detect Shadows", 0, 1, 4, p.detectShadows);
            addTrackbar(i, "Shadow Value", 0, 255, 5, p.shadowValue);
        }
            stepName += "MOG2 Background Subtraction";
            break;

        case StepType::Threshold: {
            auto& p = std::get<ThresholdParams>(initParam.params);
            addTrackbar(i, "Threshold Value", 0, 255, 0, p.thresholdValue);
            addTrackbar(i, "Max Value", 0, 255, 1, p.maxValue);
            addTrackbar(i, "Threshold Type", 0, 4, 2, p.thresholdType);
        }
            stepName += "Threshold";
            break;

        case StepType::Erosion: {
            auto& p = std::get<ErosionParams>(initParam.params);
            addTrackbar(i, "Morph Shape", 0, 2, 0, p.morphShape);
            // assuming kernel width and height is equal
            addTrackbar(i, "Kernel Size", 1, 21, 1, p.kernelSize.width);
            addTrackbar(i, "Iterations", 1, 10, 2, p.iterations);
        }
            stepName += "Erosion";
            break;

        case StepType::Dilation: {
            auto& p = std::get<DilationParams>(initParam.params);
            addTrackbar(i, "Morph Shape", 0, 2, 0, p.morphShape);
            // assuming kernel width and height is equal
            addTrackbar(i, "Kernel Size", 1, 21, 1, p.kernelSize.width);
            addTrackbar(i, "Iterations", 1, 10, 2, p.iterations);
        }
            stepName += "Dilation";
            break;
        }

        cv::putText(displayPipelineInfo,
                    stepName,
                    cv::Point(20, (i + 1) * 30),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.7,
                    cv::Scalar(0, 255, 0),
                    2);
    }

    cv::imshow(windowName, displayPipelineInfo);
}

void PipelineTrackbar::addTrackbar(size_t stepIndex,
                                   const std::string& paramName,
                                   int minValue,
                                   int maxValue,
                                   int paramId,
                                   int initialValue)
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

    cv::setTrackbarPos(trackbarName, windowName, initialValue);

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
