#include "TrimPerspective.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Initialize Trim Perspective for a focused ROI.
 * This function takes the inside ROI pixels
 * and fills the rest with black pixels.
 * @param frame needed to get frame size and type.
 * @param roiPoints the four unsorted ROI points.
 * @param roiMatrix the matrix to fill non-black pixels.
 */
void TrimPerspective::initialize(cv::Mat& frame,
                                 std::vector<cv::Point2f>& roiPoints,
                                 cv::Mat& roiMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortPoints(roiPoints, sortedPoints);

    // we go the order 0, 1, 3, 2 because we want to order clockwise
    sortedPoints = {
        sortedPoints[0], sortedPoints[1], sortedPoints[3], sortedPoints[2]};

    // convert to int for poly compatibility
    std::vector<cv::Point> intPoints;
    for(const auto& point : sortedPoints)
    {
        intPoints.push_back(
            cv::Point(static_cast<int>(point.x), static_cast<int>(point.y)));
    }

    // fill only the regoin of interest part
    roiMatrix = cv::Mat::zeros(frame.size(), frame.type());
    cv::fillConvexPoly(roiMatrix,
                       intPoints.data(),
                       static_cast<int>(intPoints.size()),
                       cv::Scalar(255, 255, 255));

    isBoxInitialized = false;
}

/**
 * @brief Apply Trim Perspective for a focused ROI.
 * This function takes the inside ROI pixels
 * and fills the rest with black pixels.
 * @param input input frame from videostream.
 * @param output the trimmed ROI frame.
 * @param roiMatrix matrix to mask only the ROI.
 */
void TrimPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& roiMatrix)
{
    // mask everything except the ROI
    cv::bitwise_and(input, roiMatrix, output);

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