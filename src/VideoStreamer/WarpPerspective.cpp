#include "WarpPerspective.h"
#include <opencv2/opencv.hpp>

void WarpPerspective::initialize(std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{
    /// Sort the points based on y-coordinates
    std::sort(srcPoints.begin(),
              srcPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.y < b.y; });

    // Split the sorted points into top and bottom
    std::vector<cv::Point2f> topPoints, bottomPoints;
    topPoints.push_back(srcPoints[0]);
    topPoints.push_back(srcPoints[1]);
    bottomPoints.push_back(srcPoints[2]);
    bottomPoints.push_back(srcPoints[3]);

    // Sort the top and bottom points based on x-coordinates
    std::sort(topPoints.begin(),
              topPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });
    std::sort(bottomPoints.begin(),
              bottomPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });

    // Combine the top and bottom points
    std::vector<cv::Point2f> sortedPoints = {
        topPoints[0], topPoints[1], bottomPoints[0], bottomPoints[1]};

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
}

void WarpPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix,
                            std::vector<cv::Point2f>& dstPoints)
{
    // Calculate the size of the output image
    int width = static_cast<int>(cv::norm(dstPoints[1] - dstPoints[0]));
    int height = static_cast<int>(cv::norm(dstPoints[2] - dstPoints[0]));

    // Create a Size object with the calculated width and height
    cv::Size outputSize(width, height);

    cv::warpPerspective(input, output, perspectiveMatrix, outputSize);
}