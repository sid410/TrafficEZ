#ifndef HEADLESS_H
#define HEADLESS_H

#include "TrafficState.h"
#include <string>

class Headless
{
public:
    virtual ~Headless() {}

    virtual void process(const std::string& streamName,
                         const std::string& calibName) = 0;

    void setCurrentTrafficState(TrafficState state)
    {
        currentTrafficState = state;
    }

protected:
    TrafficState currentTrafficState;
};

#endif
