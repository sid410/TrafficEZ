#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

class VideoStreamer;

class TrafficManager
{
public:
    TrafficManager(int numCars, int numPedestrians, bool debug, bool calib);

    void start();

    void testGuiFactory();
    void testHeadlessFactory();

private:
    int numberOfCars;
    int numberOfPedestrians;
    bool debugMode;
    bool calibMode;
};

#endif