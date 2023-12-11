#ifndef CALIBRATE_VIDEO_STREAMER_H
#define CALIBRATE_VIDEO_STREAMER_H

#include "VideoStreamer.h"
#include <vector>

class CalibrateVideoStreamer : public VideoStreamer
{
public:
    CalibrateVideoStreamer();
    ~CalibrateVideoStreamer();

    void initCalibrationPoints(const cv::String& windowName);
    void resetCalibrationPoints();
    void saveCalibrationPoints(const cv::String& filename);
    void showCalibrationPoints(cv::Mat& frame);
    void initPreview(TransformPerspective& perspective);

    bool settingCalibrationPoints(cv::Mat& frame);
    bool haveSetFourPoints();

private:
    bool pointsSetSuccessfully;
    std::vector<cv::Point2f> mouseCalibrationPoints;
};

#endif
