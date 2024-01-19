#include "HullTrackable.h"

HullTrackable::HullTrackable(int newId, const std::vector<cv::Point>& newHull)
    : id(newId)
    , hull(newHull)
    , framesSinceLastSeen(0)
    , centroidCalculated(false)
{}

int HullTrackable::getId() const
{
    return id;
}

const std::vector<cv::Point>& HullTrackable::getHull() const
{
    return hull;
}

void HullTrackable::setHull(const std::vector<cv::Point>& newHull)
{
    hull = newHull;
    centroidCalculated = false;
}

void HullTrackable::setHull(std::vector<cv::Point>&& newHull)
{
    hull = std::move(newHull);
    centroidCalculated = false;
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
    if(!centroidCalculated)
    {
        centroid = computeCentroid(hull);
        centroidCalculated = true;
    }
    return centroid;
}

cv::Point2f HullTrackable::computeCentroid(const std::vector<cv::Point>& hull)
{
    if(hull.empty())
    {
        return cv::Point2f(0, 0);
    }

    cv::Moments moments = cv::moments(hull);
    return cv::Point2f(static_cast<float>(moments.m10 / moments.m00),
                       static_cast<float>(moments.m01 / moments.m00));
}
