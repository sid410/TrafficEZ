#ifndef PIPELINETRACKBAR_H
#define PIPELINETRACKBAR_H

#include "PipelineBuilder.h"

class PipelineTrackbar
{
public:
    explicit PipelineTrackbar(PipelineBuilder& builder);

private:
    struct TrackbarContext
    {
        size_t stepIndex;
        int paramId;
        PipelineTrackbar* pipelineTrackbar;
    };

    PipelineBuilder& pipelineBuilder;
    std::vector<std::unique_ptr<TrackbarContext>> trackbarContexts;

    void initializeTrackbars();

    void addTrackbar(size_t stepIndex,
                     const std::string& paramName,
                     int minValue,
                     int maxValue,
                     int paramId);

    static void onTrackbarChange(int value, void* userdata);
};

#endif
