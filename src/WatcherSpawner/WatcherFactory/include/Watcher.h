#ifndef WATCHER_H
#define WATCHER_H

#include "TrafficState.h"
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

    virtual void setCurrentTrafficState(TrafficState state) = 0;
    virtual void processFrame() = 0;
};

#endif
