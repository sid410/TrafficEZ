#include "WarpPerspective.h"
#include <opencv2/opencv.hpp>

/**
 * @brief Initialize Warp Perspective for a bird's eye view
 */
void WarpPerspective::initialize(cv::Mat& frame,
                                 std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortPoints(srcPoints, sortedPoints);

    double length1 = cv::norm(sortedPoints[0] - sortedPoints[2]); // left side
    double length2 = cv::norm(sortedPoints[1] - sortedPoints[3]); // right side
    double width1 = cv::norm(sortedPoints[0] - sortedPoints[1]); // top side
    double width2 = cv::norm(sortedPoints[2] - sortedPoints[3]); // bottom side

    double maxLength = std::max(length1, length2);
    double maxWidth = std::max(width1, width2);

    // create rectangle with 4 points
    dstPoints = {cv::Point2f(0, 0),
                 cv::Point2f(maxLength - 1, 0),
                 cv::Point2f(0, maxWidth - 1),
                 cv::Point2f(maxLength - 1, maxWidth - 1)};

    perspectiveMatrix = cv::getPerspectiveTransform(sortedPoints, dstPoints);

    outputSize.width = static_cast<int>(cv::norm(dstPoints[1] - dstPoints[0]));
    outputSize.height = static_cast<int>(cv::norm(dstPoints[2] - dstPoints[0]));
}

/**
 * @brief Apply the bird's eye view from input to output frame
 */
void WarpPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix)
{
    cv::warpPerspective(input, output, perspectiveMatrix, outputSize);
}