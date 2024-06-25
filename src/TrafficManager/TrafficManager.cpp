#include "TrafficManager.h"
#include "MultiprocessTraffic.h"
#include "WatcherSpawner.h"

TrafficManager::TrafficManager(const std::string& configFile,
                               bool debug,
                               bool calib,
                               bool verbose)
    : configFile(configFile)
    , debugMode(debug)
    , calibMode(calib)
    , verbose(verbose)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    std::cout << "Calib Mode: " << (calibMode ? "true" : "false") << "\n";
    std::cout << "Verbose Mode: " << (verbose ? "true" : "false") << "\n";

    if(calibMode)
    {
        handleCalibrationMode();
    }

    if(debugMode)
    {
        handleDebugMode();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::handleCalibrationMode()
{
    WatcherSpawner spawner;

    Watcher* calibrateWatcherGui = spawner.spawnWatcher(WatcherType::CALIBRATE,
                                                        RenderMode::GUI,
                                                        "debug.mp4",
                                                        "debug_calib.yaml");
    delete calibrateWatcherGui;
}

void TrafficManager::handleDebugMode()
{
    MultiprocessTraffic multiprocessTraffic(configFile, debugMode, verbose);
    multiprocessTraffic.start();
}
