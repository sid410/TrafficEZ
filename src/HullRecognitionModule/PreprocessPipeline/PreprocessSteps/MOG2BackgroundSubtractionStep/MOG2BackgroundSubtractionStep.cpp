#include "MOG2BackgroundSubtractionStep.h"

MOG2BackgroundSubtractionStep::MOG2BackgroundSubtractionStep(
    int history,
    double varThreshold,
    double varThresholdGen,
    int nMixtures,
    bool detectShadows,
    int shadowValue)
    : history(history)
    , varThreshold(varThreshold)
    , varThresholdGen(varThresholdGen)
    , nMixtures(nMixtures)
    , detectShadows(detectShadows)
    , shadowValue(shadowValue)
{
    bgSubtractor = cv::createBackgroundSubtractorMOG2();
    bgSubtractor->setHistory(history);
    bgSubtractor->setVarThreshold(varThreshold);
    bgSubtractor->setVarThresholdGen(varThresholdGen);
    bgSubtractor->setNMixtures(nMixtures);
    bgSubtractor->setDetectShadows(detectShadows);
    bgSubtractor->setShadowValue(shadowValue);
}

void MOG2BackgroundSubtractionStep::process(cv::Mat& frame) const
{
    bgSubtractor->apply(frame, frame);
}

void MOG2BackgroundSubtractionStep::updateParameters(
    const StepParameters& newParams)
{
    auto params =
        std::get_if<MOG2BackgroundSubtractionParams>(&newParams.params);
    if(params == nullptr)
    {
        std::cerr << "Please provide a valid MOG2BackgroundSubtractionParams, "
                     "or check if you are using the correct builder index.\n";
        return;
    }

    bgSubtractor->setHistory(params->history);
    bgSubtractor->setVarThreshold(params->varThreshold);
    bgSubtractor->setVarThresholdGen(params->varThresholdGen);
    bgSubtractor->setNMixtures(params->nMixtures);
    bgSubtractor->setDetectShadows(params->detectShadows);
    bgSubtractor->setShadowValue(params->shadowValue);
}