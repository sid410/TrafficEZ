#include "HullTracker.h"

HullTracker::HullTracker(double minArea)
    : minContourArea(minArea)
{}

/**
 * @brief Gets the convex hulls from the contours generated
 * from the preprocessed frame.
 * @param frame the preprocessed frame.
 * @param hulls the hulls reference to store data.
 */
void HullTracker::getHulls(const cv::Mat& frame,
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

/**
 * @brief Draw the unreliable/unfiltered hulls onto the frame
 * @param inputFrame the frame to be drawn on.
 * @param unreliableHulls the hulls to draw to the frame.
 */
void HullTracker::drawUnreliableHulls(
    const cv::Mat& inputFrame,
    std::vector<std::vector<cv::Point>>& unreliableHulls)
{
    cv::drawContours(inputFrame, unreliableHulls, -1, cv::Scalar(0, 0, 255), 2);
}