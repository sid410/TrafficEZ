#include "VehicleGui.h"
#include <iostream>
#include <unistd.h>

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName)
{
    initialize(streamName, calibName);

    currentTrafficState = TrafficState::GREEN_PHASE;

    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        if(currentTrafficState == TrafficState::GREEN_PHASE)
        {
            processTrackingState();
        }
        else if(currentTrafficState == TrafficState::RED_PHASE)
        {
            processSegmentationState();
            // cv::waitKey(0);
        }

        int key = cv::waitKey(1);

        if(key == 27)
        {
            std::cout << "Total time: " << fpsHelper.endSample() / 1000
                      << " s\n";
            std::cout << "Total Area: " << hullTracker.getTotalHullArea()
                      << " px^2\n";
            std::cout << "Total Speed: "
                      << hullTracker.calculateAllAveragedSpeed() << " px/s\n";

            sendDataToParent();

            break;
        }

        // '1' button
        else if(key == 49)
        {
            if(currentTrafficState == TrafficState::GREEN_PHASE)
                setCurrentTrafficState(TrafficState::RED_PHASE);
            else
                setCurrentTrafficState(TrafficState::GREEN_PHASE);
        }
    }

    std::cout << "YOLO Area: " << segmentation.getTotalWhiteArea(warpedMask)
              << " px^2\n";
}

void VehicleGui::sendDataToParent()
{
    std::string data =
        "Total Area: " + std::to_string(hullTracker.getTotalHullArea()) +
        " px^2, Total Speed: " +
        std::to_string(hullTracker.calculateAllAveragedSpeed()) + " px/s\n";

    write(writePipe, data.c_str(), data.size() + 1);
}

void VehicleGui::setCurrentTrafficState(TrafficState state)
{
    currentTrafficState = state;
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

    fpsHelper.avgFps();
    fpsHelper.displayFps(warpedMask);

    cv::imshow(streamWindow, warpedMask);
}