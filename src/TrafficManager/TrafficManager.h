#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include "TrafficVideoStreamer.h"

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug);
    void start();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
    TrafficVideoStreamer videoStreamer;
};

#endif