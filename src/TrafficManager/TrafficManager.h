#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug, bool calib);

    void start();

    void testEmptyWatchers();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
    bool calibMode;
};

#endif