#ifndef STEP_FACTORY_H
#define STEP_FACTORY_H

#include "IPreprocessStep.h"
#include <yaml-cpp/yaml.h>

/**
 * @brief A factory class for creating instances of preprocessing steps.
 * 
 * The StepFactory class provides a centralized mechanism to instantiate concrete classes of
 * preprocessing steps, such as Grayscale, Gaussian Blur, etc., based on the provided StepType and
 * StepParameters. It also supports serialization and deserialization of step parameters to and from
 * YAML format, facilitating easy configuration and persistence of preprocessing pipelines.
 */
class StepFactory
{
public:
    static std::unique_ptr<IPreprocessStep>
    createStep(StepType stepType, const StepParameters& params);

    static void serializeStepParameters(const StepParameters& params,
                                        YAML::Node& stepNode);
    static StepParameters deserializeStepParameters(const YAML::Node& node,
                                                    StepType stepType);

    static std::string stepTypeToString(StepType stepType);
    static StepType stringToStepType(const std::string& strStepType);
};

#endif
