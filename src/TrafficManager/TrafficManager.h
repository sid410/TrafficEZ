#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug);
    void start();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
};

#endif