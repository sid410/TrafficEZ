#ifndef HULLTRACKER_H
#define HULLTRACKER_H

#include "HullTrackable.h"
#include <unordered_map>
#include <vector>

class HullTracker
{
private:
    std::unordered_map<int, HullTrackable>
        trackedHulls; // Map to store tracked hulls
    const double maxDistance; // Max allowed distance for matching
    const int maxFramesNotSeen; // Frames to wait before removing a hull
    const int maxId; // Max ID to give to HullTrackable objects before resetting
    int nextId; // Counter for generating unique IDs for HullTrackables

    void matchAndUpdateTrackables(
        const std::vector<std::vector<cv::Point>>& newHulls,
        std::vector<bool>& matched);
    void addNewTrackables(const std::vector<std::vector<cv::Point>>& newHulls,
                          const std::vector<bool>& matched);
    void removeStaleTrackables();

public:
    HullTracker(double maxDistance = 100.0,
                int maxFramesNotSeen = 3,
                int maxId = 1000);
    void update(const std::vector<std::vector<cv::Point>>& newHulls);
    const std::unordered_map<int, HullTrackable>& getTrackedHulls() const;
    void drawTrackedHulls(cv::Mat& frame) const;
};

#endif
