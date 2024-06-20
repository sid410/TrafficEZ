#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include "Pipe.h"

class ChildProcess
{
public:
    ChildProcess(int childIndex,
                 Pipe& pipeParentToChild,
                 Pipe& pipeChildToParent,
                 bool verbose = false);
    void runVehicle(bool debug, int vehicleId);

private:
    bool verbose;

    int childIndex;
    Pipe& pipeParentToChild;
    Pipe& pipeChildToParent;

    void closeUnusedPipes();
};

#endif
