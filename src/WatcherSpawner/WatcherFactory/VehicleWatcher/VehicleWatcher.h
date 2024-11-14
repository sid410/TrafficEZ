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

    void processFrame() override;

    void setCurrentTrafficState(TrafficState state) override;
    float getTrafficDensity() override;
    int getInstanceCount() override;
    std::unordered_map<std::string, int> getVehicleTypeAndCount() override;

private:
    Gui* gui;
    Headless* headless;
    RenderMode currentMode;
};

#endif
