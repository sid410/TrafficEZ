#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

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
    void parentProcess(int pipe_parent_to_child[2],
                       int pipe_child_to_parent[2]);
    void childProcess(int pipe_parent_to_child[2], int pipe_child_to_parent[2]);
};

#endif