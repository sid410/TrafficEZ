#ifndef TRAFFICVIDEOSTREAMER_H
#define TRAFFICVIDEOSTREAMER_H

#include <opencv2/opencv.hpp>

class TrafficVideoStreamer
{
public:
    TrafficVideoStreamer();
    ~TrafficVideoStreamer();

    bool openVideoStream(const std::string& filename);
    void constructStreamWindow(const std::string& windowName);

    bool getNextFrame(cv::Mat& frame);

    bool perspectiveMatrixInitialized;
    void initializePerspectiveTransform();
    void warpFrame(const cv::Mat& inputFrame, cv::Mat& warpedFrame);

private:
    cv::VideoCapture stream;
    int originalWidth;
    int originalHeight;

    const std::vector<cv::Point2f> srcPoints = {cv::Point2f(666, 306),
                                                cv::Point2f(796, 303),
                                                cv::Point2f(789, 702),
                                                cv::Point2f(1274, 566)};

    std::vector<cv::Point2f> dstPoints;
    cv::Mat perspectiveMatrix;
};

#endif