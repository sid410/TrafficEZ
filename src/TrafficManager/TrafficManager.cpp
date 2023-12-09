#include "TrafficManager.h"
#include "CalibrateVideoStreamer.h"
#include "VideoStreamer.h"
#include <iostream>
#include <opencv2/opencv.hpp>

TrafficManager::TrafficManager(int numCars, int numPedestrians, bool debug)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

    // This is blocking! change this later
    calibrateStreamPoints();

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::calibrateStreamPoints()
{
    CalibrateVideoStreamer calibrateStreamer;

    std::vector<cv::Point2f> calibrationPoints = {cv::Point2f(666, 0),
                                                  cv::Point2f(0, 303),
                                                  cv::Point2f(789, 702),
                                                  cv::Point2f(1274, 566)};

    calibrateStreamer.setCalibrationPoints(calibrationPoints);
    std::cout << "Successfully calibrated.\n";
}

void TrafficManager::spawnCarObserverDebug()
{
    VideoStreamer videoStreamer;

    if(!videoStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

    // videoStreamer.constructStreamWindow("Debug Video");
    // ^ this is only needed after implementing mouse click 4 points

    if(!videoStreamer.readCalibrationPoints("calib_points.yaml"))
    {
        return;
    }

    cv::Mat frame;
    cv::Mat warpedFrame;

    videoStreamer.initializePerspectiveTransform();

    if(!videoStreamer.perspectiveMatrixInitialized)
    {
        std::cerr << "Error: Failed to initialize perspective matrix.\n";
        return;
    }

    while(videoStreamer.getNextFrame(frame))
    {
        videoStreamer.warpFrame(frame, warpedFrame);

        cv::imshow("Debug Warped Video", warpedFrame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}