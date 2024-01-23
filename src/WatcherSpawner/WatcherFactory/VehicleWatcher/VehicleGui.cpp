#include "VehicleGui.h"
#include "FPSHelper.h"
#include "HullDetector.h"
#include "HullTracker.h"
#include "PreprocessPipelineBuilder.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName) const
{
    int frameCounter = 0; // temporary, for estimating traffic flow

    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    FPSHelper fpsHelper;

    PreprocessPipelineBuilder pipeBuilder;
    HullDetector hullDetector;
    HullTracker hullTracker;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat processFrame;

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationPoints(calibName))
        return;

    static int laneLength = videoStreamer.getLaneLength();
    static int laneWidth = videoStreamer.getLaneWidth();

    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);

    pipeBuilder.addGrayscaleStep()
        .addGaussianBlurStep()
        .addMOG2BackgroundSubtractionStep()
        .addThresholdStep()
        .addDilationStep()
        .addErosionStep();

    // for initialization of detector and tracker
    videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective);
    hullDetector.initialize(warpedFrame);
    hullTracker.initialize(hullDetector.getOutBoundaryLine());

    // update loop
    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        warpedFrame.copyTo(processFrame);
        pipeBuilder.process(processFrame);

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

    cv::destroyAllWindows();
}
