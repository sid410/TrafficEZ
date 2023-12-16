#include "TrafficManager.h"
#include "WatcherSpawner.h"
#include <iostream>

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

    WatcherSpawner spawner;

    if(calibMode)
    {
        Watcher* calibrateWatcherGui =
            spawner.spawnWatcher(WatcherType::CALIBRATE,
                                 RenderMode::GUI,
                                 "debug.mp4",
                                 "calib_points.yaml");
        delete calibrateWatcherGui;
    }
    if(debugMode)
    {
        testEmptyWatchers();
    }

    Watcher* vehicleWatcherGui = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                      RenderMode::GUI,
                                                      "debug.mp4",
                                                      "calib_points.yaml");
    delete vehicleWatcherGui;

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::testEmptyWatchers()
{
    WatcherSpawner testSpawner;

    Watcher* pedestrianWatcherGui =
        testSpawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                 RenderMode::GUI,
                                 "PedestrianStream",
                                 "PedestrianCalib");

    Watcher* calibrateWatcherHeadless =
        testSpawner.spawnWatcher(WatcherType::CALIBRATE,
                                 RenderMode::HEADLESS,
                                 "CalibrateStream",
                                 "CalibrateCalib");

    Watcher* vehicleWatcherHeadless =
        testSpawner.spawnWatcher(WatcherType::VEHICLE,
                                 RenderMode::HEADLESS,
                                 "VehicleStream",
                                 "VehicleCalib");

    Watcher* pedestrianWatcherHeadless =
        testSpawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                 RenderMode::HEADLESS,
                                 "PedestrianStream",
                                 "PedestrianCalib");

    delete pedestrianWatcherGui;
    delete calibrateWatcherHeadless;
    delete vehicleWatcherHeadless;
    delete pedestrianWatcherHeadless;
}