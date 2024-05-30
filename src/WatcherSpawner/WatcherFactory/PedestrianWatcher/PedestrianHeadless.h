#ifndef PEDESTRIAN_HEADLESS_H
#define PEDESTRIAN_HEADLESS_H

#include "Headless.h"

class PedestrianHeadless : public Headless
{
public:
    void process(const std::string& streamName,
                 const std::string& calibName) override;
};

#endif
