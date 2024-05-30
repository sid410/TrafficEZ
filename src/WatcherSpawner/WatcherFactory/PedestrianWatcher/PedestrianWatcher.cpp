#include "PedestrianWatcher.h"
#include "PedestrianGui.h"
#include "PedestrianHeadless.h"

void PedestrianWatcher::spawn(RenderMode mode,
                              const std::string& streamName,
                              const std::string& calibName)
{
    if(mode == RenderMode::GUI)
    {
        Gui* gui = new PedestrianGui();
        gui->display(streamName, calibName);
        delete gui;
    }
    else if(mode == RenderMode::HEADLESS)
    {
        Headless* headless = new PedestrianHeadless();
        headless->process(streamName, calibName);
        delete headless;
    }
}
