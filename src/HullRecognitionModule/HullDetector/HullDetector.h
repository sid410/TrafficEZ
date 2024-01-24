#ifndef HULLDETECTOR_H
#define HULLDETECTOR_H

#include <opencv2/opencv.hpp>

/**
 * @brief Class for detecting convex hulls.
 * Optionally, you can specify in the constructor 
 * the following:
 * @param minContourArea contours less than this is ignored.
 * @param detectionStartPercent %height of the frame y-axis to start detecting.
 * @param detectionEndPercent %height of the frame y-axis to end detecting.
 */
class HullDetector
{
public:
    HullDetector(double minContourArea = 2000.0,
                 int detectionStartPercent = 20,
                 int detectionEndPercent = 70);

    void initDetectionBoundaries(const cv::Mat& frame) const;

    void getHulls(const cv::Mat& frame,
                  std::vector<std::vector<cv::Point>>& hulls);
    int getEndDetectionLine() const;

    void drawLengthBoundaries(cv::Mat& frame) const;

private:
    const double minContourArea;
    const int startDetectionPercent;
    const int endDetectionPercent;

    mutable int startDetectionY;
    mutable int endDetectionY;

    void calculateBoundaries(int frameHeight) const;
};

#endif
