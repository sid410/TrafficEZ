#ifndef HULLTRACKER_H
#define HULLTRACKER_H

#include "HullTrackable.h"
#include <memory>
#include <unordered_map>
#include <vector>

class HullTracker
{
private:
    std::unordered_map<int, std::shared_ptr<HullTrackable>> trackedHulls;

    const double maxDistance;
    const float thresholdArea;
    const int pixelBoundaryCushion;
    const int maxFramesNotSeen;
    const int maxId;

    int nextId;
    int hullCount;
    int boundLineY;

    void matchAndUpdateTrackables(
        const std::vector<std::vector<cv::Point>>& newHulls,
        std::vector<bool>& matched);
    void addNewTrackables(const std::vector<std::vector<cv::Point>>& newHulls,
                          const std::vector<bool>& matched);

    void countHullsCrossed();
    void removeStaleTrackables();

public:
    HullTracker(double maxDistance = 100.0,
                float thresholdArea = 0.5,
                int pixelBoundaryCushion = 20,
                int maxFramesNotSeen = 3,
                int maxId = 1000);
    void update(const std::vector<std::vector<cv::Point>>& newHulls);
    const std::unordered_map<int, std::shared_ptr<HullTrackable>>&
    getTrackedHulls() const;
    void drawTrackedHulls(cv::Mat& frame) const;
    void drawLanesInfo(cv::Mat& frame, int laneLength, int laneWidth) const;
};

#endif
