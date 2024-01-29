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
    if(auto params =
           std::get_if<MOG2BackgroundSubtractionParams>(&newParams.params))
    {
        bgSubtractor->setHistory(params->history);
        bgSubtractor->setVarThreshold(params->varThreshold);
        bgSubtractor->setVarThresholdGen(params->varThresholdGen);
        bgSubtractor->setNMixtures(params->nMixtures);
        bgSubtractor->setDetectShadows(params->detectShadows);
        bgSubtractor->setShadowValue(params->shadowValue);
    }
    else
    {
        std::cerr << "Please provide a valid MOG2BackgroundSubtractionParams, "
                     "or check if you are using the correct builder index.\n";
    }
}