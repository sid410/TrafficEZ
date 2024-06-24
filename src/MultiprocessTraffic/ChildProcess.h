#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include "Pipe.h"
#include "WatcherSpawner.h"

class ChildProcess
{
public:
    ChildProcess(int childIndex,
                 Pipe& pipeParentToChild,
                 Pipe& pipeChildToParent,
                 bool verbose = false);

    void runVehicle(bool debug, int vehicleId);
    void runPedestrian(bool debug, int pedestrianId);

private:
    static constexpr int BUFFER_SIZE = 128;
    static constexpr int CPU_SLEEP_US = 1000;

    bool verbose;

    WatcherSpawner spawner;

    int childIndex;
    Pipe& pipeParentToChild;
    Pipe& pipeChildToParent;

    Watcher* createWatcher(WatcherType watcherType,
                           bool debug,
                           const std::string& configFile,
                           const std::string& streamFile);
    void sendDensityToParent(float density);
    void closeUnusedPipes();
};

#endif
