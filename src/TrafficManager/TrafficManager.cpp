#include "TrafficManager.h"
#include "CalibrateVideoStreamer.h"
#include "TrimPerspective.h"
#include "VideoStreamer.h"
#include "WarpPerspective.h"
#include "WatcherSpawner.h"
#include <iostream>
#include <opencv2/opencv.hpp>

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
        testGuiFactory();
    }
    if(debugMode)
    {
        testHeadlessFactory();
    }

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::testGuiFactory()
{
    WatcherSpawner spawner;

    Watcher* calibrateWatcherGui = spawner.spawnWatcher(WatcherType::CALIBRATE,
                                                        RenderMode::GUI,
                                                        "debug.mp4",
                                                        "calib_points.yaml");

    Watcher* vehicleWatcherGui = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                      RenderMode::GUI,
                                                      "debug.mp4",
                                                      "calib_points.yaml");

    Watcher* pedestrianWatcherGui =
        spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                             RenderMode::GUI,
                             "PedestrianStream",
                             "PedestrianCalib");

    delete vehicleWatcherGui;
    delete pedestrianWatcherGui;
    delete calibrateWatcherGui;
}

void TrafficManager::testHeadlessFactory()
{
    WatcherSpawner spawner;

    Watcher* calibrateWatcherHeadless =
        spawner.spawnWatcher(WatcherType::CALIBRATE,
                             RenderMode::HEADLESS,
                             "CalibrateStream",
                             "CalibrateCalib");

    Watcher* vehicleWatcherHeadless = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                           RenderMode::HEADLESS,
                                                           "VehicleStream",
                                                           "VehicleCalib");

    Watcher* pedestrianWatcherHeadless =
        spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                             RenderMode::HEADLESS,
                             "PedestrianStream",
                             "PedestrianCalib");

    delete vehicleWatcherHeadless;
    delete pedestrianWatcherHeadless;
    delete calibrateWatcherHeadless;
}