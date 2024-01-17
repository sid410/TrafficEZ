#ifndef HULLDETECTOR_H
#define HULLDETECTOR_H

#include <opencv2/opencv.hpp>

/**
 * @brief Class for detecting convex hulls.
 * Optionally, you can specify the minimum area of
 * hulls to detect in the constructor.
 */
class HullDetector
{
private:
    double minContourArea;

public:
    HullDetector(double minArea = 500.0);
    void getHulls(const cv::Mat& frame,
                  std::vector<std::vector<cv::Point>>& hulls);
};

#endif
