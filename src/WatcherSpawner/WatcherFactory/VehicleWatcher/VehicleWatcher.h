#ifndef VEHICLE_WATCHER_H
#define VEHICLE_WATCHER_H

#include "Gui.h"
#include "Headless.h"
#include "Watcher.h"

class VehicleWatcher : public Watcher
{
public:
    void spawn(RenderMode mode,
               const std::string& streamName,
               const std::string& calibName) override;

    void setCurrentTrafficState(TrafficState state) override;
    void processFrame() override;

private:
    Gui* gui;
    Headless* headless;
    RenderMode currentMode;
};

#endif
