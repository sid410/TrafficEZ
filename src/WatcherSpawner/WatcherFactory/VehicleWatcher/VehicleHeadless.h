#ifndef VEHICLE_HEADLESS_H
#define VEHICLE_HEADLESS_H

#include "Headless.h"

class VehicleHeadless : public Headless
{
public:
    void process(const std::string& streamName,
                 const std::string& calibName) override;
};

#endif
