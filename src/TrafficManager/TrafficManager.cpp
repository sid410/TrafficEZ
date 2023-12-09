#include "TrafficManager.h"
#include "CalibrateVideoStreamer.h"
#include "VideoStreamer.h"
#include <iostream>
#include <opencv2/opencv.hpp>

TrafficManager::TrafficManager(int numCars,
                               int numPedestrians,
                               bool debug,
                               bool calib)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
    , calibMode(calib)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

    if(calibMode)
    {
        calibrateStreamPoints();
    }
    if(debugMode)
    {
        // This is blocking! change this later
        spawnCarObserverDebug();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::calibrateStreamPoints()
{
    CalibrateVideoStreamer calibrateStreamer;
    std::string calibWindow = "Calibrate Points";

    if(!calibrateStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

    calibrateStreamer.constructStreamWindow(calibWindow);
    calibrateStreamer.setCalibrationPointsFromMouse(calibWindow);
}

void TrafficManager::spawnCarObserverDebug()
{
    VideoStreamer videoStreamer;

    if(!videoStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

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