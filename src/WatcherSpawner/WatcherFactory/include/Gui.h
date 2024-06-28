#ifndef GUI_H
#define GUI_H

#include "TrafficState.h"
#include <iostream>
#include <string>

class Gui
{
public:
    virtual ~Gui() {}

    virtual void initialize(const std::string& streamName,
                            const std::string& calibName) = 0;

    virtual void display()
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
