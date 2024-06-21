#include "CalibrateWatcher.h"
#include "CalibrateGui.h"
#include "CalibrateHeadless.h"

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