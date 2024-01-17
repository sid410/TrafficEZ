#include "HullTracker.h"
#include "HullTrackable.h"
#include <opencv2/opencv.hpp>

HullTracker::HullTracker()
    : maxDistance(100.0)
    , maxFramesNotSeen(3)
{}

void HullTracker::update(const std::vector<std::vector<cv::Point>>& newHulls)
{
    std::vector<bool> matched(newHulls.size(), false);

    matchAndUpdateTrackables(newHulls, matched);
    addNewTrackables(newHulls, matched);
    removeStaleTrackables();
}

void HullTracker::matchAndUpdateTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    std::vector<bool>& matched)
{
    // Iterate over trackedHulls
    for(auto& trackablePair : trackedHulls)
    {
        HullTrackable& trackable = trackablePair.second;
        bool isMatched = false;

        for(size_t i = 0; i < newHulls.size(); ++i)
        {
            if(matched[i])
                continue; // Skip already matched hulls

            if(cv::norm(trackable.centroid - HullTrackable::computeCentroid(
                                                 newHulls[i])) < maxDistance)
            {
                // Update existing trackable with new hull
                trackable.hull = newHulls[i];
                trackable.centroid =
                    HullTrackable::computeCentroid(newHulls[i]);
                trackable.framesSinceLastSeen = 0;
                matched[i] = true;
                isMatched = true;
                break;
            }
        }

        if(!isMatched)
        {
            // Increment the 'not seen' counter if no match found
            trackable.framesSinceLastSeen++;
        }
    }
}

void HullTracker::addNewTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    const std::vector<bool>& matched)
{
    for(size_t i = 0; i < newHulls.size(); ++i)
    {
        if(!matched[i])
        {
            HullTrackable newTrackable(newHulls[i]);
            trackedHulls[newTrackable.id] = newTrackable;
        }
    }
}

void HullTracker::removeStaleTrackables()
{
    for(auto it = trackedHulls.begin(); it != trackedHulls.end();)
    {
        if(it->second.framesSinceLastSeen > maxFramesNotSeen)
        {
            it = trackedHulls.erase(it);
        }
        else
        {
            ++it;
        }
    }
}

const std::unordered_map<int, HullTrackable>&
HullTracker::getTrackedHulls() const
{
    return trackedHulls;
}

void HullTracker::drawTrackedHulls(cv::Mat& frame) const
{
    for(const auto& pair : trackedHulls)
    {
        const auto& id = pair.first;
        const auto& trackable = pair.second;

        std::vector<std::vector<cv::Point>> hullVec = {trackable.hull};
        cv::drawContours(frame, hullVec, -1, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame,
                    std::to_string(id),
                    trackable.centroid,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(255, 255, 255),
                    2);
    }
}