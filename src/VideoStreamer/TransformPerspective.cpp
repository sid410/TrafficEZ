#include "TransformPerspective.h"

void TransformPerspective::sortPoints(std::vector<cv::Point2f>& unsortedPoints,
                                      std::vector<cv::Point2f>& sortedPoints)
{
    /// Sort the points based on y-coordinates
    std::sort(unsortedPoints.begin(),
              unsortedPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.y < b.y; });

    // Split the sorted points into top and bottom
    std::vector<cv::Point2f> topPoints, bottomPoints;
    topPoints.push_back(unsortedPoints[0]);
    topPoints.push_back(unsortedPoints[1]);
    bottomPoints.push_back(unsortedPoints[2]);
    bottomPoints.push_back(unsortedPoints[3]);

    // Sort the top and bottom points based on x-coordinates
    std::sort(topPoints.begin(),
              topPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });
    std::sort(bottomPoints.begin(),
              bottomPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });

    // Combine the top and bottom points
    sortedPoints = {
        topPoints[0], topPoints[1], bottomPoints[0], bottomPoints[1]};
}
