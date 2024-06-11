#ifndef CHILD_PROCESS_H
#define CHILD_PROCESS_H

#include "Pipe.h"

class ChildProcess
{
public:
    ChildProcess(int childIndex,
                 Pipe& pipeParentToChild,
                 Pipe& pipeChildToParent);
    void run();

private:
    int childIndex;
    Pipe& pipeParentToChild;
    Pipe& pipeChildToParent;

    void closeUnusedPipes();
};

#endif
