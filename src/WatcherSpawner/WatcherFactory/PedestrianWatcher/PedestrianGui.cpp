#include "PedestrianGui.h"

void PedestrianGui::initialize(const std::string& streamName,
                               const std::string& calibName)
{
    if(!videoStreamer.openVideoStream(streamName) ||
       !videoStreamer.readCalibrationData(calibName))
    {
        std::cerr << "Failed to initialize video stream or calibration data.\n";
        return;
    }

    streamWindow = streamName + " Pedestrian GUI";

    videoStreamer.constructStreamWindow(streamWindow);

    // there is a bug where the matrix sizes or types do not match,
    // as workaround for now, process one frame of warp to initialize correctly,
    videoStreamer.initializePerspectiveTransform(inputFrame, warpPerspective);
    videoStreamer.applyFrameRoi(inputFrame, trimmedFrame, warpPerspective);

    // then we can use the trim after one warp.
    videoStreamer.initializePerspectiveTransform(inputFrame, trimPerspective);
    videoStreamer.applyFrameRoi(inputFrame, trimmedFrame, trimPerspective);

    segModel = videoStreamer.getSegModel();

    std::unique_ptr<ISegmentationStrategy> strategy =
        std::make_unique<PersonSegmentationStrategy>();
    segmentation.initializeModel(segModel, std::move(strategy));
}

void PedestrianGui::display()
{
    if(!videoStreamer.applyFrameRoi(inputFrame, trimmedFrame, trimPerspective))
        return;

    cv::Mat segMask = segmentation.generateMask(trimmedFrame, false);
    cv::imshow(streamWindow, segMask);
    cv::waitKey(1);
}

int PedestrianGui::getInstanceCount()
{
    return segmentation.getDetectionResultSize();
}
