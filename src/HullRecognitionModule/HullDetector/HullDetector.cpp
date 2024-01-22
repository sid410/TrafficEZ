#include "HullDetector.h"

HullDetector::HullDetector(double minArea, int startPercent, int endPercent)
    : minContourArea(minArea)
    , startDetectPercent(std::clamp(startPercent, 0, 100))
    , endDetectPercent(std::clamp(endPercent, 0, 100))
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

void HullDetector::drawLengthBoundaries(cv::Mat& frame) const
{
    int frameHeight = frame.rows;
    int startY = static_cast<int>(frameHeight * startDetectPercent / 100.0);
    int endY = static_cast<int>(frameHeight * endDetectPercent / 100.0);

    cv::line(frame,
             cv::Point(0, startY),
             cv::Point(frame.cols, startY),
             cv::Scalar(255, 0, 0),
             2);
    cv::line(frame,
             cv::Point(0, endY),
             cv::Point(frame.cols, endY),
             cv::Scalar(255, 0, 0),
             2);
}
