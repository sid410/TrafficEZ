#include "VehicleGui.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName) const
{
    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;

    cv::Mat inputFrame;
    cv::Mat warpedFrame;

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationPoints(calibName))
        return;

    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);

    while(videoStreamer.applyFrameRoi(inputFrame, warpedFrame, warpPerspective))
    {
        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
