#ifndef WATCHER_FACTORY_H
#define WATCHER_FACTORY_H

#include "CalibrateWatcher.h"
#include "PedestrianWatcher.h"
#include "VehicleWatcher.h"

enum class WatcherType
{
    VEHICLE,
    PEDESTRIAN,
    CALIBRATE
};

class WatcherFactory
{
public:
    static Watcher* createWatcher(WatcherType type);
};

#endif
