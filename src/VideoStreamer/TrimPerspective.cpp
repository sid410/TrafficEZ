#include "TrimPerspective.h"
#include <opencv2/opencv.hpp>

void TrimPerspective::initialize(std::vector<cv::Point2f>& srcPoints,
                                 std::vector<cv::Point2f>& dstPoints,
                                 cv::Mat& perspectiveMatrix)
{
    std::vector<cv::Point2f> sortedPoints;
    sortPoints(srcPoints, sortedPoints);

    // we go the order 0, 1, 3, 2 because we want to order clockwise
    dstPoints = {
        sortedPoints[0], sortedPoints[1], sortedPoints[3], sortedPoints[2]};
}

void TrimPerspective::apply(const cv::Mat& input,
                            cv::Mat& output,
                            cv::Mat& perspectiveMatrix,
                            std::vector<cv::Point2f>& dstPoints)
{
    perspectiveMatrix = cv::Mat::zeros(input.size(), input.type());

    std::vector<cv::Point> dstPointsInt;
    for(const auto& point : dstPoints)
    {
        dstPointsInt.push_back(
            cv::Point(static_cast<int>(point.x), static_cast<int>(point.y)));
    }

    cv::polylines(perspectiveMatrix,
                  std::vector<std::vector<cv::Point>>{dstPointsInt},
                  true,
                  cv::Scalar(255, 255, 255),
                  1,
                  cv::LINE_AA);

    cv::fillConvexPoly(perspectiveMatrix,
                       dstPointsInt.data(),
                       static_cast<int>(dstPointsInt.size()),
                       cv::Scalar(255, 255, 255));

    cv::bitwise_and(input, perspectiveMatrix, output);
}