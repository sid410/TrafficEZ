#include "TrafficVideoStreamer.h"
#include <iostream>

TrafficVideoStreamer::TrafficVideoStreamer()
    : perspectiveMatrixInitialized(false)
{}

TrafficVideoStreamer::~TrafficVideoStreamer()
{
    stream.release();
}

bool TrafficVideoStreamer::openVideoStream(const std::string& filename)
{
    stream.open(filename);

    if(!stream.isOpened())
    {
        std::cerr << "Error: Unable to open stream." << std::endl;
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

    return true;
}

void TrafficVideoStreamer::initializePerspectiveTransform()
{
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

void TrafficVideoStreamer::warpFrame(const cv::Mat& inputFrame,
                                     cv::Mat& warpedFrame)
{
    cv::warpPerspective(inputFrame,
                        warpedFrame,
                        perspectiveMatrix,
                        cv::Size(static_cast<int>(dstPoints[1].x),
                                 static_cast<int>(dstPoints[2].y)));
}