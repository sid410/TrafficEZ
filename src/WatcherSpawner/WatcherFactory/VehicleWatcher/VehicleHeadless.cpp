#include "VehicleHeadless.h"
#include <chrono>
#include <thread>

void VehicleHeadless::initialize(const std::string& streamName,
                                 const std::string& calibName)
{
    if(!videoStreamer.openVideoStream(streamName) ||
       !videoStreamer.readCalibrationData(calibName))
    {
        std::cerr << "Error: Failed to initialize video stream or calibration "
                     "data.\n";
        return;
    }

    laneLength = videoStreamer.getLaneLength();
    laneWidth = videoStreamer.getLaneWidth();
    segModel = videoStreamer.getSegModel();
    sleepTime = 1000 / videoStreamer.getFPS();

    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);
    pipeDirector.loadPipelineConfig(pipeBuilder, calibName);

    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);

    hullDetector.initDetectionBoundaries(warpedFrame);
    hullTracker.initExitBoundaryLine(hullDetector.getEndDetectionLine());

    std::unique_ptr<ISegmentationStrategy> strategy =
        std::make_unique<VehicleSegmentationStrategy>();
    segmentation.initializeModel(segModel, std::move(strategy));

    isTracking = false;
}

void VehicleHeadless::process()
{
    if(!videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
        return;

    if(!isTracking)
    {
        fpsHelper.startSample();
        isTracking = true;
    }

    (currentTrafficState == TrafficState::GREEN_PHASE)
        ? processTrackingState()
        : processSegmentationState();
}

float VehicleHeadless::getTrafficDensity()
{
    float density = 0;

    if(currentTrafficState == TrafficState::GREEN_PHASE)
    {
        float totalTime = fpsHelper.endSample() / 1000;
        float flow = hullTracker.getTotalHullArea() / totalTime;
        float average = hullTracker.getAveragedSpeed();

        density = (flow == 0) ? 0 : flow / (average * laneWidth);

        hullTracker.resetTrackerVariables();
    }

    else if(currentTrafficState == TrafficState::RED_PHASE)
    {
        float totalArea = segmentation.getWhiteArea(warpedMask);
        density = totalArea / (laneLength * laneWidth);
    }

    isTracking = false;

    return density;
}

int VehicleHeadless::getInstanceCount()
{
    int count = 0;

    if(currentTrafficState == TrafficState::GREEN_PHASE)
    {
        count = hullTracker.getHullCount();
    }

    else if(currentTrafficState == TrafficState::RED_PHASE)
    {
        count = segmentation.getDetectionResultSize();
    }

    return count;
}

void VehicleHeadless::processTrackingState()
{
    warpedFrame.copyTo(processFrame);
    pipeBuilder.process(processFrame);

    std::vector<std::vector<cv::Point>> hulls;
    hullDetector.getHulls(processFrame, hulls);
    hullTracker.update(hulls);

    std::this_thread::sleep_for(std::chrono::milliseconds(sleepTime));
}

void VehicleHeadless::processSegmentationState()
{
    cv::Mat segMask = segmentation.generateMask(inputFrame);
    // still need to update warpedMask for getWhiteArea method
    warpedMask = videoStreamer.applyPerspective(segMask, warpPerspective);
}

std::unordered_map<std::string, int> VehicleHeadless::getVehicleTypeAndCount()
{
    if(currentTrafficState == TrafficState::GREEN_PHASE)
    {
        int hullCount = hullTracker.getHullCount();
        if(hullCount < 1)
        {
            return {};
        }
        return {{"unknown", hullCount}};
    }
    else if(currentTrafficState == TrafficState::RED_PHASE)
    {
        return segmentation.getClassTypeAndCounts();
    }

    return {};
}

float VehicleHeadless::getAverageSpeed()
{
    float avgSpeed = 0.0f;

    if(currentTrafficState == TrafficState::GREEN_PHASE)
    {
        avgSpeed = hullTracker.getAveragedSpeed();
    }

    // Default speed when not in GREEN_PHASE
    return avgSpeed;
}