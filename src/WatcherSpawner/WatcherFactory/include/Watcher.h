#ifndef WATCHER_H
#define WATCHER_H

#include "TrafficState.h"
#include <iostream>
#include <string>

enum class RenderMode
{
    GUI,
    HEADLESS
};

class Watcher
{
public:
    virtual ~Watcher() {}

    virtual void spawn(RenderMode mode,
                       const std::string& streamName,
                       const std::string& calibName) = 0;

    virtual void processFrame()
    {
        std::cerr << "This method has no implementation. \nEXITING...\n\n";
        exit(EXIT_FAILURE);
    }

    virtual void setCurrentTrafficState(TrafficState state)
    {
        std::cerr << "This method has no implementation. \nEXITING...\n\n";
        exit(EXIT_FAILURE);
    }

    virtual float getTrafficDensity()
    {
        std::cerr << "This method has no implementation. \nEXITING...\n\n";
        exit(EXIT_FAILURE);
    }

    virtual int getInstanceCount()
    {
        std::cerr << "This method has no implementation. \nEXITING...\n\n";
        exit(EXIT_FAILURE);
    }
};

#endif
