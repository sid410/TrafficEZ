#ifndef HULLTRACKER_H
#define HULLTRACKER_H

#include "HullTrackable.h"

/**
 * @brief Class for tracking convex hulls.
 * Optionally, you can specify in the constructor the following:
 * @param maxDiffDistance Maximum distance difference for Centroid Tracking.
 * @param hullAreaThreshold Area threshold for hull similarity comparison.
 * @param boundaryCushionPixels boundary line cushion (pixels) to start considering exit hulls.
 * @param maxFramesNotSeen Maximum number of frames a hull can be not seen before removal.
 * @param maxId Maximum ID value for trackables before resetting the count.
 */
class HullTracker
{
public:
    HullTracker(double maxDiffDistance = 100.0,
                float hullAreaThreshold = 0.5,
                int boundaryCushionPixels = 20,
                int maxFramesNotSeen = 3,
                int maxId = 1000);

    void initExitBoundaryLine(int lineY) const;
    void update(const std::vector<std::vector<cv::Point>>& newHulls);

    const std::unordered_map<int, std::shared_ptr<HullTrackable>>&
    getTrackedHulls() const;

    float getTotalHullArea() const;
    float calculateAllAveragedSpeed() const;
    void resetTrackerVariables();

    void drawTrackedHulls(cv::Mat& frame) const;
    void drawLanesInfo(cv::Mat& frame, int laneLength, int laneWidth) const;

private:
    const double maxDiffDistance;
    const float hullAreaThreshold;
    const int boundaryCushionPixels;
    const int maxFramesNotSeen;
    const int maxId;

    int currentId;
    int hullCount;
    float totalHullArea;
    float totalAverageSpeed;

    mutable int boundaryLineY;
    std::unordered_map<int, std::shared_ptr<HullTrackable>> trackedHulls;

    void matchAndUpdateTrackables(
        const std::vector<std::vector<cv::Point>>& newHulls,
        std::vector<bool>& matched);

    void createAndAddNewTrackables(
        const std::vector<std::vector<cv::Point>>& newHulls,
        const std::vector<bool>& matched);

    void removeStaleTrackables();
    void processCrossedTrackables();
};

#endif