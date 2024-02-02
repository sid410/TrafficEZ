#ifndef HEADLESS_H
#define HEADLESS_H

#include <string>

class Headless
{
public:
    virtual ~Headless() {}

    virtual void process(const std::string& streamName,
                         const std::string& calibName) const = 0;
};

#endif
