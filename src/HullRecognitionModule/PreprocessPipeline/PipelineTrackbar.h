#ifndef PIPELINETRACKBAR_H
#define PIPELINETRACKBAR_H

#include "PipelineBuilder.h"

/**
 * @brief Class for controlling the parameters of each steps
 * in the Pipeline and see the projected results in realtime
 * through the builder's processDebugStack method. 
 * 
 * WARNING: This should only be used solely for debugging.
 * There will be a segmentation fault that is difficult to workaround,
 * due to OpenCV's way of implementing trackbar callbacks.
 * When this PipelineTrackbar object is destroyed, there is no way to
 * clean up dangling pointers, thus OpenCV callbacks would try to
 * access invalid memory.
 */
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

    cv::Mat displayPipelineInfo;
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
