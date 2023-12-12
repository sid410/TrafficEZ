#ifndef WARP_PERSPECTIVE_H
#define WARP_PERSPECTIVE_H

#include "TransformPerspective.h"

/**
 * @brief Warp strategy of TransformPerspective
 */
class WarpPerspective : public TransformPerspective
{
public:
    WarpPerspective();

    virtual void initialize(cv::Mat& frame,
                            std::vector<cv::Point2f>& roiPoints,
                            cv::Mat& roiMatrix) override;
    virtual void
    apply(const cv::Mat& input, cv::Mat& output, cv::Mat& roiMatrix) override;

private:
    cv::Size outputSize;
};

#endif