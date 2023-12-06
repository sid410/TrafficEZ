#include "TrafficManager.h"
#include <iostream>

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
}