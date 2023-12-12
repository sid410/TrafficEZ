#include "TrimPerspective.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Initialize Trim Perspective for a focused ROI.
 * This function only takes the inside ROI
 * and fills the rest with black pixels.
 */
void TrimPerspective::initialize(cv::Mat& frame,
                                 std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortPoints(srcPoints, sortedPoints);

    // we go the order 0, 1, 3, 2 because we want to order clockwise
    dstPoints = {
        sortedPoints[0], sortedPoints[1], sortedPoints[3], sortedPoints[2]};

    // convert to int for poly compatibility
    std::vector<cv::Point> dstPointsInt;
    for(const auto& point : dstPoints)
    {
        dstPointsInt.push_back(
            cv::Point(static_cast<int>(point.x), static_cast<int>(point.y)));
    }

    // fill only the regoin of interest part
    perspectiveMatrix = cv::Mat::zeros(frame.size(), frame.type());
    cv::fillConvexPoly(perspectiveMatrix,
                       dstPointsInt.data(),
                       static_cast<int>(dstPointsInt.size()),
                       cv::Scalar(255, 255, 255));

    isBoxInitialized = false;
}

/**
 * @brief Focus the frame to only the ROI.
 */
void TrimPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix)
{
    // mask everything except the ROI
    cv::bitwise_and(input, perspectiveMatrix, output);

    // throw away unnecessary black area
    if(!isBoxInitialized)
    {
        // we only need to do this once with the output frame
        cv::Mat outputGray;
        cv::cvtColor(output, outputGray, cv::COLOR_BGR2GRAY);
        boundingBox = cv::boundingRect(outputGray);
        isBoxInitialized = true;
    }
    output = output(boundingBox);
}