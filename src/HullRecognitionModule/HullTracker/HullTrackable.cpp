#include "HullTrackable.h"

HullTrackable::HullTrackable()
    : id(-1)
    , framesSinceLastSeen(0)
    , centroid(cv::Point2f(0.0f, 0.0f))
{}

HullTrackable::HullTrackable(int newId, const std::vector<cv::Point>& newHull)
    : id(newId)
    , hull(newHull)
    , framesSinceLastSeen(0)
    , centroid(computeCentroid(newHull))
{}

int HullTrackable::getId() const
{
    return id;
}

void HullTrackable::setId(int newId)
{
    id = newId;
}

const std::vector<cv::Point>& HullTrackable::getHull() const
{
    return hull;
}

void HullTrackable::setHull(const std::vector<cv::Point>& newHull)
{
    hull = newHull;
    centroid = computeCentroid(newHull);
}

int HullTrackable::getFramesSinceLastSeen() const
{
    return framesSinceLastSeen;
}

void HullTrackable::setFramesSinceLastSeen(int newFramesSinceLastSeen)
{
    framesSinceLastSeen = newFramesSinceLastSeen;
}

cv::Point2f HullTrackable::getCentroid() const
{
    return centroid;
}

void HullTrackable::setCentroid(const cv::Point2f& newCentroid)
{
    centroid = newCentroid;
}

cv::Point2f HullTrackable::computeCentroid(const std::vector<cv::Point>& hull)
{
    if(hull.empty())
        return cv::Point2f(0, 0);

    cv::Moments moments = cv::moments(hull);
    return cv::Point2f(static_cast<float>(moments.m10 / moments.m00),
                       static_cast<float>(moments.m01 / moments.m00));
}
