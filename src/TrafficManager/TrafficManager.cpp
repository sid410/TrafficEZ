#include "TrafficManager.h"
#include "MultiprocessTraffic.h"

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

    MultiprocessTraffic multiprocessTraffic(configFile, debugMode, verbose);

    if(calibMode)
        multiprocessTraffic.calibrate();
    else
        multiprocessTraffic.start();

    std::cout << "TrafficManager ended.\n";
}