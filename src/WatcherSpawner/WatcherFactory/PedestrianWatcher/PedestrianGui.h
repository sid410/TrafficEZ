#ifndef PEDESTRIAN_GUI_H
#define PEDESTRIAN_GUI_H

#include "Gui.h"
#include <opencv2/opencv.hpp>

#include "PersonSegmentationStrategy.h"
#include "SegmentationMask.h"
#include "TrimPerspective.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"

class PedestrianGui : public Gui
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;

    void display() override;
    int getInstanceCount() override;

private:
    VideoStreamer videoStreamer;
    TrimPerspective trimPerspective;
    WarpPerspective warpPerspective;
    SegmentationMask segmentation;

    std::string streamWindow;
    std::string segModel;

    cv::Mat inputFrame;
    cv::Mat trimmedFrame;
    cv::Mat outputMask;
};

#endif
