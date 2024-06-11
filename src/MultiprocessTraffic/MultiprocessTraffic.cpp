#include "MultiprocessTraffic.h"
#include <cstring>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>

MultiprocessTraffic::MultiprocessTraffic(int numChildren)
    : numChildren(numChildren)
{}

void MultiprocessTraffic::start()
{
    createPipes();
    forkChildren();
    ParentProcess parentProcess(
        numChildren, pipesParentToChild, pipesChildToParent);
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
                i, pipesParentToChild[i], pipesChildToParent[i]);
            childProcess.runVehicle(true, i + 1);
            exit(EXIT_SUCCESS);
        }
        else
        {
            childPids.push_back(pid);
        }
    }
}
