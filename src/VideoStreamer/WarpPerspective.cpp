#include "WarpPerspective.h"
#include <opencv2/opencv.hpp>

void WarpPerspective::initialize(std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortPoints(srcPoints, sortedPoints);

    double length1 = cv::norm(sortedPoints[0] - sortedPoints[2]);
    double length2 = cv::norm(sortedPoints[1] - sortedPoints[3]);
    double width1 = cv::norm(sortedPoints[0] - sortedPoints[1]);
    double width2 = cv::norm(sortedPoints[2] - sortedPoints[3]);

    double maxLength = std::max(length1, length2);
    double maxWidth = std::max(width1, width2);

    // Destination points for the bird's eye view
    dstPoints = {cv::Point2f(0, 0),
                 cv::Point2f(maxLength - 1, 0),
                 cv::Point2f(0, maxWidth - 1),
                 cv::Point2f(maxLength - 1, maxWidth - 1)};

    perspectiveMatrix = cv::getPerspectiveTransform(sortedPoints, dstPoints);

    outputSize.width = static_cast<int>(cv::norm(dstPoints[1] - dstPoints[0]));
    outputSize.height = static_cast<int>(cv::norm(dstPoints[2] - dstPoints[0]));
}

void WarpPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix,
                            std::vector<cv::Point2f>& dstPoints)
{
    cv::warpPerspective(input, output, perspectiveMatrix, outputSize);
}