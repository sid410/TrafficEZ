#include "CalibrateWatcher.h"
#include "CalibrateGui.h"
#include "CalibrateHeadless.h"
#include <iostream>

void CalibrateWatcher::spawn(RenderMode mode,
                             const std::string& streamName,
                             const std::string& calibName)
{
    currentMode = mode;

    if(mode == RenderMode::GUI)
    {
        gui = new CalibrateGui();
        gui->initialize(streamName, calibName);
    }
    else if(mode == RenderMode::HEADLESS)
    {
        headless = new CalibrateHeadless();
        headless->process(streamName, calibName);
    }
}

void CalibrateWatcher::setCurrentTrafficState(TrafficState state)
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

void CalibrateWatcher::processFrame()
{
    std::cout << "CalibrateWatcher::processFrame not yet implemented...\n";
}

float CalibrateWatcher::getTrafficDensity()
{
    std::cout << "CalibrateWatcher::getTrafficDensity not yet implemented...\n";
    return -1;
}