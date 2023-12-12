#ifndef TRIM_PERSPECTIVE_H
#define TRIM_PERSPECTIVE_H

#include "TransformPerspective.h"

class TrimPerspective : public TransformPerspective
{
public:
    virtual void initialize(cv::Mat& frame,
                            std::vector<cv::Point2f>& roiPoints,
                            cv::Mat& roiMatrix) override;
    virtual void apply(const cv::Mat& input,
                       cv::Mat& output,
                       cv::Mat& roiMatrix) override;

private:
    bool isBoxInitialized;
    cv::Rect boundingBox;
};

#endif