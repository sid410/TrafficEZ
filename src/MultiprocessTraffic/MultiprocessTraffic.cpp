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

    createPipes();
    forkChildren();

    ParentProcess parentProcess(numVehicle,
                                numPedestrian,
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
    int pipeIndex = 0;

    for(int i = 0; i < numVehicle; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            ChildProcess childProcess(pipeIndex,
                                      pipesParentToChild[pipeIndex],
                                      pipesChildToParent[pipeIndex],
                                      verbose);
            childProcess.runVehicle(true, i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
        ++pipeIndex;
    }

    for(int i = 0; i < numPedestrian; ++i)
    {
        pid_t pid = fork();
        if(pid < 0)
        {
            std::cerr << "Fork failed: " << strerror(errno) << "\n";
            exit(EXIT_FAILURE);
        }
        else if(pid == 0)
        {
            ChildProcess childProcess(pipeIndex,
                                      pipesParentToChild[pipeIndex],
                                      pipesChildToParent[pipeIndex],
                                      verbose);
            childProcess.runPedestrian(true, i);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
        ++pipeIndex;
    }
}

void MultiprocessTraffic::loadPhasingInfo()
{
    // Example phasing information for 4 signals and 3 phase cycles
    phases = {
        {"GREEN_PHASE",
         "RED_PHASE",
         "GREEN_PHASE",
         "RED_PHASE",
         "GREEN_PED",
         "RED_PED"}, // Phase 1
        {"GREEN_PHASE",
         "GREEN_PHASE",
         "RED_PHASE",
         "RED_PHASE",
         "RED_PED",
         "RED_PED"}, // Phase 2
        {"RED_PHASE",
         "RED_PHASE",
         "RED_PHASE",
         "GREEN_PHASE",
         "RED_PED",
         "GREEN_PED"} // Phase 3
    };

    // Phase durations in milliseconds
    phaseDurations = {65000, 25000, 35000};

    if(phases.size() != phaseDurations.size())
    {
        std::cerr << "Size of phase info and duration do not match!\n";
        exit(EXIT_FAILURE);
    }

    setVehicleAndPedestrianCount();
}

void MultiprocessTraffic::setVehicleAndPedestrianCount()
{
    numVehicle = 0;
    numPedestrian = 0;

    numChildren = phases[0].size();

    for(const auto& phase : phases[0])
    {
        if(phase == "GREEN_PHASE" || phase == "RED_PHASE")
        {
            numVehicle++;
        }
        else if(phase == "GREEN_PED" || phase == "RED_PED")
        {
            numPedestrian++;
        }
    }

    if(numChildren != numVehicle + numPedestrian)
    {
        std::cerr
            << "Count of children do not match numVehicle and numPedestrian!\n";
        exit(EXIT_FAILURE);
    }
}