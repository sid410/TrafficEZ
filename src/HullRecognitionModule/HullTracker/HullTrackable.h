#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include <chrono>
#include <opencv2/opencv.hpp>
#include <vector>

class HullTrackable
{
private:
    const int id; // Unique ID for each trackable, made constant
    std::vector<cv::Point> hull; // Hull of the object
    int framesSinceLastSeen; // Counter for frames since last seen
    mutable cv::Point2f
        centroid; // Centroid of the hull, mutable for lazy calculation
    mutable bool centroidCalculated; // Flag to check if centroid is calculated

    float avgSpeed; // Average speed in pixels per second
    float lastDistance;
    int movingAvgCounter;
    bool speedInitialized;
    std::chrono::steady_clock::time_point
        lastUpdateTime; // Time of the last update

public:
    HullTrackable(int id, const std::vector<cv::Point>& hull);

    int getId() const;
    const std::vector<cv::Point>& getHull() const;

    void
    setHull(const std::vector<cv::Point>& newHull); // Pass by const reference
    void
    setHull(std::vector<cv::Point>&& newHull); // Overload for move semantics

    int getFramesSinceLastSeen() const;
    void setFramesSinceLastSeen(int newFramesSinceLastSeen);

    cv::Point2f getCentroid() const; // Now calculates centroid lazily

    float getAvgSpeed() const;
    void setAvgSpeed(float newDistance);

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);
};

#endif
