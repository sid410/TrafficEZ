#include "HullTrackable.h"

HullTrackable::HullTrackable()
    : id(-1)
    , framesSinceLastSeen(0)
    , centroid(cv::Point2f(0.0f, 0.0f))
{}

HullTrackable::HullTrackable(int id, const std::vector<cv::Point>& hull)
    : id(id)
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
