#include "TrafficManager.h"
#include "MultiprocessTraffic.h"
#include "WatcherSpawner.h"
#include <azure_c_shared_utility/platform.h>
#include <iothub_client.h>
#include <iothub_message.h>
#include <iothubtransportmqtt.h>

static const char* connectionString = "HostName=trafficez.azure-devices.net;SharedAccessKeyName=iothubowner;SharedAccessKey=P9kfF3Inx9wxpKM90Xdf9CTR8Z5XGGZDHAIoTJnwVeA=";

void sendMessage(const char* message)
{
    IOTHUB_CLIENT_LL_HANDLE iotHubClientHandle = IoTHubClient_LL_CreateFromConnectionString(connectionString, MQTT_Protocol);
    if (iotHubClientHandle == nullptr)
    {
        printf("Failed to create IoT Hub client handle\n");
        return;
    }

    IOTHUB_MESSAGE_HANDLE messageHandle = IoTHubMessage_CreateFromString(message);
    if (messageHandle == nullptr)
    {
        printf("Failed to create message handle\n");
        IoTHubClient_LL_Destroy(iotHubClientHandle);
        return;
    }

    if (IoTHubClient_LL_SendEventAsync(iotHubClientHandle, messageHandle, nullptr, nullptr) != IOTHUB_CLIENT_OK)
    {
        printf("Failed to send message\n");
    }
    else
    {
        printf("Message sent successfully\n");
    }

    IoTHubMessage_Destroy(messageHandle);
    IoTHubClient_LL_Destroy(iotHubClientHandle);
}

TrafficManager::TrafficManager(const std::string& configFile,
                               bool debug,
                               bool calib,
                               bool verbose,
                               bool test)
    : configFile(configFile)
    , debugMode(debug)
    , calibMode(calib)
    , verbose(verbose)
    , testMode(test)
    , sendMessage("{\"vehicleCount\": 100, \"density\": 30}");
{}

