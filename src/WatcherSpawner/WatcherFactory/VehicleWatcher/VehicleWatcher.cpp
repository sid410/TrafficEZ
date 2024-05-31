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
        gui->initialize(streamName, calibName);
    }
    else if(mode == RenderMode::HEADLESS)
    {
        headless = new VehicleHeadless();
        headless->process(streamName, calibName);
    }
}

void VehicleWatcher::setCurrentTrafficState(TrafficState state)
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

void VehicleWatcher::processFrame()
{
    if(currentMode == RenderMode::GUI)
    {
        gui->display();
    }
    // else if(currentMode == RenderMode::HEADLESS)
    // {
    //     headless->process(streamName, calibName);
    // }
}

float VehicleWatcher::getTrafficDensity()
{
    if(currentMode == RenderMode::GUI)
    {
        return gui->getTrafficDensity();
    }

    return -1;
}