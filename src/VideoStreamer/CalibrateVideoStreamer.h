#ifndef CALIBRATE_VIDEO_STREAMER_H
#define CALIBRATE_VIDEO_STREAMER_H

#include "VideoStreamer.h"
#include <vector>

class CalibrateVideoStreamer : public VideoStreamer
{
public:
    CalibrateVideoStreamer();
    ~CalibrateVideoStreamer();

    void setCalibrationPointsFromMouse(const std::string& windowName);

private:
};

#endif
