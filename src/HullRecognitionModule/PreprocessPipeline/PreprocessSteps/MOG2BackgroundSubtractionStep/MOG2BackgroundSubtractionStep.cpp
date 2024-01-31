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

void MOG2BackgroundSubtractionStep::updateParameterById(int paramId,
                                                        const std::any& value)
{
    switch(paramId)
    {
    case 0: // history
        if(value.type() == typeid(int))
        {
            history = std::any_cast<int>(value);
        }
        bgSubtractor->setHistory(history);
        break;

    case 1: // varThreshold
        if(value.type() == typeid(double))
        {
            varThreshold = std::any_cast<double>(value);
        }
        else if(value.type() == typeid(int))
        {
            varThreshold = static_cast<double>(std::any_cast<int>(value));
        }
        bgSubtractor->setVarThreshold(varThreshold);
        break;

    case 2: // varThresholdGen
        if(value.type() == typeid(double))
        {
            varThresholdGen = std::any_cast<double>(value);
        }
        else if(value.type() == typeid(int))
        {
            varThresholdGen = static_cast<double>(std::any_cast<int>(value));
        }
        bgSubtractor->setVarThresholdGen(varThresholdGen);
        break;

    case 3: // nMixtures
        if(value.type() == typeid(int))
        {
            nMixtures = std::any_cast<int>(value);
        }
        bgSubtractor->setNMixtures(nMixtures);
        break;

    case 4: // detectShadows
        if(value.type() == typeid(bool))
        {
            detectShadows = std::any_cast<bool>(value);
        }
        // Convert to bool; 0 is false, non-zero is true
        else if(value.type() == typeid(int))
        {
            int intValue = std::any_cast<int>(value);
            detectShadows = (intValue == 1)   ? true
                            : (intValue == 0) ? false
                                              : detectShadows;
        }
        bgSubtractor->setDetectShadows(detectShadows);
        break;

    case 5: // shadowValue
        if(value.type() == typeid(int))
        {
            shadowValue = std::any_cast<int>(value);
        }
        bgSubtractor->setShadowValue(shadowValue);
        break;

    default:
        std::cerr
            << "Invalid parameter ID for MOG2BackgroundSubtractionStep.\n";
        break;
    }
}

void MOG2BackgroundSubtractionStep::setStepParameters(
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

StepType MOG2BackgroundSubtractionStep::getType() const
{
    return StepType::MOG2BackgroundSubtraction;
}

StepParameters MOG2BackgroundSubtractionStep::getCurrentParameters() const
{
    MOG2BackgroundSubtractionParams params;
    params.history = history;
    params.varThreshold = varThreshold;
    params.varThresholdGen = varThresholdGen;
    params.nMixtures = nMixtures;
    params.detectShadows = detectShadows;
    params.shadowValue = shadowValue;

    StepParameters stepParams;
    stepParams.params = params;

    return stepParams;
}
