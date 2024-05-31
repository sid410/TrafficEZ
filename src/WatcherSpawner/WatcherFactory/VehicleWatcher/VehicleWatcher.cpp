#include "VehicleWatcher.h"
#include "VehicleGui.h"
#include "VehicleHeadless.h"

void VehicleWatcher::spawn(RenderMode mode,
                           const std::string& streamName,
                           const std::string& calibName)
{
    currentMode = mode;

    if(mode == RenderMode::GUI)
    {
        gui = new VehicleGui();
        gui->display(streamName, calibName);
    }
    else if(mode == RenderMode::HEADLESS)
    {
        headless = new VehicleHeadless();
        headless->process(streamName, calibName);
    }
}

void VehicleWatcher::setCurrentTrafficState(TrafficState state)
{
    std::cout << "VehicleWatcher setting Traffic State to: ";

    if(currentMode == RenderMode::GUI)
    {
        std::cout << "VehicleWatcher Gui\n";
        if(state == TrafficState::RED_PHASE)
            std::cout << "Red State\n";
        else
            std::cout << "Green State\n";
        gui->setCurrentTrafficState(state);
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        std::cout << "VehicleWatcher Headless\n";
        headless->setCurrentTrafficState(state);
    }
}