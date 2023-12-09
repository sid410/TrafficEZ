#include "TrafficManager.h"
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
    std::cout << "TrafficManager starting..." << std::endl;
    std::cout << "Number of Cars: " << numberOfCars << std::endl;
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << std::endl;
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << std::endl;

    // This is blocking! change this later
    spawnCarObserverDebug();

    std::cout << "TrafficManager ended" << std::endl;
}

void TrafficManager::spawnCarObserverDebug()
{
    VideoStreamer videoStreamer;

    if(!videoStreamer.openVideoStream("debug.mp4"))
    {
        std::cerr << "Error: Failed to open the video stream." << std::endl;
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
        std::cerr << "Error: Failed to initialize perspective matrix."
                  << std::endl;
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