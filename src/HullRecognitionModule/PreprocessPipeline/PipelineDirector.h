#ifndef PIPELINE_DIRECTOR_H
#define PIPELINE_DIRECTOR_H

#include "PipelineBuilder.h"

/**
 * @brief Director class to construct predefined pipeline configurations.
 */
class PipelineDirector
{
public:
    static void setupDefaultPipeline(PipelineBuilder& builder);
    void savePipelineConfig(PipelineBuilder& builder,
                            const cv::String& yamlFilename);
    void loadPipelineConfig(PipelineBuilder& builder,
                            const cv::String& yamlFilename);

private:
    std::string stepTypeToString(StepType stepType);
    StepType stringToStepType(const std::string& typeStr);
};

#endif
