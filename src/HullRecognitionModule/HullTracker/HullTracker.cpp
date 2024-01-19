#include "HullTracker.h"
#include <opencv2/opencv.hpp>

HullTracker::HullTracker(double maxDist, int maxFrames, int maxIdValue)
    : maxDistance(maxDist)
    , maxFramesNotSeen(maxFrames)
    , maxId(maxIdValue)
    , nextId(0)
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
    for(auto& trackablePair : trackedHulls)
    {
        HullTrackable& trackable = trackablePair.second;
        bool isMatched = false;

        for(size_t i = 0; i < newHulls.size(); ++i)
        {
            if(matched[i])
                continue; // Skip already matched hulls

            if(cv::norm(trackable.getCentroid() -
                        HullTrackable::computeCentroid(newHulls[i])) <
               maxDistance)
            {
                trackable.setHull(newHulls[i]);
                trackable.setCentroid(
                    HullTrackable::computeCentroid(newHulls[i]));
                trackable.setFramesSinceLastSeen(0);
                matched[i] = true;
                isMatched = true;
                break;
            }
        }

        if(!isMatched)
        {
            trackable.setFramesSinceLastSeen(
                trackable.getFramesSinceLastSeen() + 1);
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
            if(nextId > maxId)
            {
                nextId = 0;
            }

            HullTrackable newTrackable(nextId++, newHulls[i]);
            trackedHulls[newTrackable.getId()] = newTrackable;
        }
    }
}

void HullTracker::removeStaleTrackables()
{
    for(auto it = trackedHulls.begin(); it != trackedHulls.end();)
    {
        if(it->second.getFramesSinceLastSeen() > maxFramesNotSeen)
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
        const HullTrackable& trackable = pair.second;
        std::vector<std::vector<cv::Point>> hullVec = {trackable.getHull()};
        cv::drawContours(frame, hullVec, -1, cv::Scalar(0, 255, 0), 2);
        cv::putText(frame,
                    std::to_string(trackable.getId()),
                    trackable.getCentroid(),
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(255, 255, 255),
                    2);
    }
}
