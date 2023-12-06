#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

class TrafficVideoStreamer;

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug);
    void start();
    void spawnCarObserverDebug();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
};

#endif