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

void VideoStreamer::initializePerspectiveTransform(
    TransformPerspective& perspective)
{
    if(!readCalibSuccess)
    {
        std::cerr << "Error: Not calibrated.\n";
        return;
    }

    if(srcPoints.size() < 4)
    {
        std::cerr << "Error: Insufficient calibration points.\n";
        return;
    }

    perspective.initialize(srcPoints, dstPoints, perspectiveMatrix);
    perspectiveMatrixInitialized = true;
}

bool VideoStreamer::applyFrameRoi(cv::Mat& frame,
                                  cv::Mat& preprocessedFrame,
                                  TransformPerspective& perspective)
{
    if(!perspectiveMatrixInitialized)
    {
        std::cerr << "Error: Failed to initialize.\n";
        return false;
    }

    if(!getNextFrame(frame))
    {
        std::cerr << "Error: Unable to retrieve the next frame.\n";
        return false;
    }

    perspective.apply(frame, preprocessedFrame, perspectiveMatrix, dstPoints);

    return true;
}