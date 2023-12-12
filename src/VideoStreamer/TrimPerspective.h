#ifndef TRIM_PERSPECTIVE_H
#define TRIM_PERSPECTIVE_H

#include "TransformPerspective.h"

class TrimPerspective : public TransformPerspective
{
public:
    virtual void initialize(cv::Mat& frame,
                            std::vector<cv::Point2f>& srcPoints,
                            std::vector<cv::Point2f>& dstPoints,
                            cv::Mat& perspectiveMatrix) override;
    virtual void apply(const cv::Mat& input,
                       cv::Mat& output,
                       cv::Mat& perspectiveMatrix,
                       std::vector<cv::Point2f>& dstPoints) override;
};

#endif