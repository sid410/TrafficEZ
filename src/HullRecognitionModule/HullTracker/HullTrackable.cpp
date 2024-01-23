#include "HullTrackable.h"
#include "FPSHelper.h"

HullTrackable::HullTrackable(int newId, const std::vector<cv::Point>& newHull)
    : id(newId)
    , hull(newHull)
    , framesSinceLastSeen(0)
    , centroidCalculated(false)
    , avgSpeed(0)
    , movingAvgCounter(2)
    , speedInitialized(false)
    , lastDistance(0)
{}

int HullTrackable::getId() const
{
    return id;
}

const std::vector<cv::Point>& HullTrackable::getHull() const
{
    return hull;
}

float HullTrackable::getHullArea() const
{
    return cv::contourArea(hull);
}

void HullTrackable::setHull(const std::vector<cv::Point>& newHull)
{
    hull = newHull;
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

float HullTrackable::getAvgSpeed() const
{
    return avgSpeed;
}

void HullTrackable::setAvgSpeed(float deltaDistance)
{
    auto currentTime = std::chrono::steady_clock::now();

    if(!speedInitialized)
    {
        lastUpdateTime = currentTime;
        speedInitialized = true;
        avgSpeed = 0;
        movingAvgCounter = 2;
    }
    else
    {
        std::chrono::duration<float> timeElapsed = currentTime - lastUpdateTime;
        float speed =
            deltaDistance / timeElapsed.count(); // Speed in pixels/second

        // Update average speed
        avgSpeed =
            (avgSpeed == 0) ? speed : (avgSpeed + speed) / movingAvgCounter;

        // Update the last update time and distance
        lastUpdateTime = currentTime;
        movingAvgCounter++;
    }
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
