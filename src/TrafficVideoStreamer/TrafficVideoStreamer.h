#ifndef TRAFFICVIDEOSTREAMER_H
#define TRAFFICVIDEOSTREAMER_H

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

class TrafficVideoStreamer
{
public:
    TrafficVideoStreamer();
    ~TrafficVideoStreamer();

    bool openVideoStream(const std::string& filename);
    void constructStreamWindow(const std::string& windowName);
    bool getNextFrame(cv::Mat& frame);

    bool readCalibrationPoints(const std::string& filename);
    bool perspectiveMatrixInitialized;
    void initializePerspectiveTransform();
    void warpFrame(const cv::Mat& inputFrame, cv::Mat& warpedFrame);

private:
    cv::VideoCapture stream;
    int originalWidth;
    int originalHeight;

    bool readCalibSuccess;
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> dstPoints;
    cv::Mat perspectiveMatrix;
};

#endif