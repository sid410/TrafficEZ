#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include <iostream>

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug, bool calib);

    void start();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
    bool calibMode;

    void handleCalibrationMode();
    void handleDebugMode();
};

#endif
