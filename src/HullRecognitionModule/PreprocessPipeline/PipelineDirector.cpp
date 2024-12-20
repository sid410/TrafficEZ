#include "PipelineDirector.h"
#include <fstream>

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

/**
 * @brief Saves the current pipeline configuration to a YAML file.
 * @param builder Reference to a PipelineBuilder instance defining the steps.
 * @param yamlFilename The name of the YAML file to save the pipeline configuration to.
 * If the file exists, it will be overwritten; otherwise, a new file will be created.
 */
void PipelineDirector::savePipelineConfig(PipelineBuilder& builder,
                                          const std::string& yamlFilename)
{
    YAML::Node root;
    try
    {
        root = YAML::LoadFile(yamlFilename);
    }
    catch(const YAML::Exception&)
    {
        // If there's an error loading the file, create a new one
        std::cerr << "Warning: Creating a new YAML file.\n";
    }

    YAML::Node pipelineConfig = YAML::Node(YAML::NodeType::Sequence);
    size_t stepCount = builder.getNumberOfSteps();

    for(size_t i = 0; i < stepCount; ++i)
    {
        StepType type = builder.getStepType(i);
        StepParameters params = builder.getStepCurrentParameters(i);

        YAML::Node stepNode;
        stepNode["type"] = StepFactory::stepTypeToString(type);
        StepFactory::serializeStepParameters(params, stepNode);

        pipelineConfig.push_back(stepNode);
    }

    root["pipeline_config"] = pipelineConfig;

    std::ofstream fout(yamlFilename);
    if(!fout)
    {
        std::cerr << "Error writing to: " << yamlFilename << ".\n";
        return;
    }
    fout << root;
    std::cout << "Pipeline config saved to " << yamlFilename << ".\n";
}

/**
 * @brief Loads a pipeline configuration from a YAML file and applies it to the builder.
 * @param builder Reference to a PipelineBuilder instance to be updated.
 * @param yamlFilename The name of the YAML file to load the pipeline configuration from.
 */
void PipelineDirector::loadPipelineConfig(PipelineBuilder& builder,
                                          const std::string& yamlFilename)
{
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

    if(!root["pipeline_config"])
    {
        std::cerr << "Error: No pipeline_config found in " << yamlFilename
                  << ".\n";
        return;
    }

    // clear first before adding the new config.
    builder.clearAllSteps();

    YAML::Node pipelineConfig = root["pipeline_config"];
    for(const auto& stepNode : pipelineConfig)
    {
        if(!stepNode["type"])
        {
            std::cerr << "Error: A step in the pipeline_config does not have a "
                         "type.\n";
            continue;
        }

        std::string strStepType = stepNode["type"].as<std::string>();
        StepType type = StepFactory::stringToStepType(strStepType);

        YAML::Node parametersNode =
            stepNode["parameters"] ? stepNode["parameters"] : YAML::Node();
        StepParameters params =
            StepFactory::deserializeStepParameters(parametersNode, type);

        builder.addStep(type, params);
    }

    std::cout << "Pipeline config loaded from " << yamlFilename << ".\n";
}