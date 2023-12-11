#include "VideoStreamer.h"
#include <fstream>
#include <iostream>
#include <yaml-cpp/yaml.h>

VideoStreamer::VideoStreamer()
    : perspectiveMatrixInitialized(false)
    , readCalibSuccess(false)
{}

VideoStreamer::~VideoStreamer()
{
    stream.release();
}

bool VideoStreamer::openVideoStream(const cv::String& streamName)
{
    stream.open(streamName);

    if(!stream.isOpened())
    {
        std::cerr << "Error: Unable to open stream: " << streamName << "\n";
        return false;
    }

    return true;
}

void VideoStreamer::constructStreamWindow(const cv::String& windowName)
{
    originalWidth = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_WIDTH));
    originalHeight = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, originalWidth, originalHeight);
}

bool VideoStreamer::getNextFrame(cv::Mat& frame)
{
    stream.read(frame);
    if(frame.empty())
    {
        return false;
    }

    return true;
}

bool VideoStreamer::readCalibrationPoints(const cv::String& yamlFilename)
{
    std::ifstream fin(yamlFilename);

    try
    {
        if(!fin.is_open())
        {
            std::cerr << "Error: Failed to open calibration file.\n";
            return false;
        }

        YAML::Node yamlNode = YAML::Load(fin);

        const YAML::Node& pointsNode = yamlNode["calibration_points"];
        if(!pointsNode || !pointsNode.IsSequence())
        {
            std::cerr << "Error: Calibration points not found or not in the "
                         "correct format.\n";
            return false;
        }

        srcPoints.clear();

        for(const auto& point : pointsNode)
        {
            double x = point["x"].as<double>();
            double y = point["y"].as<double>();
            srcPoints.emplace_back(x, y);
        }

        fin.close();
        readCalibSuccess = true;
    }
    catch(const YAML::Exception& e)
    {
        std::cerr << "Error while parsing YAML: " << e.what() << "\n";
        fin.close();
        readCalibSuccess = false;
    }

    return readCalibSuccess;
}

void VideoStreamer::initializePerspectiveTransform()
{
    if(!readCalibSuccess)
    {
        return;
    }

    if(srcPoints.size() < 4)
    {
        std::cerr << "Error: Insufficient calibration points.\n";
        return;
    }

    /// Sort the points based on y-coordinates
    std::sort(srcPoints.begin(),
              srcPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.y < b.y; });

    // Split the sorted points into top and bottom
    std::vector<cv::Point2f> topPoints, bottomPoints;
    topPoints.push_back(srcPoints[0]);
    topPoints.push_back(srcPoints[1]);
    bottomPoints.push_back(srcPoints[2]);
    bottomPoints.push_back(srcPoints[3]);

    // Sort the top and bottom points based on x-coordinates
    std::sort(topPoints.begin(),
              topPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });
    std::sort(bottomPoints.begin(),
              bottomPoints.end(),
              [](cv::Point2f a, cv::Point2f b) { return a.x < b.x; });

    // Combine the top and bottom points
    std::vector<cv::Point2f> sortedPoints = {
        topPoints[0], topPoints[1], bottomPoints[0], bottomPoints[1]};

    double length1 = cv::norm(sortedPoints[0] - sortedPoints[1]);
    double length2 = cv::norm(sortedPoints[1] - sortedPoints[2]);
    double width1 = cv::norm(sortedPoints[1] - sortedPoints[3]);
    double width2 = cv::norm(sortedPoints[2] - sortedPoints[3]);

    double maxLength = std::max(length1, length2);
    double maxWidth = std::max(width1, width2);

    // Destination points for the bird's eye view
    dstPoints = {cv::Point2f(0, 0),
                 cv::Point2f(maxLength - 1, 0),
                 cv::Point2f(0, maxWidth - 1),
                 cv::Point2f(maxLength - 1, maxWidth - 1)};

    perspectiveMatrix = cv::getPerspectiveTransform(sortedPoints, dstPoints);
    perspectiveMatrixInitialized = true;
}

bool VideoStreamer::warpFrame(cv::Mat& frame, cv::Mat& warpedFrame)
{
    if(!perspectiveMatrixInitialized)
    {
        std::cerr << "Error: Failed to initialize perspective matrix.\n";
        return false;
    }

    if(!getNextFrame(frame))
    {
        std::cerr << "Error: Unable to retrieve the next frame.\n";
        return false;
    }

    cv::warpPerspective(frame,
                        warpedFrame,
                        perspectiveMatrix,
                        cv::Size(static_cast<int>(dstPoints[1].x),
                                 static_cast<int>(dstPoints[2].y)));

    return true;
}