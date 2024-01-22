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
    removeStaleTrackables();
    addNewTrackables(newHulls, matched);
}

void HullTracker::matchAndUpdateTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    std::vector<bool>& matched)
{
    for(auto& trackablePair : trackedHulls)
    {
        auto& trackable = trackablePair.second;
        bool isMatched = false;

        for(size_t i = 0; i < newHulls.size(); ++i)
        {
            if(matched[i])
                continue;

            float distance =
                cv::norm(trackable->getCentroid() -
                         HullTrackable::computeCentroid(newHulls[i]));

            if(distance < maxDistance)
            {
                trackable->setAvgSpeed(distance);
                trackable->setHull(newHulls[i]);
                trackable->setFramesSinceLastSeen(0);

                matched[i] = true;
                isMatched = true;
                break;
            }
        }

        if(!isMatched)
        {
            trackable->setFramesSinceLastSeen(
                trackable->getFramesSinceLastSeen() + 1);
        }
    }
}

void HullTracker::addNewTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    const std::vector<bool>& matched)
{
    for(size_t i = 0; i < newHulls.size(); ++i)
    {
        if(matched[i])
            continue;

        if(nextId > maxId)
        {
            nextId = 0;
        }

        auto newTrackable =
            std::make_shared<HullTrackable>(nextId++, newHulls[i]);
        trackedHulls[newTrackable->getId()] = newTrackable;
    }
}

void HullTracker::removeStaleTrackables()
{
    std::vector<int> idsToRemove;
    for(const auto& pair : trackedHulls)
    {
        if(pair.second->getFramesSinceLastSeen() > maxFramesNotSeen)
        {
            idsToRemove.push_back(pair.first);
        }
    }

    for(int id : idsToRemove)
    {
        trackedHulls.erase(id);
    }
}

const std::unordered_map<int, std::shared_ptr<HullTrackable>>&
HullTracker::getTrackedHulls() const
{
    return trackedHulls;
}

void HullTracker::drawTrackedHulls(cv::Mat& frame) const
{
    for(const auto& pair : trackedHulls)
    {
        const auto& trackable = pair.second;
        std::vector<std::vector<cv::Point>> hullVec = {trackable->getHull()};
        cv::drawContours(frame, hullVec, -1, cv::Scalar(0, 255, 0), 2);

        cv::Point2f idPos = trackable->getCentroid();
        std::string idText = std::to_string(trackable->getId());
        cv::putText(frame,
                    idText,
                    idPos,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(255, 255, 255),
                    2);

        cv::Point2f speedPos =
            idPos + cv::Point2f(0, 15); // Position below the ID

        // print the average speed
        int intSpeed = static_cast<int>(trackable->getAvgSpeed());
        std::string speedText = std::to_string(intSpeed) + " px/s";

        cv::putText(frame,
                    speedText,
                    speedPos,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.8,
                    cv::Scalar(0, 255, 255),
                    2);
    }
}

// temporary, just for debugging
void HullTracker::drawLanesInfo(cv::Mat& frame,
                                int laneLength,
                                int laneWidth) const
{
    std::string laneLengthText =
        "Lane Length: " + std::to_string(laneLength) + " m";
    cv::Size textSize = cv::getTextSize(
        laneLengthText, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
    cv::Point laneLengthPos(frame.cols - textSize.width / 2, textSize.height);
    cv::putText(frame,
                laneLengthText,
                laneLengthPos,
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                2);

    std::string laneWidthText =
        "<-Lane Width: " + std::to_string(laneWidth) + " m->";
    textSize = cv::getTextSize(
        laneWidthText, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
    cv::Point laneWidthPos((frame.cols - textSize.width / 2) / 2,
                           frame.rows - 15);
    cv::putText(frame,
                laneWidthText,
                laneWidthPos,
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                cv::Scalar(0, 0, 255),
                2);
}