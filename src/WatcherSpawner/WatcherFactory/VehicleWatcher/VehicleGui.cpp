#include "VehicleGui.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName)
{
    initialize(streamName, calibName);

    TrafficState currentTrafficState = TrafficState::GREEN_LIGHT;

    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        processTrackingState();

        if(cv::waitKey(30) == 27)
        {
            std::cout << "Total time: " << fpsHelper.endSample() / 1000
                      << " s\n";
            std::cout << "Total Area: " << hullTracker.getTotalHullArea()
                      << " px^2\n";
            std::cout << "Total Speed: "
                      << hullTracker.calculateAllAveragedSpeed() << " px/s\n";
            break;
        }
    }

    processSegmentationState();

    std::cout << "YOLO Area: " << segmentation.getTotalWhiteArea(warpedMask)
              << " px^2\n";

    cv::waitKey(0);
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

    fpsHelper.startSample();
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

    cv::imshow(streamWindow, warpedMask);
}