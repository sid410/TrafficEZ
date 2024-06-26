#ifndef HEADLESS_H
#define HEADLESS_H

#include "TrafficState.h"
#include <iostream>
#include <string>

class Headless
{
public:
    virtual ~Headless() {}

    virtual void initialize(const std::string& streamName,
                            const std::string& calibName) = 0;

    virtual void process()
    {
        std::cerr << "This method has no implementation. \nEXITING...\n\n";
        exit(EXIT_FAILURE);
    }

    void setCurrentTrafficState(TrafficState state)
    {
        currentTrafficState = state;
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

protected:
    TrafficState currentTrafficState;
};

#endif
