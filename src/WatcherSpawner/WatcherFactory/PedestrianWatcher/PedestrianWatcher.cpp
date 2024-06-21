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
        gui->initialize(streamName, calibName);
    }
    else if(mode == RenderMode::HEADLESS)
    {
        headless = new PedestrianHeadless();
        headless->process(streamName, calibName);
    }
}

void PedestrianWatcher::processFrame()
{
    if(currentMode == RenderMode::GUI)
    {
        gui->display();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        // headless->process();
        std::cerr << "PedestrianWatcher::processFrame headless not yet "
                     "implemented...\n";
    }
}

int PedestrianWatcher::getInstanceCount()
{
    if(currentMode == RenderMode::GUI)
    {
        return gui->getInstanceCount();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        // return headless->getTrafficDensity();
        std::cerr << "PedestrianWatcher::getTrafficDensity headless not yet "
                     "implemented...\n";
    }

    return -1;
}