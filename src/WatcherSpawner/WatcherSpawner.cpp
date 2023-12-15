#include "WatcherSpawner.h"

Watcher* WatcherSpawner::spawnWatcher(WatcherType type,
                                      RenderMode mode,
                                      const std::string& streamName,
                                      const std::string& calibName)
{
    Watcher* watcher = WatcherFactory::createWatcher(type);

    if(watcher)
    {
        watcher->spawn(mode, streamName, calibName);
    }

    return watcher;
}