#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include "TransformPerspective.h"
#include <opencv2/opencv.hpp>
#include <yaml-cpp/yaml.h>

/**
 * @brief Class for reading/getting frames from stream,
 * then transform that to the ROI based on the chosen
 * TransformPerspective strategy: Warp/Trim.
 */
class VideoStreamer
{
public:
    VideoStreamer();
    ~VideoStreamer();

    bool openVideoStream(const cv::String& streamName);
    void constructStreamWindow(const cv::String& windowName);

    bool getNextFrame(cv::Mat& frame);
    bool readCalibrationPoints(const cv::String& filename);

    void initializePerspectiveTransform(cv::Mat& frame,
                                        TransformPerspective& perspective);
    bool applyFrameRoi(cv::Mat& frame,
                       cv::Mat& roiFrame,
                       TransformPerspective& perspective);

protected:
    bool readCalibSuccess; // used also in the child class

    cv::Mat roiMatrix;
    std::vector<cv::Point2f> roiPoints;

private:
    cv::VideoCapture stream;
    int originalWidth;
    int originalHeight;

    bool roiMatrixInitialized;
};

#endif