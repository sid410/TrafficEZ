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
                                 "debug_calib.yaml");
        delete calibrateWatcherGui;
    }
    if(debugMode)
    {
        Watcher* vehicleWatcherGui = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                          RenderMode::GUI,
                                                          "debug.mp4",
                                                          "debug_calib.yaml");

        int i = 0;

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::RED_PHASE);
        while(i < 100)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Red: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::GREEN_PHASE);
        while(i < 500)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Green: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::RED_PHASE);
        while(i < 600)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Red: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::GREEN_PHASE);
        while(i < 1100)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Green: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::RED_PHASE);
        while(i < 1200)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Red: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        vehicleWatcherGui->setCurrentTrafficState(TrafficState::GREEN_PHASE);
        while(i < 1700)
        {
            vehicleWatcherGui->processFrame();
            i++;
        }
        std::cout << "Traffic Density Green: "
                  << vehicleWatcherGui->getTrafficDensity() << "\n";

        delete vehicleWatcherGui;
    }

    Watcher* vehicleWatcherHeadless = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                           RenderMode::HEADLESS,
                                                           "debug.mp4",
                                                           "debug_calib.yaml");
    delete vehicleWatcherHeadless;

    std::cout << "TrafficManager ended.\n";
}