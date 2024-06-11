#include "ParentProcess.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

ParentProcess::ParentProcess(int numChildren,
                             std::vector<Pipe>& pipesParentToChild,
                             std::vector<Pipe>& pipesChildToParent)
    : numChildren(numChildren)
    , pipesParentToChild(pipesParentToChild)
    , pipesChildToParent(pipesChildToParent)
{}

void ParentProcess::run()
{
    closeUnusedPipes();

    char buffer[128];

    const char* redPhaseMsg = "RED_PHASE";
    const char* greenPhaseMsg = "GREEN_PHASE";

    // Traffic state phases
    const int redPhaseDuration = 1000;
    const int greenPhaseDuration = 500;

    // Initial state
    const char* currentPhaseMsg = greenPhaseMsg;

    while(true)
    {
        for(int i = 0; i < numChildren; ++i)
        {
            std::cout << "Parent: Sending phase message to child " << i << ": "
                      << currentPhaseMsg << "\n";
            if(write(pipesParentToChild[i].fds[1],
                     currentPhaseMsg,
                     strlen(currentPhaseMsg) + 1) == -1)
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
        usleep(currentPhaseMsg == redPhaseMsg ? redPhaseDuration * 10000
                                              : greenPhaseDuration * 10000);

        // Toggle phase
        currentPhaseMsg =
            (currentPhaseMsg == redPhaseMsg) ? greenPhaseMsg : redPhaseMsg;
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
