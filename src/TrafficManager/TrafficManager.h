#ifndef TRAFFIC_MANAGER_H
#define TRAFFIC_MANAGER_H

#include <string>
#include "IotHubClient.h"

class TrafficManager
{
public:
    TrafficManager(const std::string& configFile,
                   bool debug,
                   bool calib,
                   bool verbose,
                   bool test);

    void start();

private:
    IotHubClient client;
    std::string configFile;
    bool debugMode;
    bool calibMode;
    bool verbose;
    bool testMode;

    void test();
    void initTestVariables();
    void testVehicleWatcherGui(int greenFramesToCheck, int redFramesToCheck);
    void testVehicleWatcherHeadless(int greenFramesToCheck,
                                    int redFramesToCheck);
    void testPedestrianWatcherGui(int redFramesToCheck);
    void testPedestrianWatcherHeadless(int redFramesToCheck);
    void compareVehicleResults();
    void comparePedestrianResults();

    // GUI mode results for test comparison
    int greenCountGui;
    float greenDensityGui;
    int redCountGui;
    float redDensityGui;
    int pedCountGui;

    // Headless mode results for test comparison
    int greenCountHeadless;
    float greenDensityHeadless;
    int redCountHeadless;
    float redDensityHeadless;
    int pedCountHeadless;
};

#endif
