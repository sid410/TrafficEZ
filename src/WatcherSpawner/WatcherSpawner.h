#ifndef WATCHER_SPAWNER_H
#define WATCHER_SPAWNER_H

#include "WatcherFactory.h"

class WatcherSpawner
{
public:
    static Watcher* spawnWatcher(WatcherType type,
                                 RenderMode mode,
                                 const std::string& streamName,
                                 const std::string& calibName);
};

#endif