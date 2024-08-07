#ifndef CALIBRATE_HEADLESS_H
#define CALIBRATE_HEADLESS_H

#include "Headless.h"

class CalibrateHeadless : public Headless
{
public:
    void initialize(const std::string& streamName,
                    const std::string& calibName) override;
};

#endif
