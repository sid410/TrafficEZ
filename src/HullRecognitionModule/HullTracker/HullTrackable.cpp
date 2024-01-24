#include "HullTrackable.h"

HullTrackable::HullTrackable(int id, const std::vector<cv::Point>& initHull)
    : trackableId(id)
    , hullPoints(initHull)
    , trackingStartPoint(computeCentroid(hullPoints))
    , trackingEndPoint(0, 0)
    , framesSinceSeen(0)
    , averageSpeed(0.0)
    , hullCentroid(0, 0)
    , isCentroidCalculated(false)
    , fpsHelper()
{
    fpsHelper.startSample();
}

/**
 * @brief Gets the trackable object's ID.
 * @return The ID of the trackable object.
 */
int HullTrackable::getTrackableId() const
{
    return trackableId;
}

/**
 * @brief Calculates and returns the area of the trackable object's hull.
 * @return The area of the hull.
 */
float HullTrackable::getHullArea() const
{
    return cv::contourArea(hullPoints);
}

/**
 * @brief Gets the current hull points of the trackable object.
 * @return A constant reference to the vector of hull points.
 */
const std::vector<cv::Point>& HullTrackable::getHullPoints() const
{
    return hullPoints;
}

/**
 * @brief Gets the number of frames since the object was last seen.
 * @return The number of frames since last seen.
 */
int HullTrackable::getFramesSinceSeen() const
{
    return framesSinceSeen;
}

/**
 * @brief Sets the number of frames since the trackable object was last seen.
 * @param untrackedFrames The number of frames the object was untracked.
 */
void HullTrackable::setFramesSinceSeen(int untrackedFrames)
{
    framesSinceSeen = untrackedFrames;
}

/**
 * @brief Updates the hull points of the trackable object.
 * @param newHull The new set of hull points.
 */
void HullTrackable::setHullPoints(const std::vector<cv::Point>& newHull)
{
    hullPoints = newHull;
    isCentroidCalculated = false;
}

/**
 * @brief Calculates the average speed of the trackable object based on its movement.
 * @return The average speed of the object, in pixels/second
 */
float HullTrackable::calculateAverageSpeed()
{
    float travelTime = fpsHelper.endSample() / 1000.0;
    trackingEndPoint = computeCentroid(hullPoints);

    averageSpeed = cv::norm(trackingStartPoint - trackingEndPoint) / travelTime;

    return averageSpeed;
}

/**
 * @brief Lazily calculates and returns the centroid of the trackable object.
 * @return The centroid of the hull.
 */
cv::Point2f HullTrackable::calculateCentroid() const
{
    if(!isCentroidCalculated)
    {
        hullCentroid = computeCentroid(hullPoints);
        isCentroidCalculated = true;
    }
    return hullCentroid;
}

/**
 * @brief Static method to compute the centroid of a given hull.
 * This can also be used outside the HullTrackable Class, e.g. HullTracker.
 * @param hull The hull for which to compute the centroid.
 * @return The centroid points (x, y) of the given hull.
 */
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
