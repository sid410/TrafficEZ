#ifndef HULLTRACKER_H
#define HULLTRACKER_H

#include <opencv2/opencv.hpp>

/**
 * @brief Class for detecting convex hulls.
 * Optionally, you can specify the minimum area of
 * hulls to detect in the constructor.
 */
class HullTracker
{
private:
    double minContourArea;

public:
    HullTracker(double minArea = 500.0);
    void getHulls(const cv::Mat& frame,
                  std::vector<std::vector<cv::Point>>& hulls);
    void
    drawUnreliableHulls(const cv::Mat& inputFrame,
                        std::vector<std::vector<cv::Point>>& unreliableHulls);
};

#endif
