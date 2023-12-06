#include "TrafficManager.h"
#include "TrafficVideoStreamer.h"
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
    TrafficVideoStreamer videoStreamer;

    if(!videoStreamer.openVideoStream("debug.mp4"))
    {
        return;
    }

    cv::namedWindow("Debug Video", cv::WINDOW_NORMAL);

    cv::Mat frame;

    while(videoStreamer.getNextFrame(frame))
    {
        cv::imshow("Debug Video", frame);

        if(cv::waitKey(30) == 27)
            break;
    }

    cv::destroyAllWindows();
}