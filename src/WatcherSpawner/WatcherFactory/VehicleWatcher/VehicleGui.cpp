#include "VehicleGui.h"
#include "FPSHelper.h"
#include "HullDetector.h"
#include "HullTracker.h"
#include "PersonSegmentationStrategy.h"
#include "PipelineBuilder.h"
#include "PipelineDirector.h"
#include "PipelineTrackbar.h"
#include "SegmentationMask.h"
#include "VehicleSegmentationStrategy.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName) const
{
    // int frameCounter = 0; // temporary, for estimating traffic flow
    const std::string modelYolo = "yolov8n-seg.onnx";

    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    FPSHelper fpsHelper;

    PipelineBuilder pipeBuilder;
    PipelineDirector pipeDirector;

    HullDetector hullDetector;
    HullTracker hullTracker;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat processFrame;

    cv::String streamWindow = streamName + " Vehicle GUI";

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationData(calibName))
        return;

    static int laneLength = videoStreamer.getLaneLength();
    static int laneWidth = videoStreamer.getLaneWidth();

    videoStreamer.constructStreamWindow(streamWindow);
    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);

    // load settings, and create trackbar
    pipeDirector.loadPipelineConfig(pipeBuilder, "debug_calib.yaml");
    // PipelineTrackbar pipeTrackbar(pipeBuilder, streamName);

    // for initialization of detector and tracker
    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);
    videoStreamer.resizeStreamWindow(warpedFrame);

    hullDetector.initDetectionBoundaries(warpedFrame);
    hullTracker.initExitBoundaryLine(hullDetector.getEndDetectionLine());

    // used for measuring the total time spent in the loop
    fpsHelper.startSample();

    // frame update loop
    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        warpedFrame.copyTo(processFrame);
        pipeBuilder.process(processFrame);
        // pipeBuilder.processDebugStack(processFrame);

        std::vector<std::vector<cv::Point>> hulls;
        hullDetector.getHulls(processFrame, hulls);

        hullTracker.update(hulls);

        // draw information on frame, only for GUI
        hullTracker.drawTrackedHulls(warpedFrame);
        hullTracker.drawLanesInfo(warpedFrame, laneLength, laneWidth);
        hullDetector.drawLengthBoundaries(warpedFrame);

        fpsHelper.avgFps();
        fpsHelper.displayFps(warpedFrame);
        cv::imshow(streamWindow, warpedFrame);

        // temporary, for estimating traffic flow
        // need a way to constantly cut to uniformly measure
        // std::cout << frameCounter++ << "\n";
        // if(frameCounter >= 1000)
        //     break;

        if(cv::waitKey(30) == 27)
            break;
    }

    std::cout << "Total time: " << fpsHelper.endSample() / 1000 << " s\n";
    std::cout << "Total Area: " << hullTracker.getTotalHullArea() << " px^2\n";
    std::cout << "Total Speed: " << hullTracker.calculateAllAveragedSpeed()
              << " px/s\n";

    // Segmentation part
    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);

    std::unique_ptr<ISegmentationStrategy> strategy =
        std::make_unique<VehicleSegmentationStrategy>();

    SegmentationMask segmentation(modelYolo, std::move(strategy));

    // choose which to display, mask only or highlight overlay
    cv::Mat mask = segmentation.generateMask(inputFrame);
    cv::Mat highlight = segmentation.processResultsDebug(inputFrame, mask);

    cv::Mat warpedMask =
        videoStreamer.applyPerspective(highlight, warpPerspective);

    cv::imshow(streamWindow, warpedMask);
    cv::waitKey(0);
}
