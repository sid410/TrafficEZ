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
