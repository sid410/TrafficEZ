#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include <opencv2/opencv.hpp>
#include <vector>

class HullTrackable
{
public:
    int id; // Unique ID for each trackable
    std::vector<cv::Point> hull; // Hull of the object
    int framesSinceLastSeen; // Counter for frames since last seen
    cv::Point2f centroid; // Centroid of the hull

    HullTrackable();
    HullTrackable(int id, const std::vector<cv::Point>& hull);

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);
};

#endif
