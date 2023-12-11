#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

class VideoStreamer
{
public:
    VideoStreamer();
    ~VideoStreamer();

    bool openVideoStream(const cv::String& filename);
    void constructStreamWindow(const cv::String& windowName);

    bool getNextFrame(cv::Mat& frame);
    bool readCalibrationPoints(const cv::String& filename);

    void initializePerspectiveTransform();
    bool warpFrame(cv::Mat& frame, cv::Mat& warpedFrame);

protected:
    bool readCalibSuccess;
    std::vector<cv::Point2f> srcPoints;

private:
    cv::VideoCapture stream;
    int originalWidth;
    int originalHeight;

    bool perspectiveMatrixInitialized;

    std::vector<cv::Point2f> dstPoints;
    cv::Mat perspectiveMatrix;
};

#endif