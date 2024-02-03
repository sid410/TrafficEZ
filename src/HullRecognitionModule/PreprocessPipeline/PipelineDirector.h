#ifndef PIPELINE_DIRECTOR_H
#define PIPELINE_DIRECTOR_H

#include "PipelineBuilder.h"

/**
 * @brief Director class to construct/save/load pipeline configurations.
 */
class PipelineDirector
{
public:
    static void setupDefaultPipeline(PipelineBuilder& builder);

    void savePipelineConfig(PipelineBuilder& builder,
                            const cv::String& yamlFilename);
    void loadPipelineConfig(PipelineBuilder& builder,
                            const cv::String& yamlFilename);
};

#endif
