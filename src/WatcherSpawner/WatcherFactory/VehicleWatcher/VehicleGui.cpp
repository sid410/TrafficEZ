#include "VehicleGui.h"
#include "FPSHelper.h"
#include "HullDetector.h"
#include "HullTracker.h"
#include "PipelineBuilder.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName) const
{
    int frameCounter = 0; // temporary, for estimating traffic flow

    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    FPSHelper fpsHelper;

    PipelineBuilder pipeBuilder;
    HullDetector hullDetector;
    HullTracker hullTracker;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat processFrame;

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationData(calibName))
        return;

    static int laneLength = videoStreamer.getLaneLength();
    static int laneWidth = videoStreamer.getLaneWidth();

    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);

    pipeBuilder.addStep(StepType::Grayscale, StepParameters{GrayscaleParams{}})
        .addStep(StepType::GaussianBlur, StepParameters{GaussianBlurParams{}})
        .addStep(StepType::MOG2BackgroundSubtraction,
                 StepParameters{MOG2BackgroundSubtractionParams{}})
        .addStep(StepType::Threshold, StepParameters{ThresholdParams{}})
        .addStep(StepType::Dilation, StepParameters{DilationParams{}})
        .addStep(StepType::Erosion, StepParameters{ErosionParams{}});

    // for initialization of detector and tracker
    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);
    hullDetector.initDetectionBoundaries(warpedFrame);
    hullTracker.initBoundaryLine(hullDetector.getEndDetectionLine());

    // used for measuring the total time spent in the loop
    fpsHelper.startSample();

    // frame update loop
    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        warpedFrame.copyTo(processFrame);
        // pipeBuilder.process(processFrame);
        pipeBuilder.processDebugStack(processFrame);

        std::vector<std::vector<cv::Point>> hulls;
        hullDetector.getHulls(processFrame, hulls);

        hullTracker.update(hulls);

        // draw information on frame, only for GUI
        hullTracker.drawTrackedHulls(warpedFrame);
        hullTracker.drawLanesInfo(warpedFrame, laneLength, laneWidth);
        hullDetector.drawLengthBoundaries(warpedFrame);

        fpsHelper.avgFps();
        fpsHelper.displayFps(warpedFrame);
        cv::imshow("Vehicle Gui", warpedFrame);

        // temporary, for estimating traffic flow
        // need a way to constantly cut to uniformly measure
        frameCounter++;
        if(frameCounter >= 1000)
            break;

        if(cv::waitKey(30) == 27)
            break;
    }

    std::cout << "Total time: " << fpsHelper.endSample() / 1000 << " s\n";
    std::cout << "Total Area: " << hullTracker.getTotalHullArea() << " px^2\n";
    std::cout << "Total Speed: " << hullTracker.calculateAllAveragedSpeed()
              << " px/s\n";

    cv::destroyAllWindows();
}
