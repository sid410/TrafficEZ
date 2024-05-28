#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include <sys/types.h>
#include <unistd.h>
#include <vector>

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

    std::vector<int> childPids;
    int pipe1[2]; // Pipe for communication with first child
    int pipe2[2]; // Pipe for communication with second child

    void forkChildProcesses();
    void handleCommunication(int readPipe);
    void handleChildProcess(int readPipe, int writePipe);
};

#endif