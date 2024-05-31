#include "PedestrianWatcher.h"
#include "PedestrianGui.h"
#include "PedestrianHeadless.h"

void PedestrianWatcher::spawn(RenderMode mode,
                              const std::string& streamName,
                              const std::string& calibName)
{
    currentMode = mode;

    if(mode == RenderMode::GUI)
    {
        gui = new PedestrianGui();
        gui->display(streamName, calibName);
    }
    else if(mode == RenderMode::HEADLESS)
    {
        headless = new PedestrianHeadless();
        headless->process(streamName, calibName);
    }
}

void PedestrianWatcher::setCurrentTrafficState(TrafficState state)
{
    if(currentMode == RenderMode::GUI)
    {
        gui->setCurrentTrafficState(state);
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        headless->setCurrentTrafficState(state);
    }
}