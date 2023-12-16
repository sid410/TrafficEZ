#include "VehicleGui.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

void VehicleGui::display(const std::string& streamName,
                         const std::string& calibName) const
{
    VideoStreamer videoStreamer;
    WarpPerspective warpPerspective;

    if(!videoStreamer.openVideoStream(streamName))
        return;

    if(!videoStreamer.readCalibrationPoints(calibName))
        return;

    cv::Mat frame;
    cv::Mat warpedFrame;
    videoStreamer.initializePerspectiveTransform(frame, warpPerspective);

    while(videoStreamer.applyFrameRoi(frame, warpedFrame, warpPerspective))
    {
        cv::imshow("Vehicle Gui", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}
