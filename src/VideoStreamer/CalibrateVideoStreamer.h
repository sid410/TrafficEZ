#ifndef CALIBRATE_VIDEO_STREAMER_H
#define CALIBRATE_VIDEO_STREAMER_H

#include "VideoStreamer.h"
#include <vector>

class CalibrateVideoStreamer : public VideoStreamer
{
public:
    CalibrateVideoStreamer();
    ~CalibrateVideoStreamer();

    void setCalibrationPoints(const std::vector<cv::Point2f>& points);

private:
    std::vector<cv::Point2f> calibrationPoints;
};

#endif
