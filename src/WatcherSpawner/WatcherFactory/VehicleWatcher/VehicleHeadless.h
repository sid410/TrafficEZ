#ifndef VEHICLE_HEADLESS_H
#define VEHICLE_HEADLESS_H

#include "Headless.h"
#include <opencv2/opencv.hpp>

#include "FPSHelper.h"
#include "HullDetector.h"
#include "HullTracker.h"
#include "PipelineBuilder.h"
#include "PipelineDirector.h"
#include "SegmentationMask.h"
#include "TrafficState.h"
#include "VehicleSegmentationStrategy.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

class VehicleHeadless : public Headless
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;

    void process() override;
    float getTrafficDensity() override;
    int getInstanceCount() override;
    std::unordered_map<std::string, int> getVehicleTypeAndCount() override;
    float getAverageSpeed() override;

private:
    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    FPSHelper fpsHelper;

    PipelineBuilder pipeBuilder;
    PipelineDirector pipeDirector;

    HullDetector hullDetector;
    HullTracker hullTracker;

    SegmentationMask segmentation;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat processFrame;
    cv::Mat warpedMask;

    std::string segModel;

    int laneLength;
    int laneWidth;

    int sleepTime;

    void processTrackingState();
    void processSegmentationState();

    bool isTracking;
};

#endif
