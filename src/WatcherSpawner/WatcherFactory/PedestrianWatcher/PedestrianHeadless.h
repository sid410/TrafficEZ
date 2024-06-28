#ifndef PEDESTRIAN_HEADLESS_H
#define PEDESTRIAN_HEADLESS_H

#include "Headless.h"
#include <opencv2/opencv.hpp>

#include "PersonSegmentationStrategy.h"
#include "SegmentationMask.h"
#include "TrimPerspective.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

class PedestrianHeadless : public Headless
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;

    void process() override;
    int getInstanceCount() override;

private:
    VideoStreamer videoStreamer;
    TrimPerspective trimPerspective;
    WarpPerspective warpPerspective;
    SegmentationMask segmentation;

    std::string segModel;

    cv::Mat inputFrame;
    cv::Mat trimmedFrame;
    cv::Mat outputMask;
};

#endif
