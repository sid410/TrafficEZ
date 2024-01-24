#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include "FPSHelper.h"
#include <opencv2/opencv.hpp>
#include <vector>

/**
 * @brief A HullTrackable object with a specified ID and hull shape.
 * @param id The unique identifier for the trackable object.
 * @param initHull The initial hull points of the object.
 */
class HullTrackable
{
public:
    HullTrackable(int id, const std::vector<cv::Point>& initHull);

    int getTrackableId() const;
    float getHullArea() const;
    const std::vector<cv::Point>& getHullPoints() const;

    int getFramesSinceSeen() const;

    void setFramesSinceSeen(int frames);
    void setHullPoints(const std::vector<cv::Point>& newHull);

    float calculateAverageSpeed();
    cv::Point2f calculateCentroid() const;

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);

private:
    const int trackableId;

    std::vector<cv::Point> hullPoints;
    cv::Point2f trackingStartPoint;
    cv::Point2f trackingEndPoint;

    int framesSinceSeen;
    float averageSpeed;

    mutable cv::Point2f hullCentroid;
    mutable bool isCentroidCalculated;

    FPSHelper fpsHelper;
};

#endif
