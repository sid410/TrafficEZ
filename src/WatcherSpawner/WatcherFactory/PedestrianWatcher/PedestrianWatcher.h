#ifndef PEDESTRIAN_WATCHER_H
#define PEDESTRIAN_WATCHER_H

#include "Watcher.h"

class PedestrianWatcher : public Watcher
{
public:
    void spawn(RenderMode mode,
               const std::string& streamName,
               const std::string& calibName) override;
};

#endif
