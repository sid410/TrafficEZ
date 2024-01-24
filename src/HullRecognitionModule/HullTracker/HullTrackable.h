#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include "FPSHelper.h"
#include <opencv2/opencv.hpp>

class HullTrackable
{
public:
    HullTrackable(int id, const std::vector<cv::Point>& hull);

    int getTrackableId() const;
    const std::vector<cv::Point>& getHullPoints() const;
    void setHullPoints(const std::vector<cv::Point>& newHull);
    float getHullArea() const;
    cv::Point2f calculateCentroid() const;
    float calculateAverageSpeed();

    int getFramesSinceSeen() const;
    void setFramesSinceSeen(int frames);

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);

private:
    FPSHelper fpsHelper;
    const int trackableId;
    std::vector<cv::Point> hullPoints;
    int framesSinceSeen;
    mutable cv::Point2f hullCentroid;
    mutable bool isCentroidCalculated;
    float averageSpeed;
    cv::Point2f trackingStartPoint;
    cv::Point2f trackingEndPoint;
};

#endif
