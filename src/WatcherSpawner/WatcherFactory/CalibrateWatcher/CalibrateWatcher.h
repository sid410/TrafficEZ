#ifndef CALIBRATE_WATCHER_H
#define CALIBRATE_WATCHER_H

#include "Watcher.h"

class CalibrateWatcher : public Watcher
{
public:
    void spawn(RenderMode mode,
               const std::string& streamName,
               const std::string& calibName) override;
};

#endif
