#ifndef MULTIPROCESS_TRAFFIC_H
#define MULTIPROCESS_TRAFFIC_H

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/wait.h>
#include <unistd.h>
#include <vector>

// Wrapper struct to encapsulate the pipe array
struct Pipe
{
    int fds[2];
};

class MultiprocessTraffic
{
public:
    MultiprocessTraffic(int numChildren);
    void start();

private:
    int numChildren;
    std::vector<pid_t> childPids;
    std::vector<Pipe> pipesParentToChild;
    std::vector<Pipe> pipesChildToParent;

    void parentProcess();
    void childProcess(int childIndex);

    void createPipes();
    void forkChildren();
    void closeUnusedPipesInParent();
    void closeUnusedPipesInChild(int childIndex);
};

#endif
