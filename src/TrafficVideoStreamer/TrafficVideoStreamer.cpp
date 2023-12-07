#include "TrafficVideoStreamer.h"
#include <iostream>

TrafficVideoStreamer::TrafficVideoStreamer() {}

TrafficVideoStreamer::~TrafficVideoStreamer()
{
    stream.release();
}

bool TrafficVideoStreamer::openVideoStream(const std::string& filename)
{
    stream.open(filename);

    if(!stream.isOpened())
    {
        std::cerr << "Error: Unable to open stream" << std::endl;
        return false;
    }

    return true;
}

void TrafficVideoStreamer::constructStreamWindow(const std::string& windowName)
{
    originalWidth = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_WIDTH));
    originalHeight = static_cast<int>(stream.get(cv::CAP_PROP_FRAME_HEIGHT));

    cv::namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::resizeWindow(windowName, originalWidth, originalHeight);
}

bool TrafficVideoStreamer::getNextFrame(cv::Mat& frame)
{
    stream.read(frame);
    if(frame.empty())
    {
        return false;
    }

    // Perform warping here

    return true;
}
