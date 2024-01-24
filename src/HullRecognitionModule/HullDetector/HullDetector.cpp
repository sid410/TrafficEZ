#include "HullDetector.h"
#include <algorithm>

HullDetector::HullDetector(double minContourArea,
                           int detectionStartPercent,
                           int detectionEndPercent)
    : minContourArea(minContourArea)
    , startDetectionPercent(std::clamp(detectionStartPercent, 0, 100))
    , endDetectionPercent(std::clamp(detectionEndPercent, 0, 100))
    , startDetectionY(0)
    , endDetectionY(0)
{}

/**
 * @brief Initializes detection boundaries based on the frame height.
 * @param frame The frame used for determining height-based detection boundaries.
 */
void HullDetector::initDetectionBoundaries(const cv::Mat& frame) const
{
    calculateBoundaries(frame.rows);
}

/**
 * @brief Calculates the Y-axis boundaries for detection based on the frame height.
 * @param frameHeight The height of the frame used for detection.
 */
void HullDetector::calculateBoundaries(int frameHeight) const
{
    startDetectionY =
        static_cast<int>(frameHeight * startDetectionPercent / 100.0);
    endDetectionY = static_cast<int>(frameHeight * endDetectionPercent / 100.0);
}

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

    if(startDetectionY == 0 && endDetectionY == 0)
    {
        std::cerr << "Error: Please call initialize first\n";
        return;
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
        if(centroid.y < startDetectionY || centroid.y > endDetectionY)
            continue; // filter contours outside bounds

        std::vector<cv::Point> hull;
        cv::convexHull(approxContour, hull);
        hulls.push_back(hull);
    }
}

/**
 * @brief Returns the Y-coordinate of the end detection boundary.
 * @return The Y-coordinate of the end detection boundary.
 */
int HullDetector::getEndDetectionLine() const
{
    return endDetectionY;
}

/**
 * @brief Draws lines on the frame to indicate the start and end detection boundaries.
 * @param frame The frame on which the boundary lines will be drawn.
 */
void HullDetector::drawLengthBoundaries(cv::Mat& frame) const
{
    if(frame.empty())
    {
        std::cerr << "Error: Input frame is empty or invalid in "
                     "drawLengthBoundaries\n";
        return;
    }

    if(startDetectionY == 0 && endDetectionY == 0)
    {
        std::cerr << "Error: Please call initialize first\n";
        return;
    }

    cv::line(frame,
             cv::Point(0, startDetectionY),
             cv::Point(frame.cols, startDetectionY),
             cv::Scalar(255, 0, 0),
             2);
    cv::line(frame,
             cv::Point(0, endDetectionY),
             cv::Point(frame.cols, endDetectionY),
             cv::Scalar(255, 0, 0),
             2);
}
