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

bool VideoStreamer::openVideoStream(const std::string& streamName)
{
    stream.open(streamName);

    if(!stream.isOpened())
    {
        std::cerr << "Error: Unable to open stream: " << streamName << "\n";
        return false;
    }

    return true;
}

void VideoStreamer::constructStreamWindow(const std::string& windowName)
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

bool VideoStreamer::readCalibrationPoints(const std::string& yamlFilename)
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

    // Determine the longer length and width
    double length1 = cv::norm(srcPoints[0] - srcPoints[1]);
    double length2 = cv::norm(srcPoints[1] - srcPoints[2]);
    double width1 = cv::norm(srcPoints[1] - srcPoints[3]);
    double width2 = cv::norm(srcPoints[2] - srcPoints[3]);

    double maxLength = std::max(length1, length2);
    double maxWidth = std::max(width1, width2);

    dstPoints = {cv::Point2f(0, 0),
                 cv::Point2f(maxLength - 1, 0),
                 cv::Point2f(0, maxWidth - 1),
                 cv::Point2f(maxLength - 1, maxWidth - 1)};

    perspectiveMatrix = cv::getPerspectiveTransform(srcPoints, dstPoints);
    perspectiveMatrixInitialized = true;
}

void VideoStreamer::warpFrame(const cv::Mat& inputFrame, cv::Mat& warpedFrame)
{
    cv::warpPerspective(inputFrame,
                        warpedFrame,
                        perspectiveMatrix,
                        cv::Size(static_cast<int>(dstPoints[1].x),
                                 static_cast<int>(dstPoints[2].y)));
}