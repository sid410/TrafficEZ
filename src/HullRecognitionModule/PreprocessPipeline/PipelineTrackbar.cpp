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
    std::cout << "Destroyed Trackbar.\n";
}

/**
 * @brief This method dynamically creates a trackbar for each preprocessing step defined in
 * the associated `PipelineBuilder` instance. It generates a visual representation of
 * the processing pipeline, displaying step names and creating trackbars for their
 * adjustable parameters.
 */
void PipelineTrackbar::initializeTrackbars()
{
    size_t stepCount = pipelineBuilder.getNumberOfSteps();
    std::cout << "Initialized trackbar with step: " << stepCount << "\n";

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

        case StepType::GaussianBlur: { // encapsulated to limit scope
            auto& p = std::get<GaussianBlurParams>(initParam.params);
            addTrackbar(i, "Kernel Size", 31, 0, p.kernelSize);
            addTrackbar(i, "Sigma", 100, 1, p.sigma);
        }
            stepName += "Gaussian Blur";
            break;

        case StepType::MOG2BackgroundSubtraction: {
            auto& p =
                std::get<MOG2BackgroundSubtractionParams>(initParam.params);
            addTrackbar(i, "History", 500, 0, p.history);
            addTrackbar(i, "Var Threshold", 100, 1, p.varThreshold);
            addTrackbar(i, "Var Threshold Gen", 50, 2, p.varThresholdGen);
            addTrackbar(i, "NMixtures", 5, 3, p.nMixtures);
            addTrackbar(i, "Detect Shadows", 1, 4, p.detectShadows);
            addTrackbar(i, "Shadow Value", 255, 5, p.shadowValue);
        }
            stepName += "MOG2 Background Subtraction";
            break;

        case StepType::Threshold: {
            auto& p = std::get<ThresholdParams>(initParam.params);
            addTrackbar(i, "Threshold Value", 255, 0, p.thresholdValue);
            addTrackbar(i, "Max Value", 255, 1, p.maxValue);
            addTrackbar(i, "Threshold Type", 4, 2, p.thresholdType);
        }
            stepName += "Threshold";
            break;

        case StepType::Erosion: { // Assuming kernelSize width==height
            auto& p = std::get<ErosionParams>(initParam.params);
            addTrackbar(i, "Morph Shape", 2, 0, p.morphShape);
            addTrackbar(i, "Kernel Size", 21, 1, p.kernelSize.width);
            addTrackbar(i, "Iterations", 10, 2, p.iterations);
        }
            stepName += "Erosion";
            break;

        case StepType::Dilation: { // Assuming kernelSize width==height
            auto& p = std::get<DilationParams>(initParam.params);
            addTrackbar(i, "Morph Shape", 2, 0, p.morphShape);
            addTrackbar(i, "Kernel Size", 21, 1, p.kernelSize.width);
            addTrackbar(i, "Iterations", 10, 2, p.iterations);
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
                    1);
    }

    cv::imshow(windowName, displayPipelineInfo);
}

/**
 * @brief Adds a trackbar to the window for controlling a parameter of a preprocessing step.
 * Each trackbar is associated with a unique `TrackbarContext` to
 * handle adjustments made through the trackbar's interface.
 * @param stepIndex The index of the preprocessing step in the pipeline, used to identify
 * which step the trackbar controls.
 * @param paramName The name of the parameter this trackbar adjusts.
 * @param maxValue The maximum value the trackbar can adjust the parameter to.
 * @param paramId An identifier for the parameter within the step, used in callback
 * functions to apply changes.
 * @param initialValue The initial value for the parameter, set when the trackbar is created.
 */
void PipelineTrackbar::addTrackbar(size_t stepIndex,
                                   const std::string& paramName,
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

/**
 * @brief Callback function for trackbar adjustments.
 * @param value The new value from the trackbar, representing the updated parameter value.
 * @param userdata A pointer to user data provided when the trackbar was created. This
 * should be cast to a `TrackbarContext*` to access `stepIndex` and `paramId`.
 */
void PipelineTrackbar::onTrackbarChange(int value, void* userdata)
{
    auto* context = static_cast<TrackbarContext*>(userdata);
    if(context && context->pipelineTrackbar)
    {
        context->pipelineTrackbar->pipelineBuilder.updateStepParameterById(
            context->stepIndex, context->paramId, value);
    }
}
