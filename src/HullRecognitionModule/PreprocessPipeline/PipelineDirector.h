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
};

#endif
