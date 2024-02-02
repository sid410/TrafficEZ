#include "PipelineDirector.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

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

void PipelineDirector::savePipelineConfig(PipelineBuilder& builder,
                                          const std::string& yamlFilename)
{
    // We need to load this first to not overwrite other sub-nodes
    YAML::Node root;

    try
    {
        root = YAML::LoadFile(yamlFilename);
    }
    catch(const YAML::Exception& ex)
    {
        std::cerr << "Warning: Failed to load existing YAML file '"
                  << yamlFilename << "': " << ex.what() << "\n";
        // If there's an error loading the file, we'll just create a new one
    }

    YAML::Node pipelineConfig = YAML::Node(YAML::NodeType::Sequence);
    size_t stepCount = builder.getNumberOfSteps();

    if(stepCount < 1)
    {
        std::cerr << "There are no steps added.\n";
        return;
    }

    for(size_t i = 0; i < stepCount; ++i)
    {
        StepType type = builder.getStepType(i);
        StepParameters params = builder.getStepCurrentParameters(i);

        YAML::Node stepNode = YAML::Node(YAML::NodeType::Map);
        stepNode["type"] = stepTypeToString(type);

        switch(type)
        {
        case StepType::Grayscale:
            break;

        case StepType::GaussianBlur: {
            auto& p = std::get<GaussianBlurParams>(params.params);
            stepNode["parameters"] = YAML::Node(YAML::NodeType::Map);
            stepNode["parameters"]["kernelSize"] = p.kernelSize;
            stepNode["parameters"]["sigma"] = p.sigma;
        }
        break;

        case StepType::MOG2BackgroundSubtraction: {
            auto& p = std::get<MOG2BackgroundSubtractionParams>(params.params);
            stepNode["parameters"] = YAML::Node(YAML::NodeType::Map);
            stepNode["parameters"]["history"] = p.history;
            stepNode["parameters"]["varThreshold"] = p.varThreshold;
            stepNode["parameters"]["varThresholdGen"] = p.varThresholdGen;
            stepNode["parameters"]["nMixtures"] = p.nMixtures;
            stepNode["parameters"]["detectShadows"] = p.detectShadows;
            stepNode["parameters"]["shadowValue"] = p.shadowValue;
        }
        break;

        case StepType::Threshold: {
            auto& p = std::get<ThresholdParams>(params.params);
            stepNode["parameters"] = YAML::Node(YAML::NodeType::Map);
            stepNode["parameters"]["thresholdValue"] = p.thresholdValue;
            stepNode["parameters"]["maxValue"] = p.maxValue;
            stepNode["parameters"]["thresholdType"] = p.thresholdType;
        }
        break;

        case StepType::Erosion: { // Assuming kernelSize width==height
            auto& p = std::get<ErosionParams>(params.params);
            stepNode["parameters"] = YAML::Node(YAML::NodeType::Map);
            stepNode["parameters"]["morphShape"] = p.morphShape;
            stepNode["parameters"]["kernelSize"] = p.kernelSize.width;
            stepNode["parameters"]["iterations"] = p.iterations;
        }
        break;

        case StepType::Dilation: { // Assuming kernelSize width==height
            auto& p = std::get<DilationParams>(params.params);
            stepNode["parameters"] = YAML::Node(YAML::NodeType::Map);
            stepNode["parameters"]["morphShape"] = p.morphShape;
            stepNode["parameters"]["kernelSize"] = p.kernelSize.width;
            stepNode["parameters"]["iterations"] = p.iterations;
        }
        break;

        default:
            std::cerr << "Unsupported step type encountered.\n";
            break;
        }

        pipelineConfig.push_back(stepNode);
    }

    // Update the root node with the new or modified pipeline_config section
    root["pipeline_config"] = pipelineConfig;

    // Write the updated document back to the file
    std::ofstream fout(yamlFilename);
    if(!fout)
    {
        std::cerr << "Error writing on : " << yamlFilename << ".\n";
        return;
    }

    fout << root;
    std::cout << "Pipeline config saved to " << yamlFilename << ".\n";
}

std::string PipelineDirector::stepTypeToString(StepType stepType)
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
