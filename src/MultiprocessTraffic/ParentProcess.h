#ifndef PARENT_PROCESS_H
#define PARENT_PROCESS_H

#include "Pipe.h"
#include <vector>

class ParentProcess
{
public:
    ParentProcess(int numChildren,
                  std::vector<Pipe>& pipesParentToChild,
                  std::vector<Pipe>& pipesChildToParent);
    void run();

private:
    int numChildren;
    std::vector<Pipe>& pipesParentToChild;
    std::vector<Pipe>& pipesChildToParent;

    void closeUnusedPipes();
};

#endif
