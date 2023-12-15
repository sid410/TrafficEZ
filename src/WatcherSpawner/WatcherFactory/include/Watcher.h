#ifndef WATCHER_H
#define WATCHER_H

#include <string>

enum class RenderMode
{
    GUI,
    HEADLESS
};

class Watcher
{
public:
    virtual void spawn(RenderMode mode,
                       const std::string& streamName,
                       const std::string& calibName) const = 0;
};

#endif
