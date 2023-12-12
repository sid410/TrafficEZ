#ifndef TRANSFORM_PERSPECTIVE_H
#define TRANSFORM_PERSPECTIVE_H

#include <opencv2/opencv.hpp>

class TransformPerspective
{
protected:
    void sortPoints(std::vector<cv::Point2f>& unsortedPoints,
                    std::vector<cv::Point2f>& sortedPoints);

public:
    virtual ~TransformPerspective() {}
    virtual void initialize(cv::Mat& frame,
                            std::vector<cv::Point2f>& srcPoints,
                            std::vector<cv::Point2f>& dstPoints,
                            cv::Mat& perspectiveMatrix) = 0;
    virtual void apply(const cv::Mat& input,
                       cv::Mat& output,
                       cv::Mat& perspectiveMatrix) = 0;
};

#endif