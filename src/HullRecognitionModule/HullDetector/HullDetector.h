#ifndef HULLDETECTOR_H
#define HULLDETECTOR_H

#include <algorithm>
#include <opencv2/opencv.hpp>

/**
 * @brief Class for detecting convex hulls.
 * Optionally, you can specify the minimum area of
 * hulls to detect in the constructor.
 */
class HullDetector
{
private:
    const double minContourArea;
    const int startDetectPercent;
    const int endDetectPercent;

    mutable int startY;
    mutable int endY;
    void calculateBoundaries(int frameHeight) const;

public:
    HullDetector(double minArea = 2500.0,
                 int startPercent = 20,
                 int endPercent = 80);
    void getHulls(const cv::Mat& frame,
                  std::vector<std::vector<cv::Point>>& hulls);
    void drawLengthBoundaries(cv::Mat& frame) const;
};

#endif
