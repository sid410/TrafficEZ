#ifndef TRANSFORM_PERSPECTIVE_H
#define TRANSFORM_PERSPECTIVE_H

#include <opencv2/opencv.hpp>

class TransformPerspective
{
public:
    virtual ~TransformPerspective() {}
    virtual void initialize(std::vector<cv::Point2f>& srcPoints,
                            std::vector<cv::Point2f>& dstPoints,
                            cv::Mat& perspectiveMatrix) = 0;
    virtual void apply(const cv::Mat& input,
                       cv::Mat& output,
                       cv::Mat& perspectiveMatrix,
                       std::vector<cv::Point2f>& dstPoints) = 0;
};

#endif