#include "TransformPerspective.h"

/**
 * @brief Sorts four points in the order:
 *
 * 0 - top left;
 * 1 - top right;
 * 2 - bottom left;
 * 3 - bottom right;
 */
void TransformPerspective::sortQuadPoints(std::vector<cv::Point2f>& unsortedPts,
                                          std::vector<cv::Point2f>& sortedPts)
{
    if(unsortedPts.size() != 4)
    {
        std::cerr << "Error: Exactly 4 points are required for perspective "
                     "transformation.\n";
        return;
    }

    // Sort the points based on y-coordinates
    std::sort(unsortedPts.begin(),
              unsortedPts.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.y < b.y; });

    // Split the sorted points into top and bottom
    std::vector<cv::Point2f> topPoints, bottomPoints;
    topPoints.push_back(unsortedPts[0]);
    topPoints.push_back(unsortedPts[1]);
    bottomPoints.push_back(unsortedPts[2]);
    bottomPoints.push_back(unsortedPts[3]);

    // Sort the top and bottom points based on x-coordinates
    std::sort(topPoints.begin(),
              topPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });
    std::sort(bottomPoints.begin(),
              bottomPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });

    // Combine the top and bottom points
    sortedPts = {topPoints[0], topPoints[1], bottomPoints[0], bottomPoints[1]};
}
