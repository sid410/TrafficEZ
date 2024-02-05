#include "HullTracker.h"
#include <memory>
#include <opencv2/opencv.hpp>
#include <unordered_map>
#include <vector>

HullTracker::HullTracker(double maxDiffDistance,
                         float hullAreaThreshold,
                         int boundaryCushionPixels,
                         int maxFramesNotSeen,
                         int maxId)
    : maxDiffDistance(maxDiffDistance)
    , hullAreaThreshold(hullAreaThreshold)
    , boundaryCushionPixels(boundaryCushionPixels)
    , maxFramesNotSeen(maxFramesNotSeen)
    , maxId(maxId)
    , currentId(0)
    , hullCount(0)
    , totalHullArea(0)
    , totalAverageSpeed(0)
    , boundaryLineY(0)
{
    trackedHulls.clear();
}

/**
 * @brief Initializes the exit boundary line for hull tracking.
 * @param lineY Y-coordinate of the boundary line.
 */
void HullTracker::initBoundaryLine(int lineY) const
{
    boundaryLineY = lineY;
}

/**
 * @brief Updates the tracked hulls with newly detected hulls.
 * @param newHulls New hull points to track and update.
 */
void HullTracker::update(const std::vector<std::vector<cv::Point>>& newHulls)
{
    std::vector<bool> matched(newHulls.size(), false);

    matchAndUpdateTrackables(newHulls, matched);

    removeStaleTrackables();
    processCrossedTrackables();

    createAndAddNewTrackables(newHulls, matched);
}

/**
 * @brief Retrieves the currently tracked hulls.
 * @return A map (dictionary) of tracked hulls keyed by their IDs.
 */
const std::unordered_map<int, std::shared_ptr<HullTrackable>>&
HullTracker::getTrackedHulls() const
{
    return trackedHulls;
}

/**
 * @brief Gets the total hull area.
 * @return Accumulated area of all exited tracked hulls (sq. pixels).
 */
float HullTracker::getTotalHullArea() const
{
    return totalHullArea;
}

/**
 * @brief Calculates the averaged speed of all exited tracked hulls.
 * @return Averaged speed of all exited tracked hulls (pixels/second).
 */
float HullTracker::calculateAllAveragedSpeed() const
{
    return totalAverageSpeed / hullCount;
}

/**
 * @brief Matches new hulls with existing tracked hulls and updates them.
 * @param newHulls New hulls detected in the current frame.
 * @param matched Vector (list) indicating which new hulls have been matched.
 */
void HullTracker::matchAndUpdateTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    std::vector<bool>& matched)
{
    for(auto& trackablePair : trackedHulls)
    {
        auto& trackable = trackablePair.second;
        float trackableArea = trackable->getHullArea();
        bool isMatched = false;

        for(size_t i = 0; i < newHulls.size(); ++i)
        {
            if(matched[i])
                continue;

            float newHullArea = cv::contourArea(newHulls[i]);
            float diffArea = std::abs(trackableArea - newHullArea);

            // check if hull areas are similar within a threshold
            if(diffArea / trackableArea > hullAreaThreshold)
                continue;

            // then check how far they are, i.e. Centroid Tracking
            float diffDistance =
                cv::norm(trackable->calculateCentroid() -
                         HullTrackable::computeCentroid(newHulls[i]));

            if(diffDistance < maxDiffDistance)
            {
                trackable->setHullPoints(newHulls[i]);
                trackable->setFramesSinceSeen(0);

                matched[i] = true;
                isMatched = true;
                break;
            }
        }

        // for the existing trackables that did not find a match in this loop
        if(!isMatched)
        {
            trackable->setFramesSinceSeen(trackable->getFramesSinceSeen() + 1);
        }
    }
}

/**
 * @brief Creates and adds new trackables for hulls 
 * that weren't matched with existing trackables.
 * @param newHulls New hulls detected in the current frame.
 * @param matched Vector (list) indicating which new hulls have been matched.
 */
void HullTracker::createAndAddNewTrackables(
    const std::vector<std::vector<cv::Point>>& newHulls,
    const std::vector<bool>& matched)
{
    for(size_t i = 0; i < newHulls.size(); ++i)
    {
        if(matched[i])
            continue;

        // reset ID, should not be a problem if only tracking few IDs at a time
        if(currentId > maxId)
        {
            currentId = 0;
        }

        // check if the hull is too near the boundary
        cv::Point2f centroid = HullTrackable::computeCentroid(newHulls[i]);
        if(centroid.y > boundaryLineY - boundaryCushionPixels)
            continue;

        auto newTrackable =
            std::make_shared<HullTrackable>(currentId++, newHulls[i]);
        trackedHulls[newTrackable->getTrackableId()] = newTrackable;
    }
}

/**
 * @brief Processes hulls that have crossed the exit boundary.
 * That is, updating data needed to estimate traffic density
 * then removing them from the tracking list, indicating successful exit.
 */
void HullTracker::processCrossedTrackables()
{
    std::vector<int> hullsToRemove;

    for(const auto& trackablePair : trackedHulls)
    {
        const auto& trackable = trackablePair.second;

        // when trackable exits the boundary line,
        if(trackable->calculateCentroid().y >
           boundaryLineY - boundaryCushionPixels)
        {
            // update the following data
            hullCount++;
            totalHullArea += trackable->getHullArea();
            totalAverageSpeed += trackable->calculateAverageSpeed();

            hullsToRemove.push_back(trackablePair.first);
        }
    }

    // remove the exited hulls from tracking list
    for(int id : hullsToRemove)
    {
        trackedHulls.erase(id);
    }
}

/**
 * @brief Removes trackables that have not been tracked
 * for a specified number of frames.
 */
void HullTracker::removeStaleTrackables()
{
    std::vector<int> idsToRemove;

    for(const auto& pair : trackedHulls)
    {
        if(pair.second->getFramesSinceSeen() > maxFramesNotSeen)
        {
            idsToRemove.push_back(pair.first);
        }
    }

    for(int id : idsToRemove)
    {
        trackedHulls.erase(id);
    }
}

/**
 * @brief Draws the tracked hulls.
 * @param frame The frame on which the hulls will be drawn.
 */
void HullTracker::drawTrackedHulls(cv::Mat& frame) const
{
    for(const auto& pair : trackedHulls)
    {
        const auto& trackable = pair.second;
        std::vector<std::vector<cv::Point>> hullVec = {
            trackable->getHullPoints()};
        cv::drawContours(frame, hullVec, -1, cv::Scalar(0, 255, 0), 2);

        cv::Point2f idPos = trackable->calculateCentroid();
        std::string idText = std::to_string(trackable->getTrackableId());
        cv::putText(frame,
                    idText,
                    idPos,
                    cv::FONT_HERSHEY_SIMPLEX,
                    0.5,
                    cv::Scalar(255, 255, 255),
                    2);
    }
}

/**
 * @brief Draws information about the lanes (only for debugging).
 * @param frame The frame on which the lane information will be drawn.
 * @param laneLength The total length of the lanes.
 * @param laneWidth The total width of the lanes.
 */
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

    std::string countText = "Count: " + std::to_string(hullCount);
    textSize =
        cv::getTextSize(countText, cv::FONT_HERSHEY_SIMPLEX, 1.0, 2, nullptr);
    cv::Point countPos(0, frame.rows - 15);
    cv::putText(frame,
                countText,
                countPos,
                cv::FONT_HERSHEY_SIMPLEX,
                1,
                cv::Scalar(0, 255, 0),
                2);
}