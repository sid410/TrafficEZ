#include "TrafficManager.h"
#include "MultiprocessTraffic.h"
#include "WatcherSpawner.h"

TrafficManager::TrafficManager(int numCars,
                               int numPedestrians,
                               bool debug,
                               bool calib)
    : numberOfCars(numCars)
    , numberOfPedestrians(numPedestrians)
    , debugMode(debug)
    , calibMode(calib)
{}

void TrafficManager::start()
{
    std::cout << "TrafficManager starting...\n";
    std::cout << "Number of Cars: " << numberOfCars << "\n";
    std::cout << "Number of Pedestrians: " << numberOfPedestrians << "\n";
    std::cout << "Debug Mode: " << (debugMode ? "true" : "false") << "\n";

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
                                                        "pedestrian.mp4",
                                                        "pedestrian.yaml");
    delete calibrateWatcherGui;
}

void TrafficManager::handleDebugMode()
{
    MultiprocessTraffic multiprocessTraffic("junction_config.yaml", true);
    multiprocessTraffic.start();

    // WatcherSpawner spawner;
    // Watcher* pedestrianWatcher = spawner.spawnWatcher(WatcherType::PEDESTRIAN,
    //                                                   RenderMode::GUI,
    //                                                   "pedestrian.mp4",
    //                                                   "pedestrian.yaml");

    // while(true)
    // {
    //     pedestrianWatcher->processFrame();

    //     std::cout << "People:" << pedestrianWatcher->getInstanceCount() << "\n";
    // }

    // delete pedestrianWatcher;
}
