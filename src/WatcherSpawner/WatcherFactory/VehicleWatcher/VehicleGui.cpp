#include "VehicleGui.h"
#include "FPSHelper.h"
#include "HullDetector.h"
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

    cv::Mat inputFrame;
    cv::Mat warpedFrame;
    cv::Mat preprocessedFrame;

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
        warpedFrame.copyTo(preprocessedFrame);
        pipeBuilder.processDebugStack(preprocessedFrame);

        std::vector<std::vector<cv::Point>> hulls;
        hullDetector.getHulls(preprocessedFrame, hulls);
        cv::drawContours(warpedFrame, hulls, -1, cv::Scalar(0, 255, 0), 2);

        fpsHelper.avgFps();
        fpsHelper.displayFps(warpedFrame);
        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
