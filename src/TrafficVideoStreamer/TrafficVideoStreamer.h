#ifndef TRAFFIC_VIDEO_STREAMER_H
#define TRAFFIC_VIDEO_STREAMER_H

#include <opencv2/opencv.hpp>

class TrafficVideoStreamer
{
public:
    TrafficVideoStreamer();
    ~TrafficVideoStreamer();
    bool openVideoStream(const std::string& filename);
    bool getNextFrame(cv::Mat& frame);

private:
    cv::VideoCapture stream;
};

#endif