#include "HullDetector.h"

HullDetector::HullDetector(double minArea, int startPercent, int endPercent)
    : minContourArea(minArea)
    , startDetectPercent(std::clamp(startPercent, 0, 100))
    , endDetectPercent(std::clamp(endPercent, 0, 100))
    , startY(0)
    , endY(0)
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

    if(startY == 0 && endY == 0)
    {
        calculateBoundaries(frame.rows);
    }

    std::vector<std::vector<cv::Point>> contours;
    cv::findContours(
        frame, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    hulls.clear();
    for(const auto& contour : contours)
    {
        // Simplify the contour
        std::vector<cv::Point> approxContour;
        cv::approxPolyDP(contour, approxContour, 5, true);

        cv::Moments mu = cv::moments(approxContour);
        if(mu.m00 < minContourArea)
            continue; // filter small contours

        cv::Point2f centroid(mu.m10 / mu.m00, mu.m01 / mu.m00);
        if(centroid.y < startY || centroid.y > endY)
            continue; // filter contours outside bounds

        std::vector<cv::Point> hull;
        cv::convexHull(approxContour, hull);
        hulls.push_back(hull);
    }
}

void HullDetector::calculateBoundaries(int frameHeight) const
{
    startY = static_cast<int>(frameHeight * startDetectPercent / 100.0);
    endY = static_cast<int>(frameHeight * endDetectPercent / 100.0);
}

void HullDetector::drawLengthBoundaries(cv::Mat& frame) const
{
    if(frame.empty())
    {
        std::cerr << "Error: Input frame is empty or invalid in "
                     "drawLengthBoundaries\n";
        return;
    }

    if(startY == 0 && endY == 0)
    {
        calculateBoundaries(frame.rows);
    }

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
