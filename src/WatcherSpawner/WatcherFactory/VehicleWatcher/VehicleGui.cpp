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
    HullDetector hullDetector;
    HullTracker hullTracker;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat processFrame;

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationPoints(calibName))
        return;

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
        // pipeBuilder.processDebugStack(processFrame);

        std::vector<std::vector<cv::Point>> hulls;
        hullDetector.getHulls(processFrame, hulls);
        hullTracker.drawUnreliableHulls(warpedFrame, hulls);

        fpsHelper.avgFps();
        fpsHelper.displayFps(warpedFrame);
        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
