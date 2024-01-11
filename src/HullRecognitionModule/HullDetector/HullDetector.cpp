#include "HullDetector.h"

HullDetector::HullDetector(double minArea)
    : minContourArea(minArea)
{}

void HullDetector::getHulls(const cv::Mat& frame,
                            std::vector<std::vector<cv::Point>>& hulls)
{
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