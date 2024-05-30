#include "VehicleWatcher.h"
#include "VehicleGui.h"
#include "VehicleHeadless.h"

void VehicleWatcher::spawn(RenderMode mode,
                           const std::string& streamName,
                           const std::string& calibName)
{
    if(mode == RenderMode::GUI)
    {
        Gui* gui = new VehicleGui();
        gui->display(streamName, calibName);
        delete gui;
    }
    else if(mode == RenderMode::HEADLESS)
    {
        Headless* headless = new VehicleHeadless();
        headless->process(streamName, calibName);
        delete headless;
    }
}