#ifndef TRANSFORM_PERSPECTIVE_H
#define TRANSFORM_PERSPECTIVE_H

#include <opencv2/opencv.hpp>

/**
 * @brief Interface for transforming the perspective
 * defined by the four ROI points.
 *
 */
class TransformPerspective
{
protected:
    void sortQuadPoints(std::vector<cv::Point2f>& unsortedPts,
                        std::vector<cv::Point2f>& sortedPts);

public:
    virtual ~TransformPerspective() {}
    virtual void initialize(cv::Mat& frame,
                            std::vector<cv::Point2f>& roiPoints,
                            cv::Mat& roiMatrix) = 0;
    virtual void
    apply(const cv::Mat& input, cv::Mat& output, cv::Mat& roiMatrix) = 0;
};

#endif