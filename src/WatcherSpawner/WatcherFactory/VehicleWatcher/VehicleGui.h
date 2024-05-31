#ifndef VEHICLE_GUI_H
#define VEHICLE_GUI_H

#include "Gui.h"
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

class VehicleGui : public Gui
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;

    void display() override;
    float getTrafficDensity() override;

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

    std::string streamWindow;
    std::string modelYolo;

    int laneLength;
    int laneWidth;

    void processTrackingState();
    void processSegmentationState();
};

#endif