void TrafficManager::start()
{
    if(testMode)
    {
        test();
        std::cout << "\nTest Successful!!\n";
        exit(EXIT_SUCCESS);
    }

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

void TrafficManager::test()
{
    initTestVariables();

    int greenFramesToCheck = 100;
    int redFramesToCheck = 20;

    testVehicleWatcherGui(greenFramesToCheck, redFramesToCheck);
    testVehicleWatcherHeadless(greenFramesToCheck, redFramesToCheck);
    compareVehicleResults();

    testPedestrianWatcherGui(redFramesToCheck);
    testPedestrianWatcherHeadless(redFramesToCheck);
    comparePedestrianResults();
}

void TrafficManager::initTestVariables()
{
    greenCountGui = 0;
    greenDensityGui = 0.0;
    redCountGui = 0;
    redDensityGui = 0.0;
    pedCountGui = 0;
    greenCountHeadless = 0;
    greenDensityHeadless = 0.0;
    redCountHeadless = 0;
    redDensityHeadless = 0.0;
    pedCountHeadless = 0;
}

void TrafficManager::testVehicleWatcherGui(int greenFramesToCheck,
                                           int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcher = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                   RenderMode::GUI,
                                                   "testVehicle.mp4",
                                                   "testVehicle.yaml");

    std::cout << "\nStarting Vehicle GUI mode...\n";

    vehicleWatcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
    for(int currFrame = 0; currFrame < greenFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    greenCountGui = vehicleWatcher->getInstanceCount();
    greenDensityGui = vehicleWatcher->getTrafficDensity();

    std::cout << "Green Vehicle count: " << greenCountGui << "\n";
    std::cout << "Green Vehicle density: " << greenDensityGui << "\n";

    if(greenCountGui <= 0 || greenDensityGui <= 0)
    {
        std::cerr << "Vehicle Detection failed on green phase\n";
        exit(EXIT_FAILURE);
    }

    vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    redCountGui = vehicleWatcher->getInstanceCount();
    redDensityGui = vehicleWatcher->getTrafficDensity();

    std::cout << "Red Vehicle count: " << redCountGui << "\n";
    std::cout << "Red Vehicle density: " << redDensityGui << "\n";

    if(redCountGui <= 0 || redDensityGui <= 0)
    {
        std::cerr << "Vehicle Detection failed on red phase\n";
        exit(EXIT_FAILURE);
    }

    delete vehicleWatcher;
}

void TrafficManager::testVehicleWatcherHeadless(int greenFramesToCheck,
                                                int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* vehicleWatcher = spawner.spawnWatcher(WatcherType::VEHICLE,
                                                   RenderMode::HEADLESS,
                                                   "testVehicle.mp4",
                                                   "testVehicle.yaml");

    std::cout << "\nStarting Vehicle Headless mode...\n";

    vehicleWatcher->setCurrentTrafficState(TrafficState::GREEN_PHASE);
    for(int currFrame = 0; currFrame < greenFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    greenCountHeadless = vehicleWatcher->getInstanceCount();
    greenDensityHeadless = vehicleWatcher->getTrafficDensity();

    std::cout << "Green Vehicle count: " << greenCountHeadless << "\n";
    std::cout << "Green Vehicle density: " << greenDensityHeadless << "\n";

    if(greenCountHeadless <= 0 || greenDensityHeadless <= 0)
    {
        std::cerr << "Vehicle Detection failed on green phase\n";
        exit(EXIT_FAILURE);
    }

    vehicleWatcher->setCurrentTrafficState(TrafficState::RED_PHASE);
    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        vehicleWatcher->processFrame();
    }

    redCountHeadless = vehicleWatcher->getInstanceCount();
    redDensityHeadless = vehicleWatcher->getTrafficDensity();

    std::cout << "Red Vehicle count: " << redCountHeadless << "\n";
    std::cout << "Red Vehicle density: " << redDensityHeadless << "\n";

    if(redCountHeadless <= 0 || redDensityHeadless <= 0)
    {
        std::cerr << "Vehicle Detection failed on red phase\n";
        exit(EXIT_FAILURE);
    }

    delete vehicleWatcher;
}

void TrafficManager::compareVehicleResults()
{
    if(greenCountHeadless != greenCountGui)
    {
        std::cerr << "Vehicle Count for GUI and Headless mode have different "
                     "results for green phase!\n";
        exit(EXIT_FAILURE);
    }

    if(redCountHeadless != redCountGui)
    {
        std::cerr << "Vehicle Count for GUI and Headless mode have different "
                     "results for red phase!\n";
        exit(EXIT_FAILURE);
    }

    float densityErrorMargin = 1.0;

    if(std::abs(redDensityHeadless - redDensityGui) > densityErrorMargin)
    {
        std::cerr << "Vehicle Density for GUI and Headless mode have different "
                     "results for red phase!\n";
        exit(EXIT_FAILURE);
    }

    // REMOVED because mp4 test playback is faster, meaning that time calculation
    // for GUI and Headless mode is different.
    // if(std::abs(greenDensityHeadless - greenDensityGui) > densityErrorMargin)
    // {
    //     std::cerr << "Vehicle Density for GUI and Headless mode have different "
    //                  "results for green phase!\n";
    //     exit(EXIT_FAILURE);
    // }
}

void TrafficManager::testPedestrianWatcherGui(int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* pedestrianWatcher = spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                                      RenderMode::GUI,
                                                      "testPedestrian.mp4",
                                                      "testPedestrian.yaml");

    std::cout << "\nStarting Pedestrian GUI mode...\n";

    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        pedestrianWatcher->processFrame();
    }

    pedCountGui = pedestrianWatcher->getInstanceCount();

    std::cout << "GUI Pedestrian count: " << pedCountGui << "\n";

    if(pedCountGui <= 0)
    {
        std::cerr << "Pedestrian Detection failed\n";
        exit(EXIT_FAILURE);
    }

    delete pedestrianWatcher;
}

void TrafficManager::testPedestrianWatcherHeadless(int redFramesToCheck)
{
    WatcherSpawner spawner;
    Watcher* pedestrianWatcher = spawner.spawnWatcher(WatcherType::PEDESTRIAN,
                                                      RenderMode::HEADLESS,
                                                      "testPedestrian.mp4",
                                                      "testPedestrian.yaml");

    std::cout << "\nStarting Pedestrian Headless mode...\n";

    for(int currFrame = 0; currFrame < redFramesToCheck; ++currFrame)
    {
        pedestrianWatcher->processFrame();
    }

    pedCountHeadless = pedestrianWatcher->getInstanceCount();

    std::cout << "Headless Pedestrian count: " << pedCountHeadless << "\n";

    if(pedCountHeadless <= 0)
    {
        std::cerr << "Pedestrian Detection failed\n";
        exit(EXIT_FAILURE);
    }

    delete pedestrianWatcher;
}

void TrafficManager::comparePedestrianResults()
{
    if(pedCountHeadless != pedCountGui)
    {
        std::cerr << "Pedestrian Count for GUI and Headless mode have "
                     "different results!\n";
        exit(EXIT_FAILURE);
    }
}
