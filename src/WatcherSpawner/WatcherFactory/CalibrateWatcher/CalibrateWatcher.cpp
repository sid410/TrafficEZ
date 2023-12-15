#include "CalibrateWatcher.h"
#include "CalibrateGui.h"
#include "CalibrateHeadless.h"

void CalibrateWatcher::spawn(RenderMode mode,
                             const std::string& streamName,
                             const std::string& calibName) const
{
    if(mode == RenderMode::GUI)
    {
        Gui* gui = new CalibrateGui();
        gui->display(streamName, calibName);
        delete gui;
    }
    else if(mode == RenderMode::HEADLESS)
    {
        Headless* headless = new CalibrateHeadless();
        headless->process(streamName, calibName);
        delete headless;
    }
}
