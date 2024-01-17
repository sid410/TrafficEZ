#include "HullDetector.h"

HullDetector::HullDetector(double minArea)
    : minContourArea(minArea)
{}

/**
 * @brief Gets the convex hulls from the contours generated
 * from the preprocessed frame.
 * @param frame the preprocessed frame.
 * @param hulls the hulls reference to store data.
 */
void HullDetector::getHulls(const cv::Mat& frame,
                            std::vector<std::vector<cv::Point>>& hulls)
{
    if(frame.empty())
    {
        std::cerr << "Error: Input frame is empty or invalid in getHulls\n";
        return;
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(
        frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for(const auto& contour : contours)
    {
        if(cv::contourArea(contour) < minContourArea)
            continue;

        std::vector<cv::Point> hull;
        cv::convexHull(contour, hull);
        hulls.push_back(hull);
    }
}