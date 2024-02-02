#ifndef PIPELINETRACKBAR_H
#define PIPELINETRACKBAR_H

#include "PipelineBuilder.h"

class PipelineTrackbar
{
public:
    explicit PipelineTrackbar(PipelineBuilder& builder,
                              const std::string& streamName);
    ~PipelineTrackbar();

private:
    struct TrackbarContext
    {
        size_t stepIndex;
        int paramId;
        PipelineTrackbar* pipelineTrackbar;
    };
    std::vector<std::unique_ptr<TrackbarContext>> trackbarContexts;

    std::string windowName;
    PipelineBuilder& pipelineBuilder;

    void initializeTrackbars();

    void addTrackbar(size_t stepIndex,
                     const std::string& paramName,
                     int minValue,
                     int maxValue,
                     int paramId,
                     int initialValue);

    static void onTrackbarChange(int value, void* userdata);
};

#endif
