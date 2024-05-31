#ifndef PEDESTRIAN_WATCHER_H
#define PEDESTRIAN_WATCHER_H

#include "Gui.h"
#include "Headless.h"
#include "Watcher.h"

class PedestrianWatcher : public Watcher
{
public:
    void spawn(RenderMode mode,
               const std::string& streamName,
               const std::string& calibName) override;

    void setCurrentTrafficState(TrafficState state) override;

private:
    Gui* gui;
    Headless* headless;
    RenderMode currentMode;
};

#endif
