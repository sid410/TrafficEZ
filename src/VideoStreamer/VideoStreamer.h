#ifndef VIDEO_STREAMER_H
#define VIDEO_STREAMER_H

#include "TransformPerspective.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Class for reading/getting frames from a stream,
 * then transforming that to the ROI based on the chosen
 * TransformPerspective strategy: Warp/Trim.
 */
class VideoStreamer
{
public:
    VideoStreamer();
    ~VideoStreamer();

    bool openVideoStream(const cv::String& streamName);
    void constructStreamWindow(const cv::String& windowName);
    void resizeStreamWindow(const cv::Mat& referenceFrame);

    bool getNextFrame(cv::Mat& frame);
    bool readCalibrationData(const cv::String& yamlFilename);

    double getLaneLength() const;
    double getLaneWidth() const;
    cv::String getSegModel() const;

    void initializePerspectiveTransform(cv::Mat& frame,
                                        TransformPerspective& perspective);
    bool applyFrameRoi(cv::Mat& frame,
                       cv::Mat& roiFrame,
                       TransformPerspective& perspective);

    cv::Mat applyPerspective(cv::Mat inputFrame,
                             TransformPerspective& perspective);

protected:
    bool readCalibSuccess; // used also in CalibrateVideoStreamer

    cv::Mat roiMatrix;
    std::vector<cv::Point2f> roiPoints;

private:
    cv::VideoCapture stream;
    cv::String streamWindowInstance;

    int originalWidth;
    int originalHeight;

    double laneLength;
    double laneWidth;
    cv::String segModel;

    bool roiMatrixInitialized;
};

#endif