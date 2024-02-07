#include "StepFactory.h"
#include "DilationStep.h"
#include "ErosionStep.h"
#include "GaussianBlurStep.h"
#include "GrayscaleStep.h"
#include "MOG2BackgroundSubtractionStep.h"
#include "ThresholdStep.h"

/**
 * @brief Creates and returns a concrete IPreprocessStep instance.
 * @param stepType The type of preprocessing step to create, as defined by the StepType enum.
 * @param params A struct containing the parameters for the step, if any.
 * @return A unique_ptr to an IPreprocessStep instance, or nullptr if an error occurs.
 */
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

/**
 * @brief Serializes step parameters into a YAML node for a specified step.
 * @param params The parameters of the preprocessing step to serialize.
 * @param stepNode The YAML node to serialize the parameters into.
 */
void StepFactory::serializeStepParameters(const StepParameters& params,
                                          YAML::Node& stepNode)
{
    std::visit(
        [&stepNode](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            YAML::Node paramsNode = stepNode["parameters"];

            // no parameters for grayscale
            // if constexpr(std::is_same_v<T, GrayscaleParams>)

            if constexpr(std::is_same_v<T, GaussianBlurParams>)
            {
                paramsNode["kernelSize"] = arg.kernelSize;
                paramsNode["sigma"] = arg.sigma;
            }

            else if constexpr(std::is_same_v<T,
                                             MOG2BackgroundSubtractionParams>)
            {
                paramsNode["history"] = arg.history;
                paramsNode["varThreshold"] = arg.varThreshold;
                paramsNode["varThresholdGen"] = arg.varThresholdGen;
                paramsNode["nMixtures"] = arg.nMixtures;
                paramsNode["detectShadows"] = arg.detectShadows;
                paramsNode["shadowValue"] = arg.shadowValue;
            }

            else if constexpr(std::is_same_v<T, ThresholdParams>)
            {
                paramsNode["thresholdValue"] = arg.thresholdValue;
                paramsNode["maxValue"] = arg.maxValue;
                paramsNode["thresholdType"] = arg.thresholdType;
            }

            else if constexpr(std::is_same_v<T, DilationParams>)
            {
                paramsNode["morphShape"] = arg.morphShape;
                // Assuming width == height
                paramsNode["kernelSize"] = arg.kernelSize.width;
                paramsNode["iterations"] = arg.iterations;
            }

            else if constexpr(std::is_same_v<T, ErosionParams>)
            {
                paramsNode["morphShape"] = arg.morphShape;
                // Assuming width == height
                paramsNode["kernelSize"] = arg.kernelSize.width;
                paramsNode["iterations"] = arg.iterations;
            }
        },
        params.params);
}

/**
 * @brief Deserializes step parameters from a YAML node for a specified step type.
 * @param node The YAML node containing the serialized parameters.
 * @param stepType The type of step for which parameters are being deserialized.
 * @return A StepParameters structure filled with the deserialized values.
 */
StepParameters StepFactory::deserializeStepParameters(const YAML::Node& node,
                                                      StepType stepType)
{
    StepParameters params;
    switch(stepType)
    {
    case StepType::Grayscale:
        // No parameters for Grayscale, so pass an empty struct
        params.params = GrayscaleParams{};
        break;

    case StepType::GaussianBlur: {
        GaussianBlurParams p;
        if(node["kernelSize"])
            p.kernelSize = node["kernelSize"].as<int>();
        if(node["sigma"])
            p.sigma = node["sigma"].as<double>();
        params.params = p;
        break;
    }

    case StepType::MOG2BackgroundSubtraction: {
        MOG2BackgroundSubtractionParams p;
        if(node["history"])
            p.history = node["history"].as<int>();
        if(node["varThreshold"])
            p.varThreshold = node["varThreshold"].as<double>();
        if(node["varThresholdGen"])
            p.varThresholdGen = node["varThresholdGen"].as<double>();
        if(node["nMixtures"])
            p.nMixtures = node["nMixtures"].as<int>();
        if(node["detectShadows"])
            p.detectShadows = node["detectShadows"].as<bool>();
        if(node["shadowValue"])
            p.shadowValue = node["shadowValue"].as<int>();
        params.params = p;
        break;
    }

    case StepType::Threshold: {
        ThresholdParams p;
        if(node["thresholdValue"])
            p.thresholdValue = node["thresholdValue"].as<int>();
        if(node["maxValue"])
            p.maxValue = node["maxValue"].as<int>();
        if(node["thresholdType"])
            p.thresholdType = node["thresholdType"].as<int>();
        params.params = p;
        break;
    }

    case StepType::Dilation: {
        DilationParams p;
        if(node["morphShape"])
            p.morphShape = node["morphShape"].as<int>();
        if(node["kernelSize"])
        {
            int size = node["kernelSize"].as<int>();
            p.kernelSize = cv::Size(size, size);
        }
        if(node["iterations"])
            p.iterations = node["iterations"].as<int>();
        params.params = p;
        break;
    }

    case StepType::Erosion: {
        ErosionParams p;
        if(node["morphShape"])
            p.morphShape = node["morphShape"].as<int>();
        if(node["kernelSize"])
        {
            int size = node["kernelSize"].as<int>();
            p.kernelSize = cv::Size(size, size);
        }
        if(node["iterations"])
            p.iterations = node["iterations"].as<int>();
        params.params = p;
        break;
    }

    default:
        throw std::runtime_error("Unsupported step type for deserialization.");
    }

    return params;
}

/**
 * @brief Converts a StepType enum value to its corresponding string representation.
 * @param stepType The StepType enum value to be converted.
 * @return A string representing the step type.
 */
std::string StepFactory::stepTypeToString(StepType stepType)
{
    switch(stepType)
    {
    case StepType::Grayscale:
        return "Grayscale";
    case StepType::GaussianBlur:
        return "GaussianBlur";
    case StepType::MOG2BackgroundSubtraction:
        return "MOG2BackgroundSubtraction";
    case StepType::Threshold:
        return "Threshold";
    case StepType::Erosion:
        return "Erosion";
    case StepType::Dilation:
        return "Dilation";
    default:
        return "Undefined";
    }
}

/**
 * @brief Converts a string representation of a step type to its corresponding StepType enum value.
 * @param strStepType The string representation of the step type.
 * @return The StepType enum value corresponding to the provided string. Returns StepType::Undefined
 * if the string does not match any known step type.
 */
StepType StepFactory::stringToStepType(const std::string& strStepType)
{
    if(strStepType == "Grayscale")
        return StepType::Grayscale;
    if(strStepType == "GaussianBlur")
        return StepType::GaussianBlur;
    if(strStepType == "MOG2BackgroundSubtraction")
        return StepType::MOG2BackgroundSubtraction;
    if(strStepType == "Threshold")
        return StepType::Threshold;
    if(strStepType == "Erosion")
        return StepType::Erosion;
    if(strStepType == "Dilation")
        return StepType::Dilation;

    return StepType::Undefined;
}