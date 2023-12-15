#include "WatcherFactory.h"

Watcher* WatcherFactory::createWatcher(WatcherType type)
{
    switch(type)
    {
    case WatcherType::VEHICLE:
        return new VehicleWatcher();
    case WatcherType::PEDESTRIAN:
        return new PedestrianWatcher();
    case WatcherType::CALIBRATE:
        return new CalibrateWatcher();
    default:
        return nullptr; // Invalid type
    }
}
