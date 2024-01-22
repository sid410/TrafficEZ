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
    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;
    FPSHelper fpsHelper;

    PreprocessPipelineBuilder pipeBuilder;
    HullDetector hullDetector(2000, 20, 80);
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

    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
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
        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
