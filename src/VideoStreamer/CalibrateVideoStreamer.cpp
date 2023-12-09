#include "CalibrateVideoStreamer.h"
#include <iostream>

CalibrateVideoStreamer::CalibrateVideoStreamer()
    : VideoStreamer()
{}

CalibrateVideoStreamer::~CalibrateVideoStreamer() {}

void CalibrateVideoStreamer::setCalibrationPoints(
    const std::vector<cv::Point2f>& points)
{
    calibrationPoints = points;
}
