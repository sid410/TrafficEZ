#ifndef HULLTRACKABLE_H
#define HULLTRACKABLE_H

#include <opencv2/opencv.hpp>
#include <vector>

class HullTrackable
{
private:
    int id; // Unique ID for each trackable
    std::vector<cv::Point> hull; // Hull of the object
    int framesSinceLastSeen; // Counter for frames since last seen
    cv::Point2f centroid; // Centroid of the hull

public:
    HullTrackable();
    HullTrackable(int id, const std::vector<cv::Point>& hull);

    int getId() const;
    void setId(int newId);
    const std::vector<cv::Point>& getHull() const;
    void setHull(const std::vector<cv::Point>& newHull);
    int getFramesSinceLastSeen() const;
    void setFramesSinceLastSeen(int newFramesSinceLastSeen);
    cv::Point2f getCentroid() const;
    void setCentroid(const cv::Point2f& newCentroid);

    static cv::Point2f computeCentroid(const std::vector<cv::Point>& hull);
};

#endif
