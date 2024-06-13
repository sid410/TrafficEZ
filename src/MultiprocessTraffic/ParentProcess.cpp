#include "ParentProcess.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

ParentProcess::ParentProcess(int numChildren,
                             std::vector<Pipe>& pipesParentToChild,
                             std::vector<Pipe>& pipesChildToParent,
                             std::vector<std::vector<const char*>>& phases,
                             std::vector<int>& phaseDurations)
    : numChildren(numChildren)
    , pipesParentToChild(pipesParentToChild)
    , pipesChildToParent(pipesChildToParent)
    , phases(phases)
    , phaseDurations(phaseDurations)
{}

void ParentProcess::run()
{
    closeUnusedPipes();

    char buffer[128];
    int phaseIndex = 0;

    while(true)
    {
        std::cerr
            << "============================================================\n";

        for(int i = 0; i < numChildren; ++i)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << phases[phaseIndex][i] << "\n";
            if(write(pipesParentToChild[i].fds[1],
                     phases[phaseIndex][i],
                     strlen(phases[phaseIndex][i]) + 1) == -1)
            {
                std::cerr << "Parent: Failed to write to pipe: "
                          << strerror(errno) << "\n";
                break;
            }
        }

        for(int i = 0; i < numChildren; ++i)
        {
            int bytesRead =
                read(pipesChildToParent[i].fds[0], buffer, sizeof(buffer) - 1);
            if(bytesRead <= 0)
            {
                std::cerr << "Parent: Failed to read from pipe or EOF reached: "
                          << strerror(errno) << "\n";
                break;
            }
            buffer[bytesRead] = '\0'; // Ensure null termination

            std::cout << "Traffic Density from child " << i << ": " << buffer
                      << "\n";
        }

        // Simulate time passing
        usleep(phaseDurations[phaseIndex] * 1000);

        // Move to the next phase
        phaseIndex = (phaseIndex + 1) % phases.size();
    }

    for(int i = 0; i < numChildren; ++i)
    {
        wait(nullptr); // Wait for child processes to finish
    }
}

void ParentProcess::closeUnusedPipes()
{
    for(int i = 0; i < numChildren; ++i)
    {
        close(pipesParentToChild[i].fds[0]);
        close(pipesChildToParent[i].fds[1]);
    }
}
