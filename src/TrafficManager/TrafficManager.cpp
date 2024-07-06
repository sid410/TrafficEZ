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

    bool testMode = true;

    if(testMode)
    {
        test();
        exit(EXIT_SUCCESS);
    }

    MultiprocessTraffic multiprocessTraffic(configFile, debugMode, verbose);

    if(calibMode)
        multiprocessTraffic.calibrate();
    else
        multiprocessTraffic.start();

    std::cout << "TrafficManager ended.\n";
}

void TrafficManager::test()
{
    WatcherSpawner spawner;

    Watcher* watcher = spawner.spawnWatcher(
        WatcherType::VEHICLE, RenderMode::GUI, "vehicle.mp4", "vehicle.yaml");

    int i = 0;
    watcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);

    while(i < 100)
    {
        watcher->processFrame();
        i++;
    }

    std::cout << "Vehicle count: " << watcher->getInstanceCount() << "\n";
    std::cout << "Traffic density: " << watcher->getTrafficDensity() << "\n";

    watcher->setCurrentTrafficState(TrafficState::RED_PHASE);
    while(i < 150)
    {
        watcher->processFrame();
        i++;
    }

    std::cout << "Vehicle count: " << watcher->getInstanceCount() << "\n";
    std::cout << "Traffic density: " << watcher->getTrafficDensity() << "\n";

    delete watcher;
}