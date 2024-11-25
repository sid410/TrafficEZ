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
        headless->initialize(streamName, calibName);
    }
}

void VehicleWatcher::processFrame()
{
    if(currentMode == RenderMode::GUI)
    {
        gui->display();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        headless->process();
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

float VehicleWatcher::getTrafficDensity()
{
    if(currentMode == RenderMode::GUI)
    {
        return gui->getTrafficDensity();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        return headless->getTrafficDensity();
    }

    return -1;
}

int VehicleWatcher::getInstanceCount()
{
    if(currentMode == RenderMode::GUI)
    {
        return gui->getInstanceCount();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        return headless->getInstanceCount();
    }

    return -1;
}

std::unordered_map<std::string, int> VehicleWatcher::getVehicleTypeAndCount()
{
    //RED_PHASE ONLY
    if(currentMode == RenderMode::GUI)
    {
        return gui->getVehicleTypeAndCount();
    }
    else if(currentMode == RenderMode::HEADLESS)
    {
        return headless->getVehicleTypeAndCount();
    }

    return {};
}
