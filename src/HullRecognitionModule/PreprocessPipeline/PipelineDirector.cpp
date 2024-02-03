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

void PipelineDirector::loadPipelineConfig(PipelineBuilder& builder,
                                          const std::string& yamlFilename)
{
    // Clear any existing steps in the builder
    builder.clearAllSteps();

    // Load the YAML file
    YAML::Node root;
    try
    {
        root = YAML::LoadFile(yamlFilename);
    }
    catch(const YAML::Exception& ex)
    {
        std::cerr << "Error loading YAML file '" << yamlFilename
                  << "': " << ex.what() << "\n";
        return;
    }

    // Check if the pipeline_config section exists
    if(!root["pipeline_config"])
    {
        std::cerr << "No pipeline_config found in " << yamlFilename << ".\n";
        return;
    }

    YAML::Node pipelineConfig = root["pipeline_config"];
    for(auto stepNode : pipelineConfig)
    {
        // Parse the type and parameters of each step
        if(!stepNode["type"])
        {
            std::cerr
                << "A step in the pipeline_config does not have a type.\n";
            continue;
        }
        std::string typeStr = stepNode["type"].as<std::string>();
        StepType type = stringToStepType(
            typeStr); // Implement this function to convert string back to StepType

        StepParameters
            params; // You'll need to properly initialize this based on the step type

        if(stepNode["parameters"])
        {
            YAML::Node parametersNode = stepNode["parameters"];
            // Based on the type, populate the params variable appropriately
            switch(type)
            {
            case StepType::GaussianBlur: {
                GaussianBlurParams p;
                p.kernelSize = parametersNode["kernelSize"].as<int>();
                p.sigma = parametersNode["sigma"].as<double>();
                params.params = p;
            }
            break;

            case StepType::MOG2BackgroundSubtraction: {
                MOG2BackgroundSubtractionParams p;
                p.history = parametersNode["history"].as<int>();
                p.varThreshold = parametersNode["varThreshold"].as<double>();
                p.varThresholdGen =
                    parametersNode["varThresholdGen"].as<double>();
                p.nMixtures = parametersNode["nMixtures"].as<int>();
                p.detectShadows = parametersNode["detectShadows"].as<bool>();
                p.shadowValue = parametersNode["shadowValue"].as<int>();
                params.params = p;
            }
            break;

            case StepType::Threshold: {
                ThresholdParams p;
                p.thresholdValue =
                    parametersNode["thresholdValue"].as<double>();
                p.maxValue = parametersNode["maxValue"].as<double>();
                p.thresholdType = parametersNode["thresholdType"].as<int>();
                params.params = p;
            }
            break;

            case StepType::Erosion: {
                ErosionParams p;
                p.morphShape = parametersNode["morphShape"].as<int>();
                p.kernelSize = cv::Size(parametersNode["kernelSize"].as<int>(),
                                        parametersNode["kernelSize"].as<int>());
                p.iterations = parametersNode["iterations"].as<int>();
                params.params = p;
            }
            break;

            case StepType::Dilation: {
                DilationParams p;
                p.morphShape = parametersNode["morphShape"].as<int>();
                p.kernelSize = cv::Size(parametersNode["kernelSize"].as<int>(),
                                        parametersNode["kernelSize"].as<int>());
                p.iterations = parametersNode["iterations"].as<int>();
                params.params = p;
            }
            break;

            default:
                std::cerr << "Unsupported or unknown step type encountered.\n";
                break;
            }
        }

        // Add the step to the builder
        builder.addStep(
            type,
            params); // Ensure addStep can handle StepParameters appropriately
    }

    std::cout << "Pipeline config loaded from " << yamlFilename << ".\n";
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

StepType PipelineDirector::stringToStepType(const std::string& strType)
{
    if(strType == "Grayscale")
        return StepType::Grayscale;
    if(strType == "GaussianBlur")
        return StepType::GaussianBlur;
    if(strType == "MOG2BackgroundSubtraction")
        return StepType::MOG2BackgroundSubtraction;
    if(strType == "Threshold")
        return StepType::Threshold;
    if(strType == "Erosion")
        return StepType::Erosion;
    if(strType == "Dilation")
        return StepType::Dilation;
    // Add other types
    return StepType::Undefined;
}