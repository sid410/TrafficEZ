#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include "FPSHelper.h"
#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

class HullTrackable
{
private:
    FPSHelper fpsHelper;

    const int id; // Unique ID for each trackable, made constant
    std::vector<cv::Point> hull; // Hull of the object

    int framesSinceLastSeen; // Counter for frames since last seen

    mutable cv::Point2f
        centroid; // Centroid of the hull, mutable for lazy calculation
    mutable bool centroidCalculated; // Flag to check if centroid is calculated

    float avgSpeed; // Average speed in pixels per second

    cv::Point2f startPoint;
    cv::Point2f endPoint;

public:
    HullTrackable(int id, const std::vector<cv::Point>& hull);

    int getId() const;

    void setHull(const std::vector<cv::Point>& newHull);
    const std::vector<cv::Point>& getHull() const;
    float getHullArea() const;

    int getFramesSinceLastSeen() const;
    void setFramesSinceLastSeen(int newFramesSinceLastSeen);

    cv::Point2f getCentroid() const; // Now calculates centroid lazily

    float getAvgSpeed();

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);
};

#endif
