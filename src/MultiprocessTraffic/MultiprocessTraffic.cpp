#include "MultiprocessTraffic.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

MultiprocessTraffic::MultiprocessTraffic(bool verbose)
    : verbose(verbose)
{}

void MultiprocessTraffic::start()
{
    loadPhasingInfo();
    numChildren = phases[0].size();

    createPipes();
    forkChildren();

    ParentProcess parentProcess(numChildren,
                                pipesParentToChild,
                                pipesChildToParent,
                                phases,
                                phaseDurations,
                                verbose);

    parentProcess.run();
}

void MultiprocessTraffic::createPipes()
{
    pipesParentToChild.resize(numChildren);
    pipesChildToParent.resize(numChildren);

    for(int i = 0; i < numChildren; ++i)
    {
        if(pipe(pipesParentToChild[i].fds) == -1 ||
           pipe(pipesChildToParent[i].fds) == -1)
        {
            std::cerr << "Pipe creation failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
    }
}

void MultiprocessTraffic::forkChildren()
{
    for(int i = 0; i < numChildren; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            ChildProcess childProcess(
                i, pipesParentToChild[i], pipesChildToParent[i], verbose);
            childProcess.runVehicle(true, i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
    }
}

void MultiprocessTraffic::loadPhasingInfo()
{
    // Example phasing information for 4 signals and 3 phase cycles
    phases = {
        {"GREEN_PHASE", "RED_PHASE", "GREEN_PHASE", "RED_PHASE"}, // Phase 1
        {"GREEN_PHASE", "GREEN_PHASE", "RED_PHASE", "RED_PHASE"}, // Phase 2
        {"RED_PHASE", "RED_PHASE", "RED_PHASE", "GREEN_PHASE"} // Phase 3
    };

    // Phase durations in milliseconds
    phaseDurations = {6500, 2500, 3500};

    if(phases.size() != phaseDurations.size())
    {
        std::cerr << "Size of phase info and duration do not match!\n";
        exit(EXIT_FAILURE);
    }
}