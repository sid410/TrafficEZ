#ifndef WARP_PERSPECTIVE_H
#define WARP_PERSPECTIVE_H

#include "TransformPerspective.h"

class WarpPerspective : public TransformPerspective
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

private:
    cv::Size outputSize;
};

#endif