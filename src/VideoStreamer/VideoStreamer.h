#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include "TransformPerspective.h"
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

    void initializePerspectiveTransform(cv::Mat& frame,
                                        TransformPerspective& perspective);
    bool applyFrameRoi(cv::Mat& frame,
                       cv::Mat& warpedFrame,
                       TransformPerspective& perspective);

protected:
    bool readCalibSuccess;

    cv::Mat perspectiveMatrix;
    std::vector<cv::Point2f> srcPoints;
    std::vector<cv::Point2f> dstPoints;

private:
    cv::VideoCapture stream;
    int originalWidth;
    int originalHeight;
    int originalFormat;

    bool perspectiveMatrixInitialized;
};

#endif