#include "HullTrackable.h"

int HullTrackable::nextId = 0; // Initialize static ID counter

HullTrackable::HullTrackable()
    : id(nextId++)
    , framesSinceLastSeen(-1)
{}

HullTrackable::HullTrackable(const std::vector<cv::Point>& hull)
    : id(nextId++)
    , hull(hull)
    , framesSinceLastSeen(0)
    , centroid(computeCentroid(hull))
{}

cv::Point2f HullTrackable::computeCentroid(const std::vector<cv::Point>& hull)
{
    if(hull.empty())
        return cv::Point2f(0, 0);

    cv::Moments moments = cv::moments(hull);
    return cv::Point2f(static_cast<float>(moments.m10 / moments.m00),
                       static_cast<float>(moments.m01 / moments.m00));
}
