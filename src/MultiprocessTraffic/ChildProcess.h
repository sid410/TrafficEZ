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
    bool verbose;

    WatcherSpawner spawner;

    int childIndex;
    Pipe& pipeParentToChild;
    Pipe& pipeChildToParent;

    void closeUnusedPipes();
};

#endif
