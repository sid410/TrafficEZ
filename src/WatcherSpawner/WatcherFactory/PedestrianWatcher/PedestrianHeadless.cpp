#include "PedestrianHeadless.h"

void PedestrianHeadless::initialize(const std::string& streamName,
                                    const std::string& calibName)
{
    if(!videoStreamer.openVideoStream(streamName) ||
       !videoStreamer.readCalibrationData(calibName))
    {
        std::cerr << "Failed to initialize video stream or calibration data.\n";
        return;
    }

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

void PedestrianHeadless::process()
{
    if(!videoStreamer.applyFrameRoi(inputFrame, trimmedFrame, trimPerspective))
        return;

    cv::Mat segMask = segmentation.generateMask(trimmedFrame);
}

int PedestrianHeadless::getInstanceCount()
{
    return segmentation.getDetectionResultSize();
}