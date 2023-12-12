#include "WarpPerspective.h"
#include <opencv2/opencv.hpp>

WarpPerspective::WarpPerspective()
    : outputSize(0, 0)
{}

/**
 * @brief Initialize Warp Perspective for a bird's eye view.
 * @param frame not used in this strategy.
 * @param roiPoints the four unsorted ROI points.
 * @param roiMatrix the matrix to store getPerspectiveTransform.
 */
void WarpPerspective::initialize(cv::Mat& frame,
                                 std::vector<cv::Point2f>& roiPoints,
                                 cv::Mat& roiMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortQuadPoints(roiPoints, sortedPoints);

    double length1 = cv::norm(sortedPoints[0] - sortedPoints[2]); // left side
    double length2 = cv::norm(sortedPoints[1] - sortedPoints[3]); // right side
    double width1 = cv::norm(sortedPoints[0] - sortedPoints[1]); // top side
    double width2 = cv::norm(sortedPoints[2] - sortedPoints[3]); // bottom side

    double maxLength = std::max(length1, length2);
    double maxWidth = std::max(width1, width2);

    // create rectangle with 4 points
    std::vector<cv::Point2f> warpPoints = {
        cv::Point2f(0, 0),
        cv::Point2f(maxLength - 1, 0),
        cv::Point2f(0, maxWidth - 1),
        cv::Point2f(maxLength - 1, maxWidth - 1)};

    roiMatrix = cv::getPerspectiveTransform(sortedPoints, warpPoints);

    outputSize.width =
        static_cast<int>(cv::norm(warpPoints[1] - warpPoints[0]));
    outputSize.height =
        static_cast<int>(cv::norm(warpPoints[2] - warpPoints[0]));
}

/**
 * @brief Apply Warp Perspective for a bird's eye view.
 * @param input input frame from videostream.
 * @param output the warped ROI frame.
 * @param roiMatrix matrix from getPerspectiveTransform.
 */
void WarpPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& roiMatrix)
{
    cv::warpPerspective(input, output, roiMatrix, outputSize);
}