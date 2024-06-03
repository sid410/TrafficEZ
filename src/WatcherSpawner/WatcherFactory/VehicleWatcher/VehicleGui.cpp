#include "VehicleGui.h"

void VehicleGui::display()
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

    cv::waitKey(1); // needed for imshow
}

float VehicleGui::getTrafficDensity()
{
    float totalTime = fpsHelper.endSample() / 1000;
    float flow = hullTracker.getTotalHullArea() / totalTime;

    float density = flow / (hullTracker.getAveragedSpeed() * laneWidth);

    hullTracker.resetTrackerVariables();
    isTracking = false;

    // std::cout << "YOLO Area: " << segmentation.getTotalWhiteArea(warpedMask)
    //           << " px^2\n";

    return density;
}

void VehicleGui::initialize(const std::string& streamName,
                            const std::string& calibName)
{
    if(!videoStreamer.openVideoStream(streamName) ||
       !videoStreamer.readCalibrationData(calibName))
    {
        std::cerr << "Failed to initialize video stream or calibration data.\n";
        return;
    }

    streamWindow = streamName + " Vehicle GUI";

    modelYolo = "yolov8n-seg.onnx";

    laneLength = videoStreamer.getLaneLength();
    laneWidth = videoStreamer.getLaneWidth();

    videoStreamer.constructStreamWindow(streamWindow);
    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);

    pipeDirector.loadPipelineConfig(pipeBuilder, "debug_calib.yaml");

    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);
    videoStreamer.resizeStreamWindow(warpedFrame);

    hullDetector.initDetectionBoundaries(warpedFrame);
    hullTracker.initExitBoundaryLine(hullDetector.getEndDetectionLine());

    std::unique_ptr<ISegmentationStrategy> strategy =
        std::make_unique<VehicleSegmentationStrategy>();
    segmentation.initializeModel(modelYolo, std::move(strategy));

    isTracking = false;
}

void VehicleGui::processTrackingState()
{
    warpedFrame.copyTo(processFrame);
    pipeBuilder.process(processFrame);

    std::vector<std::vector<cv::Point>> hulls;
    hullDetector.getHulls(processFrame, hulls);
    hullTracker.update(hulls);

    hullTracker.drawTrackedHulls(warpedFrame);
    hullTracker.drawLanesInfo(warpedFrame, laneLength, laneWidth);
    hullDetector.drawLengthBoundaries(warpedFrame);

    fpsHelper.avgFps();
    fpsHelper.displayFps(warpedFrame);

    cv::imshow(streamWindow, warpedFrame);
}

void VehicleGui::processSegmentationState()
{
    cv::Mat segMask = segmentation.generateMask(inputFrame);
    warpedMask = videoStreamer.applyPerspective(segMask, warpPerspective);

    fpsHelper.avgFps();
    fpsHelper.displayFps(warpedMask);

    cv::imshow(streamWindow, warpedMask);
}