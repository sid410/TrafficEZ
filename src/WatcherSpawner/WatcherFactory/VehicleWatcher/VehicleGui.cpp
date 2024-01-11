#include "VehicleGui.h"
#include "FPSHelper.h"
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

    cv::Mat inputFrame;
    cv::Mat warpedFrame;

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
        // hullDetector.getHulls(warpedFrame);

        fpsHelper.avgFps();
        fpsHelper.printFps();

        // pipeBuilder.process(warpedFrame);

        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
