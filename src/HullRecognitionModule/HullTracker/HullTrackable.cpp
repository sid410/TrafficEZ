#include "HullTrackable.h"

HullTrackable::HullTrackable(int id, const std::vector<cv::Point>& hull)
    : trackableId(id)
    , hullPoints(hull)
    , framesSinceSeen(0)
    , hullCentroid(0, 0)
    , isCentroidCalculated(false)
    , averageSpeed(0.0)
    , trackingStartPoint(computeCentroid(hullPoints))
    , trackingEndPoint(0, 0)
    , fpsHelper()
{
    fpsHelper.startSample();
}

int HullTrackable::getTrackableId() const
{
    return trackableId;
}

const std::vector<cv::Point>& HullTrackable::getHullPoints() const
{
    return hullPoints;
}

float HullTrackable::getHullArea() const
{
    return cv::contourArea(hullPoints);
}

void HullTrackable::setHullPoints(const std::vector<cv::Point>& newHull)
{
    hullPoints = newHull;
    isCentroidCalculated = false;
}

int HullTrackable::getFramesSinceSeen() const
{
    return framesSinceSeen;
}

void HullTrackable::setFramesSinceSeen(int untrackedFrames)
{
    framesSinceSeen = untrackedFrames;
}

cv::Point2f HullTrackable::calculateCentroid() const
{
    if(!isCentroidCalculated)
    {
        hullCentroid = computeCentroid(hullPoints);
        isCentroidCalculated = true;
    }
    return hullCentroid;
}

float HullTrackable::calculateAverageSpeed()
{
    float travelTime = fpsHelper.endSample() / 1000.0;
    trackingEndPoint = computeCentroid(hullPoints);

    averageSpeed = cv::norm(trackingStartPoint - trackingEndPoint) / travelTime;

    return averageSpeed;
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
